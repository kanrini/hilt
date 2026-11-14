#include "install.h"
#include "config.h"

#include <archive.h>
#include <archive_entry.h>
#include <curl/curl.h>
#include <swmgr.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {
namespace fs = std::filesystem;
constexpr std::size_t max_download = 64 * 1024 * 1024;
constexpr std::size_t max_extracted = 256 * 1024 * 1024;

bool valid_id(std::string_view id) {
    return !id.empty() && id.size() <= 80 &&
           id.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-") == std::string_view::npos;
}

std::size_t receive(char* data, std::size_t size, std::size_t count, void* context) {
    auto& bytes = *static_cast<std::string*>(context);
    const auto length = size * count;
    if (length > max_download - bytes.size()) return 0;
    try {
        bytes.append(data, length);
        return length;
    } catch (...) {
        return 0;
    }
}

} // namespace

std::string download_https(const std::string& url) {
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    if (!curl) throw std::runtime_error("Could not initialize HTTPS download");

    std::string bytes;
#ifdef _WIN32
    curl_easy_setopt(curl.get(), CURLOPT_SSL_OPTIONS, static_cast<long>(CURLSSLOPT_NATIVE_CA));
#endif
    curl_easy_setopt(curl.get(), CURLOPT_USERAGENT, "Hilt");
    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl.get(), CURLOPT_REDIR_PROTOCOLS_STR, "https");
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 120L);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, receive);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &bytes);
    const auto result = curl_easy_perform(curl.get());
    if (result != CURLE_OK) throw std::runtime_error(curl_easy_strerror(result));
    return bytes;
}

namespace {
void extract(const std::string& bytes, const fs::path& destination) {
    std::unique_ptr<archive, decltype(&archive_read_free)> zip(archive_read_new(), archive_read_free);
    if (!zip ||
        archive_read_support_format_zip(zip.get()) != ARCHIVE_OK ||
        archive_read_open_memory(zip.get(), bytes.data(), bytes.size()) != ARCHIVE_OK) {
        throw std::runtime_error("Could not open the CrossWire ZIP");
    }

    std::size_t extracted = 0;
    archive_entry* entry;
    int status;
    while ((status = archive_read_next_header(zip.get(), &entry)) == ARCHIVE_OK) {
        const char* name = archive_entry_pathname(entry);
        if (!name) throw std::runtime_error("Missing ZIP entry name");
        const fs::path relative = fs::u8path(name);
        if (relative.empty() || relative.has_root_path() || std::string_view(name).find_first_of("\\:") != std::string_view::npos) {
            throw std::runtime_error("Unsafe ZIP entry path");
        }
        for (const auto& part : relative) {
            if (part == "..") throw std::runtime_error("Unsafe ZIP entry path");
        }
        const auto root = *relative.begin();
        if (root != "mods.d" && root != "modules") continue;

        const auto path = destination / relative;
        const auto type = archive_entry_filetype(entry);
        if (type == AE_IFDIR) {
            fs::create_directories(path);
            continue;
        }
        if (type != AE_IFREG || archive_entry_hardlink(entry)) {
            throw std::runtime_error("Unsupported ZIP entry type");
        }
        fs::create_directories(path.parent_path());
        std::ofstream out(path, std::ios::binary);
        char buffer[16384];
        for (;;) {
            const auto count = archive_read_data(zip.get(), buffer, sizeof buffer);
            if (count < 0 || static_cast<std::size_t>(count) > max_extracted - extracted) {
                throw std::runtime_error("Could not extract CrossWire ZIP");
            }
            if (count == 0) break;
            extracted += count;
            out.write(buffer, count);
            if (!out) throw std::runtime_error("Could not write module file");
        }
        out.close();
        if (!out) throw std::runtime_error("Could not close module file");
    }
    if (status != ARCHIVE_EOF) throw std::runtime_error("Incomplete CrossWire ZIP");
}

struct StagingDirectory {
    fs::path path;
    ~StagingDirectory() {
        std::error_code error;
        fs::remove_all(path, error);
    }
};
} // namespace

std::unique_ptr<sword::SWMgr> open_installed_module(const char* name) {
    if (!valid_id(name)) {
        return nullptr;
    }

    const auto directory = config_directory();
    if (!directory) return nullptr;
    const auto path = *directory / "modules" / name;
    std::error_code error;
    if (!fs::is_directory(path / "mods.d", error)) return nullptr;

    auto manager = std::make_unique<sword::SWMgr>(path.string().c_str(), true, nullptr, false, false);
    if (!manager->getModule(name)) return nullptr;
    return manager;
}

bool install_module(const char* name) {
    if (!valid_id(name)) {
        std::fprintf(stderr, "Invalid CrossWire module ID: %s\n", name);
        return false;
    }

    try {
        const auto directory = config_directory();
        if (!directory) throw std::runtime_error("Could not locate the user configuration directory");
        const auto store = *directory / "modules";
        const auto destination = store / name;
        if (fs::exists(destination)) {
            sword::SWMgr installed(destination.string().c_str(), true, nullptr, false, false);
            if (!installed.getModule(name)) throw std::runtime_error("Existing module could not be loaded");
            return save_default_module(name);
        }

        fs::create_directories(store);
        const auto staging_path = store / (std::string(".install-") + name);
        if (!fs::create_directory(staging_path)) throw std::runtime_error("An installation is already in progress");
        const StagingDirectory staging{staging_path};
        std::printf("Downloading %s from CrossWire...\n", name);
        extract(download_https(std::string("https://crosswire.org/ftpmirror/pub/sword/packages/rawzip/") + name + ".zip"), staging.path);

        {
            sword::SWMgr candidate(staging.path.string().c_str(), true, nullptr, false, false);
            if (!candidate.getModule(name)) throw std::runtime_error("Archive does not contain the requested SWORD module");
        }

        fs::rename(staging.path, destination);
        return save_default_module(name);
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Could not install %s: %s\n", name, error.what());
        return false;
    }
}
