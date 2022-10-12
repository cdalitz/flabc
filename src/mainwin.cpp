//
// Actual Main Window (MainWin_msk is only base class for GUI stuff)
//

#include "flabc.h"
#include "mainwin.h"
#include "dialogs.h"
#include "os.h"
#include <stdio.h>
#include <errno.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/x.H>


MainWin::MainWin(Config* conf) : MainWin_msk() {

  filename = "";
  changed = loading = undoing = redoing = false;
  optionslook_dialog = NULL;
  optionsbehaviour_dialog = NULL;
  optionssystem_dialog = NULL;
  abcformatlist_dialog = NULL;
  midilist_dialog = NULL;
  commandoutput_dialog = NULL;
  helpabout_dialog = NULL;
  gotoline_dialog = NULL;
  gotobar_dialog = NULL;
  search_dialog = NULL;
  replace_dialog = NULL;
  textview_dialog = NULL;
  template_dialog = NULL;
  example_dialog = NULL;
  fixupxrefs_dialog = NULL;
  config = conf;
  abc_filename_pattern = (char*)"abc Files (*.abc)\tAll Files (*)";
  exampleloaded = false;

  // syntax highlighting stuff
  highlight = new SyntaxHighlighter(textbuffer, editor, conf->styletable, conf->nstyles);
  textbuffer->add_modify_callback(changed_cb_static, this);
  editor->highlight_data(highlight->stylebuffer, highlight->styletable,
                         highlight->nstyles, 'A',
                         SyntaxHighlighter::style_unfinished_cb, 0);
  textbuffer->add_modify_callback(SyntaxHighlighter::style_update, (void*)highlight);
  editor->color(conf->backgroundcolor);

  // option for no deadkeys (OSX only)
  editor->abcnodeadkeys = conf->abcnodeadkeys;

  // menu selection color on OSX (does not work with FLTK 1.1.7 and 1.3)
  menubar->selection_color(fl_rgb_color('\x38','\x75','\xd7'));

  // keyboard shortcuts
  // disable undo for own implementation (unlimited undo)
  editor->remove_key_binding('z', FL_CTRL);
#ifdef __APPLE__
  editor->remove_key_binding('z', FL_COMMAND);
#endif
  // disable Ctrl-A, because it should be Emacs like (goto-line-start)
  editor->remove_key_binding('a', FL_CTRL);
#ifdef __APPLE__
  // we need to disable CMD-A in order to keep Abc-menu accessible
  editor->remove_key_binding('a', FL_COMMAND);
  // use command key (instead of control) for some menu items
  menu_file_newwindow->shortcut(FL_COMMAND + 'n');
  menu_options_larger->shortcut(FL_COMMAND + '+');
  menu_options_smaller->shortcut(FL_COMMAND + '-');
  // on OSX Home/End have different meanings:
  //       Home = Buffer start, End = buffer end
  //       Ctrl + Arrow left = Line start, Ctrl + Arrow Right = Line end
//   editor->remove_key_binding(FL_Home, 0);
  menu_edit_cua_bufferstart->shortcut(FL_COMMAND + FL_Home);
  menu_edit_cua_bufferend->shortcut(FL_COMMAND + FL_End);
//   editor->remove_key_binding(FL_End, 0);
//   menu_edit_cua_bufferend->shortcut(FL_End);
//   editor->remove_key_binding(FL_Left, FL_CTRL);
//   //menu_edit_cua_linestart->shortcut(FL_CTRL + FL_Left);
//   editor->remove_key_binding(FL_Right, FL_CTRL);
//   //menu_edit_cua_lineend->shortcut(FL_CTRL + FL_Right);
#endif

  // set window icon on Win32
#ifdef WIN32
  window->icon((char *)LoadIcon(fl_display, "WinIcon"));
#endif
}


//=====================================================================
// which abc*2ps program shall be called?
//=====================================================================

// returns "abctab2ps" or "abcm2ps"
string MainWin::which_abc2ps_command()
{
  string abc2ps = config->defaultabc2ps;
  if (!config->ignoreshebang &&
      textbuffer->char_at(0) == '%' && textbuffer->char_at(1) == '!') {
    char* firstline = textbuffer->text_range(0,textbuffer->line_end(0));
    if (0 == strncmp(firstline,"%!abctab2ps",11))
      abc2ps = "abctab2ps";
    else if (0 == strncmp(firstline,"%!abcm2ps",9))
      abc2ps = "abcm2ps";
    free(firstline);
  }
  return abc2ps;
}

//=====================================================================
// file operations
//=====================================================================

void MainWin::new_file(const char* name /*= NULL*/)
{
  textbuffer->select(0, textbuffer->length());
  textbuffer->remove_selection();
  // distinguish between newly to create file (came from command line arg)
  // and a new clean window
  if (name != NULL) {
    filename = name;
    window->copy_label(basename(filename).c_str());
  } else {
    filename = "";
    window->label("flabc");
  }
  changed = 0;
  undovector.clear(); redovector.clear();
  textbuffer->call_modify_callbacks();

}

void MainWin::load_file(const char* name)
{
  bool fromlatin1 = (config->defaultabc2ps == "abctab2ps");
  loading = true;
  if (0==textbuffer->loadfile(name, fromlatin1)) {
    filename = name;
    window->copy_label(basename(filename).c_str());
    changed = false;
    undovector.clear(); redovector.clear();
    editor->insert_position(0);
    editor->scroll(0,0);
  } else {
    string msg = "Error reading from file \'" + (string)name +
      "':\n" + (string)strerror(errno);
    message_box(msg.c_str());
  }
  loading = false;
  textbuffer->call_modify_callbacks();
}

void MainWin::save_file(const char* name) {
  bool tolatin1 = (which_abc2ps_command() == "abctab2ps");
  if (0==textbuffer->savefile(name, tolatin1)) {
    filename = name;
    window->copy_label(basename(filename).c_str());
    changed = false;
  } else {
    string msg = "Error writing to file \'" + (string)name +
      "':\n" + (string)strerror(errno);
    message_box(msg.c_str());
  }
  textbuffer->call_modify_callbacks();
}

//
// ask whether the file should be saved before quit
// return values: 1 = proceed, 0 = user wants to cancel operation
//
int MainWin::ask_for_save(void) {
  if (!changed) return 1;
  string title = basename(filename);
  int r = choice_box("Changes in the current file will be lost.\n"
                     "What shall we do?",
                     "Cancel Operation", "Discard Changes", "Save File",
                     title.c_str());
  if (r == 0) {
    return 0;
  }
  else if (r == 2) {
    cbmenu_file_save();
    if (changed) return 0; // save did not work
  }
  return 1;
}

