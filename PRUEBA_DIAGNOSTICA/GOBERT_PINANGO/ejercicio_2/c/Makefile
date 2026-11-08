CC ?= gcc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
LDLIBS ?= -lgmp
TARGET = polinomio

.PHONY: all clean run bench

all: $(TARGET)

$(TARGET): polinomio.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

run: $(TARGET)
	./$(TARGET) 4 2

bench: $(TARGET)
	./$(TARGET) --bench 1000 100

clean:
	rm -f $(TARGET)
