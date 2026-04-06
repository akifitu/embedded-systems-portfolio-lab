CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC := \
	src/sequencer_types.c \
	src/rail_plan.c \
	src/power_sequencer.c \
	src/main.c

TEST_SRC := \
	src/sequencer_types.c \
	src/rail_plan.c \
	src/power_sequencer.c \
	tests/test_sequencer.c

.PHONY: all test run clean

all: build/power_sequencer_demo

build/power_sequencer_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@

build/test_power_sequencer: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@

test: build/test_power_sequencer
	./build/test_power_sequencer

run: build/power_sequencer_demo
	./build/power_sequencer_demo

build:
	mkdir -p build

clean:
	rm -rf build
