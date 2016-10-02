all: clean release
.PHONY: all

release:
	gcc -std=c99 -lSDL2 -lSDL2_ttf mahou-kagami.c -o magic
.PHONY: release

clean:
	@rm -rf magic
.PHONY: clean

