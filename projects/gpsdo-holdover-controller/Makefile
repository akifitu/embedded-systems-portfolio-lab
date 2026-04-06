CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude
LDLIBS ?= -lm

SRC := \
	src/gpsdo_types.c \
	src/drift_model.c \
	src/gpsdo_controller.c \
	src/main.c

TEST_SRC := \
	src/gpsdo_types.c \
	src/drift_model.c \
	src/gpsdo_controller.c \
	tests/test_gpsdo.c

.PHONY: all test run clean

all: build/gpsdo_demo

build/gpsdo_demo: $(SRC) | build
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

build/test_gpsdo: $(TEST_SRC) | build
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@ $(LDLIBS)

test: build/test_gpsdo
	./build/test_gpsdo

run: build/gpsdo_demo
	./build/gpsdo_demo

build:
	mkdir -p build

clean:
	rm -rf build
