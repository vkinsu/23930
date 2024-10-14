CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=string_list.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=app

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@