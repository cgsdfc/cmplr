#include "unique_map.hpp"
#include "symbol.hpp"
#include "rule_type.hpp"
#include "itemset.hpp"
#include "clang.hpp"
#include "grammar.hpp"

using namespace experiment;
using namespace std;

int main(int ac,char**av){ 
  clang cl;
  cout << cl.lang;
  cl.lang.notify();
  grammar gr(cl.lang);

}
