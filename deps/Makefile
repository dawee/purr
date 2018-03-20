all: v8-release include/v8

include/v8: include v8
	@cp -r v8/include include/v8

include:
	@mkdir include

v8-release: v8/Makefile
	@make -C v8 release

v8/Makefile: v8
	@cp v8.mk v8/Makefile

v8:
	@fetch v8

.PHONY: v8-release
