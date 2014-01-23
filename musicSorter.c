#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glib-2.0/glib.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libavformat/avformat.h>

#include "data.h"

void getFiles(char* dir, GPtrArray*);
void sortMusic(char* rootDir, GPtrArray*);
void betterMkdir(char* dir);

int main(int argc, char ** argv) {
    if ( argc != 3) {
        printf("please run with the form: sorter <src> <dest>\n");
        return -1;
    }
    av_register_all();
    char* source = argv[1];
    char* dest = argv[2];
    printf("loading all filenames\n");
    GPtrArray *files = g_ptr_array_new();
    getFiles(source, files);
    printf("loaded %d files\n", files->len);
    sortMusic(dest, files);
    return 0;
}

void getFiles(char* dir, GPtrArray* files) {
    DIR *d = opendir(dir);
    if( !d ) {
        printf("Error opening directory %s\n", dir);
        return;
    }
    struct dirent *entry = readdir(d);
    struct stat statp;
    while(entry != NULL) {
        int len = strlen(dir)+strlen(entry->d_name)+2;
        char* fullname = malloc(len);
        snprintf(fullname, len, "%s/%s", dir, entry->d_name);
        stat(fullname, &statp);
        if(S_ISDIR(statp.st_mode)){
            if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) { 
                getFiles(fullname, files);
            }
        } else {
            Song* song = song_new(fullname);
            if(song != NULL) {
                g_ptr_array_add(files, song);
            } else {
                printf("Error opening/parsing: %s\n", fullname);
            }
        }
        entry = readdir(d);
    }
    closedir(d);
    free(entry);
}

void sortMusic(char* rootDir, GPtrArray* songs) {
    betterMkdir(rootDir);
    int i;
    for(i = 0; i < songs->len; i++) {
        Song* current = (Song*)g_ptr_array_index(songs, i);
        int len = strlen(rootDir)+
                strlen(current->validArtist)+
                strlen(current->validAlbum)+
                strlen(current->validTitle)+
                strlen(current->ext)+4;
        char* dirname = malloc(len);
        snprintf(dirname, len, "%s/%s",rootDir, current->validArtist);
        betterMkdir(dirname);
        strcat(dirname, "/");
        strcat(dirname, current->validAlbum);
        betterMkdir(dirname);
        strcat(dirname, "/");
        strcat(dirname, current->validTitle);
        strcat(dirname, current->ext);
        printf("%s\n%s\n",current->filename,dirname);
        if(rename(current->filename,dirname) != 0) {
            printf("%s\n",strerror(errno));
        }
    }
}

void betterMkdir(char* dir) {
    int err = mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO );
    if(err == -1 && errno != EEXIST) {
        printf("Error occured while creating %s. Error: %s\n",dir,strerror(errno));
    }
}
