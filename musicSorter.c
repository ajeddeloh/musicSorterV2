#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <glib-2.0/glib.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <libavformat/avformat.h>

#include "data.h"

enum mode {COPY, MOVE};

void getFiles(char* dir, GPtrArray*);
void sortMusic(char* rootDir, GPtrArray*, int copy_mode);
void betterMkdir(char* dir);
void copy(char* src, char *dest);

int main(int argc, char ** argv) {
    int copy_mode = COPY;
    int c;
    char *source, *dest;
    while ((c = getopt(argc,argv,"m")) != -1) {
        switch(c) {
        case 'm':
            copy_mode = MOVE;
            break;
        default:
            printf("error, invalid argument");
            exit(EXIT_FAILURE);
        }
    }
    
    if(argv[optind] == NULL || argv[optind+1] == NULL) {
        printf("Not enough arguments");
        exit(EXIT_FAILURE);
    }
    source = argv[optind];
    dest = argv[optind+1];

    av_register_all();
    printf("loading all filenames\n");
    GPtrArray *files = g_ptr_array_new();
    getFiles(source, files);
    printf("loaded %d files\n", files->len);
    sortMusic(dest, files, copy_mode);
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

void sortMusic(char* rootDir, GPtrArray* songs, int copy_mode) {
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
        if(copy_mode == MOVE) {
            if(rename(current->filename,dirname) != 0) {
                printf("%s\n",strerror(errno));
            }
        } else {
            //todo: add error checking
            copy(current->filename, dirname);
        }
    }
}

void betterMkdir(char* dir) {
    int err = mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO );
    if(err == -1 && errno != EEXIST) {
        printf("Error occured while creating %s. Error: %s\n",dir,strerror(errno));
    }
}

void copy(char* source, char* dest) {
    struct stat stat_buf;
    off_t offset = 0;
    int src = open(source, O_RDONLY);
    fstat(src, &stat_buf);
    int dst = open(dest,O_WRONLY | O_CREAT, stat_buf.st_mode);
    sendfile(dst, src, &offset, stat_buf.st_size);
    close(src);
    close(dst);
}
