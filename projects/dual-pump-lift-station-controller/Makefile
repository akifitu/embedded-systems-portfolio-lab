CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/lift_station_types.c \
	src/level_policy.c \
	src/pump_arbiter.c \
	src/lift_station_controller.c \
	src/main.c

TEST_SRC := \
	src/lift_station_types.c \
	src/level_policy.c \
	src/pump_arbiter.c \
	src/lift_station_controller.c \
	tests/test_lift_station.c

.PHONY: all test run clean

all: build/lift_station_demo

build/lift_station_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_lift_station: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_lift_station
	./build/test_lift_station

run: build/lift_station_demo
	./build/lift_station_demo

build:
	mkdir -p build

clean:
	rm -rf build
