//
// Fl_Tile from FLTK uses different resize behaviour
// This class resets this behaviour to the same as in Fl_Group
//

#include <FL/Fl.H>
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <FL/fl_utf8.h>

#include "fltkextensions.h"
#include "utils.h"
#include "os.h"
#include "messagebox.h"

//-----------------------------------------------------------------
// modified Fl_Tile
//-----------------------------------------------------------------

resizable_Fl_Tile::resizable_Fl_Tile(int x, int y, int w, int h, const char *label) :
  Fl_Tile(x, y, w, h, label) {}
void resizable_Fl_Tile::resize(int x, int y, int w, int h) {
  this->Fl_Group::resize(x,y,w,h);
}

//-----------------------------------------------------------------
// modified Fl_Input
//-----------------------------------------------------------------

noarrow_Fl_Input::noarrow_Fl_Input(int x, int y, int w, int h, const char *label) :
  Fl_Input(x, y, w, h, label) {}

int noarrow_Fl_Input::handle(int event)
{
  if (event == FL_KEYBOARD) {
    int c = Fl::event_key();
    if ((c == FL_Left && position() == 0) ||
        (c == FL_Right && position() >= size()))
        return 1;
  }
  return Fl_Input::handle(event);
}

int noarrow_Fl_Input::value(const char* text)
{
  int rc;
  rc = Fl_Input::value(text);
  position(0);
  return rc;
}


//-----------------------------------------------------------------
// modified Fl_Text_Buffer
//-----------------------------------------------------------------

custom_Fl_Text_Buffer::custom_Fl_Text_Buffer(int requestedSize) :
  Fl_Text_Buffer(requestedSize) {}

// This function should not be used, because it is too slow
// (due to syntax highlighting callback)
// remove carriage returns from buffer
void custom_Fl_Text_Buffer::remove_cr()
{
  int pos = 0;
  while (pos < length() && findchar_forward(pos, '\r', &pos)) {
    if (char_at(pos+1) == '\n') {
      remove(pos,pos+1);
    } else {
      replace(pos,pos+1,"\n"); // newlines missing in buffer
    }
    pos++;
  }
}

// we need own implementation for removing \r
// and converting latin1 <-> utf8

int custom_Fl_Text_Buffer::loadfile(const char *file, bool fromlatin1, int buflen)
{
  //int rc = Fl_Text_Buffer::loadfile(file, buflen);
  char* buffer;
  string s;

  int rc = readfile(file, &s);
  if (0 == rc) { // no error
    if (s.rfind("%!abcm2ps", 0) == 0)
      fromlatin1 = false;
    else if (s.rfind("%!abctab2ps", 0) == 0)
      fromlatin1 = true;
    if (!fromlatin1) {
      text(s.c_str());
    } else {
      // latin1 -> utf8
      size_t buflen = s.size()*4+1;
      buffer = (char*)malloc(sizeof(char)*buflen);
      fl_utf8froma(buffer, buflen, s.c_str(), s.size());
      text(buffer);
      free(buffer);
    }
  }
  return rc;
}

int custom_Fl_Text_Buffer::insertfile(const char *file, int pos, bool fromlatin1, int buflen)
{
  char* buffer;
  string s;

  int rc = readfile(file, &s);
  if (0 == rc) { // no error
    if (s.rfind("%!abcm2ps", 0) == 0)
      fromlatin1 = false;
    else if (s.rfind("%!abctab2ps", 0) == 0)
      fromlatin1 = true;
    if (!fromlatin1) {
      insert(pos, s.c_str());
    } else {
      // latin1 -> utf8
      size_t buflen = s.size()*4+1;
      buffer = (char*)malloc(sizeof(char)*buflen);
      fl_utf8froma(buffer, buflen, s.c_str(), s.size());
      insert(pos, buffer);
      free(buffer);
    }
  }
  return rc;
}

int custom_Fl_Text_Buffer::savefile(const char *file, bool tolatin1, int buflen)
{
  int rc;
  FILE *fp;
  if (!(fp = fopen(file, "w"))) return 1;
  char* s = (char*)text();
  if (!tolatin1) {
    if (EOF == fputs(s, fp)) rc = 2; else rc = 0;
  } else {
    // utf8 -> latin1
    char* t = (char*)malloc(sizeof(char)*(length()+1));
    fl_utf8toa(s, length(), t, length()+1);
    if (EOF == fputs(t, fp)) rc = 2; else rc = 0;
    free(t);
  }
  free(s);
  fclose(fp);
  return rc;
}