//=====================================================================
// editor tools
//=====================================================================

// returns the found position or -1 when not found
int MainWin::find_string(const char* text, int matchcase, int regexp)
{
  int pos, found, matchlen;
  string msgstart;
  if (regexp) msgstart = "Regexp '"; else msgstart = "String '";
  pos = editor->insert_position();
  matchlen = strlen(text);
  if (regexp) {
    matchlen = textbuffer->search_regex(pos, text, &pos, matchcase);
    if (matchlen < 0) return -1;
    found = (matchlen > 0);
  } else {
    found = textbuffer->search_forward(pos, text, &pos, matchcase);
  }
  if (found) {
    // Found a match; select and update the position...
    textbuffer->select(pos, pos+matchlen);
    editor->insert_position(pos+matchlen);
    editor->show_insert_position();
  } else {
    string msg = msgstart + (string)text + "' not found.\n"
      "Search again from bufferstart?";
    if (1 == choice_box(msg.c_str(), "Quit", "Search", NULL, "Search")) {
      if (regexp) {
        matchlen = textbuffer->search_regex(0, text, &pos, matchcase);
        if (matchlen < 0) return -1;
        found = (matchlen > 0);
      } else {
        found = textbuffer->search_forward(0, text, &pos, matchcase);
      }
      if (found) {
        // Found a match; select and update the position...
        textbuffer->select(pos, pos+matchlen);
        editor->insert_position(pos+matchlen);
        editor->show_insert_position();
      } else {
        string msg = msgstart + (string)text + "' not found.";
        message_box(msg.c_str());
      }
    }
  }
  if (found) return pos; else return -1;
}

void MainWin::goto_line(int line, int col /* = 0 */)
{
  int i;
  if (line > -1) {
    editor->insert_position(0);
    for (i=1; i<line; i++) editor->move_down();
    if (col > 1) {
      i = textbuffer->line_end(editor->insert_position()) -
        editor->insert_position();
      if (col >= i)
        col = i + 1;
      editor->insert_position(editor->insert_position()+col-1);
    }
    editor->scroll(line,0); // make line top line in visible region
    editor->show_insert_position(); // if col outside visible region
  }
}

void MainWin::goto_bar(int bar, string voice /*= ""*/, string tune /*= ""*/)
{
  int pos, spos;  // current position in buffer, second helper position
  int pos1, nlines; // position when found == 1, current line number
  int cbar;         // current bar
  string ctune, cvoice; // current tune, voice
  char *line, *styleline, *p, *q, *nextbar;
  int found; // 0: cbar < bar
             // 2: cbar = bar and music found
             // 1: cbar = bar but not yet music found
  const char* bnumfirst = "%%barnumberfirst";
  int slenbnumfirst = strlen(bnumfirst);
  const char* whitespace = " \t\r\n";

  // look for global first barnumber before first tune
  if (!(textbuffer->search_forward(0, "X:", &spos, 1 /*matchCase*/)
      && spos == textbuffer->line_start(spos))) {
    message_box("No tune found.\nDid you forget the X: field?");
    return;
  }   
  if (textbuffer->search_forward(0, bnumfirst, &pos, 1 /*matchCase*/)
      && pos == textbuffer->line_start(pos)
      && pos < spos) {
    line = textbuffer->text_range(pos + slenbnumfirst,
                                  textbuffer->line_end(pos));
    cbar = atoi(line);
    free(line);
  } else {
    cbar = 1;
  }

  // read line for line and ignore lines not in tune/voice
  // assumption: stylebuffer is correctly filled from SyntaxHighlighter
  // possible states of variable "found":
  //   0 = no special meaning
  //   1 = bar found at end of previous line 
  //         => increment of bar counter is pending until music found
  //   2 = searched bar number found
  found = 0;
  nlines = 0;
  for (pos=0; (found < 2) && (pos < textbuffer->length());
       pos=textbuffer->line_end(pos)+1) {
    nlines++;
    line = textbuffer->line_text(pos);
    // tune change
    if (0 == strncmp(line,"X:",2)) {
      if ((p = strpbrk(line+2,whitespace))) *p = '\0';
      ctune = line + 2;
      free(line);
      // when tune found and no bar and voice given, stop here
      if (voice.empty() && bar == 0 && tune == ctune) {
        found = 2; break;
      } else {
        continue;
      }
    }
    // voice change
    else if (0 == strncmp(line,"V:",2)) {
      if ((p = strpbrk(line+2,whitespace))) *p = '\0';
      cvoice = line + 2;
      free(line);
      // when voice found and no bar given, stop here
      if (bar == 0 && voice == cvoice && (tune.empty() || ctune == tune)) {
        found = 2; break;
      } else {
        continue;
      }
    }
    // info field (except inline)
    else if (line[0] != '[' &&
             highlight->stylebuffer->char_at(pos) == 'C') {
      free(line); continue;
    }
    // bar number intitialization
    else if (0 == strncmp(line,bnumfirst,slenbnumfirst)) {
      cbar = atoi(line +  slenbnumfirst);
      free(line); continue;
    }
    // comment line
    else if (line[0] == '%') {
      free(line); continue;
    }
    // inline voice change
    // simplifying assumption: one line is only of one voice
    else if ((p = strstr(line,"[V:"))) {
      int nchars = strcspn(p+3,"]");
      cvoice = p+3; cvoice = cvoice.substr(0,nchars);
    }

    // ignore unwanted tunes/voices
    if (!tune.empty() && tune != ctune) { free(line); continue; }
    if (!voice.empty() && voice != cvoice) { free(line); continue; }

    // count bars (style 'E' in highlight data)
    styleline = highlight->stylebuffer->text_range(textbuffer->line_start(pos),
                                              textbuffer->line_end(pos));
    p = styleline;
    q = line;
    while (p && *p) {
      // when bar line found (pending bar counter increment): look for music
      if (found == 1) {
        spos = pos;
        while (*p) {
          if (*p == 'A' && !strchr(" \t\r\n\\",*q)) {
            found = 0; break;
          }
          p++; q++; spos++; 
        }
        if (found == 0) {
          cbar++; pos = spos;
        } else {
          // keep flag that we are still searching for first music symbol
          // and remember position in case we will not find one
          // found = 1;
          pos1 = pos;
          break; // nothing found in current line
        }
      }
      // searched bar already found?
      if (cbar >= bar) {
        found = 2;
        break;
      }
      // look for next bar line
      nextbar = strchr(p,'E');
      if (nextbar) {
        q += (nextbar - p); pos += (nextbar - p); p = nextbar;
        // ignore invisible bars [|]
        if (0 != strncmp(q, "[|]", 3)) {
          found = 1;
        }
        while (*p=='E') { p++; q++; pos++; }
        pos1 = pos;
      } else {
        break;
      }
    }
    free(line); free(styleline);
    if (found == 2) break; // avoid resetting of position to next line
  }

  // if trailing bar line without music found: reset position
  if (found == 1) pos = pos1;

  // move to position
  if (found && pos < textbuffer->length()) {
    editor->insert_position(pos);
    editor->scroll(nlines,0);
    editor->show_insert_position();
    if (bar > 0) {
      // mark until next bar line
      spos = textbuffer->line_end(pos);
      for (pos1 = pos; pos1 < spos; pos1++)
        if (highlight->stylebuffer->char_at(pos1) == 'E') {
          break;
        }
      textbuffer->select(pos,pos1);
    } else {
      // highlight entire line
      textbuffer->select(pos,textbuffer->line_end(pos));
    }
  } else {
    string message = "";
    char barstr[16];
    sprintf(barstr,"%d",bar);
    if (bar > 0) {
      message += "Bar " + (string)barstr + " not found";
      if (!tune.empty()) message += " in tune '" + tune + "'";
      if (!voice.empty()) message += " in voice '" + voice + "'";
    } else {
      if (voice.empty()) {
        message += "Tune '" + tune + "' not found";
      } else {
        message += "Voice '" + voice + "' not found";
        if (!tune.empty()) message += " in tune '" + tune + "'";
      }
    }
    message_box(message.c_str());
  }
}

