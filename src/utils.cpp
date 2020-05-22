//
// global utility functions
//

#include "utils.h"
#include <string.h>
#include <stdio.h>

// remove front and back whitespace from a string
string trim_space(const string& s)
{
  const char* ws = " \t\r\n";
  string::size_type start, end, len;
  start = s.find_first_not_of(ws);
  if (start == string::npos) start = 0;
  end = s.find_last_not_of(ws);
  if (end == string::npos) len = string::npos;
  else len = end - start + 1;
  return s.substr(start, len);
}

// remove back whitespace from a string
string rtrim_space(const string& s)
{
  const char* ws = " \t\r\n";
  string::size_type end;
  end = s.find_last_not_of(ws);
  if (end == string::npos) return s;
  else return s.substr(0, end + 1);
}

// reads an entire file into a provided string
// all line breaks are converted to a single '\n'
// (works with \r\n, \n, \r, and even \n\r)
// rc: 0 = ok, otherwise error
int readfile(const char* filename, string* s)
{
  FILE* fp;
  char c1,c2;
  int rc;

  *s = "";
  fp = fopen(filename,"rb");
  if (!fp) return 1;
  while ((c1 = getc(fp)) != EOF) {
    if (c1 == '\n') {
      *s += '\n';
      c2 = getc(fp);
      if (!feof(fp) && c2 != '\r')
        *s += c2;
    }
    else if (c1 == '\r') {
      *s += '\n';
      c2 = getc(fp);
      if (!feof(fp) && c2 != '\n')
        *s += c2;
    }
    else {
      *s += c1;
    }
  }

  if (ferror(fp))
    rc = 1;
  else
    rc = 0;
  fclose(fp);
  return rc;
}

// cross platform implementation of strcasecmp
int strcmpnocase (const char* s1, const char* s2)
{
  while (*s1 && *s2) {
    if (tolower(*s1) != tolower(*s2)) break;
    s1++; s2++;
  }
  if (!*s1 && !*s2)
    return 0;
  else if (tolower(*s1) > tolower(*s2))
    return 1;
  else
    return -1;
}

// cross platform implementation of strncasecmp
int strncmpnocase (const char* s1, const char* s2, int n)
{
  int i=0;
  while (*s1 && *s2 && (i<n)) {
    if (tolower(*s1) != tolower(*s2)) break;
    s1++; s2++; i++;
  }
  if ((!*s1 && !*s2) || (i==n))
    return 0;
  else if (tolower(*s1) > tolower(*s2))
    return 1;
  else
    return -1;
}
