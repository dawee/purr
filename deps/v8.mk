release:
	@gclient sync
	@./tools/dev/v8gen.py x64.release
	@ninja -C out.gn/x64.release

.PHONY: release
