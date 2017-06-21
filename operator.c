/* operator.c */
#include "operator.h"

static op_struct operators[]=
{
  {
    .kind="init_tilde_like_operator",
    .op_count=1,
    .op_states=1,
    .op={CHAR_CLASS_TILDE}, 
    .reject={CHAR_CLASS_EMPTY },
    .states={
      { TK_TILDE }
    },
    .init=add_1state
  },

  {
    .kind="init_exclaim_like_operator",
    .op_count=6,
    .op_states=3,
    .states= {
      { TK_EXCLAIM, TK_EXCLAIM_END, TK_EXCLAIM_EQUAL },
      { TK_EQUAL, TK_EQUAL_END, TK_EQUAL_EQUAL },
      { TK_STAR, TK_STAR_END, TK_STAR_EQUAL },
      { TK_PERCENT, TK_PERCENT_END, TK_PERCENT_EQUAL },
      { TK_CARET, TK_CARET_END, TK_CARET_EQUAL },
      { TK_SLASH, TK_SLASH_END, TK_SLASH_EQUAL }
    },
    .op={
      CHAR_CLASS_EXCLAIM ,
      CHAR_CLASS_EQUAL,
      CHAR_CLASS_STAR,
      CHAR_CLASS_PERCENT ,
      CHAR_CLASS_CARET ,
      CAHR_CLASS_SLASH
    },
    .reject={
      CHAR_CLASS_EQUAL,
      CHAR_CLASS_EQUAL,
      CHAR_CLASS_EQUAL,
      CHAR_CLASS_EQUAL,
      CHAR_CLASS_EQUAL,
      CHAR_CLASS_SLASH_STAR_EQUAL,
      
    },
    .init=add_3states
  },
  {
    .kind="init_ampersand_like_operator",
    .op_count=4,
    .op_states=4,
    .states={
      {
        TK_AMPERSAND, TK_AMPERSAND_END,
        TK_AMPERSAND_AMPERSAND, TK_AMPERSAND_EQUAL
      },
      {
        TK_VERTICAL_BAR, TK_VERTICAL_BAR_END, 
        TK_VERTICAL_BAR_BAR, TK_VERTICAL_BAR_EQUAL
      },
      {
        TK_POSITIVE, TK_POSITIVE_END,
        TK_POSITIVE_POSITIVE, TK_POSITIVE_EQUAL
      },
      {
        TK_NEGATIVE, TK_NEGATIVE_END,
        TK_NEGATIVE_NEGATIVE, TK_NEGATIVE_EQUAL
      }
    },
    .op={
      CHAR_CLASS_AMPERSAND ,
      CHAR_CLASS_VERTICAL_BAR ,
      CHAR_CLASS_POSITIVE ,
      CHAR_CLASS_NEGATIVE ,
    },
    .reject={
      CHAR_CLASS_AMPERSAND_EQUAL ,
      CHAR_CLASS_VERTICAL_BAR_EQUAL ,
      CHAR_CLASS_POSITIVE_EQUAL ,
      CHAR_CLASS_NEGATIVE_EQUAL ,
    },
    .init=add_4states

  },
  {
    .kind="init_less_like_operator",
    .op_count=2,
    .states={
      {
        TK_LESS, TK_LESS_END, TK_LESS_EQUAL,
        TK_LESS_LESS,TK_LESS_LESS_END, TK_LESS_LESS_EQUAL
      },
      {
        TK_GREATER, TK_GREATER_END, TK_GREATER_EQUAL,
        TK_GREATER_GREATER,TK_GREATER_GREATER_END, TK_GREATER_GREATER_EQUAL
      }
    } ,
    .op_states=6,
    .op={
      CHAR_CLASS_LESS ,
      CHAR_CLASS_GREATER ,
    } ,
    .reject={
      CHAR_CLASS_LESS_EQUAL ,
      CHAR_CLASS_GREATER_EQUAL ,
    } ,
    .init=add_6states
  },

  {0}

};

void check_operators(void)
{
  int op_count=0;
  int rej_count=0;
  op_struct *ops=NULL;
  char_class_enum *op;
  char_class_enum *rejc;
  op_struct null_ops={0};

  for (ops=operators; memcmp(ops, &null_ops, sizeof null_ops)!=0;ops++)
  {
    /* init func is not null */
    assert (ops->init);

    /* count reject */
    for (rejc=ops->reject;*rejc;++rejc);
    /* do nothing */

    rej_count=rejc-ops->reject;
    assert (rej_count == ops->op_count);


    /* count op */
    for (op=ops->op;*op;++op); 
    /* do nothing */

    op_count=op - ops->op;
    assert (op_count == ops->op_count);

    /* count states */
    for (int i=0;i<op_count;++i)
    {
      assert (ops->states[i]!=NULL);
      for (int j=0;j<ops->op_states;++j)
      {
        assert (ops->states[i][j]!=0);
      }

    }
  }

  assert (ops-operators == N_OPERATOR_KINDS);
  printf("check_operators passed\n");
}

void init_operator(void) 
{
  check_operators();
  add_arrow_operator();
  for (int i=0;i<N_OPERATOR_KINDS;++i)
  {
    op_struct *ops=&operators[i];
    int op_count=ops->op_count;
    puts(ops->kind);

    for (int j=0;j<op_count;++j)
    {
      node *state=ops->states[j];
      char_class_enum op=ops->op[j];
      char_class_enum rejc=ops->reject[j];
      ops->init(state,op,rejc);
    }
  }

}

void add_arrow_operator(void)
{
  add_accepted(TK_NEGATIVE, TK_NEGATIVE_GREATER, CHAR_CLASS_GREATER);
}

/* accepted only needs one state */
void add_1state(node *state, char_class_enum op, char_class_enum rejc)
{
  add_accepted(TK_INIT,state[0],op);
}

/** accepted only needs 3 states */
void add_3states(node *state, char_class_enum op, char_class_enum rej)
{
  // ! -> !=
  add_initial(state[0],op);
  add_accepted_rev(state[0],state[1],rej);
  add_accepted(state[0], state[2], CHAR_CLASS_EQUAL);

}

void add_4states(node *state, char_class_enum op, char_class_enum rejc)
{
  add_initial(state [0], op);
  add_accepted_rev(state[0],state [1],rejc);
  add_accepted(state [0], state [2], op);
  add_accepted(state [0], state [3], CHAR_CLASS_EQUAL);

}

void add_6states(node *state, char_class_enum op, char_class_enum reject)
{
  add_initial(state [0],op); 
  add_accepted_rev(state [0],state [1],reject);
  add_accepted(state [0], state [2],CHAR_CLASS_EQUAL);
  add_intermedia(state [0], state [3],op);
  add_accepted_rev(state [3], state [4], CHAR_CLASS_EQUAL);
  add_accepted(state [3], state [5], CHAR_CLASS_EQUAL);

}


