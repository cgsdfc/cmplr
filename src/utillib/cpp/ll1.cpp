#include "ll1.h"

using namespace utillib::ll1;

Factory::Factory(struct utillib_symbol const * symbols,
    struct utillib_rule_literal const * rules)
{
  utillib_ll1_factory_build_init(&factory_, symbols, rules);
}

Factory::Factory(const std::size_t * gentable,
    struct utillib_symbol const * symbols,
    struct utillib_rule_literal const * rules)
{
  utillib_ll1_factory_gen_init(&factory_, gentable, symbols, rules);
}

Factory::~Factory() noexcept {
  utillib_ll1_factory_destroy(&factory_);
}

Parser * Factory::newParser(void *client_data, struct utillib_ll1_callback