// returns the number of characters matched or zero when no match
// on error -1 is returned
int custom_Fl_Text_Buffer::search_regex(int pos, const char* regex, int* foundPos, int matchCase)
{
  *foundPos = pos;
#ifdef WIN32
  // regexps not supported
  message_box("Sorry, regular expressions not supported on Windows!");
  return 0;
#else
  int cflags, err;
  int nchars = 0;
  regex_t re;
  regmatch_t rematch[2];
  if (matchCase) cflags = REG_EXTENDED | REG_NEWLINE;
  else cflags = REG_EXTENDED | REG_NEWLINE | REG_ICASE;
  err = regcomp(&re, regex, cflags);
  if (err) {
    char errstr[256];
    regerror(err, &re, errstr, 256);
    string msg = "Error in Regexp provided for search:\n";
    msg += errstr;
    message_box(msg.c_str());
    regfree(&re);
    return -1;
  }
  char* buftext = text_range(pos,length());
  if (0 == regexec(&re, buftext, 1, rematch, 0)) {
    *foundPos = pos + rematch[0].rm_so;
    nchars = rematch[0].rm_eo - rematch[0].rm_so;
  }
  free(buftext);
  regfree(&re);
  return nchars;
#endif
}

//-----------------------------------------------------------------
// modified Fl_Text_Editor
//-----------------------------------------------------------------

static void kill_selection(Fl_Text_Editor* e) {
  if (e->buffer()->selected()) {
    e->insert_position(e->buffer()->primary_selection()->start());
    e->buffer()->remove_selection();
  }
}

custom_Fl_Text_Editor::custom_Fl_Text_Editor(int X, int Y, int W, int H, const char* l) :
  Fl_Text_Editor(X,Y,W,H,l) 
{
  deadkeystate = 0;
  deadkeycolor = fl_rgb_color('\xff','\xcc','\x55');
  highlightcolor = selection_color();
  abcnodeadkeys = 0;
}

int custom_Fl_Text_Editor::handle(int event)
{
#ifdef __APPLE__
  if (event == FL_KEYBOARD) {

    // when option set, do not handle ^ and ~ as dead keys
    if (abcnodeadkeys) {
      int rc = 0;
      char to_insert;
      char key = Fl::event_key();
      // circumflex
      if (key == '\x5e' && !Fl::event_state(FL_SHIFT)) {
        to_insert = '^';
        rc = 1;
      } 
      // tilde
      else if (Fl::event_state(FL_ALT) && key == '\x6e') {
        to_insert = '~';
        rc = 1;
      }
      if (rc) {
        int pos; char insertstr[2];
        insertstr[0] = to_insert; insertstr[1] = 0;
        pos = insert_position();
        buffer()->insert(pos++, insertstr);
        insert_position(pos);
        return rc;
      }
    }

    // otherwise make dead key handling "Cocoa-like"
    int del;
    char* e_text = (char*)Fl::event_text();
    // special treatment of space after deadkey:
    // add no symbol, but materialize deadkey
    if (deadkeystate && 0==strcmp(e_text," ")) {
      buffer()->unselect();
      selection_color(highlightcolor);
      deadkeystate = 0;
      Fl::compose(del); // to reset dead key state in Fl
      return 1;
    }
    // special treatment of keys ignored by Fl::compose
    if (deadkeystate && (0==strcmp(e_text,"\t") || !*e_text)) {
      buffer()->unselect();
      selection_color(highlightcolor);
      deadkeystate = 0;
      Fl::compose(del); // to reset dead key state in Fl
      if (*e_text) {
        insert(e_text);
        return 1;
      } else {
        return Fl_Text_Editor::handle_key();
      }
    }
    if (Fl::compose(del)) {
      if (del) {
        // second part of compose character: mark pending accent for deletion
        buffer()->select(insert_position()-del, insert_position());
      } else if (deadkeystate) {
        // remove highlighting from first part of compose character 
        buffer()->unselect();
      }
      kill_selection(this);
      if (deadkeystate) selection_color(highlightcolor);
      deadkeystate = 0;
      if (Fl::event_length()) {
        if (insert_mode()) insert(Fl::event_text());
        else overstrike(Fl::event_text());
        // visualize pending deadkey character
        if (Fl::compose_state) {
          highlightcolor = selection_color();
          selection_color(deadkeycolor);
          buffer()->select(insert_position()-1, insert_position());
          deadkeystate = 1;
        }
      }
      show_insert_position();
      set_changed();
      if (when()&FL_WHEN_CHANGED) do_callback();
      return 1;
    }
    else {
      return Fl_Text_Editor::handle_key();
    }
  }
#endif
  return Fl_Text_Editor::handle(event);
}

int custom_Fl_Text_Editor::handle_old(int event)
{
#ifdef __APPLE__
  // Fl::compose() does not work on OSX
  // thus we catch abc relevant dead keys explicitly and
  // insert them immediately without waiting for a second keystrokes
  // WARNING: tested only on german keyboards
  if (event == FL_KEYBOARD) {
    int rc = 0;
    char to_insert;
    char key = Fl::event_key();
    //printf("key %x\n", key);
    // circumflex
    if (key == '\x5e' && !Fl::event_state(FL_SHIFT)) {
      to_insert = '^';
      rc = 1;
    } 
    // tilde
    else if (Fl::event_state(FL_ALT) && key == '\x6e') {
      to_insert = '~';
      rc = 1;
    }

    // insert "dead key" or pass key on to base class
    if (rc) {
      int pos; char insertstr[2];
      insertstr[0] = to_insert; insertstr[1] = 0;
      Fl_Text_Buffer* buf = buffer();
      pos = insert_position();
      buf->insert(pos++, insertstr);
      insert_position(pos);
    }
    else {
      rc = Fl_Text_Editor::handle_key();
    }
    return rc;
  }
#endif
  return Fl_Text_Editor::handle(event);
}


