//
// operating system specific stuff for flabc
//

#include "os.h"
#include "messagebox.h"
#include "utils.h"
#include <FL/x.H>
#include <FL/Fl.H>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <FL/fl_ask.H>

#ifdef WIN32
  #include <windows.h>
#endif

#ifdef __MINGW32__
  #include <malloc.h>
#endif

#ifndef WIN32
  #include <sys/wait.h>
#endif

// reads the output of the given command into a string
// and returns the exit code of the program in rc
// RC: 0: command could be invoked; 1: an error occured
int pipe_command(const char* command, string* result, int* rc)
{
#ifdef WIN32

  // popen on Win32 only works when the calling program is a
  // "console" application. Hence we need a special implementation.
  HANDLE hpread, hpwrite;
  SECURITY_ATTRIBUTES pipesa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
  PROCESS_INFORMATION procinfo;
  STARTUPINFO startinfo;
  DWORD dwrc;

  *result = ""; *rc = 0;

  // create pipe and connect write end to subprocess
  if (!CreatePipe(&hpread, &hpwrite, &pipesa, 0)) {
    return 1;
  }
  memset(&startinfo, 0, sizeof(STARTUPINFO));
  startinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  startinfo.wShowWindow = SW_HIDE;
  startinfo.hStdOutput = hpwrite;
  startinfo.hStdError = hpwrite;
  if (!CreateProcess(NULL, (char*)command, NULL, NULL, 
                     TRUE /*inherit handles*/, 0, NULL, NULL,
                     &startinfo, &procinfo)) {
    CloseHandle(hpread); CloseHandle(hpwrite);
    return 1;
  }
  CloseHandle(hpwrite); // no longer needed in this process

  // read from pipe as long as data is available and the process is running
  char buffer[81];
  DWORD n; // number of read bytes
  while (ReadFile(hpread, buffer, 80, &n, NULL)) {
	//&& GetLastError() != ERROR_BROKEN_PIPE) {
	if (n) { // something read
	  buffer[n] = '\0';
	  *result += buffer;
	}
  }

  GetExitCodeProcess(procinfo.hProcess, &dwrc);
  CloseHandle(procinfo.hThread);
  CloseHandle(procinfo.hProcess);
  CloseHandle(hpread);

  *rc = dwrc;
  return 0;

#else

  FILE* p;
  char c;
  int popenrc;
  string shcommand; // command for passing to shell (stderr -> stdout)
  *result = ""; *rc = 0;
  // workaround for bug is OSX 10.10. Yosemite that subprocesses
  // started by popen or system do not inherit PATH
  //shcommand = (string)command + " 2>&1";
  shcommand = (string)"PATH='" + getenv("PATH") + "'; " + (string)command + " 2>&1";
  p = popen(shcommand.c_str(),"r");
  if (!p) {
    return 1;
  }
  while (EOF != (c = fgetc(p)))
    *result += c;
  popenrc = pclose(p);
  *rc = WEXITSTATUS(popenrc);
  if (WIFEXITED(popenrc) && 127 == *rc) {
    return 1;
  }
  return 0;

#endif
}

// searches a filename in the given with sepchars separated directorylist
// RC: full pathname of found file or "" when not found
string find_file(const char* dirlist, const char* filename, const char* sepchars)
{
  char *dirlistcopy, *dir, *p;
  string fullpath;
  dirlistcopy = (char*)alloca(sizeof(char)*(strlen(dirlist)+4));
  strcpy(dirlistcopy, dirlist);

  dir = strtok_r(dirlistcopy,sepchars,&p);
  while (dir) {
    fullpath = (string)dir + DIRSEPCHAR + filename;
    if (0 == access(fullpath.c_str(),R_OK))
      return fullpath;
    dir = strtok_r(NULL,sepchars,&p);
  }
  return "";
}

