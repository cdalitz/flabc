//
// own implementation for fl_message etc which appear screen centered
//

#include "messagebox_msk.h"
#include <FL/fl_draw.H>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// helper function for computing maximum
inline int maxint(int x, int y) { if (x>y) return x; else return y; }

// compute size of w->label()
void compute_label_size(Fl_Widget* w, int &width, int &height)
{
  char *label, *p, *line;
  if (!w->visible()) {
    width = height = 0; return;
  }
  fl_font(w->labelfont(), w->labelsize());
  label = strdup(w->label());
  height = width = 0;
  p = label;
  do {
    line = p;
    p = strchr(p,'\n');
    if (p) {*p = 0; p++;}
    height += fl_height();
    width = maxint(width, (int)fl_width(line));
  }
  while (p && *p);

  free(label);
}

// adjust box positions and sizes so that all text fits
void resize_message_box(MessageBox_msk* mb)
{
  int w,h,x,y,totalh,totalw;

  // message
  compute_label_size(mb->message,w,h);
  mb->message->size(maxint(w,mb->message->w()),maxint(h,mb->message->h()));
  y = mb->message->y() + mb->message->h() + 20;
  totalh = y;
  totalw = mb->message->x() + mb->message->w() + 30;
  x = totalw - 30;

  // fill up buttons from right to left
  compute_label_size(mb->button0,w,h);
  if (w) {
    w = maxint(w + 10, mb->button0->w());
    h = maxint(h + 10, mb->button0->h());
    mb->button0->resize(x - w, y, w, h);
    x = x - w - 20;
  }
  compute_label_size(mb->button1,w,h);
  if (w) {
    w = maxint(w + 45, mb->button1->w());
    h = maxint(h + 10, mb->button1->h());
    mb->button1->resize(x - w, y, w, h);
    x = x - w - 20;
  }
  compute_label_size(mb->button2,w,h);
  if (w) {
    w = maxint(w + 10, mb->button2->w());
    h = maxint(h + 10, mb->button2->h());
    mb->button2->resize(x - w, y, w, h);
    x = x - w - 20;
  }
  totalh += maxint(maxint(mb->button2->h(), mb->button1->h()), mb->button0->h());
  totalh += 20;

  // adjust window size
  mb->window->size(totalw,totalh);
}

// message box with only one ok button
void message_box(const char* message, const char* iconlabel = "i")
{
  MessageBox_msk mb;
  mb.icon->label(iconlabel);
  mb.message->label(message);
  mb.button1->label("Ok");
  mb.button0->deactivate(); mb.button0->hide();
  mb.button2->deactivate(); mb.button2->hide();
  resize_message_box(&mb);
  mb.window->show();
  while (mb.buttonpressed == -1) Fl::wait();
}

// choice between up to three buttons
// buttons appear from right to left
// setting a buttonlabel to NULL hides the button
// RC = index of button pressed (or 3 on ESCAPE)
int choice_box(const char* message, const char* label0, const char* label1, const char* label2, const char* title = "", const char* iconlabel = "?")
{
  MessageBox_msk mb;
  if (title && *title) mb.window->label(title);
  mb.icon->label(iconlabel);
  mb.message->label(message);
  if (label0) {
    mb.button0->label(label0);
  } else {
    mb.button0->deactivate(); mb.button0->hide();
  }
  if (label1) {
    mb.button1->label(label1);
  } else {
    mb.button1->deactivate(); mb.button1->hide();
  }
  if (label2) {
    mb.button2->label(label2);
  } else {
    mb.button2->deactivate(); mb.button2->hide();
  }
  resize_message_box(&mb);
  mb.window->show();
  while (mb.buttonpressed == -1) Fl::wait();
  return mb.buttonpressed;
}

