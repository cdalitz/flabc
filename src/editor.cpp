//
// stuff related to the editor widget like syntax highlighting etc.
//

#include "editor.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "fltkextensions.h"

// Initialize the style buffer
SyntaxHighlighter::SyntaxHighlighter(Fl_Text_Buffer* textbuf, Fl_Text_Editor* edit, Fl_Text_Display::Style_Table_Entry* st, int nstyleentries) {
  char *style = new char[textbuf->length() + 1];
  char *text = textbuf->text();
  textbuffer = textbuf;
  editor = edit;
  memset(style, 'A', textbuf->length());
  style[textbuf->length()] = '\0';
  stylebuffer = new Fl_Text_Buffer(textbuf->length());
  style_parse(text, style, textbuf->length());
  stylebuffer->text(style);
  delete[] style;
  free(text);
  nstyles = nstyleentries;
  styletable = st;
}


void SyntaxHighlighter::style_update(
     int pos,                 // I - Position of update
     int nInserted,           // I - Number of inserted chars
     int nDeleted,            // I - Number of deleted chars
     int nRestyled,           // I - Number of restyled chars
     const char* deletedText, // I - Text that was deleted
     void* cbArg) {           // I - Callback data
  int start,				// Start of text
    end;				// End of text
  char last,				// Last style on line
    *style,				// Style data
    *text;				// Text data

  SyntaxHighlighter* sh = (SyntaxHighlighter*)cbArg;

  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
    sh->stylebuffer->unselect();
    return;
  }

  // printf("before: length textbuffer: %d, length stylebuffer: %d\n", sh->textbuffer->length(), sh->stylebuffer->length());

  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    style = new char[nInserted + 1];
    memset(style, 'A', nInserted);
    style[nInserted] = '\0';

    sh->stylebuffer->replace(pos, pos + nDeleted, style);
    delete[] style;
  } else {
    // Just delete characters in the style buffer...
    sh->stylebuffer->remove(pos, pos + nDeleted);
  }
  // printf("nInserted: %d, nDeleted: %d\n", nInserted, nDeleted);
  // printf("after: length textbuffer: %d, length stylebuffer: %d\n", sh->textbuffer->length(), sh->stylebuffer->length());

  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  sh->stylebuffer->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the line of the changed region to the end of
  // the line of the changed region. When lines end with a backslash 
  // (abc line continuation character), we need to extend the region
  // accordingly. The same when the line almost ends with a backslash
  // (for undoing a previous line continuation; note that this does
  // not work when backslahs has been deleted, but it is better than nothing).
  start = sh->textbuffer->line_start(pos);
  while (start > 1 && sh->textbuffer->char_at(start - 2) == '\\')
    start = sh->textbuffer->line_start(start - 2);
  end   = sh->textbuffer->line_end(pos + nInserted);
  if (end > 2) {
    while (end < (sh->textbuffer->length() - 1) && 
           (sh->textbuffer->char_at(end - 1) == '\\' ||
            sh->textbuffer->char_at(end - 2) == '\\'))
      end = sh->textbuffer->line_end(end + 1);
  }
  text  = sh->textbuffer->text_range(start, end);
  style = sh->stylebuffer->text_range(start, end);
  if (start==end)
    last = 0;
  else
    last  = style[end - start - 1];

  //printf("start = %d, end = %d, text = \"%s\", style = \"%s\", last='%c'...\n",
  //       start, end, text, style, last);
  // printf("before: strlen(style)==%d; ", strlen(style));

  sh->style_parse(text, style, end - start);

  //printf("new style = \"%s\", new last='%c'...\n", 
  //       style, style[end - start - 1]);
  // warning for tracking errors in style_parse:
  if (strlen(style) != strlen(text))
    printf("Warning: error in style_parse\n");

  sh->stylebuffer->replace(start, end, style);
  sh->editor->redisplay_range(start, end);

  free(text);
  free(style);
}

// parses the given text into style data
// as the first style is initialized to 'A' this only works
// when entire lines are passed
void SyntaxHighlighter::style_parse(const char *text,
                                    char *style, int length) {
  char current;
  int  col;

  // Style letters:
  //
  // A - Plain
  // B - Line comments
  // C - Info fields
  // D - Voice definitions
  // E - Bar lines

  // known info field letter
  const char* infoletters = "ABCDEFGHIJKLMNOPQRSTUVWXYZw";

  for (current = 'A', col = 0; length > 0; length --, text ++) {
    if (current == 'E') current = 'A';
    if (*text == '%') {
      // comment until end of line
      for (; length > 0 && *text != '\n'; length--, text++) {
        *style++ = 'B'; col++;
      }
      current = 'B';
    }
    if (current == 'A') {
      // check for info fields
      if (col == 0 && length > 1 && 
          text[1] == ':' && strchr(infoletters,*text)) {
        if (*text == 'V') {
          // voice field until end of line
          //for (; length > 0 && *text != '\n'; length--, text++) {
          //  *style++ = 'D'; col++;
          //}
          current = 'D';
        } else {
          // info field until end of line
          //for (; length > 0 && *text != '\n'; length--, text++) {
          //  *style++ = 'C'; col++;
          //}
          current = 'C';
        }
      }
      // check for inline info fields
      else if (*text == '[' && length > 2 && 
               text[2] == ':' && strchr(infoletters,text[1])) {
        if (text[1] == 'V') {
          // inline voice field
          for (; length > 0 && *text != ']' && *text != '\n'; length--, text++) {
            *style++ = 'D'; col++;
          }
          current = 'D';
        } else {
          // inline info field
          for (; length > 0 && *text != ']' && *text != '\n'; length--, text++) {
            *style++ = 'C'; col++;
          }
          current = 'C';
        }
        if (*text == ']') {
          *style++ = current; col++;
          current = 'A';
          continue;
        }
      }
      // check for bar lines
      else if (length>2 && 0==strncmp(text,"[|]",3)) {
        current = 'E';
        *style++ = current; col++; text++; length--;
        *style++ = current; col++; text++; length--;
      }
      else if (length>1 &&
               (0==strncmp(text,"|:",2) || 0==strncmp(text,":|",2) ||
                0==strncmp(text,"::",2) || 0==strncmp(text,"|]",2) ||
                0==strncmp(text,"[|",2))) {
        current = 'E';
        *style++ = current; col++; text++; length--;
      }
      else if (*text == '|') {
        current = 'E';
      }
      else if (*text == '[' && length>1 && isdigit(*(text+1))) {
        current = 'E';
        *style++ = current; col++; text++; length--;        
      }
    }

    // Copy style info...
    if (*style)
      *style++ = current;
    else
      break;
    col++;

    if (*text == '\n') {
      //if (col>1) printf("lastchar: '%c'\n",text[-1]);
      // Reset column and possibly reset the style
      if (col > 1 && text[-1] == '\\' && current != 'B') {
        //printf("Backslash found: keep style '%c'\n", current);
        ;
      }
      else {
        current = 'A';
        col = 0;
      }
    }
  }
}


