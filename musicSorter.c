#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <glib-2.0/glib.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>

#include "data.h"
#include "file_util.h"
#include "string_util.h"

enum mode {COPY, MOVE};

void getFiles(char* dir, GPtrArray*);
void sortMusic(char* rootDir, GPtrArray*, int copy_mode);

int main(int argc, char ** argv) {
    int copy_mode = COPY;
    int quiet_mode = false;
    int c;
    char *source, *dest;
    while ((c = getopt(argc,argv,"mq")) != -1) {
        switch(c) {
        case 'm':
            copy_mode = MOVE;
            break;
        case 'q':
            quiet_mode = true;
            break;
        default:
            printf("error, invalid argument");
            exit(EXIT_FAILURE);
        }
    }

    if(quiet_mode) {
        printf("Quiet mode not implemented yet\n");
    }
    
    if(argv[optind] == NULL || argv[optind+1] == NULL) {
        printf("Not enough arguments\n");
        exit(EXIT_FAILURE);
    }
    source = argv[optind];
    dest = argv[optind+1];

    av_register_all();
    av_log_set_level(AV_LOG_ERROR);

    printf("loading all filenames\n");
    GPtrArray *files = g_ptr_array_new();
    getFiles(source, files);
    printf("loaded %d files\n", files->len);
    sortMusic(dest, files, copy_mode);
    
    g_ptr_array_free(files, true);
    return 0;
}

/* 
 * Recursively finds all files in the directory dir and creates a Song
 * for each file found and stores them in files.
 * Returns: nothing
 */
void getFiles(char* dir, GPtrArray* files) {
    DIR *d = opendir(dir);
    if( !d ) {
        printf("Error opening directory %s\n", dir);
        return;
    }
    struct dirent *entry = readdir(d);
    struct stat statp;
    char* fullname = NULL;
    while(entry != NULL) {
        int len = strlen(dir)+strlen(entry->d_name)+2;
        fullname = realloc(fullname, len);
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
        free(fullname);
        fullname = NULL;
        entry = readdir(d);
    }
    closedir(d);
}

/*
 * takes every Song* in songs and moves/copies (dependant on copy_mode)
 * it into the correct directory into rootDir.
 * Returns: nothing
 */
void sortMusic(char* rootDir, GPtrArray* songs, int copy_mode) {
    betterMkdir(rootDir);
    for(int i = 0; i < songs->len; i++) {
        Song* current = (Song*)g_ptr_array_index(songs, i);
        int len = strlen(rootDir) + strlen("/") + current->dest_fname_length + 1;
        char* fname = malloc(len);
        snprintf(fname, len, "%s/%s", rootDir, current->dest_fname);
        char* tmp = strdup(fname); //cuz dirname is stupid and modifies its argument
        mkpath(dirname(tmp), S_IRWXU | S_IRWXG | S_IRWXO);
        free (tmp);
        if(copy_mode == MOVE) {
            move(current->filename, fname);
        } else {
            copy(current->filename, fname);
        }
        free(fname);
        song_free(current);
    }
}

