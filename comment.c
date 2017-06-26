#include "comment.h"

// TODO comments are replaced with a single space
// in **preprocess** stage, not in tokenize, move
// this to preprocess
/* different language use different kinds of comments */
void init_single_line_comment_cfamily(void)
{
  add_skip(TK_SLASH,TK_SINGLE_LINE_COMENT_BEGIN,CAHR_CLASS_SLASH);
  add_skip_rev_loop(TK_SINGLE_LINE_COMENT_BEGIN, CHAR_CLASS_NEWLINE);
  add_skip (TK_SINGLE_LINE_COMENT_BEGIN, TK_INIT,CHAR_CLASS_NEWLINE);

}

void init_multi_line_comment_cfamily(void)
{
  /* multi_line coment */
  add_skip(TK_SLASH,TK_MULTI_LINE_COMENT_BEGIN,CHAR_CLASS_STAR);
  // TODO: add free in skip for '/' may alloc a brief token

  // loop on stars
  add_skip_loop(TK_MULTI_LINE_COMENT_END, CHAR_CLASS_STAR);

  // selfloop on non stars
  add_skip_rev_loop(TK_MULTI_LINE_COMENT_BEGIN, CHAR_CLASS_STAR);

  add_skip_rev(TK_MULTI_LINE_COMENT_END,TK_MULTI_LINE_COMENT_BEGIN,CHAR_CLASS_STAR_SLASH);

  add_skip(TK_MULTI_LINE_COMENT_BEGIN,TK_MULTI_LINE_COMENT_END,CHAR_CLASS_STAR);

  // end
  add_skip(TK_MULTI_LINE_COMENT_END, TK_INIT,CAHR_CLASS_SLASH);

}

