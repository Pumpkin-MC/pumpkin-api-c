#include "pumpkin_api.hpp"
#include <utility>

static ::pumpkin::Plugin g_plugin = {};

namespace pumpkin {

void register_plugin(Plugin plugin) { g_plugin = plugin; }

} // namespace pumpkin

static wit::vector<wit::string>
to_wit_strings(std::span<const std::string_view> items) {
  auto vec = wit::vector<wit::string>::allocate(items.size());
  for (size_t i = 0; i < items.size(); ++i) {
    vec.initialize(i, wit::string::from_view(items[i]));
  }
  return vec;
}

// WIT exports

exports::pumpkin::plugin::metadata::PluginMetadata
exports::pumpkin::plugin::metadata::GetMetadata() {
  ::pumpkin::Metadata meta =
      g_plugin.get_metadata ? g_plugin.get_metadata() : ::pumpkin::Metadata{};
  return exports::pumpkin::plugin::metadata::PluginMetadata{
      .name = wit::string::from_view(meta.name),
      .version = wit::string::from_view(meta.version),
      .authors = to_wit_strings(meta.authors),
      .description = wit::string::from_view(meta.description),
      .dependencies = to_wit_strings(meta.dependencies),
      .permissions = to_wit_strings(meta.permissions),
  };
}

std::expected<void, wit::string>
exports::plugin::OnLoad(::pumpkin::plugin::context::Context &&context) {
  if (g_plugin.on_load) {
    g_plugin.on_load(std::move(context));
  }
  return {};
}

std::expected<void, wit::string>
exports::plugin::OnUnload(::pumpkin::plugin::context::Context &&context) {
  if (g_plugin.on_unload) {
    g_plugin.on_unload(std::move(context));
  }
  return {};
}

::pumpkin::plugin::event::Event
exports::plugin::HandleEvent(uint32_t event_id,
                             ::pumpkin::plugin::server::Server &&server,
                             ::pumpkin::plugin::event::Event event) {
  auto handler = reinterpret_cast<::pumpkin::EventHandlerFn>(
      static_cast<uintptr_t>(event_id));
  handler(std::move(server), event);
  return event;
}

std::expected<int32_t, ::pumpkin::plugin::command::CommandError>
exports::plugin::HandleCommand(
    uint32_t command_id, ::pumpkin::plugin::command::CommandSender &&sender,
    ::pumpkin::plugin::server::Server &&server,
    ::pumpkin::plugin::command::ConsumedArgs &&args) {
  auto handler = reinterpret_cast<::pumpkin::CommandHandlerFn>(
      static_cast<uintptr_t>(command_id));
  return handler(std::move(sender), std::move(server), std::move(args));
}

::pumpkin::plugin::command::CommandSuggestions
exports::plugin::HandleCommandSuggestion(
    uint32_t handler_id, ::pumpkin::plugin::command::CommandSender &&sender,
    ::pumpkin::plugin::server::Server &&server,
    ::pumpkin::plugin::command::SuggestionRequest request) {
  auto handler = reinterpret_cast<::pumpkin::CommandSuggestionHandlerFn>(
      static_cast<uintptr_t>(handler_id));
  return handler(std::move(sender), std::move(server), request);
}

void exports::plugin::HandleTask(uint32_t handler_id,
                                 ::pumpkin::plugin::server::Server &&server) {
  auto handler = reinterpret_cast<::pumpkin::TaskHandlerFn>(
      static_cast<uintptr_t>(handler_id));
  handler(std::move(server));
}

std::expected<wit::vector<uint8_t>, wit::string>
exports::plugin::HandleIpcMessage(wit::string sender,
                                  wit::vector<uint8_t> message) {
  if (g_plugin.handle_ipc_message) {
    return g_plugin.handle_ipc_message(sender.get_view(),
                                       message.get_const_view());
  }
  return std::unexpected(
      wit::string::from_view("This plugin cannot receive messages"));
}

bool exports::plugin::HandleAiGoalCanStart(
    uint32_t goal_id, ::pumpkin::plugin::server::Server &&server,
    ::pumpkin::plugin::world::Entity &&entity) {
  auto *goal =
      reinterpret_cast<::pumpkin::AiGoal *>(static_cast<uintptr_t>(goal_id));
  return goal->can_start(std::move(server), std::move(entity));
}

bool exports::plugin::HandleAiGoalShouldContinue(
    uint32_t goal_id, ::pumpkin::plugin::server::Server &&server,
    ::pumpkin::plugin::world::Entity &&entity) {
  auto *goal =
      reinterpret_cast<::pumpkin::AiGoal *>(static_cast<uintptr_t>(goal_id));
  return goal->should_continue(std::move(server), std::move(entity));
}

void exports::plugin::HandleAiGoalStart(
    uint32_t goal_id, ::pumpkin::plugin::server::Server &&server,
    ::pumpkin::plugin::world::Entity &&entity) {
  auto *goal =
      reinterpret_cast<::pumpkin::AiGoal *>(static_cast<uintptr_t>(goal_id));
  goal->start(std::move(server), std::move(entity));
}

void exports::plugin::HandleAiGoalTick(
    uint32_t goal_id, ::pumpkin::plugin::server::Server &&server,
    ::pumpkin::plugin::world::Entity &&entity) {
  auto *goal =
      reinterpret_cast<::pumpkin::AiGoal *>(static_cast<uintptr_t>(goal_id));
  goal->tick(std::move(server), std::move(entity));
}

void exports::plugin::HandleAiGoalStop(
    uint32_t goal_id, ::pumpkin::plugin::server::Server &&server,
    ::pumpkin::plugin::world::Entity &&entity) {
  auto *goal =
      reinterpret_cast<::pumpkin::AiGoal *>(static_cast<uintptr_t>(goal_id));
  goal->stop(std::move(server), std::move(entity));
}

void exports::plugin::HandleGeneratePhase(
    uint32_t generator_id, ::pumpkin::plugin::world::GenerationPhase phase,
    ::pumpkin::plugin::world::ChunkBuffer &&chunk) {
  auto handler = reinterpret_cast<::pumpkin::ChunkGeneratorFn>(
      static_cast<uintptr_t>(generator_id));
  handler(phase, std::move(chunk));
}
