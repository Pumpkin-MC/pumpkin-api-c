#ifndef PUMPKIN_API_H
#define PUMPKIN_API_H

#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const char *name;
  const char *version;

  const char **authors;
  size_t authors_count;

  const char *description;

  const char **dependencies;
  size_t dependencies_count;

  const char **permissions;
  size_t permissions_count;
} pumpkin_metadata_t;

typedef void (*pumpkin_on_load_t)(plugin_own_context_t ctx);
typedef void (*pumpkin_on_unload_t)(plugin_own_context_t ctx);
typedef bool (*pumpkin_ipc_message_handler_t)(plugin_plugin_id_t *sender,
                                              plugin_ipc_message_t *message,
                                              plugin_ipc_message_t *ret,
                                              plugin_string_t *err);
typedef pumpkin_metadata_t (*pumpkin_get_metadata_t)(void);

typedef void (*pumpkin_event_handler_t)(plugin_own_server_instance_t server,
                                        plugin_event_t *event);
typedef bool (*pumpkin_command_handler_t)(plugin_own_command_sender_t sender,
                                          plugin_own_server_instance_t server,
                                          plugin_own_consumed_args_t args,
                                          int32_t *ret,
                                          plugin_command_error_t *err);
typedef void (*pumpkin_command_suggestion_handler_t)(
    plugin_own_command_sender_t sender, plugin_own_server_instance_t server,
    plugin_suggestion_request_t *request, plugin_command_suggestions_t *ret);
typedef void (*pumpkin_task_handler_t)(plugin_own_server_instance_t server);
typedef void (*pumpkin_chunk_generator_t)(plugin_generation_phase_t phase,
                                          plugin_own_chunk_buffer_t chunk);
typedef bool (*pumpkin_ai_goal_predicate_t)(plugin_own_server_instance_t server,
                                            plugin_own_entity_t entity);
typedef void (*pumpkin_ai_goal_action_t)(plugin_own_server_instance_t server,
                                         plugin_own_entity_t entity);

typedef struct {
  pumpkin_ai_goal_predicate_t can_start;
  pumpkin_ai_goal_predicate_t should_continue;
  pumpkin_ai_goal_action_t start;
  pumpkin_ai_goal_action_t tick;
  pumpkin_ai_goal_action_t stop;
} pumpkin_ai_goal_t;

typedef struct {
  pumpkin_get_metadata_t get_metadata;
  pumpkin_on_load_t on_load;
  pumpkin_on_unload_t on_unload;
  pumpkin_ipc_message_handler_t handle_ipc_message;
} pumpkin_plugin_t;

void pumpkin_register_plugin(pumpkin_plugin_t plugin);

#define REGISTER_PUMPKIN_PLUGIN(plugin)                                        \
  void exports_plugin_init_plugin(void) { pumpkin_register_plugin(plugin); }

static inline void pumpkin_context_register_event(
    pumpkin_plugin_context_borrow_context_t self,
    pumpkin_event_handler_t handler,
    pumpkin_plugin_context_event_type_t event_type,
    pumpkin_plugin_context_event_priority_t event_priority, bool blocking) {
  pumpkin_plugin_context_method_context_register_event(
      self, (uint32_t)(uintptr_t)handler, event_type, event_priority, blocking);
}

static inline uint32_t
pumpkin_scheduler_schedule_delayed_task(pumpkin_task_handler_t handler,
                                        uint64_t delay_ticks) {
  return pumpkin_plugin_scheduler_schedule_delayed_task(
      (uint32_t)(uintptr_t)handler, delay_ticks);
}

static inline uint32_t
pumpkin_scheduler_schedule_repeating_task(pumpkin_task_handler_t handler,
                                          uint64_t delay_ticks,
                                          uint64_t period_ticks) {
  return pumpkin_plugin_scheduler_schedule_repeating_task(
      (uint32_t)(uintptr_t)handler, delay_ticks, period_ticks);
}

static inline void pumpkin_scheduler_cancel_task(uint32_t task_id) {
  pumpkin_plugin_scheduler_cancel_task(task_id);
}

static inline void pumpkin_command_node_execute_with_handler(
    pumpkin_plugin_command_borrow_command_node_t self,
    pumpkin_command_handler_t handler) {
  pumpkin_plugin_command_method_command_node_execute_with_handler_id(
      self, (uint32_t)(uintptr_t)handler);
}

static inline void pumpkin_command_node_suggest_with_handler(
    pumpkin_plugin_command_borrow_command_node_t self,
    pumpkin_command_suggestion_handler_t handler) {
  pumpkin_plugin_command_method_command_node_suggest_with_handler_id(
      self, (uint32_t)(uintptr_t)handler);
}

static inline void
pumpkin_world_set_chunk_generator(pumpkin_plugin_world_borrow_world_t self,
                                  pumpkin_chunk_generator_t handler) {
  pumpkin_plugin_world_method_world_set_chunk_generator(
      self, (uint32_t)(uintptr_t)handler);
}

static inline void
pumpkin_mob_add_custom_ai_goal(pumpkin_plugin_world_borrow_mob_t self,
                               uint8_t priority, pumpkin_ai_goal_t *goal) {
  pumpkin_plugin_world_method_mob_add_custom_ai_goal(self, priority,
                                                     (uint32_t)(uintptr_t)goal);
}

#ifdef __cplusplus
}
#endif

#endif
