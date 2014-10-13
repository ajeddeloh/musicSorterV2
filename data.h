#ifndef DATA_H
#define DATA_H

#include <libavformat/avformat.h>
#include <libavutil/dict.h>

#define true 1
#define false 0

#define ARTIST "A"
#define ALBUM "b"
#define ALBUM_ARTIST "a"
#define TITLE "t"
#define DISK "d"
#define TRACK "T"
#define VALID "AabtdT"

typedef struct Metadata {
    char *title;
    char *album;
    char *album_artist;
    char *artist;
    int track_no;
    int disk_no;
} Metadata;

typedef struct Song {
    Metadata metadata;
    Metadata valid_metadata;
    char* filename;
    char* ext;
    char* dest_fname;
} Song;


//creates a Song from the specified file and returns it.
Song* song_new(const char* filename);

//free the specified metadata
//frees the specifed song
void song_free(Song* song);

void metadata_free(Metadata *metadata);

char *getDestPath(Song *song);

char *getAttribute(Metadata *metadata, char attrib);

void validateMetadata(Metadata *src, Metadata *dst);

void fixInvalidMetadata(Song *song);

//sets the title, artist, and title char**'s based on the metadata in the
//AVFormatContext. 
//Returns nothing
void getMetadata(AVFormatContext*, Metadata*);

//returns if the AVFormatConext has just audio in it (not video)
int isAudio(AVFormatContext*);

#endif
