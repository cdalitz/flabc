//
// stuff related to the editor widget like syntax highlighting etc.
//

#ifndef _editor_H
#define _editor_H

#include <string>
#include <vector>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Display.H>

using namespace std;

//--------------------------------------------------------------------
// for undoing/redoing
//--------------------------------------------------------------------

// undo information for a single operation
class UndoOperation {
 public:
  // undo data
  int pos, nInserted, nDeleted, nRestyled;
  string deletedText;
  // constructors
  UndoOperation(int p, int nI, int nD,int nR, const char* text) {
    pos = p; nInserted = nI; nDeleted = nD; nRestyled = nR;
    deletedText = (text ? text : "");
  }
  UndoOperation(const UndoOperation &u) {
    pos = u.pos; nInserted = u.nInserted; nDeleted = u.nDeleted;
    nRestyled = u.nRestyled; deletedText = u.deletedText;
  }
  UndoOperation operator=(UndoOperation u) {
    pos = u.pos; nInserted = u.nInserted; nDeleted = u.nDeleted;
    nRestyled = u.nRestyled; deletedText = u.deletedText;
    return *this;
  }
};

typedef vector<UndoOperation> UndoVector;

//--------------------------------------------------------------------
// for syntax highlighting
//--------------------------------------------------------------------

class SyntaxHighlighter {
 public:
  Fl_Text_Buffer* textbuffer;
  Fl_Text_Buffer* stylebuffer;
  Fl_Text_Editor* editor;
  Fl_Text_Display::Style_Table_Entry* styletable;
  int nstyles;

  SyntaxHighlighter(Fl_Text_Buffer* textbuf, Fl_Text_Editor* edit,
                    Fl_Text_Display::Style_Table_Entry* st, int nstyles);
  // update style buffer
  static void style_update(int pos,        // I - Position of update
                  int nInserted,           // I - Number of inserted chars
                  int nDeleted,            // I - Number of deleted chars
                  int nRestyled,           // I - Number of restyled chars
                  const char* deletedText, // I - Text that was deleted
                  void* thisobject);       // I - Callback data
  // parse text and produce style data.
  void style_parse(const char *text, char *style, int length);
  static void style_unfinished_cb(int, void*) {}
};

//--------------------------------------------------------------------
// Apple only: conversion from macroman to latin1 in textbuffer
//--------------------------------------------------------------------
#ifdef __APPLE__
void macroman_to_latin1(int pos,           // I - Position of update
                  int nInserted,           // I - Number of inserted chars
                  int nDeleted,            // I - Number of deleted chars
                  int nRestyled,           // I - Number of restyled chars
                  const char* deletedText, // I - Text that was deleted
                  void* textbuffer);       // I - Fl_Text_Buffer
#endif

#endif
