#ifndef EXPERIMENT_SMALL_LANG_HPP
#define EXPERIMENT_SMALL_LANG_HPP 1
#include "language.hpp"

namespace experiment {
struct simple {
  static language& definition(language& lang);
};
struct quick {
  static language& definition(language& lang);
};
struct easy {
  static language& definition(language& lang);
};
} // namespace experiment
#endif // EXPERIMENT_SMALL_LANG_HPP
