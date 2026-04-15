# Pumpkin Plugin API for C

This package provides everything needed to write a Pumpkin server plugin compiled to WebAssembly using C or C++.

## Quick start

1. Clone this repository with submodules:

```bash
git clone --recursive <url-to-this-repo>
# Or if you already cloned it:
git submodule update --init --recursive
```

2. Create your plugin (`main.c`):

```c
#include "pumpkin_api.h"
#include <stdio.h>

pumpkin_metadata_t get_meta(void) {
    static const char* authors[] = {"you"};
    return (pumpkin_metadata_t) {
        .name = "my-c-plugin",
        .version = "0.1.0",
        .authors = authors,
        .authors_count = 1,
        .description = "A simple C plugin for Pumpkin",
        .dependencies_count = 0
    };
}

void on_load(plugin_own_context_t ctx) {
    printf("C plugin loaded!\n");
}

REGISTER_PUMPKIN_PLUGIN(((pumpkin_plugin_t){
    .get_metadata = get_meta,
    .on_load = on_load
}))
```

3. Build your plugin into a WebAssembly component:

To build for Pumpkin, you'll need the `wasi-sdk` and `wit-bindgen`.

```bash
# Compile to Wasm using wasi-sdk
/path/to/wasi-sdk/bin/clang -O3 \
    -Iinclude -Isrc/gen \
    src/gen/plugin.c src/pumpkin_api.c example/main.c \
    -o my_plugin.wasm \
    -mexec-model=reactor
```

