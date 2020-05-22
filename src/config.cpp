//
// stuff related to customzable aspects of the flabcor
//

#include "flabc.h"
#include "config.h"
#include "os.h"
#include "messagebox.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>

string Config::programpath = "";

Config::Config() {
  // get default colors from system setting
  // currently supported by FLTK only on Win32
#ifdef WIN32
  Fl::get_system_colors();
  backgroundcolor = (Fl_Color)Fl::get_color(FL_BACKGROUND2_COLOR);
  selectioncolor = (Fl_Color)Fl::get_color(FL_SELECTION_COLOR);
#else
  backgroundcolor = fl_rgb_color('\xff','\xff','\xff');
  selectioncolor = fl_rgb_color('\xb5','\xd5','\xff');
#endif
  // set fonts
  plainfont = FL_COURIER;
  italicfont = FL_COURIER_ITALIC;
  boldfont = FL_COURIER_BOLD;
  bolditalicfont = FL_COURIER_BOLD_ITALIC;
  // default highlighting styles in text editor
  fontsize = 16;
  Fl_Text_Display::Style_Table_Entry st[] = {
    { FL_BLACK,        plainfont,   16 }, // A - Plain
    { FL_DARK_GREEN,   italicfont,  16 }, // B - Comments
    { FL_BLUE,         plainfont,   16 }, // C - Info fields
    { FL_DARK_MAGENTA, plainfont,   16 }, // D - Voice definitions
    { FL_RED,          boldfont,    16 }, // E - Bar lines
  };
  nstyles = sizeof(st) / sizeof(st[0]);
  styletable = new Fl_Text_Display::Style_Table_Entry[nstyles];
  stylefontattribs = new int[nstyles];
  for (int i=0; i<nstyles; i++) {
    styletable[i] = st[i];
    if (st[i].font == boldfont)
      stylefontattribs[i] = FL_BOLD;
    else if (st[i].font == italicfont)
      stylefontattribs[i] = FL_ITALIC;
    else if (st[i].font == bolditalicfont)
      stylefontattribs[i] = FL_ITALIC | FL_BOLD;
    else
      stylefontattribs[i] = 0;
  }
#ifdef __APPLE__
  scheme = "plastic";
#else
  scheme = "none";
#endif
  visiblefocus = 0;
  abcnodeadkeys = 0;

  // behaviour
  askonexit = 0;
  ignoreshebang = 0;
  defaultabc2ps = "abctab2ps";

  // operating system environment
  abctab2ps = "abctab2ps";
  abcm2ps = "abcm2ps";
  abc2midi = "abc2midi";
  abctabfonts = "";
#ifdef WIN32
  char* tmp = getenv("TMP");
  if (!tmp) tmp = getenv("TEMP");
  if (tmp) { abctmpfile = tmp; pstmpfile = tmp; miditmpfile = tmp; }
  abctmpfile += "\\tune" + get_uid_string() + ".abc";
  pstmpfile += "\\abc" + get_uid_string() + ".ps";
  miditmpfile += "\\abc" + get_uid_string() + ".mid";
  docdirs = "";
  templatedirs = "";
  exampledirs = "";
#else
  abctmpfile = "/tmp/tune" + get_uid_string() + ".abc";
  pstmpfile = "/tmp/abc" + get_uid_string() + ".ps";
  miditmpfile = "/tmp/abc" + get_uid_string() + ".mid";
  docdirs = "/usr/share/doc/flabc;/usr/local/share/doc/flabc;/usr/share/doc/abctab2ps;/usr/local/share/doc/abctab2ps";
  templatedirs = "/usr/share/flabc/templates;/usr/local/share/flabc/templates";
  exampledirs = "/usr/share/flabc/examples;/usr/local/share/flabc/examples";
#endif
#ifdef __APPLE__
  psviewer = "psview";
#else
  psviewer = "gv";
#endif
  midiplayer = "timidity -ia";
  webbrowser = "firefox;mozilla;konqueror;opera";

  AddBundleDirs();
}

Config::~Config() {
  delete[] styletable;
  delete[] stylefontattribs;
}

