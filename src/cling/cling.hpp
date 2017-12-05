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

#include <utility>

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
    public:
      symbol_entry();
      ~symbol_entry() {}
      symbol_entry(struct cling_symbol_entry const* entry);
      symbol_entry(int kind, json_value value);
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
      void insert(char const *name, symbol_entry const& entry, int scope_kind);
      void reserve(char const *name, int scope_kind);
      std::pair<bool, symbol_entry> find(char const* name, int scope_kind) const;
      bool exist_name(char const *name, int scope_kind) const;
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

  class arglist : public json_array {
    public:
      arglist(json_array array);
      size_t argc() const;
      json_array argv();
  };

  class function : private ast_node {
    public:
      function(json_value val);
      json_value name();
      json_array arglist();
      json_object composite_stmt();
  };

  class statement: private ast_node {
    public:
      size_t type() const;
      bool is_printf_stmt() const;
      bool is_scanf_stmt() const;
      bool is_composite_stmt() const;
      bool is_empty_stmt() const;
      bool is_expr_stmt() const;
  };

  class scanf_stmt: public statement {
    public:
      json_array args();
  };

  class printf_stmt : private ast_node {
    public:
      json_array args();
  };
      
  class expression : private ast_node {
    public:
      json_value op();
      json_value lhs();
      json_value rhs();
  };

  class call_expr : private expression {
    public:
      json_value callee() ;
      json_array args();
  };

} // cling
#endif // CLING_CLING_HPP

