#pragma once

#include <optional>
#include <sword/listkey.h>
#include <sword/swmgr.h>
#include <sword/swmodule.h>
#include <sword/versekey.h>

std::optional<sword::ListKey> parse_reference(const sword::SWModule* module, const char* reference);