// save the tune around current cursorposition
// skipbegin and skiplen are filled with the first line
// that is skipped and the number of lines skipped until the
// current tune begins
// RC: 0 = operation successful; 1 = an error occured
int MainWin::save_current_tune(const char* filename, int* skipbegin, int* skiplines, bool forabc2midi /*=false*/)
{
  FILE* f;
  char *text, *textlatin1;
  string preparedabc;
  bool savelatin1 = true;
  int sb, tbpos, pos, n; // skipbegin, tunebegin position, current position

  f = fopen(filename, "w");
  if (!f) {
    string msg = "Cannot write to '" + (string)filename + 
      "':\n" + (string)strerror(errno);
    message_box(msg.c_str());
    return 1;
  }

  // abcm2ps or abc2midi use utf8 encoding
  if (which_abc2ps_command() == "abcm2ps")
    savelatin1 = false;
  if (forabc2midi) {
    savelatin1 = false;
  }

  // find line in which current tune begins
  pos = textbuffer->line_start(editor->insert_position());
  while (pos > 0) {
    if (textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':')
      break;
    pos = textbuffer->line_start(pos-1);
  }
  // if cursor in header: set tunebegin to next found tune
  if (pos == 0 && 
      !(textbuffer->char_at(pos) == 'X' && textbuffer->char_at(pos+1) == ':')) {
    pos = textbuffer->line_end(editor->insert_position()) + 1;
    while (pos < textbuffer->length() - 1) {
      if (textbuffer->char_at(pos) == 'X' && 
          textbuffer->char_at(pos+1) == ':')
        break;
      pos = textbuffer->line_end(pos) + 1;
    }
  }
  if (!(textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':')) {
    message_box("No tune found in buffer");
    return 1;
  }
  tbpos = pos;

  // write header before all tunes
  sb = 1; pos = 0;
  while (pos < textbuffer->length()) {
    if (textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':')
      break;
    pos = textbuffer->line_end(pos) + 1;
    sb++;
  }
  if (pos > 0) {
    text = textbuffer->text_range(0, pos); // start <= range < end
    n = strlen(text);
    if (savelatin1) {
      textlatin1 = (char*)malloc(sizeof(char)*(n+1));
      fl_utf8toa(text, n, textlatin1, n+1);
    } else {
      textlatin1 = strdup(text);
    }
    if (forabc2midi) {
      preparedabc = prepare_for_abc2midi(textlatin1);
      fputs(preparedabc.c_str(), f);
    } else {
      fputs(textlatin1, f);
    }
    free(text);
    free(textlatin1);
  }

  // skip all lines until tunepos
  *skipbegin = sb;
  *skiplines = textbuffer->count_lines(pos, tbpos);
  pos = textbuffer->line_end(tbpos) + 1;

  // write current tune until next tune is found
  while (pos < textbuffer->length() - 1) {
    if (textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':')
      break;
    pos = textbuffer->line_end(pos) + 1;
  }
  if (pos > textbuffer->length())
    pos = textbuffer->length();
  text = textbuffer->text_range(tbpos,pos); // tbpos <= range < pos
  n = strlen(text);
  if (savelatin1) {
    textlatin1 = (char*)malloc(sizeof(char)*(n+1));
    fl_utf8toa(text, n, textlatin1, n+1);
  } else {
    textlatin1 = strdup(text);
  }
  if (forabc2midi) {
    preparedabc = prepare_for_abc2midi(textlatin1);
    fputs(preparedabc.c_str(), f);
  } else {
    fputs(textlatin1, f);
  }
  free(text);
  free(textlatin1);

  fclose(f);
  return 0;
}

// prepare abc code such that it is better suitable for abc2midi
string MainWin::prepare_for_abc2midi(const char* text)
{
  char* s;
  bool intext = false;
  string prepedabc = "";
  string line = "";
  for (s=(char*)text; *s; s++) {
    line += *s;
    if (*s == '\n') { // line finished
      if (0 == strncmp(line.c_str(), "%%begintext", 11))
        intext = true;
      if (intext)
        prepedabc += "% ";
      if (0 == strncmp(line.c_str(), "%%endtext", 9))
        intext = false;
      prepedabc += line;
      line = "";
    }
  }
  // do not forget last line
  if (intext) prepedabc += "% ";
  prepedabc += line;
  return prepedabc;
}


//=====================================================================
// menu item callbacks
//=====================================================================

//---------------------------------------------------------------------
// File menu
//---------------------------------------------------------------------

void MainWin::cbmenu_file_new()
{
  if (!ask_for_save()) return;
  new_file();
}

