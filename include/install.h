#pragma once

#include <memory>
#include <string>
namespace sword { class SWMgr; }

bool install_module(const char* name);
std::unique_ptr<sword::SWMgr> open_installed_module(const char* name);

std::string download_https(const std::string& url);
bool update_hilt();
