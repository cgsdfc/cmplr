#include "dumper.hpp"
namespace exp = experiment;
int main(int ac, char** av) {
  exp::dumper d;
  d.dump(ac, av);
}
