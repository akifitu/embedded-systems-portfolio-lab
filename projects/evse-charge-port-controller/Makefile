CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/evse_types.c \
	src/pilot_decoder.c \
	src/current_policy.c \
	src/evse_controller.c \
	src/main.c

TEST_SRC := \
	src/evse_types.c \
	src/pilot_decoder.c \
	src/current_policy.c \
	src/evse_controller.c \
	tests/test_evse.c

.PHONY: all test run clean

all: build/evse_demo

build/evse_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_evse: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_evse
	./build/test_evse

run: build/evse_demo
	./build/evse_demo

build:
	mkdir -p build

clean:
	rm -rf build
