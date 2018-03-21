CXX := g++
CXX_FLAGS := -I./deps/include -I./deps/include/v8 -I./deps/include/SDL2 --std=c++11 -pthread -ldl
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

dist/purr: src/purr.cc dist deps
	@${CXX} ${V8_OBJECTS} ${SDL2_OBJECTS} src/purr.cc -o $@ ${CXX_FLAGS}

dist:
	@mkdir dist

clean:
	@rm -rf dist

deps:
	@make -C deps all

.PHONY: deps
