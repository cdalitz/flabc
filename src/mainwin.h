//
// Actual Main Window (MainWin_msk is only base class for GUI stuff)
//

#ifndef _mainwin_H
#define _mainwin_H

#include "mainwin_msk.h"
#include "editor.h"
#include "config.h"
#include "messagebox.h"

#include <string>

using namespace std;

// forward declaration to avoid cyclyc dependency
class OptionsLook;
class OptionsBehaviour;
class OptionsSystem;
class AbcFormatList;
class CommandOutput;
class HelpAbout;
class GotoLine;
class GotoBar;
class Search;
class Replace;
class MidiList;
class TextViewer;
class TemplatePicker;
class FixupXrefs;

class MainWin : public MainWin_msk {
 private:
  bool loading, undoing, redoing;
  UndoVector undovector, redovector;
  SyntaxHighlighter* highlight;
  OptionsLook* optionslook_dialog;
  OptionsBehaviour* optionsbehaviour_dialog;
  OptionsSystem* optionssystem_dialog;
  AbcFormatList* abcformatlist_dialog;
  MidiList* midilist_dialog;
  CommandOutput* commandoutput_dialog;
  HelpAbout* helpabout_dialog;
  GotoLine* gotoline_dialog;
  GotoBar* gotobar_dialog;
  Search* search_dialog;
  Replace* replace_dialog;
  TemplatePicker *template_dialog, *example_dialog;
  FixupXrefs* fixupxrefs_dialog;
  char* abc_filename_pattern;
  bool exampleloaded;
 public:
  Config* config;
  MainWin(Config* conf);
  bool changed;
  string filename;
  TextViewer* textview_dialog;
  // which abc*2ps program shall be called?
  string which_abc2ps_command();
  // file operations
  void new_file(const char* name = NULL);
  void load_file(const char* name);
  void save_file(const char* name);
  int ask_for_save();
  // editor tools
  int find_string(const char* text, int matchcase, int regexp);
  void goto_line(int line, int col = 0);
  void goto_bar(int bar, string voice = "", string tune = "");
  int save_current_tune(const char* filename, int* skipbegin, int* skiplines, bool forabc2midi = false);
  string prepare_for_abc2midi(const char* text);
  // menu item callbacks
  virtual void cbmenu_file_exit();
  virtual void cbmenu_file_new();
  virtual void cbmenu_file_newwindow();
  virtual void cbmenu_file_newtemplate();
  virtual void cbmenu_file_save();
  virtual void cbmenu_file_saveas();
  virtual void cbmenu_file_open();
  virtual void cbmenu_file_insert();
  virtual void cbmenu_file_loadexample();
  virtual void cbmenu_edit_undo();
  virtual void cbmenu_edit_redo();
  virtual void cbmenu_search_find();
  virtual void cbmenu_search_findagain();
  virtual void cbmenu_search_replace();
  virtual void cbmenu_search_gotoline();
  virtual void cbmenu_search_gotobar();
  virtual void cbmenu_options_larger();
  virtual void cbmenu_options_smaller();
  virtual void cbmenu_options_look();
  virtual void cbmenu_options_behaviour();
  virtual void cbmenu_options_system();
  virtual void cbmenu_options_save();
  virtual void cbmenu_abc_preview();
  virtual void cbmenu_abc_previewtune();
  virtual void cbmenu_abc_listentune();
  virtual void cbmenu_abc_marktune();
  virtual void cbmenu_abc_fixupxrefs();
  virtual void cbmenu_abc_commentregion();
  virtual void cbmenu_abc_uncommentregion();
  virtual void cbmenu_abc_regionoctaveup();
  virtual void cbmenu_abc_regionoctavedown();
  virtual void cbmenu_abc_insertheader();
  virtual void cbmenu_abc_midiinstrument();
  virtual void cbmenu_abc_format_font();
  virtual void cbmenu_abc_format_layout();
  virtual void cbmenu_abc_format_tablature();
  virtual void cbmenu_abc_format_all();
  virtual void cbmenu_help_usersguide();
  virtual void cbmenu_help_manpage();
  virtual void cbmenu_help_flabc();
  virtual void cbmenu_help_flabcchanges();
  virtual void cbmenu_help_about();
  // editor callbacks
  static void changed_cb_static(int pos, int nInserted, int nDeleted,int nRestyled, const char* text, void* thisobject);
  void changed_cb(int pos, int nInserted, int nDeleted,int nRestyled, const char* text);
};

#endif
