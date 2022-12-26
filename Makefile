TARGET = bin/modric

LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = src/alvarez.o src/modric.o src/cJSON.o src/json_pprint.o src/edn_parse.o

.PRECIOUS: $(TARGET) $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

run: clean $(TARGET)
	./$(TARGET) $(ARGS)
