CC = gcc
CFLAGS = -std=c99 -Wall -O3 -ffast-math
LDFLAGS = -lm

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

OUTPUT = convex-hull

.PHONY = clean cleanbin

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)
	strip $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

cleanbin: clean
	rm -f $(OUTPUT) > /dev/null

clean:
	rm -f $(OBJECTS) > /dev/null

remake: clean all