void MainWin::cbmenu_file_newwindow()
{
  // note that we never delete the newly created window
  // => memory will only be freed on program termination
  MainWin* mw = new MainWin(this->config);
  mw->show();
}

void MainWin::cbmenu_file_newtemplate()
{
  string templatefile;

  if (!ask_for_save()) return;
  if (!template_dialog) template_dialog = new TemplatePicker(this);
  if (template_dialog->get_template(&templatefile) && !templatefile.empty()) {
    // clear contents
    textbuffer->select(0, textbuffer->length());
    textbuffer->remove_selection();
    filename = "";
    window->label("flabc");
    changed = 0;
    undovector.clear(); redovector.clear();
    // load template
    textbuffer->loadfile(templatefile.c_str());
    editor->insert_position(0);
    textbuffer->call_modify_callbacks();
  }
}

void MainWin::cbmenu_file_open()
{
  if (!ask_for_save()) return;
  char *newfile;
  if (!filename.empty()) {
    newfile = fl_file_chooser("Open File?", abc_filename_pattern, filename.c_str());
  } else {
    string defdir = get_default_directory();
    newfile = fl_file_chooser("Open File?", abc_filename_pattern, defdir.c_str());
  }
  if (newfile != NULL) load_file(newfile);
}

void MainWin::cbmenu_file_save()
{
  bool saveas = false;
  if (exampleloaded) {
    int rc;
    rc = choice_box("Marning: This will overwrite example file!", 
                    "Cancel", "Save as...", "Overwrite", "Saving Example");
    if (rc == 0 || rc > 2) return;
    else if (rc == 1) saveas = true;
  }
  if (filename == "" || saveas) {
    string defdir = get_default_directory() + "/";
    char* newfile = fl_file_chooser("Save File?", abc_filename_pattern, defdir.c_str());
    if (newfile != NULL) {
      save_file(newfile);
      exampleloaded = false;
    }
  } else {
    save_file(filename.c_str());
    exampleloaded = false;
  }
}

void MainWin::cbmenu_file_saveas()
{
    char* newfile;
    if (filename == "") {
      string defdir = get_default_directory() + "/";
      newfile = fl_file_chooser("Save File?", abc_filename_pattern, defdir.c_str());
    } else {
      newfile = fl_file_chooser("Save File?", abc_filename_pattern, filename.c_str());
    }
    if (newfile != NULL) {
      save_file(newfile);
      exampleloaded = false;
    }
}

void MainWin::cbmenu_file_insert()
{
  char* newfile;
  if (filename == "") {
    string defdir = get_default_directory() + "/";
    newfile = fl_file_chooser("Insert File?", abc_filename_pattern, defdir.c_str());
  } else {
    newfile = fl_file_chooser("Insert File?", abc_filename_pattern, filename.c_str());
  }
  if (newfile != NULL) {
    bool fromlatin1 = (config->defaultabc2ps == "abctab2ps");
    textbuffer->insertfile(newfile, editor->insert_position(), fromlatin1);
    textbuffer->call_modify_callbacks();
  }
}

void MainWin::cbmenu_file_loadexample()
{
  string examplefile;

  if (!ask_for_save()) return;
  if (!example_dialog) example_dialog = new TemplatePicker(this,true);
  if (example_dialog->get_template(&examplefile) && !examplefile.empty()) {
    load_file(examplefile.c_str());
    exampleloaded = true;
  }
}

void MainWin::cbmenu_file_exit()
{
  if (config->askonexit && !ask_for_save()) return;
  if (optionslook_dialog) optionslook_dialog->window->hide();
  if (optionsbehaviour_dialog) optionsbehaviour_dialog->window->hide();
  if (optionssystem_dialog) optionssystem_dialog->window->hide();
  if (abcformatlist_dialog) abcformatlist_dialog->window->hide();
  if (midilist_dialog) midilist_dialog->window->hide();
  if (commandoutput_dialog) commandoutput_dialog->window->hide();
  if (helpabout_dialog) helpabout_dialog->window->hide();
  if (gotoline_dialog) gotoline_dialog->window->hide();
  if (gotobar_dialog) gotobar_dialog->window->hide();
  if (search_dialog) search_dialog->window->hide();
  if (replace_dialog) replace_dialog->window->hide();
  if (template_dialog) template_dialog->window->hide();
  if (example_dialog) example_dialog->window->hide();
  if (fixupxrefs_dialog) fixupxrefs_dialog->window->hide();
  window->hide();
}

//---------------------------------------------------------------------
// Edit menu
//---------------------------------------------------------------------

void MainWin::cbmenu_edit_undo()
{
  if (undovector.empty()) {
    message_box("No further undo information");
    return;
  }
  UndoOperation u = undovector.back();
  undoing = true;
  if (u.nInserted && u.nDeleted) {
    textbuffer->replace(u.pos, u.pos + u.nInserted, u.deletedText.c_str());
    editor->insert_position(u.pos);
  }
  else if (u.nInserted) {
    textbuffer->remove(u.pos, u.pos + u.nInserted);
    editor->insert_position(u.pos);
  }
  else if (u.nDeleted) {
    textbuffer->insert(u.pos, u.deletedText.c_str());
    editor->insert_position(u.pos);
  }
  undovector.pop_back();
  undoing = false;
}

void MainWin::cbmenu_edit_redo()
{
  if (redovector.empty()) {
    message_box("No further redo information");
    return;
  }
  UndoOperation u = redovector.back();
  redoing = true;
  if (u.nInserted && u.nDeleted) {
    textbuffer->replace(u.pos, u.pos + u.nInserted, u.deletedText.c_str());
    editor->insert_position(u.pos);
  }
  else if (u.nInserted) {
    textbuffer->remove(u.pos, u.pos + u.nInserted);
    editor->insert_position(u.pos);
  }
  else if (u.nDeleted) {
    textbuffer->insert(u.pos, u.deletedText.c_str());
    editor->insert_position(u.pos);
  }
  redovector.pop_back();
  redoing = false;
}

//---------------------------------------------------------------------
// Search menu
//---------------------------------------------------------------------

void MainWin::cbmenu_search_find()
{
  char* text;
  int casesensitive, regexp;
  if (!search_dialog) {
    search_dialog = new Search(this);
  } else {
    if (*search_dialog->text->value()) {
      search_dialog->text->position(strlen(search_dialog->text->value()));
      search_dialog->text->mark(0);
    }
  }
  if (1 == search_dialog->get_search_data(&text,&casesensitive,&regexp)) {
    find_string(text, casesensitive, regexp);
  }
}

