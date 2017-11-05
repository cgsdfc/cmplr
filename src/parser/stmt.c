/* statement.c */
#include "statement.h"

void def_statement(void) {
  def_nonterm(&statement);
  def_oneof(statement, labeled_stmt,
