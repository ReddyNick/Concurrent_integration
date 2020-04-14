CXX = gcc
CXXFLAGS = -Wall -Wextra -g -pthread -lm -std=c99

SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

%.o: %.c 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: all clean

clean:
	rm -f *.o