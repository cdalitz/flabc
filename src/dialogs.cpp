//
// small dialogs for flabc (dialogs_msk is only for fluid)
//

#include "flabc.h"
#include "dialogs.h"
#include "mainwin.h"
#include "os.h"
#include "messagebox.h"
#include "utils.h"
#include <FL/filename.H>
#include <stdio.h>
#include <string.h>

//==========================================================================
// Search dialog
//==========================================================================

Search::Search(MainWin* mw) : Search_msk(mw)
{
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() - 50;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 80;
  if (y + window->h() > Fl::h())
    y = 80;
  window->position(x,y);
#ifdef WIN32
  // regexps not supported
  regexp->deactivate();
  regexp->hide();
#endif
}

// shows the dialog modal and fills return data
// RC: 0 = user pressed cancel, 1 = user pressed ok
int Search::get_search_data(char** rctext, int* rccase, int* rcregexp)
{
  buttonpressed = -1;
  window->show();
  while (buttonpressed == -1) Fl::wait();
  *rctext = (char*)text->value();
  *rccase = casesensitive->value();
  *rcregexp = regexp->value();
  if (!*(*rctext)) buttonpressed = 0;
  return buttonpressed;
}

//==========================================================================
// Replace dialog
//==========================================================================

Replace::Replace(MainWin* mw) : Replace_msk(mw)
{
  int x, y;

  // set initial position to upper right corner
  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 40;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);
#ifdef WIN32
  // regexps not supported
  regexp->deactivate();
#endif
  // regexp replace is not yet implemented
  regexp->hide();
}

void Replace::buttonsearch_cb()
{
  mainwin->find_string(searchtext->value(),
                       casesensitive->value(), regexp->value());
}
void Replace::buttonreplace_cb()
{
  bool wasfound = false;
  if (mainwin->textbuffer->selected()) {
    char* sel = mainwin->textbuffer->selection_text();
    if ((casesensitive->value() && 0 == strcmp(searchtext->value(),sel)) ||
        (!casesensitive->value() && 0 == strcmpnocase(searchtext->value(),sel))) {
      mainwin->textbuffer->replace_selection(replacetext->value());
      buttonsearch_cb();
      wasfound = true;
    }
    free(sel);
  }
  if (!wasfound)
    message_box("Please use 'Search' before 'Replace'");
}

void Replace::buttonreplaceall_cb()
{
  const char* stext = searchtext->value();
  const char* rtext = replacetext->value();
  if (!*stext) {
    message_box("Please enter a string for searching");
    return;
  }
  string newtext = "";
  int stextlen, nmatches, start, end, choice, pos, oldpos, p;
  Fl_Text_Buffer* tb = mainwin->textbuffer;
  int matchcase = casesensitive->value();

  // set start position
  choice = choice_box("Where shall 'Replace all' start?", "Cancel",
                      "Cursor Position", "Buffer Start", "Replace all");
  if (choice == 2) {
    start = 0;
  } else if (choice == 1) {
    if (tb->selected())
      tb->selection_position(&start,&end); // end is later overwritten
    else
      start = mainwin->editor->insert_position();
  } else {
    return;
  }
  tb->unselect();

  // loop over all matches
  stextlen = strlen(stext);
  nmatches = 0;
  end = tb->length();
  oldpos = pos = start;
  while (oldpos < end && tb->search_forward(oldpos, stext, &pos, matchcase)) {
    for (p = oldpos; p < pos; p++)
      newtext += tb->char_at(p);
    newtext += rtext;
    nmatches++;
    oldpos = pos + stextlen;
  }
  for (p = oldpos; p < end; p++)
    newtext += tb->char_at(p);

  // replace and give feedback
  tb->replace(start,end,newtext.c_str());
  char msg[64];
  sprintf(msg, "Replaced %d occurences", nmatches);
  message_box(msg);
}

//==========================================================================
// Goto Line/Column dialog
//==========================================================================

GotoLine::GotoLine(MainWin* mw) : GotoLine_msk(mw)
{
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() - 50;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 80;
  if (y + window->h() > Fl::h())
    y = 80;
  window->position(x,y);
}

// shows the dialog modal and fills line and col
// RC: 0 = user pressed cancel, 1 = user pressed ok
int GotoLine::get_line_column(int* rcline, int* rccol)
{
  buttonpressed = -1;
  window->show();
  while (buttonpressed == -1) Fl::wait();
  *rcline = atoi(line->value());
  *rccol = atoi(column->value());
  if (!*line->value() && !*column->value()) buttonpressed = 0;
  return buttonpressed;
}

//==========================================================================
// Goto Bar/Voice/Tune dialog
//==========================================================================

GotoBar::GotoBar(MainWin* mw) : GotoBar_msk(mw)
{
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() - 50;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 80;
  if (y + window->h() > Fl::h())
    y = 80;
  window->position(x,y);
}

