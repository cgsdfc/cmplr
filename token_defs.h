#ifndef TOKEN_DEF_H 
#define TOKEN_DEF_H 1

typedef enum token_len_type 
{
  TFE_BRIEF=1,
  TFE_VARLEN,
  TFE_FIXLEN,
  _TFE_LEN_TYPE_END
} token_len_type;


typedef enum integer_flag
{
  INT_FLAG_UNSIGNED=1,
  INT_FLAG_LONG=2
} integer_flag ;


typedef enum tkz_error
{
  E_UNEXPECTED_CHAR=1,
  E_PREMATURE_END,
  E_NO_MORE_PUT_CHAR,
  E_FIXLEN_TOO_LONG,
  E_NOMEM,
  E_TOKEN_NOT_BREIF,
  E_TOKEN_NOT_VARLEN,
  E_TOKEN_NOT_FIXLEN,

} tkz_error ;


#endif

