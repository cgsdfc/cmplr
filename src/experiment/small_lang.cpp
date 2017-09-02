#include "small_lang.hpp"
// compilation of definition code is slow
// so not put in header
namespace experiment {
language& simple::definition(language& lang) {
  lang.name("Simple");
  lang[start("Goal")]("Sum", eof(""));
  lang["Sum"]("Product")("Sum", terminal("+"), "Product");
  lang["Product"]("Value")("Product", terminal("*"), "Value");
  lang["Value"](terminal("Int"))(terminal("Id"));
  return lang;
}
language& quick::definition(language& lang) {
  lang.name("quick");
  lang[start("S")]("N", eof(""));
  lang["N"]("E")("V", terminal("="), "E");
  lang["E"]("V");
  lang["V"](terminal("x"))(terminal("*"), "E");
  return lang;
}
language& easy::definition(language& lang) {
  lang.name("easy");
  lang[start("Begin")]("E", eof(""));
  lang["E"]("E", "*", "B")("E", "+", "B")("B");
  lang["B"]("0")("1");
  return lang;
}
}  // namespace experiment
