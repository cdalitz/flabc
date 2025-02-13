// generated by Fast Light User Interface Designer (fluid) version 1.0309

#ifndef dialogs_msk_h
#define dialogs_msk_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "mainwin.h"
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Box.H>

class OptionsLook_msk {
public:
  Fl_Font romanfontnum; 
  Fl_Font boldfontnum; 
  Fl_Font italicfontnum; 
  Fl_Font bolditalicfontnum; 
  MainWin* mainwin; 
  OptionsLook_msk(MainWin* mw);
  Fl_Double_Window *window;
  Fl_Spinner *fontsize;
  Fl_Button *backgroundcolor;
private:
  inline void cb_backgroundcolor_i(Fl_Button*, void*);
  static void cb_backgroundcolor(Fl_Button*, void*);
public:
  Fl_Button *selectioncolor;
private:
  inline void cb_selectioncolor_i(Fl_Button*, void*);
  static void cb_selectioncolor(Fl_Button*, void*);
public:
  Fl_Choice *scheme;
  Fl_Check_Button *visiblefocus;
  Fl_Check_Button *abcnodeadkeys;
  Fl_Output *romanfont;
private:
  inline void cb_Change_i(Fl_Button*, void*);
  static void cb_Change(Fl_Button*, void*);
public:
  Fl_Output *boldfont;
private:
  inline void cb_Change1_i(Fl_Button*, void*);
  static void cb_Change1(Fl_Button*, void*);
public:
  Fl_Output *italicfont;
private:
  inline void cb_Change2_i(Fl_Button*, void*);
  static void cb_Change2(Fl_Button*, void*);
public:
  Fl_Output *bolditalicfont;
private:
  inline void cb_Change3_i(Fl_Button*, void*);
  static void cb_Change3(Fl_Button*, void*);
public:
  Fl_Button *Acolor;
private:
  inline void cb_Acolor_i(Fl_Button*, void*);
  static void cb_Acolor(Fl_Button*, void*);
public:
  Fl_Check_Button *Abold;
  Fl_Check_Button *Aitalic;
  Fl_Button *Bcolor;
private:
  inline void cb_Bcolor_i(Fl_Button*, void*);
  static void cb_Bcolor(Fl_Button*, void*);
public:
  Fl_Check_Button *Bbold;
  Fl_Check_Button *Bitalic;
  Fl_Button *Ccolor;
private:
  inline void cb_Ccolor_i(Fl_Button*, void*);
  static void cb_Ccolor(Fl_Button*, void*);
public:
  Fl_Check_Button *Cbold;
  Fl_Check_Button *Citalic;
  Fl_Button *Dcolor;
private:
  inline void cb_Dcolor_i(Fl_Button*, void*);
  static void cb_Dcolor(Fl_Button*, void*);
public:
  Fl_Check_Button *Dbold;
  Fl_Check_Button *Ditalic;
  Fl_Button *Ecolor;
private:
  inline void cb_Ecolor_i(Fl_Button*, void*);
  static void cb_Ecolor(Fl_Button*, void*);
public:
  Fl_Check_Button *Ebold;
  Fl_Check_Button *Eitalic;
private:
  inline void cb_Apply_i(Fl_Button*, void*);
  static void cb_Apply(Fl_Button*, void*);
  inline void cb_Reset_i(Fl_Button*, void*);
  static void cb_Reset(Fl_Button*, void*);
  inline void cb_Quit_i(Fl_Button*, void*);
  static void cb_Quit(Fl_Button*, void*);
public:
  virtual ~OptionsLook_msk();
  virtual void buttonapply_cb() = 0;
  virtual void buttonreset_cb() = 0;
  virtual void buttonquit_cb();
  virtual void getfont_cb(Fl_Output* output, Fl_Font* fontnum) = 0;
  void getbuttoncolor(Fl_Button* button);
  void show();
};
#include <FL/Fl_Round_Button.H>

