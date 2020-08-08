LIBS = -lSDL2
LIBS-WIN32 = -lmingw32 -lSDL2main
FLAGS = -std=c99 -pedantic -Wpedantic -Wall -Werror -O3

CC = gcc

mosaic: mosaic.c
	$(CC) $^ $(FLAGS) $(LIBS-WIN32) $(LIBS) -o $@

all: mosaic
