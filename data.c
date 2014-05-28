#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <ctype.h>
#include <string.h>
#include "data.h"
#include "string_util.h"

Song* song_new(const char* filename) {
    AVFormatContext *handle = NULL;
    if(avformat_open_input(&handle, filename, NULL, NULL) != 0) {
        printf("Error opening file: %s\n",filename);
        return NULL;
    }
    if(!isAudio(handle)) {
        avformat_close_input(&handle);
        return NULL;
    }
    Song* song = malloc(sizeof(Song));
    song->filename = strdup(filename);
    song->metadata = malloc(sizeof(Metadata));
    getMetadata(handle, song->metadata);
    if(song->metadata->title == NULL) {
        printf("No title found for %s. Using filename.\n",filename);
        song->metadata->title = strdup(strrchr(filename,'/')+1);
        *strrchr(song->metadata->title,'.') = '\0';
    }

    if(song->metadata->album == NULL) {
        printf("No album found for %s. Using <Unknown Album>\n",filename);
        song->metadata->album = strdup("Unknown Album");;
    }
    
    if(song->metadata->artist == NULL) {
        printf("No artist found for %s. Using <Unknown Artist>\n",filename);
        song->metadata->artist = strdup("Unknown Artist");
    }
        
    avformat_close_input(&handle);
    song->ext = strdup(strrchr(filename,'.'));
    
    song->validTitle = makeValid(song->metadata->title);
    song->validAlbum = makeValid(song->metadata->album);
    song->validArtist = makeValid(song->metadata->artist);
    song->dest_fname_length = strlen(song->validArtist) + strlen("/") + strlen(song->validAlbum)
        + strlen("/") + strlen(song->validTitle) + strlen(song->ext);
    song->dest_fname = malloc(song->dest_fname_length + 1);
    snprintf(song->dest_fname, song->dest_fname_length + 1, "%s/%s/%s%s",
        song->validArtist, song->validAlbum, song->validTitle, song->ext);
    return song;
}

void song_free(Song* song) {
    if(song == NULL) {
        return;
    }
    metadata_free(song->metadata);
    free(song->filename);
    free(song->validTitle);
    free(song->validAlbum);
    free(song->validArtist);
    free(song->ext);
    free(song->dest_fname);
    free(song);
}   

void getMetadata(AVFormatContext* handle, Metadata* metadata) {
    metadata->title = NULL;
    metadata->album = NULL;
    metadata->artist = NULL;
    AVDictionary *dict = handle->metadata;
    if(dict == NULL) {
        for (int i = 0; i < handle->nb_streams; i++) {
            if(handle->streams[i]->metadata != NULL) {
                av_dict_copy(&dict, handle->streams[i]->metadata, 0);
            }
        }
    }
    if(dict == NULL) { //absoltely could not find metadata
        return;
    }
    AVDictionaryEntry *ent = av_dict_get(dict, "album_artist", NULL, AV_DICT_DONT_STRDUP_VAL);
    if(ent != NULL) {
        metadata->artist = strdup(ent->value);
    } else { //could be that the artist is under album_artist
        ent = av_dict_get(dict, "artist", NULL, AV_DICT_DONT_STRDUP_VAL);
        if(ent != NULL) {
            metadata->artist = strdup(ent->value);
        }
    }

    ent = av_dict_get(dict, "album", NULL, AV_DICT_DONT_STRDUP_VAL);
    if(ent != NULL) {
        metadata->album = strdup(ent->value);
    }

    ent = av_dict_get(dict, "title", NULL, AV_DICT_DONT_STRDUP_VAL);
    if(ent != NULL) {
        metadata->title = strdup(ent->value);
    }
}

void metadata_free(Metadata* metadata) {
    free(metadata->title);
    free(metadata->album);
    free(metadata->artist);
    free(metadata);
} 

int isAudio(AVFormatContext* handle) {
    int hasAudio = false;
    for(int i = 0; i < handle->nb_streams; i++ ) {
        enum AVMediaType t = handle->streams[i]->codec->codec_type;
        if(t == AVMEDIA_TYPE_VIDEO && !(handle->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
            return false;
        }
        if(t == AVMEDIA_TYPE_AUDIO) {
            hasAudio = true;
        }
    }
    return hasAudio;
}
