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

Song* song_new(char* filename);
char* makeValid(char* fname);
void getMetadata(AVFormatContext*, char**, char**, char**, int*);
int isAudio(AVFormatContext*);
char* strip(char* input);