void MainWin::cbmenu_search_findagain()
{
  if (!search_dialog || !*search_dialog->text->value()) {
    cbmenu_search_find();
  } else {
    find_string(search_dialog->text->value(),
                search_dialog->casesensitive->value(),
                search_dialog->regexp->value());
  }
}

void MainWin::cbmenu_search_replace()
{
  if (!replace_dialog) {
    replace_dialog = new Replace(this);
  }
  replace_dialog->show();
  // moval of mouse pointer into replace dialog no longer necessary
  // because nonmodal dialogs have been fixed since fltk-1.1-svn-20061030
  //set_mouse_position(replace_dialog->window->x() + 
  //                   replace_dialog->search->x() +
  //                   3 * replace_dialog->search->w() / 4,
  //                   replace_dialog->window->y() + 
  //                   replace_dialog->search->y() +
  //                   replace_dialog->search->h() / 2);
}

void MainWin::cbmenu_search_gotoline()
{
  int line, col;
  if (!gotoline_dialog) gotoline_dialog = new GotoLine(this);
  if (1 == gotoline_dialog->get_line_column(&line,&col)) {
    goto_line(line,col);
  }
}

void MainWin::cbmenu_search_gotobar()
{
  int bar;
  string voice, tune;
  if (!gotobar_dialog) gotobar_dialog = new GotoBar(this);
  if (1 == gotobar_dialog->get_bar_voice_tune(&bar,&voice,&tune)) {
    goto_bar(bar,voice,tune);
  }
}

//---------------------------------------------------------------------
// Options menu
//---------------------------------------------------------------------

void MainWin::cbmenu_options_larger()
{
  int i;
  for (i=0; i<highlight->nstyles; i++) {
    highlight->styletable[i].size++;
  }
  //editor->redisplay_range(0,textbuffer->length());
  editor->redraw();
  editor->show_insert_position();
}

void MainWin::cbmenu_options_smaller()
{
  int i;
  if (highlight->styletable[0].size < 6) return;
  for (i=0; i<highlight->nstyles; i++) {
    highlight->styletable[i].size--;
  }
  //editor->redisplay_range(0,textbuffer->length());
  editor->redraw();
  editor->show_insert_position();
}

void MainWin::cbmenu_options_look()
{
  if (!optionslook_dialog) optionslook_dialog = new OptionsLook(this);
  optionslook_dialog->show();
}

void MainWin::cbmenu_options_behaviour()
{
  if (!optionsbehaviour_dialog) optionsbehaviour_dialog = new OptionsBehaviour(this);
  optionsbehaviour_dialog->show();
}

void MainWin::cbmenu_options_system()
{
  if (!optionssystem_dialog) optionssystem_dialog = new OptionsSystem(this);
  optionssystem_dialog->show();
}

void MainWin::cbmenu_options_save()
{
  string msg;
  msg = config->WritePrefs();
  msg = "Options saved to file\n" + msg;
  message_box(msg.c_str());
}

//---------------------------------------------------------------------
// Abc menu
//---------------------------------------------------------------------

void MainWin::cbmenu_abc_preview()
{
  string result, command, error;
  int rc, line, col, pos;
  bool hastune = false;

  // check for tunes in textbuffer (X: field)
  pos = 0;
  while (pos < textbuffer->length() - 1) {
    if (textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':') {
      hastune = true;
      break;
    }
    pos = textbuffer->line_end(pos) + 1;
  }
  if (!hastune) {
    message_box("No tune found in buffer");
    return;
  }

  // save when necessary before running abctab2ps on the file
  if (filename.empty() || changed) {
    cbmenu_file_save();
    if (filename.empty() || changed) // save did not work or user pressed esc
      return;
  }

  // call abctab2ps or abcm2ps
  if (which_abc2ps_command() == "abctab2ps") {
    // make sure abctab2ps finds its fonts
    if (!config->abctabfonts.empty()) {
      string s = config->abctabfonts + 
        ";/usr/share/abctab2ps;/usr/local/share/abctab2ps";
      setenv("ABCTABFONTS", s.c_str(), 1);
#ifndef WIN32
      // workaround for bug in OSX 10.10:
      command = (string)"ABCTABFONTS=\"" + s + "\"; ";
#endif
    } else {
      unsetenv("ABCTABFONTS");
    }
    command += config->abctab2ps + " -O \"" + config->pstmpfile + 
      "\" \"" + filename + "\"";
  } else {
    // abcm2ps
    command = config->abcm2ps + " -O \"" + config->pstmpfile + 
      "\" \"" + filename + "\"";
  }
  if (pipe_command(command.c_str(), &result, &rc)) {
    string msg = "The following command failed:\n" + command;
    message_box(msg.c_str());
    return;
  }
  if (!commandoutput_dialog) commandoutput_dialog = new CommandOutput(this);
  commandoutput_dialog->load_data(result.c_str(), "abctab2ps");
  if (rc || commandoutput_dialog->nexterror(&line, &col) > 0) {
    commandoutput_dialog->show();
    if (1 != choice_box("An error occured.\nPreview anyway?",NULL,"Preview","Cancel")) {
      return;
    }
  }

  // show PS output
  if (open_ps(config->pstmpfile.c_str(), config->psviewer.c_str(), &error)) {
    string msg = "Error in opening '" + config->pstmpfile + "'\n";
    if ("" != error) {
      msg += error;
    } else {
      msg += "with '" + config->psviewer + "'";
    }
    message_box(msg.c_str());
  }
}

