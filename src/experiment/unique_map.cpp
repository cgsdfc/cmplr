#include "unique_map.hpp"
#include <string>

using namespace experiment;
typedef unique_map<std::string> unique_map_string;

int main(int, char**) {
  unique_map_string string_map;

  auto id0 = string_map["sss"];
  auto id1 = string_map["sss"];
  auto s0 = string_map[id0];
  auto id2 = string_map["aaa"];
  auto s1 = string_map[id2];
}
