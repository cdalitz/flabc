//
// global utility functions
//

#ifndef _utils_H
#define _utils_H

#include <string>

using namespace std;

// remove front and back whitespace from a string
string trim_space(const string& s);
string rtrim_space(const string& s);

// reads an entire file into a provided string
int readfile(const char* filename, string* s);

// case insensitive string comparison 
//
// most systems have strcasecmp(), but some have stricmp() or strcmpi()
// implementing an own version of this trivial function avoids messing
// with #ifdefs for various environments
//
int strcmpnocase (const char* s1, const char* s2);
int strncmpnocase (const char* s1, const char* s2, int n);

#endif
