#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "config.h"

// Constants for configuration directory and file name
#define CONFIG_DIR "/.hilt"
#define CONFIG_FILE "/config"

// Helper function: Constructs the full path to the configuration file
// Returns a dynamically allocated string (Caller must free this memory)
static char* get_config_path() {
    const char* home = getenv("HOME");
    if (!home) return nullptr; // C23 syntax

    // Calculate required buffer size: $HOME + /.hilt + /config + NULL terminator
    const size_t len = strlen(home) + strlen(CONFIG_DIR) + strlen(CONFIG_FILE) + 1;

    // Allocate memory for the path
    char* path = malloc(len);

    if (path) {
        sprintf(path, "%s%s%s", home, CONFIG_DIR, CONFIG_FILE);
    }
    return path;
}

// Helper function: Ensures the configuration directory exists (~/.hilt)
static void ensure_config_dir() {
    const char* home = getenv("HOME");
    if (!home) return;

    char path[1024];
    sprintf(path, "%s%s", home, CONFIG_DIR);

    // Create directory with rwxr-xr-x permissions (755)
    // If the directory already exists, this typically does nothing or fails harmlessly
    mkdir(path, 0755);
}

// Save the default Bible module name to the config file
int save_default_module(const char* module_name) {
    ensure_config_dir();

    char* path = get_config_path();
    if (!path) return 0;

    // Open file for writing (Overwrites existing content)
    FILE* fp = fopen(path, "w");
    if (!fp) {
        perror("Failed to open config file");
        free(path);
        return 0;
    }

    fprintf(fp, "%s", module_name);
    fclose(fp);
    free(path);

    printf("Default module set to: %s\n", module_name);
    return 1;
}

// Load the default Bible module name from the config file
// Returns a newly allocated string (Caller must free), or nullptr if not found
char* load_default_module() {
    char* path = get_config_path();
    if (!path) return nullptr;

    // Open file for reading
    FILE* fp = fopen(path, "r");
    if (!fp) {
        free(path);
        return nullptr; // Config file does not exist yet
    }

    char buffer[128];
    if (fgets(buffer, sizeof(buffer), fp)) {
        // Remove trailing newline character if present
        buffer[strcspn(buffer, "\n")] = 0;

        fclose(fp);
        free(path);

        // Return a duplicate of the buffer (Transfers memory ownership to caller)
        return strdup(buffer);
    }

    fclose(fp);
    free(path);
    return nullptr;
}