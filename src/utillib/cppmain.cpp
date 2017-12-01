#include <utillib/utillib.hpp>
#include <vector>
#include <string>
#include <iostream>

int main() 
{
  utillib::vector strvector;
  strvector.push_back("BatMan");
  strvector.push_back("SuperMan");
  auto json=strvector.tojson(utillib_json_string_create_adaptor);
  json.pretty_print();

}
