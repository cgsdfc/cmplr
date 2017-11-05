#include "predefined_macro.h"

UTILLIB_ETAB_BEGIN(predefined_macro)

UTILLIB_ETAB_ELEM(PD_FILE)
UTILLIB_ETAB_ELEM(PD_LINE)
UTILLIB_ETAB_ELEM(PD_DATE)
UTILLIB_ETAB_ELEM(PD_TIME)
UTILLIB_ETAB_ELEM(PD_STDC)
UTILLIB_ETAB_ELEM(PD_STDC_VERSION)
UTILLIB_ETAB_ELEM(PD_GNUC)
UTILLIB_ETAB_ELEM(PD_GNUC_MINOR)
UTILLIB_ETAB_ELEM(PD_GNUG)
UTILLIB_ETAB_ELEM(PD_CPLUS_PLUS)
UTILLIB_ETAB_ELEM(PD_STRICT_ANSI)
UTILLIB_ETAB_ELEM(PD_BASE_FILE)
UTILLIB_ETAB_ELEM(PD_INCLUDE_LEVEL)
UTILLIB_ETAB_ELEM(PD_VERSION)
UTILLIB_ETAB_ELEM(PD_OPTIMIZE)
UTILLIB_ETAB_ELEM(PD_REGISTER_PREFIX)
UTILLIB_ETAB_ELEM(PD_USER_LABEL_PREFIX)

UTILLIB_ETAB_END(predefined_macro)

/* void prep_expand_predefined_macro(pbuf *buf, predefined_macro macro) */
/* { */
/*   switch(macro) { */
/*     case PD_FILE: */
/*       pbuf_output_filename(buf); */
/*       break; */
/*     case PD_LINE: */
/*       pbuf_output_filename(buf); */
/*       break; */
/*     case PD_DATE: */
/*     case PD_TIME: */
/*     case PD_STDC: */
/*     case PD_STDC_VERSION: */
/*     case PD_GNUC: */
/*     case PD_GNUC_MINOR: */
/*     case PD_GNUG: */
/*     case PD_CPLUS_PLUS: */
/*     case PD_STRICT_ANSI: */
/*     case PD_BASE_FILE: */
/*     case PD_INCLUDE_LEVEL: */
/*     case PD_VERSION: */
/*     case PD_OPTIMIZE: */
/*     case PD_REGISTER_PREFIX: */
/*     case PD_USER_LABEL_PREFIX: */
/*   } */
/* } */
