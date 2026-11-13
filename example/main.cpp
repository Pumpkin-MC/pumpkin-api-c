#include "pumpkin_api.hpp"
#include <cstdio>

pumpkin::Metadata get_meta() {
    static constexpr std::string_view authors[] = {"you"};
    return {
        .name = "my-cpp-plugin",
        .version = "0.1.0",
        .authors = authors,
        .description = "A simple C++ plugin for Pumpkin",
    };
}

void on_load(pumpkin::plugin::context::Context ctx) {
    std::printf("C++ plugin loaded!\n");
}

REGISTER_PUMPKIN_PLUGIN({
    .get_metadata = get_meta,
    .on_load = on_load,
})