void MainWin::cbmenu_abc_previewtune()
{
  string result, command, error;
  int rc, line, col, skipbegin, skiplines;

  if (0 != save_current_tune(config->abctmpfile.c_str(), &skipbegin, &skiplines))
    return;

  // call abctab2ps or abcm2ps
  if (which_abc2ps_command() == "abctab2ps") {
    // make sure abctab2ps finds its fonts
    if (!config->abctabfonts.empty()) {
      string s = config->abctabfonts + 
        ";/usr/share/abctab2ps;/usr/local/share/abctab2ps";
      setenv("ABCTABFONTS", s.c_str(), 1);
#ifndef WIN32
      // workaround for bug in OSX 10.10:
      command = (string)"ABCTABFONTS=" + s + "; ";
#endif
    } else {
      unsetenv("ABCTABFONTS");
    }
    command += config->abctab2ps + " -O \"" + config->pstmpfile + 
      "\" \"" + config->abctmpfile + "\"";
  } else {
    // abcm2ps
    command = config->abcm2ps + " -O \"" + config->pstmpfile + 
      "\" \"" + config->abctmpfile + "\"";
  }
  if (pipe_command(command.c_str(), &result, &rc)) {
    string msg = "The following command failed:\n" + command;
    message_box(msg.c_str());
    return;
  }
  if (!commandoutput_dialog)
    commandoutput_dialog = new CommandOutput(this);
  commandoutput_dialog->load_data(result.c_str(), 
                                  "abctab2ps", skipbegin, skiplines);
  if (rc || commandoutput_dialog->nexterror(&line, &col) > 0) {
    commandoutput_dialog->show();
    if (1 != choice_box("An error occured.\nPreview anyway?",NULL,"Preview","Cancel")) {
      return;
    }
  }

  // open PS output
  if (open_ps(config->pstmpfile.c_str(), config->psviewer.c_str(), &error)) {
    string msg = "Error in opening '" + config->pstmpfile + "'\n";
    if ("" != error) {
      msg += error;
    } else {
      msg += "with '" + config->psviewer + "'";
    }
    message_box(msg.c_str());
  }  
}

void MainWin::cbmenu_abc_listentune()
{
  string result, command, error;
  int rc, line, col, skipbegin, skiplines;
  if (0 != save_current_tune(config->abctmpfile.c_str(), &skipbegin, &skiplines, true))
    return;
  command = config->abc2midi +
    " \"" + config->abctmpfile + "\" -o \"" + config->miditmpfile + "\"";
  if (pipe_command(command.c_str(), &result, &rc)) {
    string msg = "The following command failed:\n" + command;
    message_box(msg.c_str());
    return;
  }
  if (!commandoutput_dialog)
    commandoutput_dialog = new CommandOutput(this);
  commandoutput_dialog->load_data(result.c_str(), 
                                  "abc2midi", skipbegin, skiplines);
  if (rc || commandoutput_dialog->nexterror(&line, &col) > 0) {
    commandoutput_dialog->show();
    if (1 != choice_box("An error occured.\nListen anyway?",NULL,"Listen","Cancel")) {
      return;
    }
  }
  if (open_midi(config->miditmpfile.c_str(), config->midiplayer.c_str(), &error)) {
    string msg = "Error in opening '" + config->miditmpfile + "'\n";
    if ("" != error) {
      msg += error;
    } else {
      msg += "with '" + config->midiplayer + "'";
    }
    message_box(msg.c_str());
  }  
}

void MainWin::cbmenu_abc_marktune()
{
  int start, end, pos, maxpos;
  // find start of current tune
  pos = textbuffer->line_start(editor->insert_position());
  while (pos > 0) {
    if (textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':')
      break;
    pos = textbuffer->line_start(pos-1);
  }
  if (pos > 0) start = pos; else start = 0;

  // find start of next tune
  pos = textbuffer->line_end(start) + 1;
  maxpos = textbuffer->length() - 1;
  while (pos < maxpos) {
    if (textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':')
      break;
    pos = textbuffer->line_end(pos) + 1;
  }
  if (pos < maxpos && pos > 0) {
    // pos is now start of next X: line => move one back
    end = textbuffer->line_end(pos - 1) + 1;
  } else {
    end = textbuffer->length();
  }
  textbuffer->select(start,end);
}

void MainWin::cbmenu_abc_fixupxrefs()
{
  int mode, pos, lineendpos, maxpos, lastx, thisx;
  char *xval, *xpostfix;
  char newxref[16];
  string lastpostfix;

  // get operation mode
  if (!fixupxrefs_dialog) fixupxrefs_dialog = new FixupXrefs(this);
  mode = fixupxrefs_dialog->get_mode();
  if (!mode) return;

  // loop over X-info fields
  pos = 0;
  lastpostfix = "";
  maxpos = textbuffer->length() - 1;
  lastx = 0;
  while (pos < maxpos) {
    if (textbuffer->char_at(pos) == 'X' && 
        textbuffer->char_at(pos+1) == ':') {
      lineendpos = textbuffer->line_end(pos+1);
      xval = textbuffer->text_range(pos+2,lineendpos);
      thisx = atoi(xval);
      if (mode == 1) {    // consecutive renumbering
        if (lastx != 0 || thisx == 0) thisx = lastx + 1;
        sprintf(newxref,"%d",thisx);
        textbuffer->replace(pos+2,lineendpos,newxref);
      } else {            // allow gaps and non digit postfix
        // look for appendix like in "X:2a"
        xpostfix = xval; 
        while (*xpostfix && (isdigit(*xpostfix) || isspace(*xpostfix)))
          xpostfix++;
        if (thisx < lastx || 
            (thisx == lastx && (!*xpostfix || lastpostfix > xpostfix))) {
          thisx = lastx + 1;
          sprintf(newxref,"%d",thisx);
          textbuffer->replace(pos+2,lineendpos,newxref);
        }
        if (*xpostfix) lastpostfix = xpostfix; else lastpostfix = "";
      }
      free(xval);
      lastx = thisx;
    }
    pos = textbuffer->line_end(pos) + 1;
  }
}

void MainWin::cbmenu_abc_commentregion()
{
  // get selection range
  int start, end, pos, maxpos;
  if (!textbuffer->selection_position(&start, &end)) {
    message_box("Nothing selected.");
    return;
  }
  start = textbuffer->line_start(start);
  if (end > 0) end = textbuffer->line_end(end-1);
  textbuffer->select(start, end+1);

  // insert comment signs in selection region
  string newtext = "";
  char* line;
  maxpos = textbuffer->length();
  for (pos = start; pos < end && pos < maxpos;
       pos = textbuffer->line_end(pos) + 1) {
    line = textbuffer->line_text(pos); // does not contain newline
    newtext += "% " + (string)line + "\n";
    free(line);
  }
  textbuffer->replace_selection(newtext.c_str());
  textbuffer->select(start,start + newtext.size());
}

void MainWin::cbmenu_abc_uncommentregion()
{
  // get selection range
  int start, end, pos, maxpos;
  if (!textbuffer->selection_position(&start, &end)) {
    message_box("Nothing selected.");
    return;
  }
  start = textbuffer->line_start(start);
  if (end > 0) end = textbuffer->line_end(end-1);
  textbuffer->select(start, end+1);

  // remove comment signs in selection region
  string newtext = "";
  char* line;
  maxpos = textbuffer->length();
  for (pos = start; pos < end && pos < maxpos;
       pos = textbuffer->line_end(pos) + 1) {
    line = textbuffer->line_text(pos); // does not contain newline
    if (0 == strncmp("% ",line,2)) {
      newtext += (line + 2);
    } else {
      newtext += line;
    }
    newtext += "\n";
    free(line);
  }
  textbuffer->replace_selection(newtext.c_str());
  textbuffer->select(start,start + newtext.size());
}

