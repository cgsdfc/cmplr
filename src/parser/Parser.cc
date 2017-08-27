#include "Language.h"
using namespace parser;

void draft(void) {
  Language lang;

  lang.lhs("program") | lang.rhs("translation unit", "EOF");

  lang.lhs("translation unit") | lang.rhs("external declaration") |
      lang.rhs("translation unit", "external declaration");

  lang.lhs("storage specifier") | lang.rhs("auto") | lang.rhs("register") |
      lang.rhs("static") | lang.rhs("extern") | lang.rhs("typedef");

  lang.lhs("pointer") | lang.rhs("*", ("type qualifier"), ("pointer"));
}

int main(int argc, char **argv) { draft(); }
