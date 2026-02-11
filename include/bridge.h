#ifndef BRIDGE_H
#define BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

    // Define opaque handle types (Internal details are hidden from C)
    typedef struct SwordContext SwordContext;

    // Opaque handle type for the Iterator
    typedef struct SwordIterator SwordIterator;

    // 1. Initialize the engine (Create SWMgr)
    SwordContext* sword_init(void);

    // 2. Check if a module exists
    int sword_module_exists(const SwordContext* ctx, const char* name);

    // Execute query and create an iterator (Used instead of simple get_verse)
    // Example: Parses ranges like "Rev 7:1-14" into a list
    SwordIterator* sword_query(const SwordContext* ctx, const char* version, const char* key);

    // Get the next verse from the iterator (Returns NULL if no more verses)
    // Stores the current verse number in verse_num_out
    char* sword_iterator_next(SwordIterator* iter, int* verse_num_out);

    // Free iterator memory
    void sword_iterator_free(const SwordIterator* iter);

    // (Backward Compatibility) Fetch a single verse
    char* sword_get_verse(const SwordContext* ctx, const char* version, const char* key);

    // 3. Shutdown engine (Free memory)
    void sword_free(const SwordContext* ctx);

#ifdef __cplusplus
}
#endif

#endif