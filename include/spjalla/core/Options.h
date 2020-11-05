#ifndef SPJALLA_CORE_SPOPT_H_
#define SPJALLA_CORE_SPOPT_H_

// Contains macros that define compile-time options.

#define SPJALLA_VERSION_NUMBER "0.2.0"

// Whether the keyboard shortcuts to switch windows should be ignored while the overlay is visible.
#define OVERLAY_PREVENTS_SWAPPING

// The name of the global variable in which the plugin instance is stored within a shared library.
#define PLUGIN_GLOBAL_VARIABLE_NAME "ext_plugin"

// The name of the folder in the user's home directory in which data is stored.
#define DEFAULT_DATA_DIR  ".spjalla"
#define DEFAULT_CONFIG_DB "client.conf"
#define DEFAULT_ALIAS_DB  "aliases.conf"

// Whether to recompute line text on every render instead of only on creation.
// #define RERENDER_LINES

// Whether to disable performance testing. Saves a bit of memory and CPU usage.
#ifndef DISABLE_PERFORMANCE
#define DISABLE_PERFORMANCE
#endif

#endif
