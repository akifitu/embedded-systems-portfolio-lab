CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/pump_types.c \
	src/safety_monitor.c \
	src/dose_engine.c \
	src/infusion_pump_controller.c \
	src/main.c

TEST_SRC := \
	src/pump_types.c \
	src/safety_monitor.c \
	src/dose_engine.c \
	src/infusion_pump_controller.c \
	tests/test_infusion_pump.c

.PHONY: all test run clean

all: build/infusion_pump_demo

build/infusion_pump_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_infusion_pump: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_infusion_pump
	./build/test_infusion_pump

run: build/infusion_pump_demo
	./build/infusion_pump_demo

build:
	mkdir -p build

clean:
	rm -rf build