// shows the dialog modal and fills bar, voice and tune
// RC: 0 = user pressed cancel, 1 = user pressed ok
int GotoBar::get_bar_voice_tune(int* rcbar, string* rcvoice, string* rctune)
{
  buttonpressed = -1;
  window->show();
  while (buttonpressed == -1) Fl::wait();
  bar->value(trim_space(bar->value()).c_str());
  voice->value(trim_space(voice->value()).c_str());
  tune->value(trim_space(tune->value()).c_str());
  *rcbar = atoi(bar->value());
  *rcvoice = voice->value();
  *rctune = tune->value();
  if (!*bar->value() && !*voice->value() && !*tune->value()) buttonpressed = 0;
  return buttonpressed;
}


//==========================================================================
// Options dialog for look & feel
//==========================================================================

// copies config setting into GUI widgets
void OptionsLook::style2gui(Fl_Button* color,
                            Fl_Check_Button* bold, Fl_Check_Button* italic,
                            Fl_Text_Display::Style_Table_Entry* stylent,
                            int* styleatt) {
  color->color(stylent->color);
  if (*styleatt & FL_BOLD) bold->value(true);
  else bold->value(false);
  if (*styleatt & FL_ITALIC) italic->value(true);
  else italic->value(false);
}

// copies GUI widget values into config setting 
void OptionsLook::gui2style(Fl_Button* color,
                            Fl_Check_Button* bold, Fl_Check_Button* italic,
                            Fl_Text_Display::Style_Table_Entry* stylent,
                            int* styleatt) {
  stylent->color = color->color();
  if (bold->value() && italic->value()) {
    stylent->font = mainwin->config->bolditalicfont;
    *styleatt = FL_BOLD | FL_ITALIC;
  } else if (bold->value()) {
    stylent->font = mainwin->config->boldfont;
    *styleatt = FL_BOLD;
  } else if (italic->value()) {
    stylent->font = mainwin->config->italicfont;
    *styleatt = FL_ITALIC;
  } else {
    stylent->font = mainwin->config->plainfont;
    styleatt = 0;
  }
}

void OptionsLook::config2gui(Config* conf) {
  // load highlighting data
  Fl_Text_Display::Style_Table_Entry* st = conf->styletable;
  int* styleatt = conf->stylefontattribs;
  style2gui(Acolor,Abold,Aitalic,&st[0],&styleatt[0]);
  style2gui(Bcolor,Bbold,Bitalic,&st[1],&styleatt[1]);
  style2gui(Ccolor,Cbold,Citalic,&st[2],&styleatt[2]);
  style2gui(Dcolor,Dbold,Ditalic,&st[3],&styleatt[3]);
  style2gui(Ecolor,Ebold,Eitalic,&st[4],&styleatt[4]);

  // load fonts
  romanfontnum = conf->plainfont;
  romanfont->value(Fl::get_font_name(romanfontnum));
  boldfontnum = conf->boldfont;
  boldfont->value(Fl::get_font_name(boldfontnum));
  italicfontnum = conf->italicfont;
  italicfont->value(Fl::get_font_name(italicfontnum));
  bolditalicfontnum = conf->bolditalicfont;
  bolditalicfont->value(Fl::get_font_name(bolditalicfontnum));

  // load other data
  fontsize->value(conf->fontsize);
  backgroundcolor->color(conf->backgroundcolor);
  selectioncolor->color(conf->selectioncolor);
  visiblefocus->value(conf->visiblefocus);
  abcnodeadkeys->value(conf->abcnodeadkeys);
  // find out menu item that matches config setting
  const Fl_Menu_Item* mi = scheme->find_item(conf->scheme.c_str());
  if (mi) scheme->value(mi);
}

OptionsLook::OptionsLook(MainWin* mw) : OptionsLook_msk(mw) {
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 40;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  fontpicker_dialog = NULL;

  // load config
  config2gui(mw->config);

#ifndef __APPLE__
  // abcnodeadkeys only supported on OSX
  // (deadkey code not accessible on Win32)
  abcnodeadkeys->hide();
#endif
}

OptionsLook::~OptionsLook() {
  if (fontpicker_dialog) delete fontpicker_dialog;
}

void OptionsLook::getfont_cb(Fl_Output* output, Fl_Font* fontnum) {
  string fontname = output->value();
  int fs = mainwin->config->fontsize;
  if (!fontpicker_dialog) fontpicker_dialog = new FontPicker(global_nfonts);
  if (fontpicker_dialog->get_monospaced_font(&fontname, &fs, fontnum))
    output->value(fontname.c_str());
}

