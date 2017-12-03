#ifndef CLING_CLING_HPP
#define CLING_CLING_HPP
extern "C" {
#include <cling/rd_parser.h>
#include <cling/opg_parser.h>
#include <cling/scanner.h>
#include <cling/symbol_table.h>
}

// How about letting cpp to document my code!
//
#include <utillib/utillib.hpp>

namespace cling {
  using utillib::json_array;
  using utillib::json_value;
  using utillib::json_object;

  class program;
  class scanner;
  class parser;
  class symbol_table;
  class symbol_entry;
  class const_decls;
  class var_decls;

  class scanner {
    public:
      struct utillib_token_scanner self;
    public:
      scanner(FILE *file);
      ~scanner();
      size_t lookahead();
      void shiftaway();
      void const * semantic();
  };

  class symbol_entry {
    public:
      struct cling_symbol_entry self;
      symbol_entry(int kind, json_value value);
      ~symbol_entry() {}
      int kind() const ;
      json_value value() const;
  };


  class symbol_table {
    public:
      struct cling_symbol_table self;
    public:
      symbol_table();
      ~symbol_table();
      void enter_scope();
      void leave_scope();
      int insert(char const *name, symbol_entry const& entry);
      void update(char const *name, symbol_entry const& entry);
      symbol_entry find(char const* name, size_t level) const;
      void reserve(char const *name);
      bool exist_name(char const *name, size_t level) const;
      json_value tojson() const;
  };

  class parser {
    public:
      struct cling_rd_parser self;
    public:
      parser(symbol_table&);
      ~parser();
      json_value parse(scanner&);
      void report_errors() const;
  };

  class ast_node {
    public:
      json_object self;
      ast_node(json_object object);

  };

  class program: private ast_node {
    public:
      program(json_value val);
      json_array const_decls();
      json_array var_decls();
      json_array func_decls();
  };

  class const_decl: private ast_node {
    public:
      json_value type();
      json_array const_defs();
  };

  class const_def: private ast_node {
    public:
      json_value name();
      json_value value();
  };

  class var_decls : private ast_node{
    public:
      json_value type();
      json_array var_defs();
  };

} // cling
#endif // CLING_CLING_HPP

