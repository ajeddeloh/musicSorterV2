#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <sys/types.h>

void betterMkdir(char* dir);
int mkpath(const char*s, mode_t mode);
void copy(char* src, char *dest);
void move(char* src, char *dest);

#endif
