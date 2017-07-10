#include "chcl.h"
#define CHAR_CLASS_MAX_TAB_LEN 100
const static char *dec = "123456789";
const static char *Dec = "0123456789";
const static char *Oct = "01234567";
const static char *oct = "1234567";
const static char *Hex = "0123456789abcdefABCDEF";
const static char *hex =  "123456789abcdefABCDEF";
const static char *Word = "0123456789"
                          "_abcdefghijklmnopqrstuvwxyz"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const static char *word =  "_abcdefghijklmnopqrstuvwxyz"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const static char *spaces=" \v\f\n\r\t";
const static char *newline="\n\r";
const static char *oper="~!%^&*-+<>=/|";
const static char *punc="(){}[]:;,?";
const static char *int_suffix="lLuU";
const static char *float_suffix="fFlL";

static char *all_cc[CHAR_CLASS_MAX_TAB_LEN];
static int all_count;

char *lookup_char_class(int chcl)
{
  if (chcl >= CHAR_CLASS_MAX_TAB_LEN)
    return NULL;
  return all_cc[chcl];
}

int alloc_char_class(const char *chcl)
{
  assert (all_count != CHAR_CLASS_MAX_TAB_LEN);
  // some short cut is supported:
  // \\D dec with 0
  // \\d dec without 0
  // \\O oct with 0
  // \\H hex ...
  // \\N newline
  // \\S spaces
  // \\W letter, number and _
  // \\w letter and _
  // \\p punctuation with dot
  // \\P with dot
  
  static char buf[BUFSIZ];
  const char *spec;
  int offset;
  char *pstr=buf;
  const char *s;
  *pstr=0;

  for (s=chcl;*s;++s)
  {
    if (*s=='\\')
    {
      switch (*++s)
      {
        case 'B':
          *pstr++='\\';
          continue;

        case 'E':
          *pstr++='e';
          *pstr++='E';
          continue;

        case 'X':
          *pstr++='x';
          *pstr++='X';
          continue;
        case 'Q':
          *pstr++='\"';
          continue;
        case 'q':
          *pstr++='\'';
          continue;
        case 'Z':
          *pstr++='0';
          continue;
        case 'D':
          spec=Dec;
          offset=strlen(Dec);
          break;
        case 'd':
          spec=dec;
          offset=strlen(dec);
          break;
        case 'O':
          spec=Oct;
          offset=strlen(Oct);
          break;
        case 'o':
          spec=oct;
          offset=7;
          break;
        case 'h':
          spec=hex;
          offset=strlen(hex);
          break;
        case 'H':
          spec=Hex;
          offset=strlen(Hex);
          break;
        case 'W':
          spec=Word;
          offset=strlen(Word);
          break;
        case 'w':
          spec=word;
          offset=strlen(word);
          break;
        case 'S':
          spec=spaces;
          offset=strlen(spaces);
          break;
        case 'N':
          spec=newline;
          offset=strlen(newline);
          break;
        case 'p':
          spec=punc;
          offset=strlen(punc);
          break;
        case 'I':
          spec=int_suffix;
          offset=strlen(int_suffix);
          break;

        case 'F':
          spec=float_suffix;
          offset=strlen(float_suffix);
          break;

      }
      strcpy (pstr, spec);
      pstr+=offset;
      continue;
    }
    *pstr++=*s;
  }

  *pstr=0;

  for (int i=0;i<all_count;++i)
  {
    if (strcmp(buf, all_cc[i])==0)
    {
      return i;
    }
  }

  all_cc[all_count++]=strdup(buf);
  return  all_count-1;

}

int cond_char_class(dfa_state *st, int ch)
{
  if (ch == EOF)
  {
    return 0;
  }
  char *cc=all_cc[st->cond];
  bool rev=st->usrd;
  bool incl=strchr(cc,ch);
  return (!rev && incl || rev && !incl);
}


