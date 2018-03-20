CXX := g++
CXX_FLAGS := -I./deps/include -I./deps/include/v8 --std=c++11 -pthread
V8_ARCHIVES = \
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

all: dist/purr

re: clean all

dist/purr: src/purr.cc dist deps
	@${CXX} ${CXX_FLAGS} ${V8_ARCHIVES} src/purr.cc -o $@

dist:
	@mkdir dist

clean:
	@rm -rf dist

deps:
	@make -C deps all

.PHONY: deps
