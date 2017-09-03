#include "small_lang.hpp"
// compilation of definition code is slow
// so not put in header
namespace experiment {
language& simple::definition(language& lang) {
  lang.name("Simple");
  lang["Sum"]("Product")("Sum", ("+"), "Product");
  lang["Product"]("Value")("Product", ("*"), "Value");
  lang["Value"](("Int"))(("Id"));
  return lang;
}
language& quick::definition(language& lang) {
  lang.name("quick");
  lang["N"]("E")("V", ("="), "E");
  lang["E"]("V");
  lang["V"](("x"))(("*"), "E");
  return lang;
}
language& easy::definition(language& lang) {
  lang.name("easy");
  lang["E"]("E", "*", "B")("E", "+", "B")("B");
  lang["B"]("0")("1");
  return lang;
}
language& loop::definition(language& lang) {
  lang.name("loop");
  lang["A"]("a")("A", "a");
  return lang;
}
}  // namespace experiment
