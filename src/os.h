//
// operating system specific stuff for flabc
//

#ifndef _os_H
#define _os_H

#include <string>

using namespace std;

// the default directory seperator
#define DIRSEPCHAR "/"
// all directory seperators
#ifdef WIN32
  #define DIRSEPCHARS "/\\"
  #define PATHSEPCHAR ";"
#else
  #define DIRSEPCHARS "/"
  #define PATHSEPCHAR ":"
#endif

// MingW32 has alloca declared in malloc.h instead of stdlib.h
#ifdef __MINGW32__
  #include <malloc.h>
#endif

// for regexp support
#ifndef WIN32
  #include <sys/types.h>
  #include <regex.h>
#endif

// read the output of the given command into a string
// RC: result of the command
int pipe_command(const char* command, string* result, int* rc);

// search a filename in the given with sepchars separated directorylist
// RC: full pathname of found file or "" when not found
string find_file(const char* dirlist, const char* filename, const char* sepchars);

// open a html file with a webbrowser
// the second argument is a semicolon seperated list of browsers
// which is only necessary on Linux;
// on OSX and Win32 the default webbrowser is started instead
// RC: 0 ok, otherwise an error occured
//           when error != NULL, it is filled with an error description
int open_html(const char* htmlfile, const char* browsers, string* error=NULL);

// open a PS file with a psviewer (eg. gv)
// the second argument is only necessary on Linux and OSX;
// on Win32 the default psviewer is started instead
// when error != NULL, it is filled with an error description
int open_ps(const char* psfile, const char* psviewer, string* error=NULL);

// open a Midi file with a midiplayer (eg. timdity)
// the second argument is only necessary on Linux and OSX;
// on Win32 the default midiplayer is started instead
// when error != NULL, it is filled with an error description
int open_midi(const char* midifile, const char* midiplayer, string* error=NULL);

// returns the basename of the given filename
string basename(const string path);

// determines the full pathname of a given executable (argv[0])
string full_exec_path(const char* program);

// get the default directory for the file open dialog when no file is loaded
string get_default_directory();

// get a unique string derived from the userid
string get_uid_string();

// set mouse position to given coordinates
// RC: 0 = ok, otherwise error
int set_mouse_position(int x, int y);


// missing functions on WIN32
//-----------------------------------------------------------------
#ifdef WIN32

int setenv(const char *name, const char *value, int overwrite);
void unsetenv(const char *name);
char* strtok_r(char* str, const char* sep, char** last);

#endif

#endif