#ifdef WIN32
// wrapper for the ShellExecuteEx function on win32
// return value: 0 = ok, otherwise error
//               when error != NULL, it is filled with an error description
int win32_open_file(const char* filename, string* error = NULL)
{
  if (error) *error = "";
  // convert slashes to backslashes (mixing confuse firefox)
  char* filecopy = (char*)alloca(sizeof(char)*(strlen(filename)+4));
  strcpy(filecopy,filename);
  for (char* c=filecopy; *c; c++) if (*c=='/') *c='\\';
  SHELLEXECUTEINFO ShExecInfo = {0};
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_FLAG_NO_UI;
  ShExecInfo.hwnd = NULL;
  ShExecInfo.lpVerb = "open";
  ShExecInfo.lpFile = filecopy;
  ShExecInfo.lpParameters = NULL; 
  ShExecInfo.lpDirectory = NULL;
  ShExecInfo.nShow = SW_SHOWNORMAL;
  ShExecInfo.hInstApp = NULL; 
  if (ShellExecuteEx(&ShExecInfo)) {
	return 0;
  } else {
    LPVOID errortext;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL, GetLastError(), 0, (LPTSTR) &errortext, 0, NULL);
    *error = (char*)errortext;
	*error = rtrim_space(*error);
    LocalFree(errortext);
	return 1;
  }
}
#endif

// open a html file with a webbrowser
// the second argument is a semicolon seperated list of browsers
// which is only necessary on Linux;
// on OSX and Win32 the default webbrowser is started instead
// RC: 0 ok, otherwise an error occured
//           when error != NULL, it is filled with an error description
int open_html(const char* htmlfile, const char* browsers, string* error/*=NULL*/)
{
  string command;
  if (error) *error = "";

#ifdef WIN32
  return win32_open_file(htmlfile, error);
  //if (32 >= (long)ShellExecute(NULL, "open", htmlfile,
  //                             NULL, NULL, SW_SHOWNORMAL)) return 1;
  //else return 0;
#endif
#if defined __APPLE__
  command = (string)"open '" + htmlfile + "'";
#else
  // pick the first found webbrowser from the semicolon seperated list
  char *browserscopy, *browser, *p;
  browserscopy = (char*)alloca(sizeof(char)*(strlen(browsers)+4));
  strcpy(browserscopy, browsers);
  browser = strtok_r(browserscopy, ";", &p);
  while (browser) {
    if ("" != find_file(getenv("PATH"), browser, ":")) {
      break;
    }
    browser = strtok_r(NULL, ";", &p);
  }
  if (!browser) {
    *error = (string)"None of the browsers '" + browsers + "' is installed";
    return 2;
  }
  command = (string) browser + " '" + htmlfile + "' &";
#endif
  // cannot get info on success, because command is run in background
  return system(command.c_str());
}

// open a PS file with a psviewer (eg. gv)
// the second argument is only necessary on Linux and OSX;
// on Win32 the default webbrowser is started instead
// when error != NULL, it is filled with an error description
int open_ps(const char* psfile, const char* psviewer, string* error /*=NULL*/)
{
#ifdef WIN32
  return win32_open_file(psfile, error);
  //if (32 >= (long)ShellExecute(NULL, "open", psfile,
  //                             NULL, NULL, SW_SHOWNORMAL)) return 1;
  //else return 0; 
#else
  string command;
  command = (string) psviewer + " '" + psfile + "' &";
  return system(command.c_str());
#endif
}


// open a Midi file with a midiplayer (eg. timdity)
// the second argument is only necessary on Linux;
// on OSX and Win32 the default midiplayer is started instead
// when error != NULL, it is filled with an error description
int open_midi(const char* midifile, const char* midiplayer, string* error /*=NULL*/)
{
  string command;
#if defined __APPLE__
  command = (string)"open '" + midifile + "'";
#elif defined WIN32
  return win32_open_file(midifile, error);
  //if (32 >= (long)ShellExecute(NULL, "open", midifile,
  //                             NULL, NULL, SW_SHOWNORMAL)) return 1;
  //else return 0; 
#else
  command = (string) midiplayer + " '" + midifile + "' &";
#endif
  return system(command.c_str());
}

