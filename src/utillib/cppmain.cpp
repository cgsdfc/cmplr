#include <utillib/utillib.hpp>
#include <vector>
#include <string>
#include <iostream>

int main() 
{
  utillib::vector iv;
  iv.push_back(new std::string("aaaaaaaaaaaaaaa"));
  auto s=iv.at<std::string*> (0);
  std::cout << *s  << '\n';
  iv.destroy_owning(operator delete);
}