class OptionsBehaviour_msk {
public:
  MainWin* mainwin; 
  OptionsBehaviour_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_Apply1_i(Fl_Button*, void*);
  static void cb_Apply1(Fl_Button*, void*);
  inline void cb_Reset1_i(Fl_Button*, void*);
  static void cb_Reset1(Fl_Button*, void*);
  inline void cb_Quit1_i(Fl_Button*, void*);
  static void cb_Quit1(Fl_Button*, void*);
public:
  Fl_Check_Button *askonexit;
  Fl_Check_Button *takecareofshebang;
  Fl_Round_Button *abctab2ps;
  Fl_Round_Button *abcm2ps;
  virtual ~OptionsBehaviour_msk();
  virtual void buttonapply_cb() = 0;
  virtual void buttonreset_cb() = 0;
  virtual void buttonquit_cb();
  void show();
};
#include <FL/Fl_Tabs.H>

class OptionsSystem_msk {
public:
  MainWin* mainwin; 
  OptionsSystem_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_Apply2_i(Fl_Button*, void*);
  static void cb_Apply2(Fl_Button*, void*);
  inline void cb_Reset2_i(Fl_Button*, void*);
  static void cb_Reset2(Fl_Button*, void*);
  inline void cb_Quit2_i(Fl_Button*, void*);
  static void cb_Quit2(Fl_Button*, void*);
public:
  noarrow_Fl_Input *abctab2ps;
  noarrow_Fl_Input *abcm2ps;
  noarrow_Fl_Input *abctabfonts;
  noarrow_Fl_Input *abctmpfile;
  noarrow_Fl_Input *psviewer;
  noarrow_Fl_Input *pstmpfile;
  noarrow_Fl_Input *abc2midi;
  noarrow_Fl_Input *midiplayer;
  noarrow_Fl_Input *miditmpfile;
  noarrow_Fl_Input *docdirs;
  noarrow_Fl_Input *webbrowser;
  noarrow_Fl_Input *templatedirs;
  noarrow_Fl_Input *exampledirs;
  virtual ~OptionsSystem_msk();
  virtual void buttonapply_cb() = 0;
  virtual void buttonreset_cb() = 0;
  virtual void buttonquit_cb();
  void show();
};
#include <FL/Fl_Multi_Browser.H>

class AbcFormatList_msk {
public:
  MainWin* mainwin; 
  AbcFormatList_msk(MainWin* mw);
  Fl_Double_Window *window;
  Fl_Multi_Browser *browser;
private:
  inline void cb_Copy_i(Fl_Button*, void*);
  static void cb_Copy(Fl_Button*, void*);
  inline void cb_Close_i(Fl_Button*, void*);
  static void cb_Close(Fl_Button*, void*);
public:
  virtual ~AbcFormatList_msk();
  virtual void buttoncopy_cb() = 0;
  virtual void buttonclose_cb();
  void show();
};

class MidiList_msk {
public:
  MainWin* mainwin; 
  MidiList_msk(MainWin* mw);
  Fl_Double_Window *window;
  Fl_Multi_Browser *browser;
  Fl_Round_Button *commoninstruments;
private:
  inline void cb_commoninstruments_i(Fl_Round_Button*, void*);
  static void cb_commoninstruments(Fl_Round_Button*, void*);
public:
  Fl_Round_Button *allinstruments;
private:
  inline void cb_allinstruments_i(Fl_Round_Button*, void*);
  static void cb_allinstruments(Fl_Round_Button*, void*);
  inline void cb_Copy1_i(Fl_Button*, void*);
  static void cb_Copy1(Fl_Button*, void*);
  inline void cb_Close1_i(Fl_Button*, void*);
  static void cb_Close1(Fl_Button*, void*);
public:
  virtual ~MidiList_msk();
  virtual void buttoncommoninstruments_cb() = 0;
  virtual void buttonallinstruments_cb() = 0;
  virtual void buttoncopy_cb() = 0;
  virtual void buttonclose_cb();
  void show();
};
#include <FL/Fl_Hold_Browser.H>

class CommandOutput_msk {
public:
  MainWin* mainwin; 
  CommandOutput_msk(MainWin* mw);
  Fl_Double_Window *window;
  Fl_Hold_Browser *browser;
private:
  inline void cb_Goto_i(Fl_Button*, void*);
  static void cb_Goto(Fl_Button*, void*);
  inline void cb_Close2_i(Fl_Button*, void*);
  static void cb_Close2(Fl_Button*, void*);
public:
  virtual ~CommandOutput_msk();
  virtual void buttonnexterror_cb() = 0;
  virtual void buttonclose_cb();
  void show();
};
#include <FL/Fl_Return_Button.H>

