#ifndef EXPERIMENT_CLANG_H
#define EXPERIMENT_CLANG_H 1
#include "language.hpp"

namespace experiment {
struct clang {
  static const unsigned num_symbols = 375;
  static const unsigned num_terminals = 68;
  static const unsigned num_nonterminals = 168;
  static language& definition(language& lang);
};
}  // namespace experiment
#endif  // EXPERIMENT_CLANG_H
