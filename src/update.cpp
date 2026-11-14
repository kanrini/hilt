#include "install.h"
#include "config.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <regex>
#include <stdexcept>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

#ifndef HILT_BUILD_VERSION
#define HILT_BUILD_VERSION "0.0.0"
#endif

namespace {
namespace fs = std::filesystem;

std::array<unsigned long, 3> version_parts(const std::string& version) {
    static const std::regex pattern(R"(^([0-9]+)\.([0-9]+)\.([0-9]+)$)");
    std::smatch match;
    if (!std::regex_match(version, match, pattern)) throw std::runtime_error("Invalid release version");
    return {std::stoul(match[1]), std::stoul(match[2]), std::stoul(match[3])};
}
} // namespace

bool update_hilt() {
#if !(defined(__x86_64__) || defined(_M_X64)) || !(defined(_WIN32) || defined(__linux__))
    std::fprintf(stderr, "Hilt updates are available for Windows and Arch Linux x86-64.\n");
    return false;
#else
    try {
#ifndef _WIN32
        if (access("/usr/bin/pacman", X_OK) != 0 ||
            !fs::equivalent("/proc/self/exe", "/usr/bin/hilt")) {
            throw std::runtime_error("Automatic updates require the Arch Linux Hilt package");
        }
#endif
        const std::string repository = "https://github.com/kanrini/hilt/releases/";
        const auto release = download_https("https://api.github.com/repos/kanrini/hilt/releases/latest");
        const std::regex tag_pattern(R"json("tag_name"\s*:\s*"(v?([0-9]+\.[0-9]+\.[0-9]+))")json");
        std::smatch tag;
        if (!std::regex_search(release, tag, tag_pattern)) throw std::runtime_error("No supported release tag found");
        const std::string version = tag[2];
        if (version_parts(version) <= version_parts(HILT_BUILD_VERSION)) {
            std::printf("Hilt %s is up to date.\n", HILT_BUILD_VERSION);
            return true;
        }

#ifdef _WIN32
        const auto filename = "hilt-" + version + "-windows-x86_64-setup.exe";
#else
        const auto filename = "hilt-" + version + "-1-x86_64.pkg.tar.zst";
#endif
        const auto url = repository + "download/" + tag[1].str() + "/" + filename;
        std::printf("Updating Hilt %s to %s.\n", HILT_BUILD_VERSION, version.c_str());
#ifdef _WIN32
        const auto directory = config_directory();
        if (!directory) throw std::runtime_error("Could not locate the user configuration directory");
        const auto stage = *directory / ("update-" + std::to_string(GetCurrentProcessId()));
        if (!fs::create_directories(stage)) throw std::runtime_error("Could not create update directory");
        const auto installer = stage / filename;
        try {
            {
                const auto bytes = download_https(url);
                const auto partial = stage / "download.part";
                std::ofstream output(partial, std::ios::binary);
                output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
                output.close();
                if (!output) throw std::runtime_error("Could not save installer");
                fs::rename(partial, installer);
            }
            if (reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", installer.c_str(),
                                                        nullptr, stage.c_str(), SW_SHOWNORMAL)) <= 32) {
                throw std::runtime_error("Could not launch the Windows installer");
            }
        } catch (...) {
            std::error_code ignored;
            fs::remove_all(stage, ignored);
            throw;
        }
        std::puts("Finish the update in the Windows installer.");
        return true;
#else
        std::fflush(stdout);
        const auto pid = fork();
        if (pid < 0) throw std::runtime_error("Could not start pacman");
        if (pid == 0) {
            if (geteuid() == 0) {
                execl("/usr/bin/pacman", "pacman", "-U", "--", url.c_str(), nullptr);
            } else {
                execl("/usr/bin/sudo", "sudo", "/usr/bin/pacman", "-U", "--", url.c_str(), nullptr);
            }
            std::perror("Could not start pacman");
            _exit(127);
        }
        int status;
        if (waitpid(pid, &status, 0) != pid || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            throw std::runtime_error("pacman did not complete the update");
        }
        return true;
#endif
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Could not update Hilt: %s\n", error.what());
        return false;
    }
#endif
}
