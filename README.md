# Pumpkin Plugin API for C

This package provides everything needed to write a Pumpkin server plugin compiled to WebAssembly using C or C++.

## Quick start

1. Download the latest release of this package.

You'll want to download the latest version of the bindings from the [CI release](https://github.com/Pumpkin-MC/pumpkin-api-c/releases/tag/CI).

We don't have stable versions as Pumpkin and its API is still pre-release.

```bash
# Assuming the latest commit is 2428b8a. Skip this if you downloaded manually
curl -OL https://github.com/Pumpkin-MC/pumpkin-api-c/releases/download/CI/pumpkin-api-c-2428b8a.tar.xz
tar -xJvf pumpkin-api-c-2428b8a.tar.xz

# Move and/or name it somewhere/something predictable
mv pumpkin-api-c-2428b8a pumpkin-api
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

### Build a plugin for a Pumpkin release

Pumpkin `0.1.0-dev+26.2-26.45` and `0.2.0+26.3-26.51` use different snapshots of the plugin WIT API. Generate the bindings from the matching snapshot when building a plugin; using the latest WIT can produce a component that fails Pumpkin's `handle-event` type check.

With `wasi-sdk` and `wit-bindgen` available, build both variants with:

```bash
make plugins
```

This creates `build/my_plugin-pumpkin-0.1.wasm` and `build/my_plugin-pumpkin-0.2.wasm`. To build one variant, use `make plugin-pumpkin-0.1` or `make plugin-pumpkin-0.2`. The source defaults to `example/main.c`; override it with `PLUGIN_SOURCE=path/to/plugin.c`. The WIT revisions can also be overridden with `PUMPKIN_01_WIT_REV` and `PUMPKIN_02_WIT_REV`.
