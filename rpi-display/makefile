CC := gcc
SRCDIR = src
INCLUDE := include
TARGET := bin/magic

FLAGS := -std=c99
LFLAGS := -lSDL2 -lSDL2_ttf
SOURCES := $(shell find $(SRCDIR) -type f -name "*.c")


all: clean release
.PHONY: all

release:
	$(CC) $(FLAGS) $(LFLAGS) -I$(INCLUDE) $(SOURCES) -o $(TARGET)
.PHONY: release

clean:
	@rm -rf bin/magic
.PHONY: clean

