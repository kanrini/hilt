#include "config.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {
std::optional<std::filesystem::path> config_directory() {
#ifdef _WIN32
    const wchar_t* home = _wgetenv(L"HOME");
    if (!home || !*home) home = _wgetenv(L"USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    if (!home || !*home) return std::nullopt;
    return std::filesystem::path(home) / ".hilt";
}
} // namespace

bool save_default_module(const char* module_name) {
    const auto directory = config_directory();
    if (!directory) return false;

    std::error_code error;
    std::filesystem::create_directory(*directory, error);
    if (error) {
        std::fprintf(stderr, "Failed to create config directory: %s\n", error.message().c_str());
        return false;
    }

    std::ofstream config(*directory / "config");
    if (!config) {
        std::fprintf(stderr, "Failed to open config file\n");
        return false;
    }
    config << module_name;
    config.close();
    if (!config) {
        std::fprintf(stderr, "Failed to write config file\n");
        return false;
    }

    std::printf("Default module set to: %s\n", module_name);
    return true;
}

std::optional<std::string> load_default_module() {
    const auto directory = config_directory();
    if (!directory) return std::nullopt;

    std::ifstream config(*directory / "config");
    std::string module_name;
    if (!std::getline(config, module_name)) return std::nullopt;
    return module_name;
}
