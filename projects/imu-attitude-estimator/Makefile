CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude
LDLIBS ?= -lm
BUILD_DIR := build
SRC := $(wildcard src/*.c)
APP := $(BUILD_DIR)/imu_attitude_demo
TEST := $(BUILD_DIR)/test_imu_attitude
APP_SRC := $(SRC)
LIB_SRC := $(filter-out src/main.c,$(SRC))

.PHONY: all test run clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(APP_SRC) $(LDLIBS) -o $(APP)

$(TEST): tests/test_imu.c $(LIB_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_imu.c $(LIB_SRC) $(LDLIBS) -o $(TEST)

test: $(TEST)
	./$(TEST)

run: $(APP)
	./$(APP)

clean:
	rm -rf $(BUILD_DIR)