void OptionsLook::buttonapply_cb() {
  int i;
  mainwin->config->plainfont = romanfontnum;
  mainwin->config->boldfont = boldfontnum;
  mainwin->config->italicfont = italicfontnum;
  mainwin->config->bolditalicfont = bolditalicfontnum;
  Fl_Text_Display::Style_Table_Entry* st = mainwin->config->styletable;
  int* styleatt = mainwin->config->stylefontattribs;
  gui2style(Acolor,Abold,Aitalic,&st[0],&styleatt[0]);
  gui2style(Bcolor,Bbold,Bitalic,&st[1],&styleatt[1]);
  gui2style(Ccolor,Cbold,Citalic,&st[2],&styleatt[2]);
  gui2style(Dcolor,Dbold,Ditalic,&st[3],&styleatt[3]);
  gui2style(Ecolor,Ebold,Eitalic,&st[4],&styleatt[4]);
  mainwin->config->fontsize = (int)fontsize->value();
  for (i=0; i<mainwin->config->nstyles; i++)
    st[i].size = (int)fontsize->value();

  mainwin->config->backgroundcolor = backgroundcolor->color();
  //Fl::set_color(FL_BACKGROUND2_COLOR, mainwin->config->backgroundcolor);
  mainwin->editor->color(mainwin->config->backgroundcolor);
  mainwin->config->selectioncolor = selectioncolor->color();
  mainwin->editor->highlightcolor = mainwin->config->selectioncolor;
  Fl::set_color(FL_SELECTION_COLOR, mainwin->config->selectioncolor);

  mainwin->config->visiblefocus = visiblefocus->value();
  mainwin->config->abcnodeadkeys = abcnodeadkeys->value();
  mainwin->editor->abcnodeadkeys = abcnodeadkeys->value();
  if (scheme->mvalue())
    mainwin->config->scheme = scheme->mvalue()->label();

  // make changes visible
  // for editor redraw() is sufficient because its styletyble
  // is just a pointer to the config styletable
  mainwin->editor->redraw();
  mainwin->editor->show_insert_position();
  Fl::visible_focus(mainwin->config->visiblefocus);
  Fl::scheme(mainwin->config->scheme.c_str());
}

void OptionsLook::buttonreset_cb() {
  Config def;
  config2gui(&def);
  window->redraw();
  buttonapply_cb();
}

//==========================================================================
// Options dialog for system environment
//==========================================================================

OptionsSystem::OptionsSystem(MainWin* mw) : OptionsSystem_msk(mw) {
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 40;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  // load config
  config2gui(mw->config);
}

void OptionsSystem::config2gui(Config* conf) {
  // copy over data from config
  abctab2ps->value(conf->abctab2ps.c_str());
  abcm2ps->value(conf->abcm2ps.c_str());
  abctabfonts->value(conf->abctabfonts.c_str());
  abctmpfile->value(conf->abctmpfile.c_str());
  psviewer->value(conf->psviewer.c_str());
  pstmpfile->value(conf->pstmpfile.c_str());
  abc2midi->value(conf->abc2midi.c_str());
  miditmpfile->value(conf->miditmpfile.c_str());
  midiplayer->value(conf->midiplayer.c_str());
  docdirs->value(conf->docdirs.c_str());
  webbrowser->value(conf->webbrowser.c_str());
  templatedirs->value(conf->templatedirs.c_str());
  exampledirs->value(conf->exampledirs.c_str());
}

void OptionsSystem::buttonapply_cb() {
  mainwin->config->abctab2ps = abctab2ps->value();
  mainwin->config->abcm2ps = abcm2ps->value();
  mainwin->config->abctabfonts = abctabfonts->value();
  mainwin->config->abctmpfile = abctmpfile->value();
  mainwin->config->psviewer = psviewer->value();
  mainwin->config->pstmpfile = pstmpfile->value();
  mainwin->config->abc2midi = abc2midi->value();
  mainwin->config->midiplayer = midiplayer->value();
  mainwin->config->miditmpfile = miditmpfile->value();
  mainwin->config->docdirs = docdirs->value();
  mainwin->config->webbrowser = webbrowser->value();
  mainwin->config->templatedirs = templatedirs->value();
  mainwin->config->exampledirs = exampledirs->value();
}

void OptionsSystem::buttonreset_cb() {
  Config def;
  config2gui(&def);
  buttonapply_cb();
}

//==========================================================================
// Options dialog for behaviour
//==========================================================================

OptionsBehaviour::OptionsBehaviour(MainWin* mw) : OptionsBehaviour_msk(mw) {
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 40;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  // load config
  config2gui(mw->config);
}

void OptionsBehaviour::config2gui(Config* conf) {
  // copy over data from config
  askonexit->value(conf->askonexit);
  takecareofshebang->value(!conf->ignoreshebang);
  if (conf->defaultabc2ps == "abcm2ps") {
    abctab2ps->value(0); abcm2ps->value(1);
  } else {
    abctab2ps->value(1); abcm2ps->value(0);
  }
}

