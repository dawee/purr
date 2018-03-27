UNAME := $(shell uname)
INCLUDES := -I./deps/include -I./deps/include/v8 -I.
SOURCES := $(wildcard src/*.cc)
OBJECTS := $(patsubst src/%.cc,build/%.o,$(SOURCES))

ifeq ($(UNAME), Linux)
	CXX := g++
	CXX_FLAGS := ${INCLUDES} --std=c++11 -pthread
	LDFLAGS := -ldl
endif

ifeq ($(UNAME), Darwin)
	CXX := clang++
	CXX_FLAGS := ${INCLUDES} --std=c++11
	LDFLAGS :=  -liconv -lobjc \
		-framework OpenGL \
		-framework ForceFeedback \
		-framework Cocoa \
		-framework Carbon \
		-framework IOKit \
		-framework CoreAudio \
		-framework CoreVideo \
		-framework Metal \
		-framework AudioToolbox \
		-framework AudioUnit
endif

V8_OBJECTS = \
	$(wildcard deps/v8/out.gn/x64.release/obj/v8_libbase/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/v8_initializers/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/third_party/icu/icui18n/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/third_party/icu/icuuc/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/v8_nosnapshot/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/v8_libplatform/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/v8_init/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/v8_base/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/v8_libsampler/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/buildtools/third_party/libc++abi/libc++abi/*.o) \
	$(wildcard deps/v8/out.gn/x64.release/obj/buildtools/third_party/libc++/libc++/*.o)

SDL2_OBJECTS = $(wildcard deps/SDL2-2.0.8/build/*.o)

all: deps dist/purr

re: clean all

dist/purr: ${OBJECTS}
	@mkdir -p dist
	@echo $@
	@${CXX} ${V8_OBJECTS} ${SDL2_OBJECTS} ${OBJECTS} -o $@ ${CXX_FLAGS} ${LDFLAGS}

build/%.o: src/%.cc
	@mkdir -p build
	@echo $@
	@${CXX} -c $< -o $@ ${CXX_FLAGS}

clean:
	@rm -rf build dist

deps:
	@make -C deps all

.PHONY: deps
