#include "language.hpp"
#include "clang.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <fstream>

using std::cout;
using std::ofstream;
using std::ifstream;
using namespace experiment;
using boost::archive::text_iarchive;
using boost::archive::text_oarchive;

int main(int,char**) {
  experiment::language lang;
  cout << experiment::lang_traits<clang>::definition(lang);
}