void OptionsBehaviour::buttonapply_cb() {
  mainwin->config->askonexit = askonexit->value();
  mainwin->config->ignoreshebang = !takecareofshebang->value();
  if (abcm2ps->value()) mainwin->config->defaultabc2ps = "abcm2ps";
  else mainwin->config->defaultabc2ps = "abctab2ps";
}

void OptionsBehaviour::buttonreset_cb() {
  Config def;
  config2gui(&def);
  buttonapply_cb();
}

//==========================================================================
// Abc format parameter list
//==========================================================================

AbcFormatList::AbcFormatList(MainWin* mw) : AbcFormatList_msk(mw) {
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 40;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  // text style as in editor
  browser->textsize(mw->config->fontsize);
}

// load format parameters into browser
// the optional second argument is a NULL terminated list of filter
// strings; when provided only matching parameters are loaded
void AbcFormatList::load_data(const char* abctab2psoutput, 
                              char** filter /* = NULL */) {
  char* formats = strdup(abctab2psoutput);
  char* format;
  char* offset;
  char** filtent;
  string entry;
  browser->clear();
  format = strtok(formats,"\n\r");
  while (format) {
    offset = format;
    if (format[0] == ' ') offset++;
    if (format[1] == ' ') offset++;
    if (!filter) {
      entry = (string)"%%" + offset;
      browser->add(entry.c_str());
    } else {
      for (filtent = filter; *filtent; filtent++) {
        if (strstr(format, *filtent)) {
          entry = (string)"%%" + offset;
          browser->add(entry.c_str());
          break;
        }
      }
    }
    format = strtok(NULL,"\n\r");
  }
  free(formats);
}

void AbcFormatList::buttoncopy_cb() {
  string copystr = "";
  int i;
  for (i=1; i<=browser->size(); i++) {
    if (browser->selected(i)) {
      copystr += browser->text(i); copystr += "\n";
    }
  }
  if (copystr.empty()) {
    message_box("Please select lines before");
  } else {
    Fl::copy(copystr.c_str(), copystr.length(), 1);
  }
}

//==========================================================================
// Midi instrument list
//==========================================================================

static const char* MidiListInstruments[] = {
  "0", "Acoustic Grand Piano",
  "1", "Bright Acoustic Piano",
  "2", "Electric Grand Piano",
  "3", "Honky-tonk Piano",
  "4", "Electric Piano 1",
  "5", "Electric Piano 2",
  "6", "Harpsichord",
  "7", "Clavi",
  "8", "Celesta",
  "9", "Glockenspiel",
  "10", "Music Box",
  "11", "Vibraphone",
  "12", "Marimba",
  "13", "Xylophone",
  "14", "Tubular Bells",
  "15", "Dulcimer",
  "16", "Drawbar Organ",
  "17", "Percussive Organ",
  "18", "Rock Organ",
  "19", "Church Organ",
  "20", "Reed Organ",
  "21", "Accordion",
  "22", "Harmonica",
  "23", "Tango Accordion",
  "24", "Acoustic Guitar (nylon)",
  "25", "Acoustic Guitar (steel)",
  "26", "Electric Guitar (jazz)",
  "27", "Electric Guitar (clean)",
  "28", "Electric Guitar (muted)",
  "29", "Overdriven Guitar",
  "30", "Distortion Guitar",
  "31", "Guitar harmonics",
  "32", "Acoustic Bass",
  "33", "Electric Bass (finger)",
  "34", "Electric Bass (pick)",
  "35", "Fretless Bass",
  "36", "Slap Bass 1",
  "37", "Slap Bass 2",
  "38", "Synth Bass 1",
  "39", "Synth Bass 2",
  "40", "Violin",
  "41", "Viola",
  "42", "Cello",
  "43", "Contrabass",
  "44", "Tremolo Strings",
  "45", "Pizzicato Strings",
  "46", "Orchestral Harp",
  "47", "Timpani",
  "48", "String Ensemble 1",
  "49", "String Ensemble 2",
  "50", "SynthStrings 1",
  "51", "SynthStrings 2",
  "52", "Choir Aahs",
  "53", "Voice Oohs",
  "54", "Synth Voice",
  "55", "Orchestra Hit",
  "56", "Trumpet",
  "57", "Trombone",
  "58", "Tuba",
  "59", "Muted Trumpet",
  "60", "French Horn",
  "61", "Brass Section",
  "62", "SynthBrass 1",
  "63", "SynthBrass 2",
  "64", "Soprano Sax",
  "65", "Alto Sax",
  "66", "Tenor Sax",
  "67", "Baritone Sax",
  "68", "Oboe",
  "69", "English Horn",
  "70", "Bassoon",
  "71", "Clarinet",
  "72", "Piccolo",
  "73", "Flute",
  "74", "Recorder",
  "75", "Pan Flute",
  "76", "Blown Bottle",
  "77", "Shakuhachi",
  "78", "Whistle",
  "79", "Ocarina",
  "80", "Lead 1 (square)",
  "81", "Lead 2 (sawtooth)",
  "82", "Lead 3 (calliope)",
  "83", "Lead 4 (chiff)",
  "84", "Lead 5 (charang)",
  "85", "Lead 6 (voice)",
  "86", "Lead 7 (fifths)",
  "87", "Lead 8 (bass + lead)",
  "88", "Pad 1 (new age)",
  "89", "Pad 2 (warm)",
  "90", "Pad 3 (polysynth)",
  "91", "Pad 4 (choir)",
  "92", "Pad 5 (bowed)",
  "93", "Pad 6 (metallic)",
  "94", "Pad 7 (halo)",
  "95", "Pad 8 (sweep)",
  "96", "FX 1 (rain)",
  "97", "FX 2 (soundtrack)",
  "98", "FX 3 (crystal)",
  "99", "FX 4 (atmosphere)",
  "100", "FX 5 (brightness)",
  "101", "FX 6 (goblins)",
  "102", "FX 7 (echoes)",
  "103", "FX 8 (sci-fi)",
  "104", "Sitar",
  "105", "Banjo",
  "106", "Shamisen",
  "107", "Koto",
  "108", "Kalimba",
  "109", "Bag pipe",
  "110", "Fiddle",
  "111", "Shanai",
  "112", "Tinkle Bell",
  "113", "Agogo",
  "114", "Steel Drums",
  "115", "Woodblock",
  "116", "Taiko Drum",
  "117", "Melodic Tom",
  "118", "Synth Drum",
  "119", "Reverse Cymbal",
  "120", "Guitar Fret Noise",
  "121", "Breath Noise",
  "122", "Seashore",
  "123", "Bird Tweet",
  "124", "Telephone Ring",
  "125", "Helicopter",
  "126", "Applause",
  "127", "Gunshot",
  0};

