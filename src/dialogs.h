//
// small dialogs for flabc (dialogs_msk is only for fluid)
//

#ifndef _dialogs_H
#define _dialogs_H

#include "dialogs_msk.h"
#include "flabc.h"
#include <string>

//==========================================================================
// Search dialog
//==========================================================================

class Search : public Search_msk {
 public:
  Search(MainWin* mw);
  int get_search_data(char** rctext, int* rccase, int* rcregexp);
};

//==========================================================================
// Replace dialog
//==========================================================================

class Replace : public Replace_msk {
 public:
  Replace(MainWin* mw);
  virtual void buttonsearch_cb();
  virtual void buttonreplace_cb();
  virtual void buttonreplaceall_cb();
};

//==========================================================================
// Goto Line/Column dialog
//==========================================================================

class GotoLine : public GotoLine_msk {
 public:
  GotoLine(MainWin* mw);
  int get_line_column(int* line, int* col);
};

//==========================================================================
// Goto Bar/Voice/Tune dialog
//==========================================================================

class GotoBar : public GotoBar_msk {
 public:
  GotoBar(MainWin* mw);
  int get_bar_voice_tune(int* bar, string* voice, string* tune);
};

//==========================================================================
// Font browser
//==========================================================================

class FontPicker : public FontPicker_msk {
  char displaytext[400]; // for storing font display string permanently
  IntVector attributes; // for storing font attributes permanently
  IntVector monospaced; // for storing monospaced attribute permanently
  IntVector fontnumbers; // Fl_Font indices of fonts
  StringVector fontnames; // human readable font names
 public:
  FontPicker(int nfonts);
  virtual ~FontPicker();
  virtual void fontbrowser_cb();
  virtual void sizebrowser_cb();
  int get_monospaced_font(string* fontname, int* size, Fl_Font* fontnumber);
};

//==========================================================================
// Options dialog for look & feel
//==========================================================================

class OptionsLook : public OptionsLook_msk {
 private:
  void style2gui(Fl_Button* color,
                 Fl_Check_Button* bold, Fl_Check_Button* italic,
                 Fl_Text_Display::Style_Table_Entry* stylent, int* styleatt);
  void gui2style(Fl_Button* color,
                 Fl_Check_Button* bold, Fl_Check_Button* italic,
                 Fl_Text_Display::Style_Table_Entry* stylent, int* styleatt);
  FontPicker* fontpicker_dialog;
 public:
  OptionsLook(MainWin* mw);
  virtual ~OptionsLook();
  void config2gui(Config* conf);
  virtual void getfont_cb(Fl_Output* output, Fl_Font* fontnum);
  virtual void buttonapply_cb();
  virtual void buttonreset_cb();
  virtual void buttonquit_cb() {config2gui(mainwin->config); window->hide();}
};

//==========================================================================
// Options dialog for system environment
//==========================================================================

class OptionsSystem : public OptionsSystem_msk {
 public:
  OptionsSystem(MainWin* mw);
  void config2gui(Config* conf);
  virtual void buttonapply_cb();
  virtual void buttonreset_cb();
  virtual void buttonquit_cb() {config2gui(mainwin->config); window->hide();}
};

//==========================================================================
// Options dialog for behaviour
//==========================================================================

class OptionsBehaviour : public OptionsBehaviour_msk {
 public:
  OptionsBehaviour(MainWin* mw);
  void config2gui(Config* conf);
  virtual void buttonapply_cb();
  virtual void buttonreset_cb();
  virtual void buttonquit_cb() {config2gui(mainwin->config); window->hide();}
};

//==========================================================================
// Abc format parameter list
//==========================================================================

class AbcFormatList : public AbcFormatList_msk {
 public:
  AbcFormatList(MainWin* mw);
  void load_data(const char* abctab2psoutput, char** filter = NULL);
  virtual void buttoncopy_cb();
};

//==========================================================================
// Midi instrument list
//==========================================================================

class MidiList : public MidiList_msk {
 public:
  MidiList(MainWin* mw);
  // hides midi instruments from -1 terminated tohidelist
  void filter_out(int* tohidelist);
  virtual void buttoncommoninstruments_cb();
  virtual void buttonallinstruments_cb();
  virtual void buttoncopy_cb();
};

//==========================================================================
// Command output (from abctab2ps/abc2midi) window
//==========================================================================

class CommandOutput : public CommandOutput_msk {
 private:
  string command;      // abctab2ps or abc2midi
  int lasterrorindex;  // line in browser with error message
  int bufferskipbegin; // first line in textbuffer skipped
  int bufferskiplines; // number of lines in textbuffer skipped
 public:
  CommandOutput(MainWin* mw);
  void load_data(const char* output, const char* cmd, int skipbegin = 0, int skiplines = 0);
  int nexterror(int* line, int* col);
  virtual void buttonnexterror_cb();
};


//==========================================================================
// About dialog
//==========================================================================

class HelpAbout : public HelpAbout_msk {
 public:
  HelpAbout(MainWin* mw);
  virtual void show();
  virtual void buttonlicense_cb();
};

//==========================================================================
// TextViewer
//==========================================================================

class TextViewer : public TextViewer_msk {
 public:
  TextViewer(MainWin* mw);
  //virtual void show();
  int load_file(const char* filename);
};

//==========================================================================
// Template and Example browser
//==========================================================================

class TemplatePicker : public TemplatePicker_msk {
  StringVector templatefiles; // for storing browser->data permanently
  bool isexamplepicker;
 public:
  TemplatePicker(MainWin* mw, bool examplepicker = false);
  virtual ~TemplatePicker();
  virtual void buttonload_cb();
  void load_template_files(string templatedirs);
  int get_template(string* filename);
};

//==========================================================================
// Fixup Xrefs dialog
//==========================================================================

class FixupXrefs : public FixupXrefs_msk {
 public:
  FixupXrefs(MainWin* mw);
  int get_mode();
};



#endif
