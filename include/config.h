#ifndef CONFIG_H
#define CONFIG_H

// Save the default module name to a file
int save_default_module(const char *module_name);

// Load the default module name from the file (Returned memory must be freed)
char *load_default_module();

#endif