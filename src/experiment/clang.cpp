#include "clang.hpp"

namespace experiment {
clang::clang() {
  lang.name("C_Programming_Language")
    ;
  lang["translation_unit"]
    ("external_declaration")
    ("translation_unit", "external_declaration")
    ;
  lang["external_declaration"]
    ("function_definition")
    ("declaration")
    ;
  lang["function_definition"]
    (optional("declaration_specifier"), "declarator", optional("declaration_list"), "composite_statement")
    ;
  lang["declaration"]
    ("declaration_specifier", optional("initial_declarator_list"))
    ;
  lang["declaration_specifier"]
    ("storage_class_specifier", optional("declaration_specifier"))
    ("type_specifier", optional("declaration_specifier"))
    ("type_qualifier", optional("declaration_specifier"))
    ;
  lang["declarator"]
    (optional("pointer"), "direct_declarator")
    ;
  lang["storage_class_specifier"]
    (("auto"))
    (("register"))
    (("static"))
    (("extern"))
    (("typedef"))
    ;
  lang["type_specifier"](("void"))(("char"))(("short"))(
      ("int"))(("long"))(("float"))(("double"))(
      ("signed"))(("unsigned"))(("identifier"))(
      "struct_union_specifier")("enum_specifier")
      ;
  lang["type_qualifier"](("const"))(("volatile"))
    ;
  lang["struct_or_union"](("struct"))(("union"))
    ;
  lang["struct_or_union_specifier"]
    ("struct_or_union", 
     optional("identifier"), 
     "{", "struct_declaration_list", "}")
    ("struct_or_union", "identifier")
    ;
  lang["struct_declaration_list"]
    ("struct_declaration_list", "struct_declaration")
    ("struct_declaration")
    ;
  lang["initial_declarator_list"]
    ("initial_declarator_list", "initial_declarator")
    ("initial_declarator")
    ;
  lang["initial_declarator"]
    ("declarator", "=", "initializer")
    ("declarator")
    ;
  lang["struct_declaration"]
    ("specifier_qualifier_list","struct_declaration_list")
    ;
  lang["specifier_qualifier_list"]
    ("type_qualifier", optional("specifier_qualifier_list"))
    ("type_specifier", optional("specifier_qualifier_list"))
    ;
  lang["struct_declarator_list"]
    ("struct_declarator_list", "struct_declarator")
    ("struct_declarator")
    ;
  lang["struct_declarator"]
    ("declarator")
    (optional("declarator"), ":", "const_expression")
    ;
  lang["enum_specifier"]
    ("enum", optional("identifier"),
     "{", "enumerator_list", "}")
    ("enum", "identifier")
    ;
  lang["enumerator_list"]
    ("enumerator_list", "enumerator")
    ("enumerator")
    ;
  lang["enumerator"]
    ("identifier", "=", "const_expression")
    ("identifier")
    ;
  lang["direct_declarator"]
    ("identifier")
    ("(", "declarator", ")")
    ("direct_declarator", "[", optional("const_expression"), "]")
    ("direct_declarator", "(", "parameter_type_list", ")")
    ("direct_declarator", "(", optional("identifier_list"), ")")
    ;
  lang["pointer"]
    ("*", optional("type_qualifier_list"), optional("pointer"))
    ;
  lang["type_qualifier_list"]
    ("type_qualifier")
    ("type_qualifier_list", "type_qualifier")
    ;
  lang["ellipses"]
    (",", "...")
    ;
  lang["parameter_type_list"]
    ("parameter_list", optional("ellipses"))
    ;
  lang["parameter_list"]
    ("parameter_declarator")
    ("parameter_list", ",", "parameter_declarator")
    ;
  lang["parameter_declarator"]
    ("declaration_specifier", optional("declarator"))
    ("declaration_specifier", optional("abstract_declarator"))
    ;
  lang["abstract_declarator"]
    (optional("pointer"), "direct_abstract_declarator")
    ;
  lang["direct_abstract_declarator"]
    ("(", "abstract_declarator", ")")
    (optional("direct_abstract_declarator"), "[", optional("const_expression"), "]")
    (optional("direct_abstract_declarator"), "(", optional("parameter_type_list"), ")")
    ;

  lang["identifier_list"]
    ("identifier")
    ("identifier_list", ",", "identifier")
    ;
  lang["initial_declarator"]
    ("assignment_expression")
    ("{", "initial_declarator_list",optional(","), "}")
    ;
  /* lang["initial_declarator_list" */
  lang["composite_statement"]
    ("{", optional("declaration_list"), optional("statement_list"), "}")
    ;
  lang["const_expression"]
    ("condition_expression")
    ;
  lang["initializer_list"]
    ("initializer")
    ("initializer_list", "initializer")
    ;
  lang["type_name"]
    ("specifier_qualifier_list", optional("abstract_declarator"))
    ;
  lang["statement"]
    ("labeled_statement")
    ("expression_statement")
    ("composite_statement")
    ("selection_statement")
    ("iteration_statement")
    ("jump_statement")
    ;
  lang["labeled_statement"]
    ("identifier", ":", "statement")
    ("case", "const_expression", ":", "statement")
    ("default", ":", "statement")
    ;
  lang["expression_statement"]
    (optional("expression"), ";")
    ;
  lang["expression"]
    ("assignment_expression")
    ("expression", ",", "assignment_expression")
    ;
  lang["selection_statement"]
    ("if", "(", "expression", ")", "statement")
    ("if", "(", "expression", ")", "statement", "else", "statement")
    ("switch", "(", "expression", ")", "statement")
    ;
  lang["iteration_statement"]
    ("while", "(", "expression",")", "statement")
    ("do", "statement", "while", "(", "expression",")")
    ("for", "(", optional("expression"), ";", optional("expression"), ";", optional("expression"), ")", "statement")
    ;
  lang["jump_statement"]
    ("goto", "identifier")
    ("return", optional("expression"))
    ("continue", ";")
    ("break", ";")
    ;
  lang["statement_list"]
    ("statement")
    ("statement_list", "statement")
    ;
  lang["assignment_operator"]
    ("*=") ("/=") ("%=") ("+=")
    ("-=") ("<<=") (">>=") ("&=")
    ;
  lang["assignment_expression"]
    ("condition_expression")
    ("unary_expression", "assignment_operator", "assignment_expression")
    ;
  lang["condition_expression"]
    ("logical_or_expression")
    ("logical_or_expression","?","expression",":","condition_expression")
    ;
  lang["logical_or_expression"]
    ("logical_and_expression")
    ("logical_or_expression", "||", "logical_and_expression")
    ;
  lang["logical_and_expression"]
    ("inclusive_or_expression")
    ("logical_and_expression", "&&", "inclusive_or_expression")
    ;
  lang["inclusive_or_expression"]
    ("exclusive_or_expression")
    ("inclusive_or_expression", "|", "exclusive_or_expression")
    ;
  lang["exclusive_or_expression"]
    ("and_expression")
    ("exclusive_or_expression", "^", "and_expression")
    ;
  lang["and_expression"]
    ("equality_expression")
    ("and_expression", "&", "equality_expression")
    ;
  lang["equality_expression"]
    ("relational_expression")
    ("equality_expression", "equality_operator", "relational_expression")
    ;
  lang["equality_operator"]
    ("==")("!=")
    ;
  lang["relational_expression"]
    ("shift_expression")
    ("relational_expression", "relational_operator", "shift_expression")
    ;
  lang["relational_operator"]
    ("<")(">")("<=")(">=")
    ;
  lang["shift_expression"]
    ("additive_expression")
    ("shift_expression", "shift_operator", "additive_expression")
    ;
  lang["shift_operator"]
    (">>")("<<");
  lang["additive_expression"]
    ("multiplicative_expression")
    ("additive_expression", "additive_operator", "multiplicative_expression")
    ;
  lang["additive_operator"]
    ("+")("-")
    ;
  lang["multiplicative_expression"]
    ("cast_expression")
    ("multiplicative_expression", "multiplcative_operator","cast_expression")
    ;
  lang["multiplcative_operator"]
    ("*")("%")("/")
    ;
  lang["cast_expression"]
    ("unary_expression")
    ("(", "cast_expression", ")")
    ;
  lang["unary_operator"]
    ("*")("+")("-")("~")("!")("&")
    ;
  lang["increment_decrement_operator"]
    ("++")("--")
    ;
  lang["unary_expression"]
    ("postfix_expression")
    ("increment_decrement_operator", "unary_expression")
    ("unary_operator", "cast_expression")
    ("sizeof", "unary_expression")
    ("sizeof", "(", "type_name", ")")
    ;
  lang["postfix_expression"]
    ("primary_expression")
    ("postfix_expression", "[", "expression", "]")
    ("postfix_expression", "(", optional("argument_expression_list"), ")")
    ("postfix_expression", "member_access_operator", "identifier")
    ("postfix_expression", "increment_decrement_operator")
    ;
  lang["primary_expression"]
    ("identifier")
    ("character_literal")
    ("floating_point_literal")
    ("integeral_literal")
    ("(", "expression", ")")
    ;
  lang["argument_expression_list"]
    ("assignment_expression")
    ("argument_expression_list", ",", "assignment_expression")
    ;
}
} // namespace experiment
