// generated by Fast Light User Interface Designer (fluid) version 1.0303

#ifndef mainwin_msk_h
#define mainwin_msk_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "fltkextensions.h"
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>

class MainWin_msk {
public:
  custom_Fl_Text_Buffer* textbuffer; 
  MainWin_msk();
  Fl_Double_Window *window;
private:
  inline void cb_window_i(Fl_Double_Window*, void*);
  static void cb_window(Fl_Double_Window*, void*);
public:
  Fl_Menu_Bar *menubar;
  static Fl_Menu_Item menu_menubar[];
private:
  inline void cb_New_i(Fl_Menu_*, void*);
  static void cb_New(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *menu_file_newwindow;
private:
  inline void cb_menu_file_newwindow_i(Fl_Menu_*, void*);
  static void cb_menu_file_newwindow(Fl_Menu_*, void*);
  inline void cb_New1_i(Fl_Menu_*, void*);
  static void cb_New1(Fl_Menu_*, void*);
  inline void cb_Open_i(Fl_Menu_*, void*);
  static void cb_Open(Fl_Menu_*, void*);
  inline void cb_Insert_i(Fl_Menu_*, void*);
  static void cb_Insert(Fl_Menu_*, void*);
  inline void cb_Load_i(Fl_Menu_*, void*);
  static void cb_Load(Fl_Menu_*, void*);
  inline void cb_Save_i(Fl_Menu_*, void*);
  static void cb_Save(Fl_Menu_*, void*);
  inline void cb_Save1_i(Fl_Menu_*, void*);
  static void cb_Save1(Fl_Menu_*, void*);
  inline void cb_E_i(Fl_Menu_*, void*);
  static void cb_E(Fl_Menu_*, void*);
  inline void cb_Undo_i(Fl_Menu_*, void*);
  static void cb_Undo(Fl_Menu_*, void*);
  inline void cb_Redo_i(Fl_Menu_*, void*);
  static void cb_Redo(Fl_Menu_*, void*);
  inline void cb_Mark_i(Fl_Menu_*, void*);
  static void cb_Mark(Fl_Menu_*, void*);
  inline void cb_Cut_i(Fl_Menu_*, void*);
  static void cb_Cut(Fl_Menu_*, void*);
  inline void cb_Copy_i(Fl_Menu_*, void*);
  static void cb_Copy(Fl_Menu_*, void*);
  inline void cb_Paste_i(Fl_Menu_*, void*);
  static void cb_Paste(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *menu_edit_cua_linestart;
private:
  inline void cb_menu_edit_cua_linestart_i(Fl_Menu_*, void*);
  static void cb_menu_edit_cua_linestart(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *menu_edit_cua_lineend;
private:
  inline void cb_menu_edit_cua_lineend_i(Fl_Menu_*, void*);
  static void cb_menu_edit_cua_lineend(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *menu_edit_cua_bufferstart;
private:
  inline void cb_menu_edit_cua_bufferstart_i(Fl_Menu_*, void*);
  static void cb_menu_edit_cua_bufferstart(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *menu_edit_cua_bufferend;
private:
  inline void cb_menu_edit_cua_bufferend_i(Fl_Menu_*, void*);
  static void cb_menu_edit_cua_bufferend(Fl_Menu_*, void*);
  inline void cb_Goto_i(Fl_Menu_*, void*);
  static void cb_Goto(Fl_Menu_*, void*);
  inline void cb_Goto1_i(Fl_Menu_*, void*);
  static void cb_Goto1(Fl_Menu_*, void*);
  inline void cb_Kill_i(Fl_Menu_*, void*);
  static void cb_Kill(Fl_Menu_*, void*);
  inline void cb_Yank_i(Fl_Menu_*, void*);
  static void cb_Yank(Fl_Menu_*, void*);
  inline void cb_Find_i(Fl_Menu_*, void*);
  static void cb_Find(Fl_Menu_*, void*);
  inline void cb_Find1_i(Fl_Menu_*, void*);
  static void cb_Find1(Fl_Menu_*, void*);
  inline void cb_Replace_i(Fl_Menu_*, void*);
  static void cb_Replace(Fl_Menu_*, void*);
  inline void cb_Goto2_i(Fl_Menu_*, void*);
  static void cb_Goto2(Fl_Menu_*, void*);
  inline void cb_Goto3_i(Fl_Menu_*, void*);
  static void cb_Goto3(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *menu_options_larger;
private:
  inline void cb_menu_options_larger_i(Fl_Menu_*, void*);
  static void cb_menu_options_larger(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *menu_options_smaller;
private:
  inline void cb_menu_options_smaller_i(Fl_Menu_*, void*);
  static void cb_menu_options_smaller(Fl_Menu_*, void*);
  inline void cb_Look_i(Fl_Menu_*, void*);
  static void cb_Look(Fl_Menu_*, void*);
  inline void cb_Behaviour_i(Fl_Menu_*, void*);
  static void cb_Behaviour(Fl_Menu_*, void*);
  inline void cb_System_i(Fl_Menu_*, void*);
  static void cb_System(Fl_Menu_*, void*);
  inline void cb_Save2_i(Fl_Menu_*, void*);
  static void cb_Save2(Fl_Menu_*, void*);
  inline void cb_Preview_i(Fl_Menu_*, void*);
  static void cb_Preview(Fl_Menu_*, void*);
  inline void cb_Preview1_i(Fl_Menu_*, void*);
  static void cb_Preview1(Fl_Menu_*, void*);
  inline void cb_Listen_i(Fl_Menu_*, void*);
  static void cb_Listen(Fl_Menu_*, void*);
  inline void cb_Mark1_i(Fl_Menu_*, void*);
  static void cb_Mark1(Fl_Menu_*, void*);
  inline void cb_Fixup_i(Fl_Menu_*, void*);
  static void cb_Fixup(Fl_Menu_*, void*);
  inline void cb_Comment_i(Fl_Menu_*, void*);
  static void cb_Comment(Fl_Menu_*, void*);
  inline void cb_Uncomment_i(Fl_Menu_*, void*);
  static void cb_Uncomment(Fl_Menu_*, void*);
  inline void cb_Octave_i(Fl_Menu_*, void*);
  static void cb_Octave(Fl_Menu_*, void*);
  inline void cb_Octave1_i(Fl_Menu_*, void*);
  static void cb_Octave1(Fl_Menu_*, void*);
  inline void cb_Insert1_i(Fl_Menu_*, void*);
  static void cb_Insert1(Fl_Menu_*, void*);
  inline void cb_Pick_i(Fl_Menu_*, void*);
  static void cb_Pick(Fl_Menu_*, void*);
  inline void cb_Font_i(Fl_Menu_*, void*);
  static void cb_Font(Fl_Menu_*, void*);
  inline void cb_Layout_i(Fl_Menu_*, void*);
  static void cb_Layout(Fl_Menu_*, void*);
  inline void cb_Tablature_i(Fl_Menu_*, void*);
  static void cb_Tablature(Fl_Menu_*, void*);
  inline void cb_All_i(Fl_Menu_*, void*);
  static void cb_All(Fl_Menu_*, void*);
  inline void cb_Flabc_i(Fl_Menu_*, void*);
  static void cb_Flabc(Fl_Menu_*, void*);
  inline void cb_Flabc1_i(Fl_Menu_*, void*);
  static void cb_Flabc1(Fl_Menu_*, void*);
  inline void cb_Abctab2ps_i(Fl_Menu_*, void*);
  static void cb_Abctab2ps(Fl_Menu_*, void*);
  inline void cb_Abctab2ps1_i(Fl_Menu_*, void*);
  static void cb_Abctab2ps1(Fl_Menu_*, void*);
  inline void cb_About_i(Fl_Menu_*, void*);
  static void cb_About(Fl_Menu_*, void*);
public:
  resizable_Fl_Tile *tile;
  Fl_Group *grouptop;
  custom_Fl_Text_Editor *editor;
  Fl_Box *infoline;
  virtual ~MainWin_msk();
  void show();
  void show(int argc, char**argv);
  virtual void cbmenu_file_new() = 0;
  virtual void cbmenu_file_newwindow() = 0;
  virtual void cbmenu_file_newtemplate() = 0;
  virtual void cbmenu_file_open() = 0;
  virtual void cbmenu_file_insert() = 0;
  virtual void cbmenu_file_loadexample() = 0;
  virtual void cbmenu_file_save() = 0;
  virtual void cbmenu_file_saveas() = 0;
  virtual void cbmenu_file_exit() = 0;
  virtual void cbmenu_edit_undo() = 0;
  virtual void cbmenu_edit_redo() = 0;
  virtual void cbmenu_search_find() = 0;
  virtual void cbmenu_search_findagain() = 0;
  virtual void cbmenu_search_replace() = 0;
  virtual void cbmenu_search_gotoline() = 0;
  virtual void cbmenu_search_gotobar() = 0;
  virtual void cbmenu_options_larger() = 0;
  virtual void cbmenu_options_smaller() = 0;
  virtual void cbmenu_options_look() = 0;
  virtual void cbmenu_options_behaviour() = 0;
  virtual void cbmenu_options_system() = 0;
  virtual void cbmenu_options_save() = 0;
  virtual void cbmenu_abc_preview() = 0;
  virtual void cbmenu_abc_previewtune() = 0;
  virtual void cbmenu_abc_listentune() = 0;
  virtual void cbmenu_abc_marktune() = 0;
  virtual void cbmenu_abc_fixupxrefs() = 0;
  virtual void cbmenu_abc_commentregion() = 0;
  virtual void cbmenu_abc_uncommentregion() = 0;
  virtual void cbmenu_abc_regionoctaveup() = 0;
  virtual void cbmenu_abc_regionoctavedown() = 0;
  virtual void cbmenu_abc_insertheader() = 0;
  virtual void cbmenu_abc_midiinstrument() = 0;
  virtual void cbmenu_abc_format_font() = 0;
  virtual void cbmenu_abc_format_layout() = 0;
  virtual void cbmenu_abc_format_tablature() = 0;
  virtual void cbmenu_abc_format_all() = 0;
  virtual void cbmenu_help_usersguide() = 0;
  virtual void cbmenu_help_manpage() = 0;
  virtual void cbmenu_help_flabc() = 0;
  virtual void cbmenu_help_flabcchanges() = 0;
  virtual void cbmenu_help_about() = 0;
};
#endif
