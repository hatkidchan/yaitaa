CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -Wno-attributes
CLIBS := -lm
INCLUDES := -Isrc
OBJECTS := obj/stb_image.o obj/stb_image_resize.o \
					 obj/colors.o obj/args.o obj/image.o obj/commons.o \
					 obj/mod_blocks.o obj/mod_braille.o
VERSION := "v-git-$(shell git rev-parse --short HEAD)"

all: lib yaitaa

lib: $(OBJECTS)

clean:
	$(RM) yaitaa obj/*

yaitaa: lib
	$(CC) $(CFLAGS) src/main.c $(INCLUDES) $(OBJECTS) $(CLIBS) -o ./yaitaa

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $^ $(INCLUDES) -DVERSION="\"$(VERSION)\"" -c -o $@

testbuild:
	make clean all CC=clang
	make clean all
	oclint src/*.c
