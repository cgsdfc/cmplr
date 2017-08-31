#include "clang.hpp"

namespace experiment {
language&  clang::definition(language& lang) {
  lang.name("C Programming Language");
  lang[start("program")]
    (optional("translation-unit"), eof("eof"))
    ;
  lang["translation-unit"]
    ("translation-unit", "external-declaration")
    ;
 lang["external-declaration"]
    ("function-definition")
    ("declaration")
    ;
  return lang;
}
}

