%{
/* prefer c style comment */
 /* include, declaration goes here */

%}

/* union token type left right declaration goes here */

%union {
  char *text;
  node_base * node_type;
}

%token <text> IDENTIFIER
%type <node_type> exression assign_expr condition_expr
logical_or_expr logical_and_expr

%%

/* grammar rules and action goes here */

expression : assign_expr | exression ',' assign_expr ;

assign_expr : condition_expr | unary_expr assign_op assign_expr ;

assign_op : K_EQ | K_MUL_EQ | K_DIV_EQ | K_MOD_EQ | K_PLUS_EQ | K_MINUS_EQ | K_LSH_EQ
          | K_RSH_EQ | K_AND_EQ | K_XOR_EQ | K_OR_EQ ;

condition_expr : logical_or_expr 
               | logical_or_expr '?' expression ':' condition_expr ;

constant_expr : condition_expr ;

logical_or_expr : logical_and_expr 
                | logical_or_expr K_LOR logical_and_expr ;



%%

