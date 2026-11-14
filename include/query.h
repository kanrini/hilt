#pragma once

#include <optional>
#include <listkey.h>
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

std::optional<sword::ListKey> parse_reference(const sword::SWModule* module, const char* reference);
