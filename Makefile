CC = gcc
GLIB = `pkg-config glib-2.0 --cflags --libs`
FFMPEG = `pkg-config libavformat libavutil --cflags --libs`
FLAGS = -std=gnu99 -g -Wall

all:
	$(CC) $(GLIB) $(FFMPEG) $(FLAGS) *.c -o musicSorter