// returns the basename of the given filename
string basename(const string path)
{
  int start, end, len;
  end = path.find_last_not_of(DIRSEPCHARS);
  if (end == (int)string::npos) return "";
  start = path.find_last_of(DIRSEPCHARS, end);
  if (start == (int)string::npos) {
    len = end + 1; start = 0;
  } else {
    len = end - start; start += 1;
  }
  return path.substr(start,len);
}

// determines the full pathname of a given executable (argv[0])
string full_exec_path(const char* program)
{
  char* path;

  // already full pathname? (second test is for Win32)
  if (program[0] == DIRSEPCHAR[0] || (strlen(program) > 1 && program[1] == ':'))
    return (string)program;

  path = getenv("PATH");
  if (!path) return "";
  else return find_file(path, program, PATHSEPCHAR);
}


// get the default directory for the file open dialog when no file is loaded
string get_default_directory()
{
  string defdir = "";
  char* envvar;

#if defined __APPLE__
  envvar = getenv("HOME");
  if (envvar) defdir = envvar;
#elif defined WIN32
  envvar = getenv("HOME"); // in case user has explicitly set it
  if (envvar) {
    defdir = envvar;
  } else {
    envvar = getenv("HOMEDRIVE");
    if (envvar) {
      defdir = envvar;
      envvar = getenv("HOMEPATH");
      if (envvar) defdir += envvar;
    }
  }
#else
  envvar = NULL; // to suppress gcc warning
  char cwd[512];
  if (getcwd(cwd, 512)) {
    defdir = cwd;
  } else { // cannot read current directory
    envvar = getenv("HOME");
    if (envvar)
      defdir = envvar;
    else
      defdir = "";
  }
#endif

  return defdir;
}

// get a unique string derived from the userid
string get_uid_string()
{
  string retval;
#ifdef WIN32
  retval = "";
#else
  char uidstr[16];
  int uidint = (int)getuid();
  sprintf(uidstr,"%d",uidint);
  retval = uidstr;
#endif
  return retval;
}

// set mouse position to given coordinates
// RC: 0 = ok, otherwise error
int set_mouse_position(int x, int y)
{
#if defined WIN32
  if (SetCursorPos(x, y)) return 0;

#elif defined __APPLE__
  /*CGPoint pos;
  pos.x = x;
  pos.y = y;
  if (!CGWarpMouseCursorPosition(pos)) return 0;*/
  return 0;

#else
  // assume this is X11
  Window rootwindow = DefaultRootWindow(fl_display);
  int err = XWarpPointer(fl_display, rootwindow, rootwindow, 0, 0,
                         0, 0, x, y);
  if (err != BadWindow) return 0;
#endif

  // when we have come here, there was an error
  return 1;
}


// missing functions on WIN32
//-----------------------------------------------------------------
#ifdef WIN32

// sets an environment variable
int setenv(const char *name, const char *value, int overwrite)
{
  if (!overwrite && getenv(name))
	return 0;
  if (SetEnvironmentVariable(name, value))
	return 0;
  else
	return -1;
}

// unsets an environment variable
void unsetenv(const char *name)
{
  SetEnvironmentVariable(name, NULL);
}

// reentrant version of strtok
char* strtok_r(char* str, const char* sep, char** last)
{
  char *p, *retval;
  if (str) *last = str;
  if (**last == '\0') return NULL;
  retval = *last;
  for (p = *last; *p; p++) {
	if (strchr(sep,*p)) {
	  // eat empty fields
	  do {
		*p = '\0';
		p++;
	  } while (*p && strchr(sep,*p));
	  break;
	}
  }
  *last = p;
  return retval;
}

#endif
