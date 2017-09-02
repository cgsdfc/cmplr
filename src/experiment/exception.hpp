#ifndef EXPERIMENT_EXCEPTION_HPP
#define EXPERIMENT_EXCEPTION_HPP 1

#include <exception>
#include <stdexcept>
#define EXPERIMENT_EXCEPTION(class_, base_, what_arg_)                 \
  class class_ : public base_ {                                        \
    const char *what() const noexcept override { return (what_arg_); } \
  }

namespace experiment {
class grammar_exception : public std::exception {};

EXPERIMENT_EXCEPTION(
    terminal_as_head, grammar_exception,
    "terminal symbol was used as the head of a production(rule)");
EXPERIMENT_EXCEPTION(no_start_symbol, grammar_exception,
                     "no start symbol S given in the grammar");
EXPERIMENT_EXCEPTION(no_eof_symbol, grammar_exception,
                     "no eof symbol given in the grammar");

EXPERIMENT_EXCEPTION(invalid_principle_rule, grammar_exception,
                     "invalid principle rule");

}  // namespace experiment
#endif  // EXPERIMENT_EXCEPTION_HPP
