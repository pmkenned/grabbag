CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS =
LDLIBS =

SRC_DIR = ./src
SRC = $(wildcard $(SRC_DIR)/*.c)

TARGET = a.out
BUILD_DIR = ./build
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)

DEBUG_TARGET = debug_$(TARGET)
DEBUG_DIR = $(BUILD_DIR)/debug
DEBUG_OBJ = $(SRC:$(SRC_DIR)/%.c=$(DEBUG_DIR)/%.o)
DEBUG_DEP = $(DEBUG_OBJ:%.o=%.d)

.PHONY: all run clean debug

all: $(BUILD_DIR)/$(TARGET)

run: $(BUILD_DIR)/$(TARGET)
	$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

debug: CPPFLAGS += -DDEBUG
debug: CFLAGS += -O0 -ggdb -fsanitize=address -fsanitize=undefined
debug: LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: $(DEBUG_DIR)/$(DEBUG_TARGET)

# ==== BUILD ====

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(TARGET): $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(DEP)

# ==== DEBUG ====

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(DEBUG_DIR)/$(DEBUG_TARGET): $(DEBUG_OBJ)
	mkdir -p $(DEBUG_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(DEBUG_DEP)
