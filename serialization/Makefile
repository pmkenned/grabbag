.PHONY: all run clean

CFLAGS = -std=c99 -ggdb -Wall -Wextra -Wno-parentheses

TARGET = program

OBJ = main.o tree.o zone.o

all: $(TARGET)

run: all
	./$(TARGET) 0

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJ)