void MainWin::cbmenu_abc_regionoctaveup()
{
  // get selection range
  int start, end, pos, maxpos;
  if (!textbuffer->selection_position(&start, &end)) {
    message_box("Nothing selected.");
    return;
  }

  // replace every note with octave up note
  string newtext = "";
  bool gchord = false;
  bool deco = false;
  bool header = false;
  maxpos = textbuffer->length();
  if (end > maxpos) end = maxpos;
  for (pos = start; pos < end; pos++) {
    // reset pending stuff on line breaks
    if (textbuffer->char_at(pos) == '\n') {
      gchord = false;
      deco = false;
      header = false;
    }
    // handling of comments
    if (textbuffer->char_at(pos) == '%') {
      // unless escaped: ignore until end of line
      if (!(pos > 0 && textbuffer->char_at(pos-1) == '\\')) {
          while (pos < end && textbuffer->char_at(pos) != '\n') {
            newtext += textbuffer->char_at(pos);
            pos++;
          }
        }
    }
    // handling of header fields (info C, voice definition D)
    else if (highlight->stylebuffer->char_at(pos) == 'C' ||
             highlight->stylebuffer->char_at(pos) == 'D') {
      newtext += textbuffer->char_at(pos);
      continue;
    }
    // handling of gchords
    else if (gchord) {
      newtext += textbuffer->char_at(pos);
      if (textbuffer->char_at(pos) == '"') gchord = false;
    }
    else if (textbuffer->char_at(pos) == '"') {
      gchord = true;
      newtext += textbuffer->char_at(pos);
    }
    // handling of inline headers
    if (header) {
      newtext += textbuffer->char_at(pos);
      if (textbuffer->char_at(pos) == ']') header = false;
    }
    else if (textbuffer->char_at(pos) == '[') {
      header = true;
      newtext += textbuffer->char_at(pos);
    }
    // handling of decos
    else if (deco) {
      newtext += textbuffer->char_at(pos);
      if (textbuffer->char_at(pos) == '!') deco = false;
    }
    else if (textbuffer->char_at(pos) == '!') {
      deco = true;
      newtext += textbuffer->char_at(pos);
    }
    // regular music
    else if (strchr("ABCDEFG", textbuffer->char_at(pos))) {
      if (pos+1 < end && textbuffer->char_at(pos+1) == ',') {
        newtext += textbuffer->char_at(pos);
        pos++;
      } else {
        newtext += tolower(textbuffer->char_at(pos));
      }
    }
    else if (strchr("abcdefg", textbuffer->char_at(pos))) {
      newtext += textbuffer->char_at(pos);
      newtext += "'";
    }
    else {
      newtext += textbuffer->char_at(pos);
    }
  }
  textbuffer->replace_selection(newtext.c_str());
  textbuffer->select(start,start + newtext.size());
}

void MainWin::cbmenu_abc_regionoctavedown()
{
  // get selection range
  int start, end, pos, maxpos;
  if (!textbuffer->selection_position(&start, &end)) {
    message_box("Nothing selected.");
    return;
  }

  // replace every note with octave down note
  string newtext = "";
  bool gchord = false;
  bool deco = false;
  bool header = false;
  maxpos = textbuffer->length();
  if (end > maxpos) end = maxpos;
  for (pos = start; pos < end; pos++) {
    // reset pending stuff on line breaks
    if (textbuffer->char_at(pos) == '\n') {
      gchord = false;
      deco = false;
      header = false;
    }
    // handling of comments
    if (textbuffer->char_at(pos) == '%') {
      // unless escaped: ignore until end of line
      if (!(pos > 0 && textbuffer->char_at(pos-1) == '\\')) {
          while (pos < end && textbuffer->char_at(pos) != '\n') {
            newtext += textbuffer->char_at(pos);
            pos++;
          }
        }
    }
    // handling of header fields (info C, voice definition D)
    else if (highlight->stylebuffer->char_at(pos) == 'C' ||
             highlight->stylebuffer->char_at(pos) == 'D') {
      newtext += textbuffer->char_at(pos);
      continue;
    }
    // handling of gchords
    else if (gchord) {
      newtext += textbuffer->char_at(pos);
      if (textbuffer->char_at(pos) == '"') gchord = false;
    }
    else if (textbuffer->char_at(pos) == '"') {
      gchord = true;
      newtext += textbuffer->char_at(pos);
    }
    // handling of inline headers
    if (header) {
      newtext += textbuffer->char_at(pos);
      if (textbuffer->char_at(pos) == ']') header = false;
    }
    else if (textbuffer->char_at(pos) == '[') {
      header = true;
      newtext += textbuffer->char_at(pos);
    }
    // handling of decos
    else if (deco) {
      newtext += textbuffer->char_at(pos);
      if (textbuffer->char_at(pos) == '!') deco = false;
    }
    else if (textbuffer->char_at(pos) == '!') {
      deco = true;
      newtext += textbuffer->char_at(pos);
    }
    else if (strchr("abcdefg", textbuffer->char_at(pos))) {
      if (pos+1 < end && textbuffer->char_at(pos+1) == '\'') {
        newtext += textbuffer->char_at(pos);
        pos++;
      } else {
        newtext += toupper(textbuffer->char_at(pos));
      }
    }
    else if (strchr("ABCDEFG", textbuffer->char_at(pos))) {
      newtext += textbuffer->char_at(pos);
      newtext += ",";
    }
    else {
      newtext += textbuffer->char_at(pos);
    }
  }
  textbuffer->replace_selection(newtext.c_str());
  textbuffer->select(start,start + newtext.size());
}


void MainWin::cbmenu_abc_insertheader()
{
  editor->insert("X:1\n%\nT:title\nC:composer\nL:1/4\nM:C\nK:C treble\n%\n");
}

void MainWin::cbmenu_abc_midiinstrument()
{
  if (!midilist_dialog) midilist_dialog = new MidiList(this);
  midilist_dialog->show();
}

