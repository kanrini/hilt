#pragma once

#include <optional>
#include <string>

bool save_default_module(const char* module_name);
std::optional<std::string> load_default_module();
