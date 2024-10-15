CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=access_ex.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=app

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@