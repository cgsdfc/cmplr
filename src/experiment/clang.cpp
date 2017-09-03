#include "clang.hpp"

namespace experiment {
language& clang::definition(language& lang) {
  lang.name("C Programming Language");
  lang["translation unit"]("translation unit", "external declaration");
  lang["external declaration"]("function definition")("declaration");
  lang["function definition"](optional("declaration specifier"), "declarator",
                              optional("declaration list"), "statement block");
  lang["declaration"]("declaration specifier",
                      optional("initial declarator list"));
  lang["storage specifier"](("auto"))(("register"))(
      ("static"))(("extern"))(("typedef"));
  lang["type specifier"](("void"))(("char"))(("short"))(
      ("int"))(("long"))(("float"))(("double"))(
      ("signed"))(("unsigned"))(("identifier"))(
      "struct union specifier")("enum specifier");
  lang["type qualifier"](("const"))(("volatile"));
  lang["struct or union"](("struct"))(("union"));

  return lang;
}
}