MidiList::MidiList(MainWin* mw) : MidiList_msk(mw) {
  char** p;
  char* number;
  string text;
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 40;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  // load midi table
  for (p = (char**)MidiListInstruments; *p; p++) {
    number = *p;
    p++;
    text = number + (string)". " + *p;
    browser->add(text.c_str(),(void*)number);
  }
  commoninstruments->value(1);
  buttoncommoninstruments_cb();

  // text style as in editor
  browser->textsize(mw->config->fontsize);
}

// hide given midi instruments
// to hidelist must be ordered and finished with -1 as end marker
void MidiList::filter_out(int* tohidelist) {
  int i;
  int* chide; // current number to hide
  
  browser->deselect();
  i = 1; // current number in browser
  for (chide = tohidelist; *chide > -1; chide++) {
    while (i <= browser->size() && atoi((char*)browser->data(i)) < *chide) {
      browser->show(i); i++;
    }
    if (i > browser->size()) break;
    if (atoi((char*)browser->data(i)) >= *chide) {
      browser->hide(i);
    } else {
      browser->show(i);
    }
    i++;
  }
}

void MidiList::buttoncommoninstruments_cb() {
  int uncommon[] = {1,2,3,4,5,7,8,10,12,15,17,18,20,22,
                    26,27,28,29,30,31,32,33,34,35,36,37,
                    38,39,44,45,48,49,50,51,54,55,
                    61,62,63,69,76,77,79,80,81,82,83,84,
                    85,86,87,88,89,90,91,92,93,94,95,96,97,98,
                    99,100,101,102,103,111,112,113,
                    114,115,116,117,118,119,120,121,122,123,
                    124,125,126,127,-1};
  filter_out(uncommon);
}

void MidiList::buttonallinstruments_cb() {
  int i;
  for (i = 1; i <= browser->size(); i++) browser->show(i);
}

void MidiList::buttoncopy_cb() {
  string copystr = "";
  int i;
  for (i=1; i<=browser->size(); i++) {
    if (browser->selected(i)) {
      copystr += "%%MIDI program ";
      copystr += (char*)browser->data(i);
      copystr += "  % ";
      // add instrument name as comment without leading number
      copystr += (browser->text(i) + strspn(browser->text(i),"0123456789. \t"));
      copystr += "\n";
    }
  }
  if (copystr.empty()) {
    message_box("Please select instruments before");
  } else {
    Fl::copy(copystr.c_str(), copystr.length(), 1);
  }
}

//==========================================================================
// Command output (from abctab2ps/abc2midi) window
//==========================================================================

CommandOutput::CommandOutput(MainWin* mw) : CommandOutput_msk(mw)
{
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 80;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  // text style as in editor
  browser->textsize(mw->config->fontsize);

  // unset error location parameters
  lasterrorindex = 0;
  bufferskiplines = bufferskipbegin = 0;
}

