//
// stuff related to customzable aspects of the flabcor
//

#ifndef _config_H
#define _config_H

#include <FL/Fl_Text_Display.H>
#include <string>

using namespace std;

class Config {
 public:
  // look & feel
  Fl_Font plainfont, italicfont, boldfont, bolditalicfont;
  int* stylefontattribs;     // whether style uses bold and/or italic
  int fontsize;
  Fl_Color backgroundcolor;  // background2 in FLTK
  Fl_Color selectioncolor;   // for selected text
  Fl_Text_Display::Style_Table_Entry* styletable;
  int nstyles;
  string scheme;    // "plastic" or "none"
  int visiblefocus; // dashed boxes when focus
  bool abcnodeadkeys; // ^ and ~ no dead keys on OSX

  // behaviour
  bool askonexit;       // ask on exit when buffer changed
  bool ignoreshebang;   // whether %! line shall be ignored
  string defaultabc2ps; // abctab2ps or abcm2ps

  // for finding application bundle resources on OSX and Win32
  static string programpath;

  // operating system environment specific settings
  string abctab2ps;  // abctab2ps command
  string abcm2ps;    // abcm2ps command
  string abctabfonts; // additional font directories
  string abctmpfile; // temporary file for storing abc output
  string pstmpfile;  // temporary file for storing PS output
  string psviewer;   // psview command
  string abc2midi;   // abc2midi command
  string midiplayer; // midi player command
  string miditmpfile;// temporary file for storing Midi output
  string docdirs;    // semicolon seperated list of directories
  string webbrowser; // webbrowser command (Linux only)
  string templatedirs; // semicolon seperated list of directories
  string exampledirs; // semicolon seperated list of directories

  // methods
  Config();
  ~Config();
  void LoadPrefs();
  string WritePrefs();  // returns the pathname where preferences are stored
  bool IsFltkBaseFont(Fl_Font fontnum);
  string SetFontName(Fl_Font fontnum); // build name for storing in preferences
  int GetFontByName(char* name); // look up in system fonts
  Fl_Font GetFontByAttribs(int attribs);
  void AddBundleDirs(); // adds directories of application bundle (w32, osx)
  string RemoveMatchingTokens(const char* toklist, const char* matchstr, char sepchar);
};



#endif
