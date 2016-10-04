CC := gcc
SRCDIR = src
INCLUDE := include
TARGET := magic

FLAGS := -std=c99
LFLAGS := -lSDL2 -lSDL2_ttf -lconfig
SOURCES := $(shell find $(SRCDIR) -type f -name "*.c")


all: clean release
.PHONY: all

release:
	$(CC) $(FLAGS) $(LFLAGS) -I$(INCLUDE) $(SOURCES) -o $(TARGET)
.PHONY: release

clean:
	@rm -rf magic
.PHONY: clean