class HelpAbout_msk {
public:
  MainWin* mainwin; 
  HelpAbout_msk(MainWin* mw);
  Fl_Double_Window *window;
  Fl_Box *logo;
  Fl_Box *version;
  Fl_Box *versiondate;
  Fl_Output *abctab2ps;
  Fl_Output *abcm2ps;
  Fl_Output *abc2midi;
private:
  inline void cb_Close3_i(Fl_Return_Button*, void*);
  static void cb_Close3(Fl_Return_Button*, void*);
  inline void cb_View_i(Fl_Button*, void*);
  static void cb_View(Fl_Button*, void*);
public:
  virtual ~HelpAbout_msk();
  virtual void buttonclose_cb();
  virtual void buttonlicense_cb() = 0;
  virtual void show();
};
#include <FL/Fl_Int_Input.H>

class GotoLine_msk {
public:
  MainWin* mainwin; 
  int buttonpressed; // 1 = Ok, 0 = Cancel
  GotoLine_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_window_i(Fl_Double_Window*, void*);
  static void cb_window(Fl_Double_Window*, void*);
public:
  Fl_Int_Input *line;
  Fl_Int_Input *column;
  Fl_Return_Button *ok;
private:
  inline void cb_ok_i(Fl_Return_Button*, void*);
  static void cb_ok(Fl_Return_Button*, void*);
public:
  Fl_Button *cancel;
private:
  inline void cb_cancel_i(Fl_Button*, void*);
  static void cb_cancel(Fl_Button*, void*);
public:
  virtual ~GotoLine_msk();
  virtual void buttonok_cb();
  virtual void buttoncancel_cb();
};
#include <FL/Fl_Input.H>

class GotoBar_msk {
public:
  MainWin* mainwin; 
  int buttonpressed; // 1 = Ok, 0 = Cancel
  GotoBar_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_window1_i(Fl_Double_Window*, void*);
  static void cb_window1(Fl_Double_Window*, void*);
public:
  Fl_Int_Input *bar;
  Fl_Input *voice;
  Fl_Input *tune;
  Fl_Return_Button *ok;
private:
  inline void cb_ok1_i(Fl_Return_Button*, void*);
  static void cb_ok1(Fl_Return_Button*, void*);
public:
  Fl_Button *cancel;
private:
  inline void cb_cancel1_i(Fl_Button*, void*);
  static void cb_cancel1(Fl_Button*, void*);
public:
  virtual ~GotoBar_msk();
  virtual void buttonok_cb();
  virtual void buttoncancel_cb();
};

class Search_msk {
public:
  MainWin* mainwin; 
  int buttonpressed; // 1 = Ok, 0 = Cancel
  Search_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_window2_i(Fl_Double_Window*, void*);
  static void cb_window2(Fl_Double_Window*, void*);
public:
  Fl_Input *text;
  Fl_Check_Button *casesensitive;
  Fl_Check_Button *regexp;
  Fl_Return_Button *ok;
private:
  inline void cb_ok2_i(Fl_Return_Button*, void*);
  static void cb_ok2(Fl_Return_Button*, void*);
public:
  Fl_Button *cancel;
private:
  inline void cb_cancel2_i(Fl_Button*, void*);
  static void cb_cancel2(Fl_Button*, void*);
public:
  virtual ~Search_msk();
  virtual void buttonok_cb();
  virtual void buttoncancel_cb();
};

class Replace_msk {
public:
  MainWin* mainwin; 
  Replace_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_window3_i(Fl_Double_Window*, void*);
  static void cb_window3(Fl_Double_Window*, void*);
public:
  Fl_Input *searchtext;
  Fl_Input *replacetext;
  Fl_Check_Button *casesensitive;
  Fl_Check_Button *regexp;
  Fl_Return_Button *search;
private:
  inline void cb_search_i(Fl_Return_Button*, void*);
  static void cb_search(Fl_Return_Button*, void*);
public:
  Fl_Button *replace;
private:
  inline void cb_replace_i(Fl_Button*, void*);
  static void cb_replace(Fl_Button*, void*);
public:
  Fl_Button *replaceall;
private:
  inline void cb_replaceall_i(Fl_Button*, void*);
  static void cb_replaceall(Fl_Button*, void*);
public:
  virtual ~Replace_msk();
  virtual void buttonsearch_cb() = 0;
  virtual void buttonreplace_cb() = 0;
  virtual void buttonreplaceall_cb() = 0;
  virtual void buttoncancel_cb();
  virtual void show();
};
#include <FL/Fl_Text_Display.H>

