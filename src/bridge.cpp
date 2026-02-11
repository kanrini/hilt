#include "bridge.h"
#include <sword/swmgr.h>
#include <sword/swmodule.h>
#include <sword/versekey.h>
#include <sword/listkey.h>
#include <iostream>
#include <cstring>

// Internal structure to hold the Sword Manager
struct SwordContext {
    sword::SWMgr *mgr;
};

// Internal structure to maintain the state of an iteration
struct SwordIterator {
    sword::SWModule *module{};
    sword::ListKey listKey;
    bool started{};
};

extern "C" {
// Initialize the Sword Engine
SwordContext *sword_init(void) {
    auto *ctx = new SwordContext;
    ctx->mgr = new sword::SWMgr();
    return ctx;
}

// Check if a specific Bible module exists (e.g., KJV, KorRV)
int sword_module_exists(const SwordContext *ctx, const char *name) {
    if (!ctx || !ctx->mgr || !name) return 0;
    return (ctx->mgr->getModule(name) != nullptr);
}

// Execute a query (parse reference) and return an iterator
SwordIterator *sword_query(const SwordContext *ctx, const char *version, const char *key) {
    if (!ctx || !ctx->mgr) return nullptr;

    sword::SWModule *module = ctx->mgr->getModule(version);
    if (!module) return nullptr;

    // Configure VerseKey for parsing
    sword::VerseKey vk;
    vk.setLocale("ko"); // Support Korean abbreviations (e.g., "요" for John)
    vk.setAutoNormalize(true);
    vk.setIntros(true);

    // [Fix 1] Updated API: ParseVerseList -> parseVerseList
    // Parses the input key string (e.g., "John 3:16", "Rev 7:1-3") into a list of verses.
    const sword::ListKey list = vk.parseVerseList(key, vk, true);

    // [Fix 2] Updated API: Count() -> getCount()
    if (list.getCount() == 0) return nullptr;

    auto *iter = new SwordIterator;
    iter->module = module;
    iter->listKey = list;
    iter->listKey.setPosition(sword::TOP);
    iter->started = false;

    return iter;
}

// Fetch the next verse from the iterator
char *sword_iterator_next(SwordIterator *iter, int *verse_num_out) {
    if (!iter) return nullptr;

    // Move to the next element
    if (iter->started) {
        ++iter->listKey;
    } else {
        iter->started = true;
    }

    // [Fix 3] Updated API: Error() -> popError()
    // Check if we have reached the end of the list
    if (iter->listKey.popError()) return nullptr;

    // Set the module to the current key position
    iter->module->setKey(iter->listKey);

    // Extract verse number if requested
    if (verse_num_out) {
        const auto *vk = dynamic_cast<sword::VerseKey *>(iter->module->getKey());
        *verse_num_out = vk->getVerse();
    }

    // Return the text (stripped of OSIS/XML tags)
    return strdup(iter->module->stripText());
}

// Free iterator memory
void sword_iterator_free(const SwordIterator *iter) {
    delete iter;
}

// Free engine memory
void sword_free(const SwordContext *ctx) {
    if (ctx) {
        delete ctx->mgr;
        delete ctx;
    }
}

// Wrapper function for fetching a single verse (Backward Compatibility)
char *sword_get_verse(const SwordContext *ctx, const char *version, const char *key) {
    SwordIterator *iter = sword_query(ctx, version, key);
    if (!iter) return nullptr;
    char *text = sword_iterator_next(iter, nullptr);
    sword_iterator_free(iter);

    return text;
}
}
