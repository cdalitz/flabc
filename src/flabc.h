//
// global variable declarations for flabc
//

#ifndef _flabc_H
#define _flabc_H

#include <string>
#include <vector>
#include "mainwin.h"

#define VERSION  "1.2.2"
#define VERSIONDATE "(13. Oct 2022)"

using namespace std;

// some common types
typedef vector<int> IntVector;
typedef vector<string> StringVector;
typedef vector<char*> C_StrVector;
typedef vector<MainWin*> MainWinVector;

// global variable for font access
extern int global_nfonts;

#endif
