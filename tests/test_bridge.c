#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bridge.h"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

void log_pass(const char* test_name) {
    printf("[%sPASS%s] %s\n", GREEN, RESET, test_name);
}

void log_fail(const char* test_name) {
    printf("[%sFAIL%s] %s\n", RED, RESET, test_name);
    exit(1); // Exit immediately on failure
}

// 1. Test Engine Initialization
void test_engine_init() {
    SwordContext* ctx = sword_init();
    if (ctx != NULL) {
        log_pass("Engine Initialization");
    } else {
        log_fail("Engine Initialization (Context is NULL)");
    }
    sword_free(ctx);
}

// 2. Test Fetching a Single Verse (Wrapper Function)
void test_fetch_valid_verse() {
    SwordContext* ctx = sword_init();

    // Fetch KJV John 3:16
    char* text = sword_get_verse(ctx, "KJV", "John 3:16");

    if (text != NULL && strlen(text) > 0) {
        // Check if content contains expected word "God"
        if (strstr(text, "God") != NULL) {
            // Print a snippet for visual confirmation
            printf("   -> Content: %.50s...\n", text);
            log_pass("Fetch Valid Verse (KJV John 3:16)");
        } else {
            printf("   -> RECEIVED TEXT: [%s]\n", text);
            log_fail("Fetch Valid Verse (Content mismatch)");
        }
        free(text);
    } else {
        printf("   -> RECEIVED: NULL or Empty\n");
        log_fail("Fetch Valid Verse (Return is NULL or Empty)");
    }

    sword_free(ctx);
}

// 3. Test Range Query using Iterator (New Feature)
void test_iterator_range() {
    SwordContext* ctx = sword_init();

    // Request a range: Revelation 7:1-3 (3 verses)
    // This tests list parsing, iterator creation, and loop logic.
    const char* range_query = "Rev 7:1-3";
    SwordIterator* iter = sword_query(ctx, "KJV", range_query);

    if (iter) {
        int verse_num = 0;
        char* text = NULL;
        int count = 0;

        // Iterate through results
        while ((text = sword_iterator_next(iter, &verse_num)) != NULL) {
            count++;
            // Verify verse numbers (should be 1, 2, 3 sequentially)
            if (verse_num != count) {
                printf("   -> Expected Verse %d, Got %d\n", count, verse_num);
                log_fail("Iterator Range (Verse number mismatch)");
            }
            free(text);
        }
        sword_iterator_free(iter);

        if (count == 3) {
            log_pass("Iterator Range (Rev 7:1-3 count is 3)");
        } else {
            printf("   -> Expected 3 verses, got %d\n", count);
            log_fail("Iterator Range (Count mismatch)");
        }

    } else {
        log_fail("Iterator Range (Failed to create iterator)");
    }

    sword_free(ctx);
}

// 4. Test Error Handling for Invalid Module
void test_invalid_module() {
    SwordContext* ctx = sword_init();

    // Request with non-existent module
    char* text = sword_get_verse(ctx, "GALAXY_VERSION", "John 3:16");

    // It should return NULL or an error string depending on implementation.
    // In our bridge.cpp (sword_query), it returns nullptr if module is missing.
    // However, the wrapper sword_get_verse handles nullptr by returning nullptr.

    // NOTE: Depending on your bridge implementation update:
    // If sword_query returns NULL, sword_get_verse returns NULL.
    // Let's check for NULL as the safe failure state.
    if (text == NULL) {
         log_pass("Error Handling (Invalid Module returns NULL)");
    } else {
         // If legacy wrapper returns error string
         if (strstr(text, "Error") != NULL) {
             log_pass("Error Handling (Invalid Module returns Error String)");
         } else {
             log_fail("Error Handling (Should fail gracefully)");
         }
         free(text);
    }

    sword_free(ctx);
}

int main() {
    printf("========================================\n");
    printf(" Hilt (Sword Bridge) Unit Tests \n");
    printf("========================================\n");

    test_engine_init();
    test_fetch_valid_verse();
    test_iterator_range(); // Added range test
    test_invalid_module();

    printf("========================================\n");
    printf(" %sAll Tests Passed Successfully!%s\n", GREEN, RESET);
    printf("========================================\n");

    return 0;
}