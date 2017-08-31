#include "language.hpp"

using namespace experiment;

int main(int,char**) {
  language lang("C Programming Language");

  lang[start("program")]
    (optional("translation-unit"), eof("eof"))
    ;

  std::cout << lang;

  /* lang["translation-unit"] */
  /*   (listof("external-declaration")) */
  /*   ; */

  /* lang["external-declaration"] */
  /*   ("function-definition") */
  /*   ("declaration") */
  /*   ; */

}