class TextViewer_msk {
public:
  MainWin* mainwin; 
  Fl_Text_Buffer textbuffer; 
  TextViewer_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_Close4_i(Fl_Return_Button*, void*);
  static void cb_Close4(Fl_Return_Button*, void*);
public:
  Fl_Text_Display *textdisplay;
  virtual ~TextViewer_msk();
  virtual void buttonclose_cb();
  virtual void show();
};

class TemplatePicker_msk {
public:
  MainWin* mainwin; 
  int buttonpressed; // 1 = Load, 0 = Cancel, -1 = None
  TemplatePicker_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_window4_i(Fl_Double_Window*, void*);
  static void cb_window4(Fl_Double_Window*, void*);
public:
  Fl_Hold_Browser *browser;
private:
  inline void cb_browser_i(Fl_Hold_Browser*, void*);
  static void cb_browser(Fl_Hold_Browser*, void*);
  inline void cb_Cancel_i(Fl_Button*, void*);
  static void cb_Cancel(Fl_Button*, void*);
public:
  Fl_Return_Button *buttonload;
private:
  inline void cb_buttonload_i(Fl_Return_Button*, void*);
  static void cb_buttonload(Fl_Return_Button*, void*);
public:
  Fl_Box *boxhint;
  virtual ~TemplatePicker_msk();
  virtual void buttonload_cb();
  virtual void buttoncancel_cb();
};

class FontPicker_msk {
public:
  int buttonpressed; // 1 = Ok, 0 = Cancel
  FontPicker_msk();
  Fl_Double_Window *window;
private:
  inline void cb_window5_i(Fl_Double_Window*, void*);
  static void cb_window5(Fl_Double_Window*, void*);
public:
  Fl_Hold_Browser *fontbrowser;
private:
  inline void cb_fontbrowser_i(Fl_Hold_Browser*, void*);
  static void cb_fontbrowser(Fl_Hold_Browser*, void*);
public:
  Fl_Hold_Browser *sizebrowser;
private:
  inline void cb_sizebrowser_i(Fl_Hold_Browser*, void*);
  static void cb_sizebrowser(Fl_Hold_Browser*, void*);
public:
  Fl_Box *displaybox;
  Fl_Return_Button *ok;
private:
  inline void cb_ok3_i(Fl_Return_Button*, void*);
  static void cb_ok3(Fl_Return_Button*, void*);
public:
  Fl_Button *cancel;
private:
  inline void cb_cancel3_i(Fl_Button*, void*);
  static void cb_cancel3(Fl_Button*, void*);
public:
  virtual ~FontPicker_msk();
  virtual void buttonok_cb();
  virtual void buttoncancel_cb();
  virtual void fontbrowser_cb() = 0;
  virtual void sizebrowser_cb() = 0;
};

class FixupXrefs_msk {
public:
  MainWin* mainwin; 
  int buttonpressed; // 1 = Ok, 0 = Cancel
  FixupXrefs_msk(MainWin* mw);
  Fl_Double_Window *window;
private:
  inline void cb_window6_i(Fl_Double_Window*, void*);
  static void cb_window6(Fl_Double_Window*, void*);
public:
  Fl_Round_Button *consecutive;
  Fl_Round_Button *allowgaps;
  Fl_Return_Button *ok;
private:
  inline void cb_ok4_i(Fl_Return_Button*, void*);
  static void cb_ok4(Fl_Return_Button*, void*);
public:
  Fl_Button *cancel;
private:
  inline void cb_cancel4_i(Fl_Button*, void*);
  static void cb_cancel4(Fl_Button*, void*);
public:
  virtual ~FixupXrefs_msk();
  virtual void buttonok_cb();
  virtual void buttoncancel_cb();
};
#endif
