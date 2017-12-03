#include <utillib/utillib.hpp>
#include <vector>
#include <string>
#include <iostream>

int main() 
{

  utillib::json_parser jp;
  auto result=jp.parse("{{{{{{{{{}}}}}}}}}");
  result.pretty_print();

}
