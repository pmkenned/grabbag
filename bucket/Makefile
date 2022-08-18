.PHONY: all run clean

TARGET = bucket

CFLAGS = -Wall -Wextra -std=c99
#CFLAGS += -Og -g
CFLAGS += -O3 -g
#LDFLAGS = -static

all: $(TARGET)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

$(TARGET): main.o
	gcc $(LDFLAGS) -o $@ $^
	objdump -dl $@ > $@.dasm

run: $(TARGET)
	./$(TARGET) bucket
	./$(TARGET) qsort

clean:
	rm -f *.o $(TARGET) *.dasm  *.out.* *.out report.txt

include cachegrind.mk
