#include <utillib/utillib.hpp>
using namespace utillib;

int main() 
{
  json_parser parser;
  auto json=parser.parse("[  \
      null, null, true, false, 1.222 \
  ]");
  json.element(0).pretty_print();
  json.pretty_print();
}