//-----------------------------------------------------------------
// encoding conversion for FLTK 1.1 on MacOS X
//-----------------------------------------------------------------

// converts the given text from macroman to latin1
void macroman_to_latin1(char* text)
{
  static uchar roman2latin[128] = {
    0xc4, 0xc5, 0xc7, 0xc9, 0xd1, 0xd6, 0xdc, 0xe1, 0xe0, 0xe2, 0xe4, 0xe3, 0xe5, 0xe7, 0xe9, 0xe8, 
    0xea, 0xeb, 0xed, 0xec, 0xee, 0xef, 0xf1, 0xf3, 0xf2, 0xf4, 0xf6, 0xf5, 0xfa, 0xf9, 0xfb, 0xfc, 
    0x86, 0xb0, 0xa2, 0xa3, 0xa7, 0x95, 0xb6, 0xdf, 0xae, 0xa9, 0x99, 0xb4, 0xa8, 0xbf, 0xc6, 0xd8, 
    0xbf, 0xb1, 0xbf, 0xbf, 0xa5, 0xb5, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xaa, 0xba, 0xbf, 0xe6, 0xf8, 
    0xbf, 0xa1, 0xac, 0xbf, 0x83, 0xbf, 0xbf, 0xab, 0xbb, 0x85, 0xa0, 0xc0, 0xc3, 0xd5, 0x8c, 0x9c, 
    0x96, 0x97, 0x93, 0x94, 0x91, 0x92, 0xf7, 0xbf, 0xff, 0x9f, 0xbf, 0x80, 0x8b, 0x9b, 0xbf, 0xbf, 
    0x87, 0xb7, 0x82, 0x84, 0x89, 0xc2, 0xca, 0xc1, 0xcb, 0xc8, 0xcd, 0xce, 0xcf, 0xcc, 0xd3, 0xd4, 
    0xbf, 0xd2, 0xda, 0xdb, 0xd9, 0xbf, 0x88, 0x98, 0xaf, 0xbf, 0xbf, 0xbf, 0xb8, 0xbf, 0xbf, 0xbf
  };
  char* p;
  uchar c;
  for (p = text; *p; p++) {
    c = (uchar) *p;
    if (c > 127)
      *p = roman2latin[c-128];
  }
}

// converts the given text from latin1 to macroman
void latin1_to_macroman(char* text)
{
  static uchar latin2roman[128] = {
    0xdb, 0xc0, 0xe2, 0xc4, 0xe3, 0xc9, 0xa0, 0xe0, 0xf6, 0xe4, 0xc0, 0xdc, 0xce, 0xc0, 0xc0, 0xc0, 
    0xc0, 0xd4, 0xd5, 0xd2, 0xd3, 0xa5, 0xd0, 0xd1, 0xf7, 0xaa, 0xc0, 0xdd, 0xcf, 0xc0, 0xc0, 0xd9, 
    0xca, 0xc1, 0xa2, 0xa3, 0xc0, 0xb4, 0xc0, 0xa4, 0xac, 0xa9, 0xbb, 0xc7, 0xc2, 0xc0, 0xa8, 0xf8, 
    0xa1, 0xb1, 0xc0, 0xc0, 0xab, 0xb5, 0xa6, 0xe1, 0xfc, 0xc0, 0xbc, 0xc8, 0xc0, 0xc0, 0xc0, 0xc0, 
    0xcb, 0xe7, 0xe5, 0xcc, 0x80, 0x81, 0xae, 0x82, 0xe9, 0x83, 0xe6, 0xe8, 0xed, 0xea, 0xeb, 0xec, 
    0xc0, 0x84, 0xf1, 0xee, 0xef, 0xcd, 0x85, 0xc0, 0xaf, 0xf4, 0xf2, 0xf3, 0x86, 0xc0, 0xc0, 0xa7, 
    0x88, 0x87, 0x89, 0x8b, 0x8a, 0x8c, 0xbe, 0x8d, 0x8f, 0x8e, 0x90, 0x91, 0x93, 0x92, 0x94, 0x95, 
    0xc0, 0x96, 0x98, 0x97, 0x99, 0x9b, 0x9a, 0xd6, 0xbf, 0x9d, 0x9c, 0x9e, 0x9f, 0xc0, 0xc0, 0xd8
  };
  char* p;
  uchar c;
  for (p = text; *p; p++) {
    c = (uchar) *p;
    if (c > 127)
      *p = latin2roman[c-128];
  }
}
