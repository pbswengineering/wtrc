# -*- Mode: Makefile; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-

TARGET = wtrc
LIBS = -lm $(shell pkg-config --libs glib-2.0) $(shell pkg-config --libs libcurl) $(shell xml2-config --libs)
CC = gcc
CFLAGS = -g -std=c99 -Wall -pedantic $(shell pkg-config --cflags glib-2.0) $(shell pkg-config --cflags libcurl) $(shell xml2-config --cflags)

.PHONY: default all clean indent doc valgrind

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -fr ../doc

indent:
	clang-format -i *.c *.h

doc:
	doxygen

valgrind: all
	valgrind -v --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)