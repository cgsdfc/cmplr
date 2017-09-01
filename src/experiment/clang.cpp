#include "clang.hpp"

namespace experiment {
language&  clang::definition(language& lang) {
  lang.name("C Programming Language");
  lang[start("program")]
    (optional("translation unit"), eof("eof"))
    ;
  lang["translation unit"]
    ("translation unit", "external declaration")
    ;
 lang["external declaration"]
    ("function definition")
    ("declaration")
    ;
 lang["function definition"]
   (optional("declaration specifier"),
    "declarator",
    optional("declaration list"),
    "statement block"
   );
 lang["declaration"]
   ("declaration specifier",
    optional("initial declarator list")
   );
 lang["storage specifier"]
   (terminal("auto"))
   (terminal("register"))
   (terminal("static"))
   (terminal("extern"))
   (terminal("typedef"))
   ;
 lang["type specifier"]
   (terminal("void"))
   (terminal("char"))
   (terminal("short"))
   (terminal("int"))
   (terminal("long"))
   (terminal("float"))
   (terminal("double"))
   (terminal("signed"))
   (terminal("unsigned"))
   (terminal("identifier"))
   ("struct union specifier")
   ("enum specifier")
   ;
 lang["type qualifier"]
   (terminal("const"))
   (terminal("volatile"))
   ;
 lang["struct or union"]
   (reserved("struct"))
   (reserved("union"))
   ;

 return lang;
}
}

