CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -ggdb
CLIBS := -lm
INCLUDES := -Isrc
OBJECTS := obj/stb_image.o obj/stb_image_resize.o \
					 obj/colors.o obj/args.o obj/image.o obj/commons.o \
					 obj/mod_blocks.o obj/mod_braille.o

all: lib asciify

lib: $(OBJECTS)

clean:
	$(RM) obj/*

asciify: lib
	$(CC) $(CFLAGS) src/main.c $(INCLUDES) $(OBJECTS) $(CLIBS) -o ./asciify

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $^ $(INCLUDES) -c -o $@

