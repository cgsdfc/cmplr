#include "unique_id.hpp"
#include <iostream>
#include <string>
using std::cout;

typedef unique_id<std::string> string_id;

int main(int argc, char** argv) {
  std::string str("string");
  auto id = string_id::make_id(str);
  cout << id << '\n';
  cout << id.id() << '\n';
  cout << id.content() << '\n';

  try {
    string_id::make_id(str);
  } catch (std::exception& e) {
    cout << e.what();
  }
}
