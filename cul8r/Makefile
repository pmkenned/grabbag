.PHONY: all run clean

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror

all: zone malloc

zone: zone.c
malloc: malloc.c

TIME_FMT = "real %Es\nuser %Us\nsys %Ss\n"
run: zone malloc
	@echo ZONE:
	@time -f $(TIME_FMT) ./zone
	@echo MALLOC:
	@time -f $(TIME_FMT) ./malloc

clean:
	rm -f *.o zone malloc
