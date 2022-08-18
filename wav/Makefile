.PHONY: all run clean

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -Werror -std=c99 -g
LDFLAGS = 
LDLIBS = -lm

TARGET = wav

SRC_DIR = ./src
BUILD_DIR = ./build

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)

all: $(BUILD_DIR)/$(TARGET)

ARGS ?=
#ARGS ?= ./data/*.wav
run: $(BUILD_DIR)/$(TARGET)
	$(BUILD_DIR)/$(TARGET) $(ARGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) -c $<

$(BUILD_DIR)/$(TARGET): $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEP)