void Config::LoadPrefs() {
  int intValue,i;
  char* textValue;
  char buffer[80];
  int fontnum;
  Config def;

  Fl_Preferences app(Fl_Preferences::USER, "fltk.org", "flabc/preferences");
  // ----------------------------------------------------------------
  Fl_Preferences look(app, "LookAndFeel");
  look.get("fontsize", intValue, def.fontsize);
  fontsize = intValue;
  look.get("backgroundcolor", intValue, def.backgroundcolor);
  backgroundcolor = (Fl_Color)intValue;
  look.get("selectioncolor", intValue, def.selectioncolor);
  selectioncolor = (Fl_Color)intValue;
  look.get("plainfont", textValue, Fl::get_font_name(def.plainfont));
  if (-1 <= (fontnum = GetFontByName(textValue)))
    plainfont = (Fl_Font)fontnum;
  else plainfont = def.plainfont;
  free(textValue);
  look.get("boldfont", textValue, Fl::get_font_name(def.boldfont));
  if (-1 <= (fontnum = GetFontByName(textValue)))
    boldfont = (Fl_Font)fontnum;
  else boldfont = def.boldfont;
  free(textValue);
  look.get("italicfont", textValue, Fl::get_font_name(def.italicfont));
  if (-1 <= (fontnum = GetFontByName(textValue)))
    italicfont = (Fl_Font)fontnum;
  else italicfont = def.italicfont;
  free(textValue);
  look.get("bolditalicfont", textValue, Fl::get_font_name(def.bolditalicfont));
  if (-1 <= (fontnum = GetFontByName(textValue)))
    bolditalicfont = (Fl_Font)fontnum;
  else bolditalicfont = def.bolditalicfont;
  free(textValue);
  for (i=0; i<nstyles; i++) {
    sprintf(buffer, "color%d", i);
    look.get(buffer, intValue, def.styletable[i].color);
    styletable[i].color = (Fl_Color)intValue;
    sprintf(buffer, "styleattrib%d", i);
    look.get(buffer, intValue, def.stylefontattribs[i]);
    styletable[i].font = GetFontByAttribs(intValue);
    styletable[i].size = fontsize;
  }
  look.get("scheme", textValue, def.scheme.c_str());
  scheme = textValue; free(textValue);
  look.get("visiblefocus", intValue, def.visiblefocus);
  visiblefocus = intValue;
  look.get("abcnodeadkeys", intValue, def.abcnodeadkeys);
  abcnodeadkeys = intValue;
  // ----------------------------------------------------------------
  Fl_Preferences behaviour(app, "Behaviour");
  behaviour.get("askonexit", intValue, def.askonexit);
  askonexit = intValue;
  behaviour.get("ignoreshebang", intValue, def.ignoreshebang);
  ignoreshebang = intValue;
  behaviour.get("defaultabc2ps", textValue, def.defaultabc2ps.c_str());
  defaultabc2ps = textValue; free(textValue);
  // ----------------------------------------------------------------
  Fl_Preferences system(app, "SystemEnvironment");
  system.get("abctab2ps", textValue, def.abctab2ps.c_str());
  abctab2ps = textValue; free(textValue);
  system.get("abcm2ps", textValue, def.abcm2ps.c_str());
  abcm2ps = textValue; free(textValue);
  system.get("abctabfonts", textValue, def.abctabfonts.c_str());
  abctabfonts = textValue; free(textValue);
  system.get("abctmpfile", textValue, def.abctmpfile.c_str());
  abctmpfile = textValue; free(textValue);
  system.get("pstmpfile", textValue, def.pstmpfile.c_str());
  pstmpfile = textValue; free(textValue);
  system.get("psviewer", textValue, def.psviewer.c_str());
  psviewer = textValue; free(textValue);
  system.get("docdirs", textValue, def.docdirs.c_str());
  docdirs = textValue; free(textValue);
  system.get("webbrowser", textValue, def.webbrowser.c_str());
  webbrowser = textValue; free(textValue);
  system.get("templatedirs", textValue, def.templatedirs.c_str());
  templatedirs = textValue; free(textValue);
  system.get("exampledirs", textValue, def.exampledirs.c_str());
  exampledirs = textValue; free(textValue);
  system.get("midiplayer", textValue, def.midiplayer.c_str());
  midiplayer = textValue; free(textValue);

  AddBundleDirs();
}

