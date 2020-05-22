//
// own implementation for fl_message etc which appear screen centered
//

#ifndef _messagebox_H
#define _messagebox_H

#include "messagebox_msk.h"

// message box with only one ok button
void message_box(const char* message, const char* iconlabel = "!");

// choice between up to three buttons
// buttons appear from right to left
// setting a buttonlabel to NULL hides the button
// RC = index of button pressed (or 3 on ESCAPE)
int choice_box(const char* message, const char* label0, const char* label1, const char* label2, const char* title = "", const char* iconlabel = "?");


#endif
