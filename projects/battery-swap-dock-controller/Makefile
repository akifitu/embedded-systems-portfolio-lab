CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/swap_dock_types.c \
	src/precharge_monitor.c \
	src/dock_policy.c \
	src/battery_swap_dock_controller.c \
	src/main.c

TEST_SRC := \
	src/swap_dock_types.c \
	src/precharge_monitor.c \
	src/dock_policy.c \
	src/battery_swap_dock_controller.c \
	tests/test_battery_swap_dock.c

.PHONY: all test run clean

all: build/battery_swap_dock_demo

build/battery_swap_dock_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_battery_swap_dock: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_battery_swap_dock
	./build/test_battery_swap_dock

run: build/battery_swap_dock_demo
	./build/battery_swap_dock_demo

build:
	mkdir -p build

clean:
	rm -rf build
