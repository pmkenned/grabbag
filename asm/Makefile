.PHONY: all

EXE=foo
BUILD_DIR=./build

SRC=$(wildcard *.s)
OBJ=$(SRC:%.s=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(EXE)

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(BUILD_DIR)
	as -o $@ $<

$(BUILD_DIR)/$(EXE): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	ld -o $@ $^

run: $(BUILD_DIR)/$(EXE)
	$(BUILD_DIR)/$(EXE)

clean:
	rm -rf $(BUILD_DIR)