// loads output line per line into the browser
// cmd indicates the command (abctab2ps or abc2midi)
// when the outpur only refers to a single tune of the buffer
// skipbegin must contain the first skipped line number and
// skiplines the number of skipped lines so that nexterror later
// returns the correct line numbers
void CommandOutput::load_data(const char* output, const char* cmd, 
                              int skipbegin /* = 0 */, 
                              int skiplines /* = 0 */)
{
  char* lines = strdup(output);
  char* line;
  command = cmd;
  bufferskipbegin = skipbegin;
  bufferskiplines = skiplines;
  browser->clear();
  lasterrorindex = 0;
  line = strtok(lines,"\n\r");
  while (line) {
    browser->add(line);
    line = strtok(NULL,"\n\r");
  }
  free(lines);
}

// returns index of next error line in the browser or 0 when no 
// further error is found. line and col are filled with the line and column
// number in the abc source file. When line or col are not given, they are
// set to -1
int CommandOutput::nexterror(int* line, int* col)
{
  int i;
  char *text, *p;
  *line = *col = -1;
  // find next line with error message and extract line/col
  i = browser->value() + 1;
  for (; (text=(char*)browser->text(i)); i++) {
    if ((command=="abctab2ps" && 0==strncmp("+++",text,3)) ||
        (command=="abctab2ps" && strstr(text,"Error in line")) /*abcm2ps*/ ||
        (command=="abc2midi" && 0==strncmp("Error",text,5)))
        break;
  }
  if (!text) return 0;
  if ((p = strstr(text, "in line "))) {
    p += 8; *line = atoi(p);
    if (command=="abctab2ps" && ((p = strpbrk(p, ".")))) {
      p += 1; *col = atoi(p);
    } else {
      *col = -1; // abc2midi does not report column of error
    }
  }
  // take care of skipped part in textbuffer
  if (*line >= bufferskipbegin) *line += bufferskiplines;
  // mark error message in browser
  browser->deselect();
  browser->select(i);
  return i;
}

void CommandOutput::buttonnexterror_cb()
{
  int line, col, pos;
  if (!lasterrorindex) browser->deselect();
  lasterrorindex = nexterror(&line, &col);
  while (lasterrorindex && line < 0) // skip errors without line number
    lasterrorindex = nexterror(&line, &col);
  if (lasterrorindex) {
    mainwin->goto_line(line, (col > -1 ? col : 0));
    pos = mainwin->editor->insert_position();
    if (col > -1)
      mainwin->textbuffer->select(pos, pos+1);
    else
      mainwin->textbuffer->select(pos, mainwin->textbuffer->line_end(pos));
    mainwin->show();
  } else {
    message_box("No further errors","i");
  }
}

//==========================================================================
// About dialog
//==========================================================================

HelpAbout::HelpAbout(MainWin* mw) : HelpAbout_msk(mw)
{
  int x, y;

  // set initial position centered to main window
  x = mw->window->x() + mw->window->w() / 2 - window->w() / 2;
  if (x + window->w() > Fl::w() || x < 0)
    x = Fl::w() / 2 - window->w() / 2;
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  // GUI stuff not settable from fluid
  version->label(VERSION); // from flabc.h
  versiondate->label(VERSIONDATE); // from flabc.h
  abctab2ps->color(FL_BACKGROUND_COLOR);
  abctab2ps->visible_focus(0);
  abcm2ps->color(FL_BACKGROUND_COLOR);
  abcm2ps->visible_focus(0);
  abc2midi->color(FL_BACKGROUND_COLOR);
  abc2midi->visible_focus(0);
}

void HelpAbout::show()
{
  string result, command;
  int rc, from, to;

  // find out version information of abctab2ps
  command = mainwin->config->abctab2ps + " -h";
  if (0 != pipe_command(command.c_str(), &result, &rc) || (rc != 0)) {
    abctab2ps->value("--not found--");
  } else {
    from = result.find("version");
    if (from != (int)string::npos) {
      to = result.find_first_of("\n\r",from);
      if (to != (int)string::npos)
        abctab2ps->value(result.substr(from,to-from).c_str());
      else
        abctab2ps->value(result.substr(from).c_str());
    } else {
      abctab2ps->value("unknown version");
    }
  }

  // find out version information of abctab2ps
  command = mainwin->config->abcm2ps + " -V";
  if (0 != pipe_command(command.c_str(), &result, &rc) || (rc != 0)) {
    abcm2ps->value("--not found--");
  } else {
    from = result.find("abcm2ps");
    if (from != (int)string::npos) {
      from += 8; // line starts with "abcm2ps-"
      result.insert(8, "version ");
      to = result.find_first_of("\n\r",from);
      if (to != (int)string::npos)
        abcm2ps->value(result.substr(from,to-from).c_str());
      else
        abcm2ps->value(result.substr(from).c_str());
    } else {
      abcm2ps->value("unknown version");
    }
  }

  // find out version information of abc2midi
  command = mainwin->config->abc2midi + " -ver";
  if (0 != pipe_command(command.c_str(), &result, &rc) || (rc != 0)) {
    abc2midi->value("--not found--");
  } else {
    from = 0;
    result.insert(0, "version ");
    to = result.find_first_of("\n\r",from);
    if (to != (int)string::npos)
      abc2midi->value(result.substr(from,to-from).c_str());
    else
      abc2midi->value(result.substr(from).c_str());
  }

  // show window
  //window->redraw();
  window->show();
}

