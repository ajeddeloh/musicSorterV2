#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "string_util.h"

#include <stdio.h>

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


void _nstrncat(char* src, int n, ...) {
    int count = 1;
    while(*src++ && count < n) {
        ++count;
    } //increment the pointer to the \0
    --src; 
    va_list ap;
    va_start(ap, n);
    char* arg = va_arg(ap, char*);
    while(arg != NULL) {
        while((*src++ = *arg++) && count < n) {
            count ++;
        } //copy till the end of arg
        --src;
        arg = va_arg(ap, char*);
    }
    *src = '\0';//in case it exited the loop due to going past n
    va_end(ap);
}
