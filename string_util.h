#ifndef STRING_UTIL_H
#define STRING_UTIL_H

//returns a new string that has all the invalid characters for filenames
//stripped out of it
char* makeValid(char* fname);

//strips all non-alphanumeric characters
char* strip(char* input);

#endif
