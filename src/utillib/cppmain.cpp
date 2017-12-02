#include <utillib/utillib.hpp>
#include <vector>
#include <string>
#include <iostream>

int main() 
{
  auto object=utillib::json_object_create();
  object.pretty_print();
  object.destroy();


}
