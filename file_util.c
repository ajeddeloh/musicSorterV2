#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include "file_util.h"

/* Function with behaviour like `mkdir -p' 
 * copied from http://niallohiggins.com/2009/01/08/mkpath-mkdir-p-alike-in-c-for-unix/
 */
int mkpath(const char *s, mode_t mode) {
        char *q, *r = NULL, *path = NULL, *up = NULL;
        int rv;

        rv = -1;
        if (strcmp(s, ".") == 0 || strcmp(s, "/") == 0)
                return (0);

        if ((path = strdup(s)) == NULL)
                exit(1);
     
        if ((q = strdup(s)) == NULL)
                exit(1);

        if ((r = dirname(q)) == NULL)
                goto out;
        
        if ((up = strdup(r)) == NULL)
                exit(1);

        if ((mkpath(up, mode) == -1) && (errno != EEXIST))
                goto out;

        if ((mkdir(path, mode) == -1) && (errno != EEXIST))
                rv = -1;
        else
                rv = 0;

out:
        if (up != NULL)
                free(up);
        free(q);
        free(path);
        return (rv);
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
