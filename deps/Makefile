all: v8-release sdl2-release

release: v8-release sdl2-release

v8-release: v8/purr.v8.mk include/v8
	@make -C v8 -f purr.v8.mk release

sdl2-release: SDL2-2.0.8/purr.sdl2.mk include/SDL2
	@make -C SDL2-2.0.8 -f purr.sdl2.mk release

include/v8: include v8
	@cp -r v8/include $@

include/SDL2: include SDL2-2.0.8
	@cp -r SDL2-2.0.8/include $@

include:
	@mkdir include

SDL2-2.0.8/purr.sdl2.mk: v8 sdl2.mk
	@cp sdl2.mk $@

v8/purr.v8.mk: v8 v8.mk
	@cp v8.mk $@

v8:
	@fetch v8

SDL2-2.0.8: SDL2-2.0.8.tar.gz
	@tar -xzf $<

SDL2-2.0.8.tar.gz:
	@curl -Ls https://www.libsdl.org/release/SDL2-2.0.8.tar.gz > $@

.PHONY: v8-release
