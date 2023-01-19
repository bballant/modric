TARGET = bin/modric

LIBS = -lm -lrocksdb
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = src/modric.o src/cJSON.o src/mongoose.o src/json_pprint.o src/edn_parse.o src/alvarez.o src/alvarez_rocks.o src/http_client.o

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

run-alvarez: clean $(TARGET)
	./$(TARGET) -alvarez
