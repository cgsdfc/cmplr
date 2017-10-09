lang["translation_unit"](list("external_declaration"));

lang["external_declaration"](alter("function_definition", "declaration"););

lang["function_definition"](follow(optional("declaration_specifier"),
                                   "declarator", optional("declaration_list"),
                                   "composite_statement"));

lang["declaration"](follow("declaration_specifier",
                           optional(list("initial_declarator"))));

lang["declaration_specifier"](
    alter(follow("storage_class_specifier", optional(self())),
          follow("type_specifier", optional(self())),
          follow("type_qualifier", optional(self()), )));

lang["declarator"](follow(optional("pointer"), "direct_declarator"));

lang["storage_class_specifier"](alter("auto", "register", "static", "extern",
                                      "typedef"));

lang["type_qualifier"](alter("const", "volatile"););

lang["struct_or_union_specifier"](follow(alter("struct", "union"),
                                         optional("identifier"), "{",
                                         list("struct_declaration"),
                                         "}"))(follow(alter("struct", "union"),
                                                      "identifier"));

lang["initial_declarator"]("declarator", optional("=", "initializer"));

lang["labeled_statement"]
