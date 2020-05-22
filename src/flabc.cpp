//
// main program flabc
//

#include "mainwin.h"
#include "messagebox.h"
#include "dialogs.h"
#include <stdio.h>
#include <FL/x.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>
#include <unistd.h>

MainWin* mainwin;

// global variable for font access
int global_nfonts;

#ifdef __APPLE__
// callback function for dropping files on application
void apple_open_cb(const char *c) {
  static bool firstwindow = true;
  if (firstwindow && mainwin->filename.empty() && !mainwin->changed) {
    mainwin->load_file(c);
    mainwin->show();
    firstwindow = false;
  } else {
    // note that we never delete the newly created window
    // => memory will only be freed on program termination
    MainWin* mw = new MainWin(mainwin->config);
    mw->load_file(c);
    mw->show();
  }
}

// for globally cycling through shown application windows
int cb_next_window(int event) {
  if (event == FL_SHORTCUT) {
    if (Fl::event_state(FL_COMMAND)) {
      if (Fl::event_key(FL_Right)) {
        // find the window not focused for the longest time
        Fl_Window *nw, *lw;
        lw = NULL;
        nw = Fl::first_window();
        while (nw) {
          lw = nw;
          nw = Fl::next_window(lw);
        }
        if (lw) lw->show();
        return 1;
      } else
      if (Fl::event_key(FL_Left)) {
        Fl_Window *fw, *nw;
        vector<Fl_Window*> wvec;
        if (!(fw = Fl::first_window())) return 1;
        if (!(fw = Fl::next_window(fw))) return 1;
        // fw now contains the window that shall get the focus
        // before showing it, we must get the window stack in right order 
        // (current first_window must move to bottom)
        nw = fw;
        do {
          wvec.push_back(nw);
          nw = Fl::next_window(nw);
        } while (nw);
        for (int i=wvec.size()-1; i>=0; i--) {
          Fl::first_window(wvec[i]); // this does not work
          //wvec[i]->show(); // this does, but is too flickery
        }
        fw->show();
        return 1;
      }
    }
  }
  return 0;
}
#endif

// main program
int main(int argc, char** argv)
{
  char* filename = NULL;
  string usage = string("Usage: ") + argv[0] + " [<filename>]\n";

  for (int i=1; i<argc; i++) {
#ifdef __APPLE__
    // Finder passes strange -psn_* arguments
    if (0==strncmp(argv[i],"-psn",3)) continue;
#endif
    if (argv[i][0] == '-') {
      fputs(usage.c_str(),stderr);
      return 1;
    } else if (!filename) {
      filename = argv[i];
    } else {
      fputs(usage.c_str(),stderr);
      return 1;
    }
  }

  // initialize global variables for font access
  global_nfonts = Fl::set_fonts();

  // configuration
  Config::programpath = argv[0];
  Config* config = new Config();
  config->LoadPrefs();

#ifdef __APPLE__
  // Aqua GUI does not load private environment (~/.profile)
  // as an ugly workaround we add some common paths to the search path
  char* path = getenv("PATH");
  if (path) {
    string newpath = path;
    if (string::npos == newpath.find("/usr/local/bin"))
      newpath += ":/usr/local/bin";
    if (string::npos == newpath.find("/sw/bin"))
      newpath += ":/sw/bin";
    setenv("PATH", newpath.c_str(), 1);
  }
#endif

  // some global GUI settings
  Fl::visible_focus(config->visiblefocus);
  Fl::scheme(config->scheme.c_str());
  //Fl::set_color(FL_BACKGROUND2_COLOR, (Fl_Color)config->backgroundcolor);
  Fl::set_color(FL_SELECTION_COLOR, (Fl_Color)config->selectioncolor);

  mainwin = new MainWin(config);
  if (filename) {
    if (0 == access(filename, F_OK)) {
      mainwin->load_file(filename);
    } else {
      mainwin->new_file(filename);
    }
  }
  // resize window when larger than screen
  if (Fl::w() < mainwin->window->w() + 50)
    mainwin->window->size(Fl::w() - 50, mainwin->window->h());
  if (Fl::h() < mainwin->window->h() + 50)
    mainwin->window->size(mainwin->window->w(), Fl::h() - 50);
  // show window screen centered
  mainwin->window->position((Fl::w() - mainwin->window->w())/2, 
                      (Fl::h() - mainwin->window->h())/2);

#ifdef __APPLE__
  // register callback for dropping files on application
  fl_open_callback(apple_open_cb);
  // enable cycling through windows
  Fl::add_handler(cb_next_window);
#endif


#if !defined(__APPLE__) && !defined(WIN32)
#include "logo/logoround.xpm"
  // only on X11: add icon to window
  //fl_open_display();
  Fl_Pixmap flabc_68x66_icon(logoround);
  Fl_RGB_Image flabc_icon(&flabc_68x66_icon, Fl_Color(0));
  mainwin->window->icon(&flabc_icon);
  mainwin->window->xclass("flabc");
#endif

  mainwin->show();
  Fl::run();

  return 0;
}
