#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "file_util.h"

/*
 * creates a directory and prints an error if it fails, except if the directory
 * already exists, thats ok
 * Returns: nothing
 */
void betterMkdir(char* dir) {
    int err = mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO );
    if(err == -1 && errno != EEXIST) {
        printf("Error occured while creating %s. Error: %s\n",dir,strerror(errno));
    }
}

/*
 * Moves a file, similar to mv. Tries to rename it and failing that copies it then
 * removes the original.
 */
void move(char *source, char *dest) {
    if(rename(source,dest) != 0) {
        copy(source,dest);
        remove(source);
    }
}

/* 
 * Copies the file source to location dest
 * returns: nothing
 */
void copy(char* source, char* dest) {
    struct stat stat_buf;
    off_t offset = 0;
    int src = open(source, O_RDONLY);
    fstat(src, &stat_buf);
    int dst = open(dest,O_WRONLY | O_CREAT, stat_buf.st_mode);
    if (sendfile(dst, src, &offset, stat_buf.st_size) == -1) {
        printf("Error occured while copying %s to %s. Error: %s\n",
         source, dest, strerror(errno));
    }
    close(src);
    close(dst);
}
