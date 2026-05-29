BUILD_DIR ?= build
JOBS ?= 4
APP := $(BUILD_DIR)/npp-qt
SMOKE_TIMEOUT ?= 3

.PHONY: build test

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) -j$(JOBS)

test: build
	@test -x '$(APP)' || { echo "missing executable: $(APP)"; exit 1; }
	@echo "smoke: launching $(APP) ($(SMOKE_TIMEOUT)s timeout)..."
	@QT_QPA_PLATFORM=$${QT_QPA_PLATFORM:-offscreen} timeout $(SMOKE_TIMEOUT)s '$(APP)' >/dev/null 2>&1; \
	ec=$$?; \
	if [ $$ec -eq 124 ]; then \
		echo "smoke: ok (process ran until timeout)"; \
	elif [ $$ec -eq 0 ]; then \
		echo "smoke: ok (process exited cleanly)"; \
	else \
		echo "smoke: failed (exit $$ec)"; exit $$ec; \
	fi
