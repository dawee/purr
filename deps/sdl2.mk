release: Makefile
	@make

Makefile:
	@PATH=../SDL2-2.0.8:${PATH} ./configure
