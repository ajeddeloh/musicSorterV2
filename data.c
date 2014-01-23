#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <ctype.h>
#include "data.h"

Song* song_new(char* filename) {
    Song* song = malloc(sizeof(Song));
    song->filename = filename;
    AVFormatContext *handle = NULL;
    if(avformat_open_input(&handle, filename, NULL, NULL) != 0) {
        printf("Error opening file: %s",filename);
        return NULL;
    }
    getMetadata(handle, &song->title, &song->album, &song->artist, &song->trackNo);
    if(song->title == NULL) {
        printf("No title found for %s. Using filename.\n",filename);
        song->title = strdup(strrchr(filename,'/')+1);
        *strrchr(song->title,'.') = '\0';
    }

    if(song->album == NULL) {
        printf("No album found for %s. Using <Unknown Album>\n",filename);
        song->album = "Unknown Album";
    }
    
    if(song->artist == NULL) {
        printf("No artist found for %s. Using <Unknown Artist>\n",filename);
        song->artist = "Unknown Artist";
    }
        
    avformat_close_input(&handle);
    song->ext = strdup(strrchr(filename,'.'));
    
    song->validTitle = makeValid(song->title);
    song->validAlbum = makeValid(song->album);
    song->validArtist = makeValid(song->artist);
    return song;
}

void getMetadata(AVFormatContext* handle, char** title, char** album, char** artist, int* trackNo) {
    *title = NULL;
    *album = NULL;
    *artist = NULL;
    AVDictionary *dict = handle->metadata;
    if(dict == NULL) {
        int i;
        for (i = 0; i < handle->nb_streams; i++) {
            if(handle->streams[i]->metadata != NULL) {
                av_dict_copy(&dict, handle->streams[i]->metadata, 0);
            }
        }
    }
    if(dict == NULL) { //absoltely could not find metadata
        return;
    }
    AVDictionaryEntry *ent = av_dict_get(dict, "artist", NULL, 0);
    if(ent != NULL) {
        *artist = strdup(ent->value);
    }

    ent = av_dict_get(dict, "album", NULL, 0);
    if(ent != NULL) {
        *album = strdup(ent->value);
    }

    ent = av_dict_get(dict, "title", NULL, 0);
    if(ent != NULL) {
        *title = strdup(ent->value);
    }
    //todo: add trackno    
    return;
} 

int isAudio(AVFormatContext* handle) {
   return true; 
}

char* makeValid(char* fname) {
    if(strcmp("..",fname) == 0 || strcmp(".",fname) == 0 
        || strlen(fname) == 0) {
        return "Unknown";
    }
    char *newName = malloc(strlen(fname)+1);
    char *invalid = "<>:/\"\\|?^*";
    char *tmp = newName;
    while(*fname != '\0') {
        if(strchr(invalid, *fname) == '\0') { //valid
            *tmp = *fname;
        } else {
            *tmp = '_';
        }
        tmp++;
        fname++;
    }
    *tmp = '\0';
    return newName;
}

char* strip(char* input) {
    char* ret = malloc(strlen(input)+1);
    char* tmp = ret;
    while(*input != '\0') {
        if( isalnum(*input) ) {
            *tmp = tolower(*input);
            tmp++;
        }
        input++;
    }
    *ret = '\0';
    return ret;
}

