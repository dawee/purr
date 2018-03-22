UNAME := $(shell uname)

INCLUDES = -I./deps/include -I./deps/include/v8 -I.

ifeq ($(UNAME), Linux)
	CXX := g++
	CXX_FLAGS := ${INCLUDES} --std=c++11 -pthread -ldl
endif

ifeq ($(UNAME), Darwin)
	CXX := clang++
	CXX_FLAGS := ${INCLUDES} --std=c++11 -pthread -liconv -lobjc -framework OpenGL -framework ForceFeedback -lobjc -framework Cocoa -framework Carbon -framework IOKit -framework CoreAudio -framework CoreVideo -framework Metal -framework AudioToolbox -framework AudioUnit
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

all: dist/purr

re: clean all

dist/purr: build/module.o build/project.o build/purr.o
	@${CXX} ${V8_OBJECTS} ${SDL2_OBJECTS} build/module.o build/project.o build/purr.o -o $@ ${CXX_FLAGS}

build/%.o: src/%.cc build
	@${CXX} ${V8_OBJECTS} ${SDL2_OBJECTS} -c $< -o $@ ${CXX_FLAGS}

build:
	@mkdir build

dist:
	@mkdir dist

clean:
	@rm -rf dist

deps:
	@make -C deps all

.PHONY: deps
