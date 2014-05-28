#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <glib-2.0/glib.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <ftw.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>

#include "data.h"
#include "file_util.h"
#include "string_util.h"

enum mode {COPY, MOVE};

void getFiles(char* dir, GPtrArray*);
int loadFile(const char *fpath, const struct stat *sb, int typeflag);
void sortMusic(char* rootDir, GPtrArray*, int copy_mode);

GPtrArray *files;

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
    files = g_ptr_array_new();
    ftw(source, loadFile, 100);
    printf("loaded %d files\n", files->len);
    sortMusic(dest, files, copy_mode);
    
    g_ptr_array_free(files, true);
    return 0;
}

/*
 * trys to load a file into the files list
 * for use with ftw
*/
int loadFile(const char *fpath, const struct stat * sb, int typeflag) {
    if(typeflag == FTW_F) { //file
        Song* tmp = song_new(fpath);
        if(tmp != NULL) {
            g_ptr_array_add(files, tmp);
        } else {
            printf("Error opening/parsing %s\n", fpath);
        }
    }
    return 0;
}

/*
 * takes every Song* in songs and moves/copies (dependant on copy_mode)
 * it into the correct directory into rootDir.
 * Returns: nothing
 */
void sortMusic(char* rootDir, GPtrArray* songs, int copy_mode) {
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

