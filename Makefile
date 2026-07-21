WIT_BINDGEN ?= wit-bindgen
CC_WASI ?= wasm32-wasip2-clang
AR_LLVM ?= llvm-ar
INSTALL_PREFIX ?= install

CFLAGS ?= -Oz

WIT_FILES := $(wildcard wit/v0.1/*.wit)
HEADERS := pumpkin_api.h gen/plugin.h
OBJS := build/pumpkin_api.o build/plugin.o gen/plugin_component_type.o

.PHONY: all clean install

all: build/libpumpkin-api.a

gen: $(WIT_FILES)
	$(WIT_BINDGEN) c --out-dir gen wit/v0.1

gen/plugin.c: gen
gen/plugin.h: gen
gen/plugin_component_type.o: gen

build/pumpkin_api.o: pumpkin_api.c $(HEADERS) | build
	$(CC_WASI) $(CFLAGS) -I. -Igen -c $< -o $@
build/plugin.o: gen/plugin.c $(HEADERS) | build
	$(CC_WASI) $(CFLAGS) -I. -Igen -c $< -o $@

build/libpumpkin-api.a: $(OBJS) | build
	$(AR_LLVM) rcs $@ $^

build:
	mkdir build

install: build/libpumpkin-api.a $(HEADERS)
	mkdir -p $(INSTALL_PREFIX)/include $(INSTALL_PREFIX)/lib
	cp $(HEADERS) $(INSTALL_PREFIX)/include/
	cp build/libpumpkin-api.a $(INSTALL_PREFIX)/lib/

clean:
	rm -rf build gen