void MainWin::cbmenu_abc_format_font()
{
  string result, command; int rc;
  char* filter[] = {(char*)"font", 0};
  if (which_abc2ps_command() == "abctab2ps") {
    command = config->abctab2ps + " -H";
  } else { // abcm2ps
    command = config->abcm2ps + " -H";
  }
  if (0 == pipe_command(command.c_str(), &result, &rc)) {
    if (!abcformatlist_dialog)
      abcformatlist_dialog = new AbcFormatList(this);
    abcformatlist_dialog->load_data(result.c_str(), filter);
    abcformatlist_dialog->show();
  } else {
    string msg = "The following command failed:\n" + command;
    message_box(msg.c_str());
  }
}

void MainWin::cbmenu_abc_format_layout()
{
  string result, command; int rc;
  char* filter[] = {(char*)"width", (char*)"height", (char*)"margin", (char*)"space", (char*)"sep", (char*)"stretch", 0};
  if (which_abc2ps_command() == "abctab2ps") {
    command = config->abctab2ps + " -H";
  } else { // abcm2ps
    command = config->abcm2ps + " -H";
  }
  if (0 == pipe_command(command.c_str(), &result, &rc)) {
    if (!abcformatlist_dialog)
      abcformatlist_dialog = new AbcFormatList(this);
    abcformatlist_dialog->load_data(result.c_str(), filter);
    abcformatlist_dialog->show();
  } else {
    string msg = "The following command failed:\n" + command;
    message_box(msg.c_str());
  }
}

void MainWin::cbmenu_abc_format_tablature()
{
  string result, command; int rc;
  char* filter[] = {(char*)"tab", 0};
  if (which_abc2ps_command() == "abctab2ps") {
    command = config->abctab2ps + " -H";
  } else { // abcm2ps
    message_box("abcm2ps does not support tablature");
    return;
  }
  if (0 == pipe_command(command.c_str(), &result, &rc)) {
    if (!abcformatlist_dialog)
      abcformatlist_dialog = new AbcFormatList(this);
    abcformatlist_dialog->load_data(result.c_str(), filter);
    abcformatlist_dialog->show();
  } else {
    string msg = "The following command failed:\n" + command;
    message_box(msg.c_str());
  }
}

void MainWin::cbmenu_abc_format_all()
{
  string result, command; int rc;
  if (which_abc2ps_command() == "abctab2ps") {
    command = config->abctab2ps + " -H";
  } else { // abcm2ps
    command = config->abcm2ps + " -H";
  }
  if (0 == pipe_command(command.c_str(), &result, &rc)) {
    if (!abcformatlist_dialog)
      abcformatlist_dialog = new AbcFormatList(this);
    abcformatlist_dialog->load_data(result.c_str());
    abcformatlist_dialog->show();
  } else {
    string msg = "The following command failed:\n" + command;
    message_box(msg.c_str());
  }
}

//---------------------------------------------------------------------
// Help menu
//---------------------------------------------------------------------

void MainWin::cbmenu_help_usersguide()
{
  string userguide, error;
  const char* basename = "userguide/html/index.html";

  userguide = find_file(config->docdirs.c_str(), basename, ";");
  if (userguide == "") {
    string msg = "Could not find '" + (string)basename + 
      "' in directories\n'" + config->docdirs + "'";
    message_box(msg.c_str());
    return;
  }
  if (open_html(userguide.c_str(), config->webbrowser.c_str(), &error)) {
    string msg = "Error in opening '" + userguide + "'\n" + error;
    message_box(msg.c_str());
  }
}

void MainWin::cbmenu_help_manpage()
{
  string userguide, error;
  const char* basename = "abctab2ps-man.html";

  userguide = find_file(config->docdirs.c_str(), basename, ";");
  if (userguide == "") {
    string msg = "Could not find '" + (string)basename + 
      "' in directories\n'" + config->docdirs + "'";
    message_box(msg.c_str());
    return;
  }
  if (open_html(userguide.c_str(), config->webbrowser.c_str(), &error)) {
    string msg = "Error in opening '" + userguide + "'\n" + error;
    message_box(msg.c_str());
  }
}

void MainWin::cbmenu_help_flabc()
{
  string userguide, error;
  const char* basename = "flabc.html";

  userguide = find_file(config->docdirs.c_str(), basename, ";");
  if (userguide == "") {
    string msg = "Could not find '" + (string)basename + 
      "' in directories\n'" + config->docdirs + "'";
    message_box(msg.c_str());
    return;
  }
  if (open_html(userguide.c_str(), config->webbrowser.c_str(), &error)) {
    string msg = "Error in opening '" + userguide + "'\n" + error;
    message_box(msg.c_str());
  }
}

void MainWin::cbmenu_help_flabcchanges()
{
  string textfile;
  const char* basename = "CHANGES";

  textfile = find_file(config->docdirs.c_str(), basename, ";");
  if (textfile == "") {
    string msg = "Could not find '" + (string)basename + 
      "' in directories\n'" + config->docdirs + "'";
    message_box(msg.c_str());
    return;
  }
  if (!textview_dialog) textview_dialog = new TextViewer(this);
  if (0 == textview_dialog->load_file(textfile.c_str())) {
    textview_dialog->window->label("Flabc Changelog");
    textview_dialog->show();
  } else {
    string msg = "Error while reading '" + textfile + "'";
    message_box(msg.c_str());    
  }
}

void MainWin::cbmenu_help_about()
{
  if (!helpabout_dialog)
    helpabout_dialog = new HelpAbout(this);
  helpabout_dialog->show();
}


//=====================================================================
// editor (textbuffer) callbacks
//=====================================================================

// static wrapper function
void MainWin::changed_cb_static(int pos, int nInserted, int nDeleted,int nRestyled, const char* text, void* v) {
  MainWin* m = (MainWin*) v;
  m->changed_cb(pos, nInserted, nDeleted, nRestyled, text);
}
// actual callback member function
void MainWin::changed_cb(int pos, int nInserted, int nDeleted,int nRestyled, const char* text) {
  if ((nInserted || nDeleted) && !loading) {
    if (!changed && !filename.empty()) {
      // mark changed status in window title
      string label = basename(filename) + " *";
      window->copy_label(label.c_str());
    }
    changed = true;
  }
  if (!loading) {
    if (nInserted || nDeleted) {
      if (undoing) {
        redovector.push_back(UndoOperation(pos, nInserted, nDeleted, nRestyled, text));
      } else {
        undovector.push_back(UndoOperation(pos, nInserted, nDeleted, nRestyled, text));
        if (!redoing)
          redovector.clear();
      }
    }
  }
}
