#include <libavformat/avformat.h>
#include <libavutil/dict.h>

#define true 1
#define false 0

typedef struct Song {
    char* filename;
    char* title;
    char* validTitle;//valid for filenames
    char* album;
    char* validAlbum;
    char* artist;
    char* validArtist;
    char* ext;
    int trackNo;
    char* validname;
} Song;

//creates a Song from the specified file and returns it.
Song* song_new(char* filename);

//returns a new string that has all the invalid characters for filenames
//stripped out of it
char* makeValid(char* fname);

//sets the title, artist, and title char**'s based on the metadata in the
//AVFormatContext. 
//Returns nothing
void getMetadata(AVFormatContext*, char**, char**, char**, int*);

//returns if the AVFormatConext has just audio in it (not video)
int isAudio(AVFormatContext*);

//strips all non-alphanumeric characters
char* strip(char* input);

