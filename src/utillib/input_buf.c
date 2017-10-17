#include "input_buf.h"
#include "color.h" // for GREEN_CARET 
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

static void save_line_offset(utillib_input_buf *self) {
  // called when newline was encountered
  size_t cur_offset=(size_t) utillib_vector_back(&(self->line_offset));
  cur_offset+=self->col+1; // for the \n char
  utillib_vector_push_back(&(self->line_offset), (utillib_vector_elem_t) cur_offset);
}
static char const * static_line(utillib_input_buf *self, utillib_position const* pos) {
  size_t offset=(size_t) utillib_vector_at(&(self->line_offset), UTILLIB_POS_ROW(pos));
  long cur_pos=ftell(self->file);
  char * buf=utillib_char_buf_static_buf();
  if (fseek(self->file, SEEK_SET, offset)<0) {
    utillib_die("get_specific_line: fseek failed");
  }
  if (NULL == fgets(buf, UTILLIB_CHAR_BUF_STATIC_BUF_SIZE, self->file)) {
    utillib_die("static_line: fgets failed");
  }
  fseek(self->file, SEEK_SET, cur_pos);
  return buf;
}

static void insert_indicator(utillib_position const * pos) {
  for (int i=0;i<UTILLIB_POS_COL(pos);++i) {
    fputc(' ', stderr);
  }
  fputs(GREEN_CARET, stderr);
  fputc('\n', stderr);
}

static char const *static_prefix(utillib_input_buf *self, int lv, 
    utillib_position const *pos) {
  static char absdir[UTILLIB_PATH_MAX];
  if (NULL == getcwd(absdir, sizeof absdir)) {
    utillib_die("utillib_input_buf_sprintf: absdir overflowed");
  }
  return  utillib_char_buf_static_sprintf("%s/%s:%lu:%lu:%s", 
      absdir, self->filename,
      UTILLIB_POS_ROW(pos),
      UTILLIB_POS_COL(pos),
      utillib_error_lv_tostring(lv));
}

void utillib_input_buf_init(utillib_input_buf *self, FILE *file, char const *filename ) {
  static const size_t init_line_capacity=10;
  self->file=file;
  self->filename=filename;
  self->row = 0;
  self->col = 0;
  utillib_vector_init(&(self->line_offset));
  utillib_vector_reserve(&(self->line_offset), init_line_capacity);
  utillib_vector_push_back(&(self->line_offset), (utillib_vector_elem_t)0);
}

int utillib_input_buf_getc(utillib_input_buf *self) {
  int ch = fgetc(self->file);
  switch (ch) {
  case '\n':
    save_line_offset(self);
    self->row++;
    self->col = 0;
    break;
  case EOF:
    return EOF;
  default:
    self->col++;
    break;
  }
  return ch;
}

int utillib_input_buf_ungetc(utillib_input_buf *self, int ch) {
  switch (ch) {
  case '\n':
    self->row--;
    break;
  default:
    self->col--;
    break;
  }
  return ungetc(ch, self->file);
}

long utillib_input_buf_ftell(utillib_input_buf *self) { return ftell(self->file); }

void utillib_input_buf_pretty_perror(utillib_input_buf *self, utillib_error *error)
{
  utillib_position *pos=&(error->ue_pos);
  int lv=error->ue_lv;
  char const * str=static_prefix(self, lv, pos);
  fputs(str, stderr);
  fputs(error->ue_msg, stderr);
  fputc('\n', stderr);
  str=static_line(self, pos);
  fputs(str, stderr);
  insert_indicator(pos);
}

bool utillib_input_buf_feof(utillib_input_buf *self) { return feof(self->file); }
const char *utillib_input_buf_filename(utillib_input_buf *self) {
  return self->filename;
}

size_t utillib_input_buf_row(utillib_input_buf *self) { return self->row; }

size_t utillib_input_buf_col(utillib_input_buf *self) { return self->col; }

utillib_position const * utillib_input_buf_current_pos(utillib_input_buf *self) {
  static utillib_position pos;
  pos.lineno = self->row;
  pos.column = self->col;
  return &pos;
}

void utillib_input_buf_destroy(utillib_input_buf *self) {
  utillib_vector_destroy(&(self->line_offset));
  if (self->file != stdin) {
    fclose(self->file);
  }
}

const utillib_char_buf_ft *utillib_input_buf_getft(void) {
  static const utillib_char_buf_ft ft = {
      .cb_getc = (utillib_getc_func_t *)utillib_input_buf_getc,
      .cb_ungetc = (utillib_ungetc_func_t *)utillib_input_buf_ungetc,
  };
  return &ft;
}