string Config::WritePrefs() {
  int i;
  char buffer[80];

  Fl_Preferences app(Fl_Preferences::USER, "fltk.org", "flabc/preferences");
  // ---------------------------------------------------------
  Fl_Preferences look(app, "LookAndFeel");
  look.set("fontsize", fontsize);
  look.set("backgroundcolor", (int)backgroundcolor);
  look.set("selectioncolor", (int)selectioncolor);
  look.set("plainfont", SetFontName(plainfont).c_str());
  look.set("boldfont", SetFontName(boldfont).c_str());
  look.set("italicfont", SetFontName(italicfont).c_str());
  look.set("bolditalicfont", SetFontName(bolditalicfont).c_str());
  for (i=0; i<nstyles; i++) {
    sprintf(buffer, "color%d", i);
    look.set(buffer, (int)styletable[i].color);
    sprintf(buffer, "styleattrib%d", i);
    look.set(buffer, stylefontattribs[i]);
  }
  look.set("scheme", scheme.c_str());
  look.set("visiblefocus", visiblefocus);
  look.set("abcnodeadkeys", abcnodeadkeys);
  // ---------------------------------------------------------
  Fl_Preferences behaviour(app, "Behaviour");
  behaviour.set("askonexit", askonexit);
  behaviour.set("ignoreshebang", ignoreshebang);
  behaviour.set("defaultabc2ps", defaultabc2ps.c_str());
  // ---------------------------------------------------------
  Fl_Preferences system(app, "SystemEnvironment");
  system.set("abctab2ps", abctab2ps.c_str());
  system.set("abcm2ps", abcm2ps.c_str());
  system.set("abc2midi", abc2midi.c_str());
  system.set("abctabfonts", abctabfonts.c_str());
  system.set("abctmpfile", abctmpfile.c_str());
  system.set("pstmpfile", pstmpfile.c_str());
  system.set("psviewer", psviewer.c_str());
  system.set("docdirs", docdirs.c_str());
  system.set("webbrowser", webbrowser.c_str());
  system.set("templatedirs", templatedirs.c_str());
  system.set("exampledirs", exampledirs.c_str());
  system.set("midiplayer", midiplayer.c_str());

  // return pathname where preferences are stored
  char path[FL_PATH_MAX];
  app.getUserdataPath(path, FL_PATH_MAX);
  // this is an ugly hack that assumes a specific form of path:
  // /path/to/preferences/ while the actual file is /path/to/preferences.prefs
  size_t n = strlen(path);
  if (n > 0 && path[n-1] == '/') path[n-1] = 0;
  return (string(path) + ".prefs");
}

// checks whether the given font number is one of
// the FLTK default fonts
bool Config::IsFltkBaseFont(Fl_Font fontnum)
{
  return (fontnum < FL_FREE_FONT);
}

// builds a font name for storing in preferences file
string Config::SetFontName(Fl_Font fontnum)
{
  char numstr[16];
  string fontstr;
  if (fontnum < FL_FREE_FONT) {
    sprintf(numstr, "%d", (int)fontnum);
    fontstr = (string)"FLTK Default " + numstr;
  } else {
    fontstr = Fl::get_font_name(fontnum);
  }
  return fontstr;
}

// looks up a font in system fonts and returns its number
// when the font is not found, -1 is returned
int Config::GetFontByName(char* name)
{
  int i;
  const char* prefix = "FLTK Default";
  int prefixlen = strlen(prefix);

  // check for our private names for FLTK base fonts
  if (0 == strncmp(name, prefix, prefixlen)) {
    return atoi(name + prefixlen);
  }

  // look up system fonts
  for (i=0; i<global_nfonts; i++) {
    if (0 == strcmp(name, Fl::get_font_name((Fl_Font)i)))
      return i;
  }

  // nothing found
  return -1;
}

