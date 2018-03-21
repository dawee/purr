release: .sync
	@./tools/dev/v8gen.py x64.release
	@ninja -C out.gn/x64.release

.sync:
	@gclient sync
	@touch $@

.PHONY: release
