#include "pumpkin_api.h"
#include <stdlib.h>
#include <string.h>

static pumpkin_plugin_t g_plugin = {0};

void pumpkin_register_plugin(pumpkin_plugin_t plugin) {
    g_plugin = plugin;
}

// WIT exports

void exports_pumpkin_plugin_metadata_get_metadata(exports_pumpkin_plugin_metadata_plugin_metadata_t *ret) {
    if (g_plugin.get_metadata) {
        pumpkin_metadata_t meta = g_plugin.get_metadata();
        plugin_string_dup(&(ret->name), meta.name);
        plugin_string_dup(&(ret->version), meta.version);
        
        ret->authors.len = meta.authors_count;
        ret->authors.ptr = (plugin_string_t*)malloc(ret->authors.len * sizeof(plugin_string_t));
        for (size_t i = 0; i < ret->authors.len; ++i) {
            plugin_string_dup(&(ret->authors.ptr[i]), meta.authors[i]);
        }

        plugin_string_dup(&(ret->description), meta.description);

        ret->dependencies.len = meta.dependencies_count;
        ret->dependencies.ptr = (plugin_string_t*)malloc(ret->dependencies.len * sizeof(plugin_string_t));
        for (size_t i = 0; i < ret->dependencies.len; ++i) {
            plugin_string_dup(&(ret->dependencies.ptr[i]), meta.dependencies[i]);
        }
    }
}

bool exports_plugin_on_load(plugin_own_context_t context_handle, plugin_string_t *err) {
    if (g_plugin.on_load) {
        g_plugin.on_load(context_handle);
        return true;
    }
    return true; // No on_load is not an error
}

bool exports_plugin_on_unload(plugin_own_context_t context_handle, plugin_string_t *err) {
    if (g_plugin.on_unload) {
        g_plugin.on_unload(context_handle);
        return true;
    }
    return true;
}

void exports_plugin_handle_event(uint32_t event_id, plugin_own_server_instance_t server, plugin_event_t *event, plugin_event_t *ret) {
    // Basic event pass-through
    *ret = *event;
}

bool exports_plugin_handle_command(uint32_t command_id, plugin_own_command_sender_t sender, plugin_own_server_instance_t server, plugin_own_consumed_args_t args, int32_t *ret, plugin_command_error_t *err) {
    err->is_err = true;
    plugin_string_dup(&(err->val.err.val.command_failed.val.text), "Command not implemented");
    return false;
}

void exports_plugin_handle_task(uint32_t handler_id, plugin_own_server_instance_t server) {
}