// return font number from preferences fonts that has the given attributes
Fl_Font Config::GetFontByAttribs(int attribs)
{
  if ((attribs & FL_BOLD) && (attribs & FL_ITALIC))
    return bolditalicfont;
  if (attribs & FL_BOLD)
    return boldfont;
  if (attribs & FL_ITALIC)
    return italicfont;
  return plainfont;
}

// add application bundle directories to search paths
void Config::AddBundleDirs() {
  string pgmpostfix, bundledocdir, bundlebindir;
  string bundlefontdir, bundletemplatedir, bundleexampledir;
#ifdef WIN32
  pgmpostfix = "bin\\flabc";
  bundledocdir = "doc";
  bundlebindir = "bin";
  bundlefontdir = "fonts";
  bundletemplatedir = "templates";
  bundleexampledir = "examples";
  templatedirs = "";
#endif
#ifdef __APPLE__
  pgmpostfix = "MacOS/flabc";
  bundledocdir = "Resources/doc";
  bundlebindir = "Resources/bin";
  bundlefontdir = "Resources/fonts";
  bundletemplatedir = "Resources/templates";
  bundleexampledir = "Resources/examples";
#endif
  // the following will only apply to Win32 and OSX
  if (!Config::programpath.empty() && !pgmpostfix.empty()) {
    string bundledir;
    char* path;
    int pos = Config::programpath.find(pgmpostfix);
    if (pos != (int)string::npos) {
#ifdef __APPLE__
      // just in case stored preferences hav a different bundle location
      docdirs = RemoveMatchingTokens(docdirs.c_str(),
                                     "Contents/Resources/doc", ';');
      abctabfonts = RemoveMatchingTokens(abctabfonts.c_str(),
                                         "Contents/Resources/fonts", ';');
      templatedirs = RemoveMatchingTokens(templatedirs.c_str(),
                                          "Contents/Resources/templates", ';');
      exampledirs = RemoveMatchingTokens(exampledirs.c_str(),
                                          "Contents/Resources/examples", ';');
#endif
      bundledir = Config::programpath.substr(0,pos);
      bundledocdir = bundledir + bundledocdir;
      if (string::npos == docdirs.find(bundledocdir)) {
        if (docdirs.empty()) docdirs = bundledocdir;
        else docdirs = bundledocdir + ";" + docdirs;
      }
      bundlebindir = bundledir + bundlebindir;
      if (((path = getenv("PATH"))) && !strstr(path,bundlebindir.c_str())) {
        bundlebindir += (string)PATHSEPCHAR + path;
        setenv("PATH", bundlebindir.c_str(), 1);
      }
      bundlefontdir = bundledir + bundlefontdir;
      if (string::npos == abctabfonts.find(bundlefontdir)) {
        if (abctabfonts.empty()) abctabfonts = bundlefontdir;
        else abctabfonts = bundlefontdir + ";" + abctabfonts;
      }
      bundletemplatedir = bundledir + bundletemplatedir;
      if (string::npos == templatedirs.find(bundletemplatedir)) {
        if (templatedirs.empty()) templatedirs = bundletemplatedir;
        else templatedirs = bundletemplatedir + ";" + templatedirs;
      }
      bundleexampledir = bundledir + bundleexampledir;
      if (string::npos == exampledirs.find(bundleexampledir)) {
        if (exampledirs.empty()) exampledirs = bundleexampledir;
        else exampledirs = bundleexampledir + ";" + exampledirs;
      }
    }
  }
}

// removes from a sepchar seperated list toklist all entries that
// have matchstr as a substring
string Config::RemoveMatchingTokens(const char* toklist, const char* matchstr, char sepchar)
{
  char *toklistcopy, *tok;
  string newstr = "";
  char sepchars[2];
  sepchars[0] = sepchar; sepchars[1] = 0;
  toklistcopy = (char*)alloca(sizeof(char)*(strlen(toklist)+4));
  strcpy(toklistcopy, toklist);

  tok = strtok(toklistcopy,sepchars);
  while (tok) {
    if (!strstr(tok, matchstr)) {
      if (!newstr.empty()) newstr += sepchar;
      newstr += tok;
    }
    tok = strtok(NULL,sepchars);
  }
  return newstr;

}
