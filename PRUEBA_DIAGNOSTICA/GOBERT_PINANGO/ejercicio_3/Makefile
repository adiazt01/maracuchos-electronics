CC ?= gcc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
TARGET = traductor

.PHONY: all clean informe traducir

all: $(TARGET)

$(TARGET): traductor.c
	$(CC) $(CFLAGS) -o $@ $<

informe: $(TARGET)
	./$(TARGET) ejemplo.c

traducir: $(TARGET)
	./$(TARGET) ejemplo.c --traducir

clean:
	rm -f $(TARGET)
