#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <ctype.h>
#include <string.h>
#include "data.h"
#include "string_util.h"

extern int use_artist;
extern int use_album_artist;
extern int quiet_mode;
extern const char *fmt_string;

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

    Song *song = malloc(sizeof(Song));
    song->filename = strdup(filename);
    song->metadata = (Metadata){0}; 
    song->valid_metadata = (Metadata){0};

    getMetadata( handle, &(song->metadata) );
    avformat_close_input(&handle);

    fixInvalidMetadata(song);
    validateMetadata( &(song->metadata), &(song->valid_metadata) );
        
    song->ext = strdup(strrchr(filename,'.')+1);
    
    song->dest_fname = getDestPath(song);
    printf("DEBUG: dest = %s\n", song->dest_fname);
    return song;
}

//does not change song->valid_metadata
void fixInvalidMetadata(Song *song) {
    Metadata *m = &(song->metadata);
    if(m->artist == NULL) {
        if(use_album_artist && m->album_artist != NULL) {
        printf("No artist found for %s. Using Album Artist\n",song->filename);
            m->artist = strdup(m->album_artist);
        } else {
            printf("No artist found for %s. Using <Unknown Artist>\n",song->filename);
            m->artist = strdup("Unknown Artist");
        }
    }
    if(m->album_artist == NULL) {
        if(use_artist && m->artist != NULL) {
            m->album_artist = strdup(m->artist);
        } else {
            m->album_artist = strdup("Unknown Artist");
        }
    }
    if(m->album == NULL) {
        printf("No album found for %s. Using <Unknown Album>\n",song->filename);
        m->album = strdup("Unknown Album");
    }

    if(song->metadata.title == NULL) {
        printf("No title found for %s. Using filename.\n",song->filename);
        song->metadata.title = strdup(strrchr(song->filename,'/')+1);
        *strrchr(song->metadata.title,'.') = '\0'; //turn the last . into a null terminator to cut off the extension
    }
}
    

void validateMetadata( Metadata *src, Metadata *dst) {
    dst->title = makeValid(src->title);
    dst->album = makeValid(src->album);
    dst->album_artist = makeValid(src->album_artist);
    dst->artist = makeValid(src->artist);
    dst->track_no = src->track_no;
    dst->disk_no = src->disk_no;
}

char* getDestPath(Song *song) {
    Metadata *m = &(song->valid_metadata);

    const char *cursor = fmt_string;
    int len = 0;
    while( *cursor != '\0' ) {
        if( *cursor == '%' ) {
            char *tmp = getAttribute ( m, *++cursor );
            if( tmp != NULL ) {
                len += strlen(tmp);
            } else {
                len++;
            }
        } else {
            len++;
        }
        cursor++;
    }
    len ++; //for null terminator
    
    cursor = fmt_string;
    char *ret = malloc( len );
    strcpy(ret, "");
    while( *cursor != '\0' ) {
        if( *cursor != '%' ) {
            strncat( ret, cursor++, 1);
        } else {
            char *tmp = getAttribute( m, *++cursor );
            if( tmp == NULL ) {
                tmp = "%";
            } else {
                cursor ++;
            }
            strcat(ret, tmp);
        }
    }

    return ret;
}    

char *getAttribute( Metadata *metadata, char attrib ) {
    switch (attrib) {
        case 'A': return metadata->artist;
        case 'a': return metadata->album_artist;
        case 'b': return metadata->album;
        case 't': return metadata->title;
        default: return NULL;
    }
}


void song_free(Song* song) {
    if(song == NULL) {
        return;
    }
    metadata_free(&(song->metadata));
    metadata_free(&(song->valid_metadata));
    free(song->filename);
    free(song->ext);
    free(song->dest_fname);
    free(song);
}   

void getMetadata(AVFormatContext* handle, Metadata* metadata) {
    metadata->title = NULL;
    metadata->album = NULL;
    metadata->artist = NULL;
    metadata->album_artist = NULL;
    AVDictionary *dict = handle->metadata;
    if(dict == NULL) {
        for (unsigned int i = 0; i < handle->nb_streams; i++) {
            if(handle->streams[i]->metadata != NULL) {
                av_dict_copy(&dict, handle->streams[i]->metadata, 0);
            }
        }
    }
    if(dict == NULL) { //absoltely could not find metadata
        return;
    }
    AVDictionaryEntry *ent = av_dict_get(dict, "album_artist", NULL, 0);
    if(ent != NULL) {
        metadata->album_artist = strdup(ent->value);
    }

    ent = av_dict_get(dict, "artist", NULL, 0);
    if(ent != NULL) {
        metadata->artist = strdup(ent->value);
    }

    ent = av_dict_get(dict, "album", NULL, 0);
    if(ent != NULL) {
        metadata->album = strdup(ent->value);
    }

    ent = av_dict_get(dict, "title", NULL, 0);
    if(ent != NULL) {
        metadata->title = strdup(ent->value);
    }
}

void metadata_free(Metadata* metadata) {
    free(metadata->title);
    free(metadata->album);
    free(metadata->artist);
    free(metadata->album_artist);
} 

int isAudio(AVFormatContext* handle) {
    int hasAudio = false;
    for(unsigned int i = 0; i < handle->nb_streams; i++ ) {
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
