#ifndef DATA_H
#define DATA_H

#include <libavformat/avformat.h>
#include <libavutil/dict.h>

#define true 1
#define false 0

typedef struct Metadata {
    char *title;
    char *album;
    char *artist;
    int trackNo;
    int diskNo;
} Metadata;

typedef struct Song {
    Metadata *metadata;
    char* filename;
    char* validTitle;//valid for filenames
    char* validAlbum;
    char* validArtist;
    char* ext;
    char* dest_fname;
    int dest_fname_length; //does not contain '\0'
} Song;

//creates a Song from the specified file and returns it.
Song* song_new(char* filename);

//free the specified metadata
void metadata_free(Metadata* metadata);

//frees the specifed song
void song_free(Song* song);

//sets the title, artist, and title char**'s based on the metadata in the
//AVFormatContext. 
//Returns nothing
void getMetadata(AVFormatContext*, Metadata*);

//returns if the AVFormatConext has just audio in it (not video)
int isAudio(AVFormatContext*);

#endif
