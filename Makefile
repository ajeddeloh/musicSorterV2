CC = gcc
GLIB = `pkg-config glib-2.0 --cflags --libs`
TAGLIB = `pkg-config taglib_c --cflags --libs`
FFMPEG = `pkg-config libavformat libavutil --cflags --libs`
FLAGS = -g -Wall

all:
	$(CC) $(GLIB) $(FFMPEG) $(FLAGS) *.c
