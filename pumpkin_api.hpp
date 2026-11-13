#ifndef PUMPKIN_API_HPP
#define PUMPKIN_API_HPP

#include "plugin_cpp.h"
#include <cstdint>
#include <expected>
#include <span>
#include <string_view>

namespace pumpkin {

struct Metadata {
  std::string_view name;
  std::string_view version;

  std::span<const std::string_view> authors = {};

  std::string_view description = {};

  std::span<const std::string_view> dependencies = {};
  std::span<const std::string_view> permissions = {};
};

using OnLoadFn = void (*)(::pumpkin::plugin::context::Context ctx);
using OnUnloadFn = void (*)(::pumpkin::plugin::context::Context ctx);
using IpcMessageHandlerFn =
    std::expected<wit::vector<uint8_t>, wit::string> (*)(
        std::string_view sender, std::span<const uint8_t> message);
using GetMetadataFn = Metadata (*)();

using EventHandlerFn = void (*)(::pumpkin::plugin::server::Server server,
                                ::pumpkin::plugin::event::Event &event);
using CommandHandlerFn =
    std::expected<int32_t, ::pumpkin::plugin::command::CommandError> (*)(
        ::pumpkin::plugin::command::CommandSender sender,
        ::pumpkin::plugin::server::Server server,
        ::pumpkin::plugin::command::ConsumedArgs args);
using CommandSuggestionHandlerFn =
    ::pumpkin::plugin::command::CommandSuggestions (*)(
        ::pumpkin::plugin::command::CommandSender sender,
        ::pumpkin::plugin::server::Server server,
        const ::pumpkin::plugin::command::SuggestionRequest &request);
using TaskHandlerFn = void (*)(::pumpkin::plugin::server::Server server);
using ChunkGeneratorFn =
    void (*)(::pumpkin::plugin::world::GenerationPhase phase,
             ::pumpkin::plugin::world::ChunkBuffer chunk);
using AiGoalPredicateFn = bool (*)(::pumpkin::plugin::server::Server server,
                                   ::pumpkin::plugin::world::Entity entity);
using AiGoalActionFn = void (*)(::pumpkin::plugin::server::Server server,
                                ::pumpkin::plugin::world::Entity entity);

struct AiGoal {
  AiGoalPredicateFn can_start = nullptr;
  AiGoalPredicateFn should_continue = nullptr;
  AiGoalActionFn start = nullptr;
  AiGoalActionFn tick = nullptr;
  AiGoalActionFn stop = nullptr;
};

struct Plugin {
  GetMetadataFn get_metadata = nullptr;
  OnLoadFn on_load = nullptr;
  OnUnloadFn on_unload = nullptr;
  IpcMessageHandlerFn handle_ipc_message = nullptr;
};

void register_plugin(Plugin plugin);

inline void register_event(
    const ::pumpkin::plugin::context::Context &ctx, EventHandlerFn handler,
    ::pumpkin::plugin::event::EventType event_type,
    ::pumpkin::plugin::event::EventPriority event_priority, bool blocking) {
  ctx.RegisterEvent(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handler)),
                    event_type, event_priority, blocking);
}

inline uint32_t schedule_delayed_task(TaskHandlerFn handler,
                                      uint64_t delay_ticks) {
  return ::pumpkin::plugin::scheduler::ScheduleDelayedTask(
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handler)), delay_ticks);
}

inline uint32_t schedule_repeating_task(TaskHandlerFn handler,
                                        uint64_t delay_ticks,
                                        uint64_t period_ticks) {
  return ::pumpkin::plugin::scheduler::ScheduleRepeatingTask(
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handler)), delay_ticks,
      period_ticks);
}

inline void cancel_task(uint32_t task_id) {
  ::pumpkin::plugin::scheduler::CancelTask(task_id);
}

inline void
execute_with_handler(const ::pumpkin::plugin::command::CommandNode &node,
                     CommandHandlerFn handler) {
  node.ExecuteWithHandlerId(
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handler)));
}

inline void execute_with_handler(const ::pumpkin::plugin::command::Command &cmd,
                                 CommandHandlerFn handler) {
  cmd.ExecuteWithHandlerId(
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handler)));
}

inline void
suggest_with_handler(const ::pumpkin::plugin::command::CommandNode &node,
                     CommandSuggestionHandlerFn handler) {
  node.SuggestWithHandlerId(
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handler)));
}

inline void set_chunk_generator(const ::pumpkin::plugin::world::World &world,
                                ChunkGeneratorFn handler) {
  world.SetChunkGenerator(
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handler)));
}

inline void add_custom_ai_goal(const ::pumpkin::plugin::world::Mob &mob,
                               uint8_t priority, AiGoal *goal) {
  mob.AddCustomAiGoal(priority,
                      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(goal)));
}

} // namespace pumpkin

#define REGISTER_PUMPKIN_PLUGIN(...)                                           \
  void exports::plugin::InitPlugin() {                                         \
    ::pumpkin::register_plugin(__VA_ARGS__);                                   \
  }

#endif
