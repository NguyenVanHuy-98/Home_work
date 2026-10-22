CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
LDFLAGS =

TARGET = memory
SRC = memory.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
