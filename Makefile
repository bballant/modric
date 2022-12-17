TARGET = bin/modric

LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = src/modric.o src/miniprintf.o

.PRECIOUS: $(TARGET) $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)