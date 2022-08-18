.PHONY: all run clean

all: a.out

run: a.out
	./a.out

clean:
	rm -f a.out

a.out: main.c
	gcc -std=c11 -Wall -Wextra -Wno-unused-parameter main.c
