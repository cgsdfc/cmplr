#include "cling.hpp"

namespace cling {

//
// scanner
// 

scanner::scanner(FILE *file) {
  cling_scanner_init(&self, file);
}

scanner::~scanner() {
  utillib_token_scanner_destroy(&self);
}

size_t scanner::lookahead() {
  return utillib_token_scanner_lookahead(&self);
}

void scanner::shiftaway() {
  utillib_token_scanner_shiftaway(&self);
}

//
// symbol_entry
// 

symbol_entry::symbol_entry(int kind, json_value value) {
  self.kind=kind;
  self.value=value.self;
}

int symbol_entry::kind() const {
  return self.kind;
}

json_value symbol_entry::value() const {
  return json_value(self.value);
}

//
// symbol_table
// 

symbol_table::symbol_table() {
  cling_symbol_table_init(&self);
}

symbol_table::~symbol_table() {
  cling_symbol_table_destroy(&self);
}

void symbol_table::enter_scope() {
  cling_symbol_table_enter_scope(&self);
}

void symbol_table::leave_scope() {
  cling_symbol_table_leave_scope(&self);
}

int symbol_table::insert(char const *name, const symbol_entry& entry) {
  return  cling_symbol_table_insert(&self, name, entry.self.kind, entry.self.value);
}

void symbol_table::update(char const *name, const symbol_entry& entry) {
  cling_symbol_table_update(&self, name, entry.self.kind, entry.self.value);
}

bool symbol_table::exist_name(char const *name, size_t level) const {
  return cling_symbol_table_exist_name(&self, name, level);
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

parser::~parser() {
  cling_rd_parser_destroy(&self);
}

json_value parser::parse(scanner& scanner) {
  auto val=cling_rd_parser_parse(&self, &scanner.self);
  return val? json_value(val) : json_value();
}

ast_node::ast_node(json_object object): self(object) {}

program::program(json_value val): ast_node(val) {}

json_array program::const_decls() {
  return self.at("const_decls");
}

json_array program::var_decls() {
  return self.at("var_decls");
}

json_array program::func_decls() {
  return self.at("func_decls");
}

json_value const_decl::type() {
  return self.at("type");
}

json_array const_decl::const_defs() {
  return self.at("const_defs");
}

} // cling

