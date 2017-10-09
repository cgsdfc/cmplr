typedef enum directive_type {
  _DR_BEGIN,
  DR_DEFINE,
  DR_UNDEF,
  DR_IF,
  DR_IFNDEF,
  DR_DEFINED,
  DR_ERROR,
  /* error string && fatal abort */
  DR_LINE,
  DR_INCLUDE,
  DR_IFDEF,
  DR_ASSERT,
  /* #assert predicate (answer) */
  DR_CPU,
  DR_ELIF,
  DR_ELSE,
  DR_IDENT,
  DR_IMPORT,
  DR_INCLUDE_NEXT,
  DR_MACHINE,
  DR_PRAGMA, /* obsolete */
  DR_SYSTEM,
  DR_PRAGMA_ONCE,
  /* obsolete */ /* include only once */
  DR_UNASSERT,
  DR_WARNING,
  _DR_END,

} directive_type;

const char *pp_directives[] = {
        [DR_DEFINE] = "define",
        [DR_UNDEF] = "undef",
        [DR_IF] = "if",
        [DR_IFNDEF] = "ifndef",
        [DR_DEFINED] = "defined",
        [DR_LINE] = "line",
        [DR_ERROR] = "error",
        [DR_INCLUDE] = "include",
        [DR_IFDEF] = "ifdef",
        [DR_ASSERT] = "assert",
        [DR_CPU] = "cpu",
        [dr_elif] = "elif",
        [DR_ELSE] = "else",
        [dr_ident] = "ident",
        [DR_IMPORT] = "import",
        [DR_INCLUDE_NEXT] = "include_next",
        [DR_MACHINE] = "machine",
        [DR_PRAGMA] = "pragma",
        [DR_SYSTEM] = "system",
        [DR_PRAGMA_ONCE] = "pragma once",
        [DR_UNASSERT] = "unassert",
        [DR_WARNING] = "warning",

};

typedef enum predined_macro {
  _PD_BEGIN,
  PD_FILE,
  PD_LINE,
  PD_DATE,
  PD_TIME,
  PD_STDC,
  PD_STDC_VERSION,
  PD_GNUC,
  PD_GNUC_MINOR,
  PD_GNUG,
  PD_CPLUS_PLUS,
  PD_STRICT_ANSI,
  PD_BASE_FILE,
  PD_INCLUDE_LEVEL,
  PD_VERSION,
  PD_OPTIMIZE,
  PD_REGISTER_PREFIX,
  PD_USER_LABEL_PREFIX,

  _PD_END,
} predined_macro;

const char *pd_tab[] = {
        [PD_FILE] = "__FILE__",
        [PD_LINE] = "__LINE__",
        [PD_DATE] = "__DATE__",
        [PD_TIME] = "__TIME__",
        [PD_STDC] = "__STDC__",
        [PD_STDC_VERSION] = "__STDC_VERSION__",
        [PD_GNUC] = "__GNUC__",
        [PD_GNUC_MINOR] = "__GNUC_MINOR__",
        [PD_GNUG] = "__GNUG__",
        [PD_CPLUS_PLUS] = "__cplusplus",
        [PD_STRICT_ANSI] = "__STRICT_ANSI__",
        [PD_BASE_FILE] = "__BASE_FILE__",
        [PD_INCLUDE_LEVEL] = "__INCLUDE_LEVEL__",
        [PD_VERSION] = "__VERSION__",
        [PD_OPTIMIZE] = "__OPTIMIZE__",
        [PD_REGISTER_PREFIX] = "__REGISTER_PREFIX__",
        [PD_USER_LABEL_PREFIX] = "__USER_LABEL_PREFIX__",
};

typedef enum pp_state {

} pp_state;

static state_table *pp_table;

void init_preprocessor(void) {}