void HelpAbout::buttonlicense_cb()
{
  string textfile;
  const char* basename = "LICENSE";

  textfile = find_file(mainwin->config->docdirs.c_str(), basename, ";");
  if (textfile == "") {
    string msg = "Could not find '" + (string)basename + 
      "' in directories\n'" + mainwin->config->docdirs + "'";
    message_box(msg.c_str());
    return;
  }
  if (!mainwin->textview_dialog)
    mainwin->textview_dialog = new TextViewer(mainwin);
  if (0 == mainwin->textview_dialog->load_file(textfile.c_str())) {
    mainwin->textview_dialog->window->label("Flabc License");
    mainwin->textview_dialog->show();
  } else {
    string msg = "Error while reading '" + textfile + "'";
    message_box(msg.c_str());
  }

  // to avoid modal about dialog staying on top
  window->hide();
}


//==========================================================================
// TextViewer
//==========================================================================

TextViewer::TextViewer(MainWin* mw) : TextViewer_msk(mw)
{
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() + 80;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 40;
  if (y + window->h() > Fl::h())
    y = 40;
  window->position(x,y);

  // GUI stuff not settable from fluid
  textdisplay->clear_visible_focus();
}

// load a text file into browser
// RC: 0 = ok, 1 = error
int TextViewer::load_file(const char* filename)
{
  return textbuffer.loadfile(filename);
}

//==========================================================================
// Template and Example browser
//==========================================================================

TemplatePicker::TemplatePicker(MainWin* mw, bool examplepicker /* = false*/) : TemplatePicker_msk(mw)
{
  int x, y;

  // set initial position to upper left corner
  x = mw->window->x() + 40;
  y = mw->window->y() + 40;
  window->position(x,y);

  // adjust for examples instead of templates
  isexamplepicker = examplepicker;
  if (examplepicker) {
    window->label("Open Example");
    buttonload->label("Load Example");
    boxhint->label("Hint: You can customize example directories "
                   "with the tab \"Templates\" in "
                   "\"Options/System Environment\"");
  }

  // fill browser
  if (examplepicker)
    load_template_files(mw->config->exampledirs);
  else
    load_template_files(mw->config->templatedirs);
}

TemplatePicker::~TemplatePicker()
{
  templatefiles.clear();
}

void TemplatePicker::buttonload_cb()
{
  // make sure that user picked template
  if (!browser->value()) {
    if (isexamplepicker)
      message_box("Please select example or press Cancel");
    else
      message_box("Please select template or press Cancel");
  } else {
    buttonpressed = 1;
    window->hide();
  }
}

// reads all abc files from the given semicolon seperated list
// of template directories
void TemplatePicker::load_template_files(string templatedirs)
{
  char *dirlistcopy, *dir;
  const char* sepchars = ";";
  string file;
  int i,n;
  dirent** list;

  // fill filelist with all *.abc files i template directories
  dirlistcopy = (char*)alloca(sizeof(char)*(templatedirs.size()+4));
  strcpy(dirlistcopy, templatedirs.c_str());
  dir = strtok(dirlistcopy,sepchars);
  while (dir) {
    n = fl_filename_list(dir, &list);
    for (int i = n-1; i >= 0; i--) {
      file = list[i]->d_name;
      if (fl_filename_match(file.c_str(), "*.abc")) {
        file = (string)dir + DIRSEPCHAR + file;
        templatefiles.push_back(file);
      }
      free((void*)(list[i]));
    }
    if (n > 0) free((void*)list);
    dir = strtok(NULL,sepchars);
  }

  // copy data from filelist over into browser
  browser->clear();
  for (i=0; i < (int)templatefiles.size(); i++) {
    browser->add((basename(templatefiles[i])).c_str(), (void*)&templatefiles[i]);
  }

}


// shows the dialog modal and fills line and col
// RC: 0 = user pressed cancel, 1 = user pressed ok
int TemplatePicker::get_template(string* rcfile)
{
  buttonpressed = -1;
  *rcfile = "";
  window->show();
  while (buttonpressed == -1) Fl::wait();
  if (buttonpressed == 0) 
    return 0;

  if (browser->value())
    //*rcfile = (char*)(browser->data(browser->value()));
    *rcfile = *(string*)(browser->data(browser->value()));
  return 1;
}

