#pragma once

#include <filesystem>
#include <optional>
#include <string>

std::optional<std::filesystem::path> config_directory();
bool save_default_module(const char* module_name);
std::optional<std::string> load_default_module();
