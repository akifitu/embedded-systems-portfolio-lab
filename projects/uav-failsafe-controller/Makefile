CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/failsafe_types.c \
	src/geofence.c \
	src/reserve_model.c \
	src/uav_failsafe_controller.c \
	src/main.c

TEST_SRC := \
	src/failsafe_types.c \
	src/geofence.c \
	src/reserve_model.c \
	src/uav_failsafe_controller.c \
	tests/test_uav_failsafe.c

.PHONY: all test run clean

all: build/uav_failsafe_demo

build/uav_failsafe_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_uav_failsafe: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_uav_failsafe
	./build/test_uav_failsafe

run: build/uav_failsafe_demo
	./build/uav_failsafe_demo

build:
	mkdir -p build

clean:
	rm -rf build
