#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bridge.h"
#include "config.h"

// ANSI Color Codes
#define COLOR_CYAN  "\033[1;36m"
#define COLOR_RESET "\033[0m"
#define COLOR_RED   "\033[0;31m"

// Function Prototypes
void print_usage(const char *prog_name);

char *join_arguments(int start_index, int argc, char *argv[], bool *out_list_mode);

int handle_set_default(const char *module_name);

void print_results(SwordIterator *iter, bool list_mode);

// Display usage information with detailed examples
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

// Parses command line arguments to build the Bible reference string.
// Detects and excludes the '-l' flag.
char *join_arguments(const int start_index, const int argc, char *argv[], bool *out_list_mode) {
    size_t total_len = 0;
    *out_list_mode = false;

    // Pass 1: Calculate total length and check for flags
    for (int i = start_index; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            *out_list_mode = true;
            continue;
        }
        total_len += strlen(argv[i]) + 1; // +1 for space or terminator
    }

    if (total_len == 0) return nullptr;

    char *result = (char *) malloc(total_len + 1);
    if (!result) return nullptr;
    result[0] = '\0';

    // Pass 2: Concatenate strings
    for (int i = start_index; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) continue;

        strcat(result, argv[i]);
        // Add space if it's not the last argument
        if (i < argc - 1) strcat(result, " ");
    }
    return result;
}

// Handles the logic for the '-s' flag (Saving default module)
int handle_set_default(const char *module_name) {
    // Validate module existence before saving
    const SwordContext *tmp_engine = sword_init();
    if (tmp_engine) {
        if (!sword_module_exists(tmp_engine, module_name)) {
            fprintf(stderr, "Warning: Module '%s' is not currently installed.\n", module_name);
        }
        sword_free(tmp_engine);
    }

    if (save_default_module(module_name)) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// Iterates through the results and prints them based on the mode
void print_results(SwordIterator *iter, bool list_mode) {
    if (!iter) {
        printf("No results found. (Check parsing or range)\n");
        return;
    }

    int verse_num = 0;
    char *text;
    int count = 0;

    printf("\n"); // Padding

    while ((text = sword_iterator_next(iter, &verse_num)) != NULL) {
        count++;

        if (list_mode) {
            // List Mode: Print verse numbers in a grid (10 per line)
            printf("%-3d ", verse_num);
            if (count % 10 == 0) printf("\n");
        } else {
            // Text Mode: Print verse number and content
            printf(":%d %s\n", verse_num, text);
        }
        free(text);
    }

    // Final formatting for list mode
    if (list_mode && count % 10 != 0) printf("\n");

    printf("\n"); // Padding
}

// Main Entry Point
int main(const int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // --- 1. Configuration Mode (-s) ---
    if (strcmp(argv[1], "-s") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing version name.\n");
            return EXIT_FAILURE;
        }
        return handle_set_default(argv[2]);
    }

    // --- 2. Query Mode ---

    // Initialize Sword Engine
    SwordContext *engine = sword_init();
    if (!engine) {
        fprintf(stderr, "Error: Failed to initialize Sword engine.\n");
        return EXIT_FAILURE;
    }

    const char *target_version;
    char *target_key;
    char *loaded_default = nullptr;
    bool list_mode = false;

    // Logic: Check if the first argument is a valid Module ID (e.g., KJV, KorRV)
    if (sword_module_exists(engine, argv[1])) {
        // Case A: Explicit Version -> hilt KJV John 3:16
        target_version = argv[1];
        target_key = join_arguments(2, argc, argv, &list_mode);
    } else {
        // Case B: Default Version -> hilt John 3:16
        loaded_default = load_default_module();
        // Fallback to KJV if no default is set
        target_version = loaded_default ? loaded_default : "KJV";
        target_key = join_arguments(1, argc, argv, &list_mode);
    }

    // Validate Input
    if (!target_key || strlen(target_key) == 0) {
        printf("Please provide a bible reference.\n");
        if (target_key) free(target_key);
        if (loaded_default) free(loaded_default);
        sword_free(engine);
        return EXIT_FAILURE;
    }

    // Execute Query
    printf("%s[%s] %s%s\n", COLOR_CYAN, target_version, target_key, COLOR_RESET);

    SwordIterator *iter = sword_query(engine, target_version, target_key);
    print_results(iter, list_mode);

    // Cleanup
    if (iter) sword_iterator_free(iter);
    if (target_key) free(target_key);
    if (loaded_default) free(loaded_default);
    sword_free(engine);

    return EXIT_SUCCESS;
}
