CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99
CLIBS := -lm
INCLUDES := -Isrc
OBJECTS := obj/stb_image.o

all: lib asciify

lib: $(OBJECTS)

clean:
	$(RM) obj/*

asciify: lib
	$(CC) $(CFLAGS) src/main.c $(INCLUDES) $(OBJECTS) $(CLIBS) -o ./asciify

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $^ $(INCLUDES) -c -o $@

