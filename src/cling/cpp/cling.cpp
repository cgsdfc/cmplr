#include "cling.hpp"
#include <string.h>

namespace cling {

//
// scanner
//

scanner::scanner(FILE *file) { cling_scanner_init(&self, file); }

scanner::~scanner() { utillib_token_scanner_destroy(&self); }

size_t scanner::lookahead() { return utillib_token_scanner_lookahead(&self); }

void scanner::shiftaway() { utillib_token_scanner_shiftaway(&self); }

//
// symbol_entry
//

symbol_entry::symbol_entry() {}

symbol_entry::symbol_entry(struct cling_symbol_entry const *entry) {
  memcpy(&self, entry, sizeof self);
}

symbol_entry::symbol_entry(int kind, json_value value) {
  self.kind = kind;
  self.value = value.self;
}

int symbol_entry::kind() const { return self.kind; }

json_value symbol_entry::value() const { return json_value(self.value); }

//
// symbol_table
//

symbol_table::symbol_table() { cling_symbol_table_init(&self); }

symbol_table::~symbol_table() { cling_symbol_table_destroy(&self); }

void symbol_table::enter_scope() { cling_symbol_table_enter_scope(&self); }

void symbol_table::leave_scope() { cling_symbol_table_leave_scope(&self); }

void symbol_table::insert(char const *name, const symbol_entry &entry,
                          int scope_kind) {
  cling_symbol_table_insert(&self, name, entry.kind(), entry.self.value,
                            scope_kind);
}

bool symbol_table::exist_name(char const *name, int scope_kind) const {
  return cling_symbol_table_exist_name(&self, name, scope_kind);
}

std::pair<bool, symbol_entry> symbol_table::find(char const *name,
                                                 int scope_kind) const {
  auto result = cling_symbol_table_find(&self, name, scope_kind);
  if (result)
    return std::make_pair(true, symbol_entry(result));
  return std::make_pair(false, symbol_entry());
}

void symbol_table::reserve(char const *name, int scope_kind) {
  cling_symbol_table_reserve(&self, name, scope_kind);
}

json_value symbol_table::tojson() const {
  return json_value(cling_symbol_table_json_object_create(&self));
}

//
// parser
//

parser::parser(symbol_table &symbol_table_) {
  cling_rd_parser_init(&self, &symbol_table_.self, NULL);
}

parser::~parser() { cling_rd_parser_destroy(&self); }

json_value parser::parse(scanner &scanner) {
  auto val = cling_rd_parser_parse(&self, &scanner.self);
  return val ? json_value(val) : json_value();
}

void parser::report_errors() const { cling_rd_parser_report_errors(&self); }

ast_node::ast_node(json_object object) : self(object) {}

//
// program
//

program::program(json_value val) : ast_node(val) {}

json_array program::const_decls() { return self.at("const_decls"); }

json_array program::var_decls() { return self.at("var_decls"); }

json_array program::func_decls() { return self.at("func_decls"); }

//
// const_decl
//

json_value const_decl::type() { return self.at("type"); }

json_array const_decl::const_defs() { return self.at("const_defs"); }

arglist::arglist(json_array array) : json_array(array) {}

size_t arglist::argc() const { return size(); }

json_array arglist::argv() { return *this; }

//
// function
//

function::function(json_value val) : ast_node(val) {}

json_value function::name() { return self.at("name"); }

json_array function::arglist() { return self.at("arglist"); }

json_object function::composite_stmt() { return self.at("comp"); }

//
// expression
//

json_value expression::op() { return self.at("op"); }

json_value expression::lhs() { return self.at("lhs"); }

json_value expression::rhs() { return self.at("rhs"); }

json_value call_expr::callee() { return lhs(); }

json_array call_expr::args() { return rhs(); }

//
// statement
//

size_t statement::type() const { return size_t(self.at("type")); };

bool statement::is_printf_stmt() const { return type() == SYM_PRINTF_STMT; }

} // cling
