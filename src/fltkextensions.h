//
// classes derived from FLTK classes for slightly different behaviour
//

#ifndef _fltkextensions_H
#define _fltkextensions_H

#include <FL/Fl_Tile.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Editor.H>

//-----------------------------------------------------------------
// Fl_Tile from FLTK uses different resize behaviour
// This class resets this behaviour to the same as in Fl_Group
//-----------------------------------------------------------------

class resizable_Fl_Tile : public Fl_Tile {
 public:
  resizable_Fl_Tile(int x, int y, int w, int h, const char *label = 0);
  virtual void resize(int x, int y, int w, int h);
};

//-----------------------------------------------------------------
// Fl_Input moves to next Widget in focus list when arrow keys
// are pressed. This derived class keeps the cursor in the text area
// and initializes it to the start of the entry rather than the end.
//-----------------------------------------------------------------

class noarrow_Fl_Input : public Fl_Input {
 public:
  noarrow_Fl_Input(int x, int y, int w, int h, const char *label = 0);
  virtual int handle(int event);
  virtual int value(const char* text);
  virtual const char* value() {return Fl_Input::value();}
};

//-----------------------------------------------------------------
// fix some problems with Fl_Text_Editor/Buffer/Display
//-----------------------------------------------------------------

class custom_Fl_Text_Buffer : public Fl_Text_Buffer {
  void remove_cr(); // removes '\r' from the buffer
 public:
  custom_Fl_Text_Buffer(int requestedSize = 0);
  virtual ~custom_Fl_Text_Buffer() {}
  virtual int loadfile(const char *file, bool fromlatin1=true, int buflen = 128*1024);
  virtual int insertfile(const char *file, int pos, bool fromlatin1=true, int buflen = 128*1024);
  virtual int savefile(const char *file, bool tolatin1=true, int buflen = 128*1024);
  virtual int search_regex(int pos, const char* regex, int* foundPos, int matchCase);
};

class custom_Fl_Text_Editor : public Fl_Text_Editor {
 public:
  Fl_Color deadkeycolor;
  Fl_Color highlightcolor;
  int deadkeystate;  // 1 when deadkey pending, 0 otherwise
  int abcnodeadkeys; // when true, ^ and ~ are no dead keys on OSX
  custom_Fl_Text_Editor(int X, int Y, int W, int H, const char* l = 0);
  virtual ~custom_Fl_Text_Editor() {}
  virtual int handle(int event);
  virtual int handle_old(int event);
};

//-----------------------------------------------------------------
// encoding conversion for MacOS X
//-----------------------------------------------------------------

// converts the given text to latin1 and vice versa
void macroman_to_latin1(char* text);
void latin1_to_macroman(char* text);

#endif
