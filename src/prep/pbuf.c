#include <time.h>

void pbuf_output_filename(pbuf *p) {
  printf("%s", utillib_input_buf_filename(&(p->input)));
}

void pbuf_output_lineno(pbuf *p) {
  printf("%lu", utillib_input_buf_row(&(p->input)));
}

void pbuf_output_date(pbuf *p) {
  static char time_buf[100];
  printf("%s", strftime(time_buf, sizeof time_buf, 
        PREP_PD_DATE_FMT
