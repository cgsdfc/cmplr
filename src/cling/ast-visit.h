#ifndef CLING_AST_VISIT_H
#define CLING_AST_VISIT_H

struct ast_visit_callback {




};

struct ast_visitor {
        struct ast_visit_callback const *callback;
        void *visit_data;
};

#endif /* CLING_AST_VISIT_H */
