.PHONY: all main shared

all: main shared

main: shared main.c
	gcc -g -L. -Wl,-rpath=. -Wall main.c -lshared -ldl -lncurses

shared: shared.c
	gcc -Wno-error=unused-function -g -c -Wall -Werror -fpic -o shared.o shared.c
	gcc -shared -o libshared.so shared.o -lncurses
	touch sentinel

clean:
	rm *.o *.so
