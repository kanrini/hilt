#include "config.h"
#include "query.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>

#if defined(_WIN32) || defined(HILT_LOCALE_DIR)
#include <localemgr.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <vector>
#endif

namespace {
constexpr auto COLOR_CYAN = "\033[1;36m";
constexpr auto COLOR_RESET = "\033[0m";
void print_usage(const char* prog_name) {
    printf("  Hilt: The Sword Interface (v1.0.0)\n");
    printf("  A lightweight CLI Bible reader based on the SWORD Project.\n\n");

    printf("  Usage:\n");
    printf("    %s [OPTIONS] [VERSION] <REFERENCE>\n\n", prog_name);

    printf("  Arguments:\n");
    printf("    VERSION      Specific Bible module to use (e.g., KJV, KorRV).\n");
    printf("                 If omitted, the default configured version is used.\n");
    printf("    REFERENCE    Target scripture (Book Chapter:Verse).\n");
    printf("                 Supports standard abbreviations and ranges.\n\n");

    printf("  Options:\n");
    printf("    -s <VERSION> Set the default Bible version permanently (saved to ~/.hilt/config).\n");
    printf("    -l           List mode: Display verse numbers only (useful for overview).\n");

    printf("\n  Examples:\n");
    printf("    # Configuration\n");
    printf("    %s -s KorRV            Set default version to Korean Revised\n", prog_name);
    printf("\n    # Reading\n");
    printf("    %s John 3:16           Read single verse (using default)\n", prog_name);
    printf("    %s KJV Gen 1:1         Read specific version explicitly\n", prog_name);
    printf("    %s Rev 7:1-14          Read a range of verses\n", prog_name);
    printf("    %s John 3:             Read entire chapter\n", prog_name);
    printf("\n    # Utilities\n");
    printf("    %s Ps 119 -l           List all verse numbers in Psalm 119\n\n", prog_name);
}

void print_results(sword::SWModule* module, const std::string& reference, bool list_mode) {
    auto verses = parse_reference(module, reference.c_str());
    if (!verses) {
        std::printf("No results found. (Check parsing or range)\n");
        return;
    }

    int count = 0;

    std::printf("\n");

    for (; !verses->popError(); ++*verses) {
        module->setKey(*verses);
        const auto* key = dynamic_cast<const sword::VerseKey*>(module->getKey());
        if (!key) break;

        ++count;
        if (list_mode) {
            std::printf("%-3d ", key->getVerse());
            if (count % 10 == 0) std::printf("\n");
        } else {
            std::printf(":%d %s\n", key->getVerse(), module->stripText());
        }
    }

    if (list_mode && count % 10 != 0) std::printf("\n");
    std::printf("\n");
}
int run(int argc, const char* const* argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const std::string_view first_arg = argv[1];
    if (first_arg == "-s" && argc < 3) {
        std::fprintf(stderr, "Error: Missing version name.\n");
        return EXIT_FAILURE;
    }

    sword::SWMgr manager;
    if (first_arg == "-s") {
        if (!manager.getModule(argv[2])) {
            std::fprintf(stderr, "Warning: Module '%s' is not currently installed.\n", argv[2]);
        }
        return save_default_module(argv[2]) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    auto* module = manager.getModule(argv[1]);
    const bool explicit_version = module != nullptr;
    const std::string version = explicit_version ? argv[1] : load_default_module().value_or("KJV");

    std::string reference;
    bool list_mode = false;
    for (int i = explicit_version ? 2 : 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "-l") {
            list_mode = true;
            continue;
        }
        reference += argv[i];
        if (i < argc - 1) reference += ' ';
    }
    if (reference.empty()) {
        std::printf("Please provide a bible reference.\n");
        return EXIT_FAILURE;
    }

    if (!explicit_version) module = manager.getModule(version.c_str());

    std::printf("%s[%s] %s%s\n", COLOR_CYAN, version.c_str(), reference.c_str(), COLOR_RESET);

    print_results(module, reference, list_mode);

    return EXIT_SUCCESS;
}

#ifdef _WIN32
std::string to_utf8(const wchar_t* input) {
    const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, input, -1,
                                        nullptr, 0, nullptr, nullptr);
    if (size == 0) return {};

    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, input, -1,
                        result.data(), size, nullptr, nullptr);
    result.pop_back();
    return result;
}

void configure_console() {
    SetConsoleOutputCP(CP_UTF8);
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(output, &mode)) {
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

void load_installed_locales() {
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(),
                                            static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size()) return;

    executable.resize(length);
    const auto locales = std::filesystem::path(executable).parent_path() / "locales.d";
    std::error_code error;
    if (std::filesystem::is_directory(locales, error)) {
        sword::LocaleMgr::getSystemLocaleMgr()->loadConfigDir(locales.string().c_str());
    }
}
#endif
} // namespace

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
    configure_console();
    load_installed_locales();

    std::vector<std::string> arguments;
    arguments.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        arguments.push_back(to_utf8(argv[i]));
    }
    std::vector<const char*> pointers;
    pointers.reserve(argc);
    for (const auto& argument : arguments) {
        pointers.push_back(argument.c_str());
    }
    return run(argc, pointers.data());
}
#else
int main(int argc, char* argv[]) {
#ifdef HILT_LOCALE_DIR
    sword::LocaleMgr::getSystemLocaleMgr()->loadConfigDir(HILT_LOCALE_DIR);
#endif
    return run(argc, argv);
}
#endif
