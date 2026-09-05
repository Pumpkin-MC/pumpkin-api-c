#include "pumpkin_api.h"
#include <stdlib.h>
#include <string.h>

static pumpkin_plugin_t g_plugin = {0};

void pumpkin_register_plugin(pumpkin_plugin_t plugin) { g_plugin = plugin; }

// WIT exports

void exports_pumpkin_plugin_metadata_get_metadata(
    exports_pumpkin_plugin_metadata_plugin_metadata_t *ret) {
  if (g_plugin.get_metadata) {
    pumpkin_metadata_t meta = g_plugin.get_metadata();
    plugin_string_dup(&(ret->name), meta.name);
    plugin_string_dup(&(ret->version), meta.version);

    ret->authors.len = meta.authors_count;
    ret->authors.ptr =
        (plugin_string_t *)malloc(ret->authors.len * sizeof(plugin_string_t));
    for (size_t i = 0; i < ret->authors.len; ++i) {
      plugin_string_dup(&(ret->authors.ptr[i]), meta.authors[i]);
    }

    plugin_string_dup(&(ret->description), meta.description);

    ret->dependencies.len = meta.dependencies_count;
    ret->dependencies.ptr = (plugin_string_t *)malloc(ret->dependencies.len *
                                                      sizeof(plugin_string_t));
    for (size_t i = 0; i < ret->dependencies.len; ++i) {
      plugin_string_dup(&(ret->dependencies.ptr[i]), meta.dependencies[i]);
    }

    ret->permissions.len = meta.permissions_count;
    ret->permissions.ptr = (plugin_string_t *)malloc(ret->permissions.len *
                                                     sizeof(plugin_string_t));
    for (size_t i = 0; i < ret->permissions.len; ++i) {
      plugin_string_dup(&(ret->permissions.ptr[i]), meta.permissions[i]);
    }
  }
}

bool exports_plugin_on_load(plugin_own_context_t context_handle,
                            plugin_string_t *err) {
  if (g_plugin.on_load) {
    g_plugin.on_load(context_handle);
  }
  return true;
}

bool exports_plugin_on_unload(plugin_own_context_t context_handle,
                              plugin_string_t *err) {
  if (g_plugin.on_unload) {
    g_plugin.on_unload(context_handle);
  }
  return true;
}

void exports_plugin_handle_event(uint32_t event_id,
                                 plugin_own_server_instance_t server,
                                 plugin_event_t *event, plugin_event_t *ret) {
  *ret = *event;
  pumpkin_event_handler_t handler =
      (pumpkin_event_handler_t)(uintptr_t)event_id;
  handler(server, ret);
}

bool exports_plugin_handle_command(uint32_t command_id,
                                   plugin_own_command_sender_t sender,
                                   plugin_own_server_instance_t server,
                                   plugin_own_consumed_args_t args,
                                   int32_t *ret, plugin_command_error_t *err) {
  pumpkin_command_handler_t handler =
      (pumpkin_command_handler_t)(uintptr_t)command_id;
  return handler(sender, server, args, ret, err);
}

void exports_plugin_handle_command_suggestion(
    uint32_t handler_id, plugin_own_command_sender_t sender,
    plugin_own_server_instance_t server, plugin_suggestion_request_t *request,
    plugin_command_suggestions_t *ret) {
  pumpkin_command_suggestion_handler_t handler =
      (pumpkin_command_suggestion_handler_t)(uintptr_t)handler_id;
  handler(sender, server, request, ret);
}

void exports_plugin_handle_task(uint32_t handler_id,
                                plugin_own_server_instance_t server) {
  pumpkin_task_handler_t handler =
      (pumpkin_task_handler_t)(uintptr_t)handler_id;
  handler(server);
}

bool exports_plugin_handle_ipc_message(plugin_plugin_id_t *sender,
                                       plugin_ipc_message_t *message,
                                       plugin_ipc_message_t *ret,
                                       plugin_string_t *err) {
  if (g_plugin.handle_ipc_message) {
    return g_plugin.handle_ipc_message(sender, message, ret, err);
  }

  plugin_string_dup(err, "This plugin cannot recieve messages");
  return false;
}

bool exports_plugin_handle_ai_goal_can_start(
    uint32_t goal_id, plugin_own_server_instance_t server,
    plugin_own_entity_t entity) {
  pumpkin_ai_goal_t *goal = (pumpkin_ai_goal_t *)(uintptr_t)goal_id;
  return goal->can_start(server, entity);
}

bool exports_plugin_handle_ai_goal_should_continue(
    uint32_t goal_id, plugin_own_server_instance_t server,
    plugin_own_entity_t entity) {
  pumpkin_ai_goal_t *goal = (pumpkin_ai_goal_t *)(uintptr_t)goal_id;
  return goal->should_continue(server, entity);
}

void exports_plugin_handle_ai_goal_start(uint32_t goal_id,
                                         plugin_own_server_instance_t server,
                                         plugin_own_entity_t entity) {
  pumpkin_ai_goal_t *goal = (pumpkin_ai_goal_t *)(uintptr_t)goal_id;
  goal->start(server, entity);
}

void exports_plugin_handle_ai_goal_tick(uint32_t goal_id,
                                        plugin_own_server_instance_t server,
                                        plugin_own_entity_t entity) {
  pumpkin_ai_goal_t *goal = (pumpkin_ai_goal_t *)(uintptr_t)goal_id;
  goal->tick(server, entity);
}

void exports_plugin_handle_ai_goal_stop(uint32_t goal_id,
                                        plugin_own_server_instance_t server,
                                        plugin_own_entity_t entity) {
  pumpkin_ai_goal_t *goal = (pumpkin_ai_goal_t *)(uintptr_t)goal_id;
  goal->stop(server, entity);
}

void exports_plugin_handle_generate_phase(uint32_t generator_id,
                                          plugin_generation_phase_t phase,
                                          plugin_own_chunk_buffer_t chunk) {
  pumpkin_chunk_generator_t handler =
      (pumpkin_chunk_generator_t)(uintptr_t)generator_id;
  handler(phase, chunk);
}
