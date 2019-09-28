#ifndef SPJALLA_CORE_SPOPT_H_
#define SPJALLA_CORE_SPOPT_H_

// Contains macros that define compile-time options.

// Whether the keyboard shortcuts to switch windows should be ignored while the overlay is visible.
#define OVERLAY_PREVENTS_SWAPPING

// The name of the global variable in which the plugin instance is stored within a shared library.
#define PLUGIN_GLOBAL_VARIABLE_NAME "ext_plugin"

// The name of the folder in the user's home directory in which data is stored.
#define DEFAULT_DATA_DIR ".spjalla"

#define DEFAULT_CONFIG_DB "client.conf"

#endif
