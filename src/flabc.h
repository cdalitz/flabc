//
// global variable declarations for flabc
//

#ifndef _flabc_H
#define _flabc_H

#include <string>
#include <vector>
#include "mainwin.h"

#define VERSION  "1.3.0"
#define VERSIONDATE "(06. Feb 2025)"

using namespace std;

// some common types
typedef vector<int> IntVector;
typedef vector<string> StringVector;
typedef vector<char*> C_StrVector;
typedef vector<MainWin*> MainWinVector;

// global variable for font access
extern int global_nfonts;

#endif
