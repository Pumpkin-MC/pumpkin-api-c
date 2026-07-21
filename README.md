# Pumpkin Plugin API for C

This package provides everything needed to write a Pumpkin server plugin compiled to WebAssembly using C or C++.

## Quick start

1. Download the latest release of this package.
```bash
# Assuming the latest version is 0.1.0-dev1
curl -OL https://github.com/Pumpkin-MC/pumpkin-api-c/releases/download/v0.1.0-dev1/pumpkin-api-0.1.0-dev1.tar.xz
tar -xJvf pumpkin-api-0.1.0-dev1.tar.xz

# Move and/or name it somewhere/something predictable
mv pumpkin-api-0.1.0-dev1 pumpkin-api
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
        .dependencies_count = 0,
        .permissions_count = 0
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

To build for Pumpkin, you'll need the `wasi-sdk`.

```bash
# Compile to Wasm using wasi-sdk
/path/to/wasi-sdk/bin/wasm32-wasip2-clang -Oz \
    -Ipumpkin-api/include \
    pumpkin-api/lib/libpumpkin-api.a \
    main.c \
    -o my_plugin.wasm \
    -mexec-model=reactor
```
