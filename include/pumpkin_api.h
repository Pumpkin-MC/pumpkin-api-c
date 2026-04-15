#ifndef PUMPKIN_API_H
#define PUMPKIN_API_H

#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* name;
    const char* version;
    const char** authors;
    size_t authors_count;
    const char* description;
    const char** dependencies;
    size_t dependencies_count;
} pumpkin_metadata_t;

typedef void (*pumpkin_on_load_t)(plugin_own_context_t ctx);
typedef void (*pumpkin_on_unload_t)(plugin_own_context_t ctx);
typedef pumpkin_metadata_t (*pumpkin_get_metadata_t)(void);

typedef struct {
    pumpkin_get_metadata_t get_metadata;
    pumpkin_on_load_t on_load;
    pumpkin_on_unload_t on_unload;
} pumpkin_plugin_t;

void pumpkin_register_plugin(pumpkin_plugin_t plugin);

#define REGISTER_PUMPKIN_PLUGIN(plugin) \
    void exports_plugin_init_plugin(void) { \
        pumpkin_register_plugin(plugin); \
    }

#ifdef __cplusplus
}
#endif

#endif
