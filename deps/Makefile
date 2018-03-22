all: release

release: v8-release sdl2-release filesystem-release

v8-release: v8/purr.v8.mk include/v8
	@make -C v8 -f purr.v8.mk release

sdl2-release: SDL2-2.0.8/purr.sdl2.mk include/SDL2
	@make -C SDL2-2.0.8 -f purr.sdl2.mk release

filesystem-release: include/filesystem

include/filesystem: include filesystem-1.0.0
	@cp -r filesystem-1.0.0/filesystem $@

include/v8: include v8
	@cp -r v8/include $@

include/SDL2: include SDL2-2.0.8
	@cp -r SDL2-2.0.8/include $@

include:
	@mkdir include

SDL2-2.0.8/purr.sdl2.mk: SDL2-2.0.8 sdl2.mk
	@cp sdl2.mk $@

v8/purr.v8.mk: v8 v8.mk
	@cp v8.mk $@

v8:
	@fetch v8

SDL2-2.0.8: SDL2-2.0.8.tar.gz
	@tar -xzf $<

SDL2-2.0.8.tar.gz:
	@curl -Ls https://www.libsdl.org/release/SDL2-2.0.8.tar.gz > $@

filesystem-1.0.0: filesystem-1.0.0.tar.gz
	@tar -xzf $<

filesystem-1.0.0.tar.gz:
	@curl -Ls https://github.com/dawee/filesystem/archive/v1.0.0.tar.gz > $@

.PHONY: v8-release