//==========================================================================
// Font browser
//==========================================================================

// when nfonts is 0, it is determined automatically by calling Fl::set_fonts()
FontPicker::FontPicker(int nfonts)
{
  uchar c;
  int i = 0;
  int t;
  char* p;
  char intstr[8];
  string namestr;

  // fill display text with all characters
  for (c = ' '+1; c < 127; c++) {
    if (!(c&0x1f)) displaytext[i++]='\n'; 
    if (c=='@') displaytext[i++]=c;
    displaytext[i++]=c;
  }
  displaytext[i++] = '\n';
  for (c = 0xA1; c; c++) {if (!(c&0x1f)) displaytext[i++]='\n'; displaytext[i++]=c;}
  displaytext[i] = 0;
  displaybox->label(displaytext);

  // load font names into browser
  if (nfonts <= 0) nfonts = Fl::set_fonts();
  for (i=0; i<nfonts; i++) {
    char* name = (char*)Fl::get_font_name((Fl_Font)i,&t);
    // when strange characters, do not use font
    for (p=name; *p && (isalnum(*p) || isspace(*p) || strchr(".-+",*p)); p++);
    if (*p) continue; // strange character in font name
    attributes.push_back(t);
    fontnumbers.push_back(i);
    fontnames.push_back(name);
    namestr = name;
    if (t) {
      if (t & FL_BOLD) namestr = "@b" + namestr;
      if (t & FL_ITALIC) namestr = "@i" + namestr;
    }    
    fontbrowser->add(namestr.c_str());
    // check whether font is proportional or monospaced
    fl_font((Fl_Font)i, 12);
    if (fl_width(".....") == fl_width("mmmmm"))
      monospaced.push_back(1);
    else
      monospaced.push_back(0);
  }

  // load sizes (currently all)
  for (i=1; i<64; i++) {
    sprintf(intstr, "%d", i);
    sizebrowser->add(intstr);
  }
}

FontPicker::~FontPicker() {}

// actualize text display when font changed
void FontPicker::fontbrowser_cb() 
{
  int i;
  Fl_Font f;
  i = fontbrowser->value();
  if (!i) return;
  f = (Fl_Font)fontnumbers[i-1];
  if (displaybox->labelfont() != f) {
    displaybox->labelfont(f);
    displaybox->redraw();
  }
}

// actualize text display when size changed
void FontPicker::sizebrowser_cb()
{
  int i,s;
  i = sizebrowser->value();
  if (!i) return;
  //s = atoi(sizebrowser->text(i));
  s = i;
  if (displaybox->labelsize() != s) {
    displaybox->labelsize(s);
    displaybox->redraw();
  }
}


// shows the dialog modal and fills fontname and size
// font and size are initialized to size and fontnumber
// bold and italic set properties for all fonts
// RC: 0 = user pressed cancel, 1 = user pressed ok
int FontPicker::get_monospaced_font(string* fontname, int* size, Fl_Font* fontnumber)
{
  int i;

  // hide proportional fonts
  for (i=0; i<fontbrowser->size(); i++) {
    if (monospaced[i]) fontbrowser->show(i+1);
    else fontbrowser->hide(i+1);
    if (*fontnumber == fontnumbers[i]) fontbrowser->value(i+1);
  }

  // set size value
  if (*size && *size <= sizebrowser->size())
    sizebrowser->value(*size);
  else
    sizebrowser->value(sizebrowser->size());

  // apply size and font setting to text display
  fontbrowser_cb(); sizebrowser_cb();

  buttonpressed = -1;
  window->show();
  while (buttonpressed == -1) Fl::wait();
  if (buttonpressed == 0) 
    return 0;

  if (fontbrowser->value()) {
    *fontname = fontnames[fontbrowser->value() - 1];
    *fontnumber = (Fl_Font)fontnumbers[fontbrowser->value() - 1];
  }
  if (sizebrowser->value())
    *size = sizebrowser->value();
  return 1;

}

//==========================================================================
// Fixup Xrefs dialog
//==========================================================================

FixupXrefs::FixupXrefs(MainWin* mw) : FixupXrefs_msk(mw)
{
  int x, y;

  // set initial position to upper right corner
  x = mw->window->x() + mw->window->w() - window->w() - 50;
  if (x + window->w() > Fl::w())
    x = Fl::w() - window->w();
  y = mw->window->y() + 80;
  if (y + window->h() > Fl::h())
    y = 80;
  window->position(x,y);
}

// shows the dialog modal and returns the working mode
// RC: 0 = user pressed cancel, 1 = consecutive numbering, 2 = allow gaps
int FixupXrefs::get_mode()
{
  buttonpressed = -1;
  window->show();
  while (buttonpressed == -1) Fl::wait();
  if (buttonpressed == 0) return 0;
  if (allowgaps->value()) return 2;
  else return 1;
}
