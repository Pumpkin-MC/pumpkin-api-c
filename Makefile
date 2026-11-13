WIT_BINDGEN ?= wit-bindgen
CC_WASI ?= wasm32-wasip3-clang
CXX_WASI ?= wasm32-wasip3-clang++
AR_LLVM ?= llvm-ar
INSTALL_PREFIX ?= install

CFLAGS ?= -Oz
CXXFLAGS ?= -Oz -std=c++23 -fbracket-depth=1024 -fno-exceptions

WIT_FILES := $(wildcard wit/v0.1/*.wit)
C_HEADERS := pumpkin_api.h gen/c/plugin.h
CPP_HEADERS := pumpkin_api.hpp gen/cpp/plugin_cpp.h gen/cpp/wit.h
HEADERS := $(C_HEADERS) $(CPP_HEADERS)

C_OBJS := build/pumpkin_api.o build/plugin.o gen/c/plugin_component_type.o
CPP_OBJS := build/pumpkin_api_cpp.o build/plugin_cpp.o gen/cpp/plugin_component_type.o

.PHONY: all clean install gen gen-c gen-cpp

all: build/libpumpkin-api.a build/libpumpkin-api-cpp.a

gen: gen-c gen-cpp

gen-c: $(WIT_FILES)
	$(WIT_BINDGEN) c --out-dir gen/c wit/v0.1

gen-cpp: $(WIT_FILES)
	$(WIT_BINDGEN) cpp --out-dir gen/cpp wit/v0.1

gen/c/plugin.c: gen-c
gen/c/plugin.h: gen-c
gen/c/plugin_component_type.o: gen-c

gen/cpp/plugin.cpp: gen-cpp
gen/cpp/plugin_cpp.h: gen-cpp
gen/cpp/wit.h: gen-cpp
gen/cpp/plugin_component_type.o: gen-cpp

build/pumpkin_api.o: pumpkin_api.c $(C_HEADERS) | build
	$(CC_WASI) $(CFLAGS) -I. -Igen/c -c $< -o $@
build/plugin.o: gen/c/plugin.c $(C_HEADERS) | build
	$(CC_WASI) $(CFLAGS) -I. -Igen/c -c $< -o $@

build/pumpkin_api_cpp.o: pumpkin_api.cpp $(CPP_HEADERS) | build
	$(CXX_WASI) $(CXXFLAGS) -I. -Igen/cpp -c $< -o $@
build/plugin_cpp.o: gen/cpp/plugin.cpp $(CPP_HEADERS) | build
	$(CXX_WASI) $(CXXFLAGS) -I. -Igen/cpp -c $< -o $@

build/libpumpkin-api.a: $(C_OBJS) | build
	$(AR_LLVM) rcs $@ $^

build/libpumpkin-api-cpp.a: $(CPP_OBJS) | build
	$(AR_LLVM) rcs $@ $^

build:
	mkdir -p build

install: build/libpumpkin-api.a build/libpumpkin-api-cpp.a $(HEADERS)
	mkdir -p $(INSTALL_PREFIX)/include $(INSTALL_PREFIX)/lib
	cp $(HEADERS) $(INSTALL_PREFIX)/include/
	cp build/libpumpkin-api.a build/libpumpkin-api-cpp.a $(INSTALL_PREFIX)/lib/

clean:
	rm -rf build gen
