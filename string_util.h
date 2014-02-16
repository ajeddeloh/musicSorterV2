#ifndef STRING_UTIL_H
#define STRING_UTIL_H

//returns a new string that has all the invalid characters for filenames
//stripped out of it
char* makeValid(char* fname);

//strips all non-alphanumeric characters
char* strip(char* input);

//safely copy n strings (the variadic arguments) to the end of the base and stop
//if the total length of base is going to exceed n including the \0
void _nstrncat(char* base, int n, ...);

//marco to automatically append a null pointer to the end of _nstrncat
#define nstrncat(base, n, ...) \
    _nstrncat(base, n, __VA_ARGS__, NULL)


#endif
