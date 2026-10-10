WIT_BINDGEN ?= wit-bindgen
CC_WASI ?= wasm32-wasip2-clang
AR_LLVM ?= llvm-ar
INSTALL_PREFIX ?= install
CFLAGS ?= -Oz
PLUGIN_SOURCE ?= example/main.c
BUILD_DIR ?= build

WIT_FILES := $(wildcard wit/v0.1/*.wit)
HEADERS := pumpkin_api.h gen/plugin.h
OBJS := $(BUILD_DIR)/pumpkin_api.o $(BUILD_DIR)/plugin.o gen/plugin_component_type.o

# Pumpkin release 0.1 and 0.2 each embed a different pumpkin-plugin-wit snapshot.
# Override these when targeting a different server build.
PUMPKIN_01_WIT_REV ?= 1ad73fff
PUMPKIN_02_WIT_REV ?= 4d7a501

.PHONY: all bindings library plugin plugins plugin-pumpkin-0.1 plugin-pumpkin-0.2 install clean

all: library

bindings: gen/plugin.c gen/plugin.h gen/plugin_component_type.o

gen/plugin.c gen/plugin.h gen/plugin_component_type.o &: $(WIT_FILES)
	mkdir -p gen
	$(WIT_BINDGEN) c --out-dir gen wit/v0.1

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/pumpkin_api.o: pumpkin_api.c pumpkin_api.h gen/plugin.h | $(BUILD_DIR)
	$(CC_WASI) $(CFLAGS) -I. -Igen -c $< -o $@

$(BUILD_DIR)/plugin.o: gen/plugin.c pumpkin_api.h gen/plugin.h | $(BUILD_DIR)
	$(CC_WASI) $(CFLAGS) -I. -Igen -c $< -o $@

library: $(BUILD_DIR)/libpumpkin-api.a

$(BUILD_DIR)/libpumpkin-api.a: $(BUILD_DIR)/pumpkin_api.o $(BUILD_DIR)/plugin.o gen/plugin_component_type.o
	$(AR_LLVM) rcs $@ $^

plugin: plugins
plugins: plugin-pumpkin-0.1 plugin-pumpkin-0.2
plugin-pumpkin-0.1: $(BUILD_DIR)/my_plugin-pumpkin-0.1.wasm
plugin-pumpkin-0.2: $(BUILD_DIR)/my_plugin-pumpkin-0.2.wasm

$(BUILD_DIR)/compat/0.1/wit/.ready:
	mkdir -p $(BUILD_DIR)/compat/0.1/wit
	git -C wit archive $(PUMPKIN_01_WIT_REV) v0.1 | tar -x -C $(BUILD_DIR)/compat/0.1/wit
	touch $@

$(BUILD_DIR)/compat/0.2/wit/.ready:
	mkdir -p $(BUILD_DIR)/compat/0.2/wit
	git -C wit archive $(PUMPKIN_02_WIT_REV) v0.1 | tar -x -C $(BUILD_DIR)/compat/0.2/wit
	touch $@

$(BUILD_DIR)/compat/0.1/gen/plugin.c $(BUILD_DIR)/compat/0.1/gen/plugin.h $(BUILD_DIR)/compat/0.1/gen/plugin_component_type.o &: $(BUILD_DIR)/compat/0.1/wit/.ready
	mkdir -p $(BUILD_DIR)/compat/0.1/gen
	$(WIT_BINDGEN) c --out-dir $(BUILD_DIR)/compat/0.1/gen $(BUILD_DIR)/compat/0.1/wit/v0.1

$(BUILD_DIR)/compat/0.2/gen/plugin.c $(BUILD_DIR)/compat/0.2/gen/plugin.h $(BUILD_DIR)/compat/0.2/gen/plugin_component_type.o &: $(BUILD_DIR)/compat/0.2/wit/.ready
	mkdir -p $(BUILD_DIR)/compat/0.2/gen
	$(WIT_BINDGEN) c --out-dir $(BUILD_DIR)/compat/0.2/gen $(BUILD_DIR)/compat/0.2/wit/v0.1

$(BUILD_DIR)/my_plugin-pumpkin-0.1.wasm: $(PLUGIN_SOURCE) pumpkin_api.c pumpkin_api.h $(BUILD_DIR)/compat/0.1/gen/plugin.c $(BUILD_DIR)/compat/0.1/gen/plugin.h $(BUILD_DIR)/compat/0.1/gen/plugin_component_type.o | $(BUILD_DIR)
	$(CC_WASI) $(CFLAGS) -I. -I$(BUILD_DIR)/compat/0.1/gen $(BUILD_DIR)/compat/0.1/gen/plugin.c $(BUILD_DIR)/compat/0.1/gen/plugin_component_type.o pumpkin_api.c $(PLUGIN_SOURCE) -o $@ -mexec-model=reactor

$(BUILD_DIR)/my_plugin-pumpkin-0.2.wasm: $(PLUGIN_SOURCE) pumpkin_api.c pumpkin_api.h $(BUILD_DIR)/compat/0.2/gen/plugin.c $(BUILD_DIR)/compat/0.2/gen/plugin.h $(BUILD_DIR)/compat/0.2/gen/plugin_component_type.o | $(BUILD_DIR)
	$(CC_WASI) $(CFLAGS) -I. -I$(BUILD_DIR)/compat/0.2/gen $(BUILD_DIR)/compat/0.2/gen/plugin.c $(BUILD_DIR)/compat/0.2/gen/plugin_component_type.o pumpkin_api.c $(PLUGIN_SOURCE) -o $@ -mexec-model=reactor

install: $(BUILD_DIR)/libpumpkin-api.a $(HEADERS)
	mkdir -p $(INSTALL_PREFIX)/include $(INSTALL_PREFIX)/lib
	cp $(HEADERS) $(INSTALL_PREFIX)/include/
	cp $(BUILD_DIR)/libpumpkin-api.a $(INSTALL_PREFIX)/lib/

clean:
	rm -rf $(BUILD_DIR) gen
