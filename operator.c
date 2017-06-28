/* operator.c */
#include "operator.h"
#include "tknzr_state.h"

op_struct operators[]=
{
  {
    .kind="init_tilde_like_operator",
    .op_count=1,
    .op_states=2,
    .op={CHAR_CLASS_TILDE}, 
    .reject={CHAR_CLASS_EMPTY },
    .states={
      { TK_TILDE, TK_TILDE_END }
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
      CHAR_CLASS_SLASH
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
      CHAR_CLASS_NEGATIVE_EQUAL_GREATER ,
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

void init_operator(void) 
{
  add_arrow_operator();
  for (int i=0;i<N_OPERATOR_KINDS;++i)
  {
    op_struct *ops=&operators[i];
    int op_count=ops->op_count;


    for (int j=0;j<op_count;++j)
    {
      tknzr_state *state=ops->states[j];
      char_class op=ops->op[j];
      char_class rejc=ops->reject[j];
      ops->init(state,op,rejc);

    }
  }

}

void add_arrow_operator(void)
{
  add_accepted(TK_NEGATIVE, TK_NEGATIVE_GREATER, CHAR_CLASS_GREATER);
}

/* accepted only needs one state */
void add_1state(tknzr_state *state, char_class op, char_class rejc)
{
  add_initial(state[0],op);
  add_accepted_rev(state[0],state[1],CHAR_CLASS_EMPTY);
}

/** accepted only needs 3 states */
void add_3states(tknzr_state *state, char_class op, char_class rej)
{
  // ! -> !=
  add_initial(state[0],op);
  add_accepted_rev(state[0],state[1],rej);
  add_accepted(state[0], state[2], CHAR_CLASS_EQUAL);

}

void add_4states(tknzr_state *state, char_class op, char_class rejc)
{
  add_initial(state [0], op);
  add_accepted_rev(state[0],state [1],rejc);
  add_accepted(state [0], state [2], op);
  add_accepted(state [0], state [3], CHAR_CLASS_EQUAL);

}

void add_6states(tknzr_state *state, char_class op, char_class reject)
{
  add_initial(state [0],op); 
  add_accepted_rev(state [0],state [1],reject);
  add_accepted(state [0], state [2],CHAR_CLASS_EQUAL);
  add_intermedia(state [0], state [3],op);
  add_accepted_rev(state [3], state [4], CHAR_CLASS_EQUAL);
  add_accepted(state [3], state [5], CHAR_CLASS_EQUAL);

}


const token_type state2operator []=
{
    /* tilde_like_operator */
  [TK_TILDE_END]=TKT_UNARY_OP_BIT_NOT,

  /* slash_like_operator */ 
  /* slash */
  [TK_SLASH_END]=TKT_BINARY_OP_DIV,
  [TK_SLASH_EQUAL]=TKT_BINARY_OP_DIV_ASSIGN,

  /* exlaim_like_operator */
  /* exlaim */
  [TK_EXCLAIM_END]=TKT_UNARY_OP_LOGICAL_NOT,
  [TK_EXCLAIM_EQUAL]=TKT_BINARY_OP_CMP_NOT_EQUAL,

  /* equal */
  [TK_EQUAL_END]=TKT_BINARY_OP_ASSIGN,
  [TK_EQUAL_EQUAL]=TKT_BINARY_OP_CMP_EQUAL,

  /* percent */
  [TK_PERCENT_END]=TKT_BINARY_OP_MOD,
  [TK_PERCENT_EQUAL]=TKT_BINARY_OP_MOD_ASSIGN,

  /* caret */
  [TK_CARET_END]=TKT_BINARY_OP_BIT_XOR,
  [TK_CARET_EQUAL]=TKT_BINARY_OP_BIT_XOR_ASSIGN,

  /* star */
  [TK_STAR_END]=TKT_STAR,
  [TK_STAR_EQUAL]=TKT_BINARY_OP_MUL_ASSIGN,

  /* ampersand_like_operator */
  /* ampersand */
  [TK_AMPERSAND_END]=TKT_AMPERSAND,
  [TK_AMPERSAND_EQUAL]=TKT_BINARY_OP_BIT_AND_ASSIGN,
  [TK_AMPERSAND_AMPERSAND]=TKT_BINARY_OP_LOGICAL_AND,

  /* vertical bar */
  [TK_VERTICAL_BAR_END]=TKT_BINARY_OP_BIT_OR,
  [TK_VERTICAL_BAR_EQUAL]=TKT_BINARY_OP_BIT_OR_ASSIGN,
  [TK_VERTICAL_BAR_BAR]=TKT_BINARY_OP_LOGICAL_OR,

  /* positive */
  [TK_POSITIVE_END]=TKT_PLUS,
  [TK_POSITIVE_POSITIVE]=TKT_UNARY_OP_PLUS_PLUS,
  [TK_POSITIVE_EQUAL]=TKT_BINARY_OP_ADD_ASSIGN,


  /* negative */
  [TK_NEGATIVE_END]=TKT_MINUS,
  [TK_NEGATIVE_NEGATIVE]=TKT_UNARY_OP_MINUS_MINUS,
  [TK_NEGATIVE_EQUAL]=TKT_BINARY_OP_SUB_ASSIGN,

  /* less_like_operator */
  /* less */ 
  [TK_LESS_END]=TKT_BINARY_OP_CMP_LESS,
  [TK_LESS_EQUAL]=TKT_BINARY_OP_CMP_LESS_EQUAL,
  [TK_LESS_LESS_END]=TKT_BINARY_OP_BIT_LEFT_SHIFT,
  [TK_LESS_LESS_EQUAL]=TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN,

  /* greater */ 
  [TK_GREATER_END]=TKT_BINARY_OP_CMP_GREATER,
  [TK_GREATER_EQUAL]=TKT_BINARY_OP_CMP_GREATER_EQUAL,
  [TK_GREATER_GREATER_END]=TKT_BINARY_OP_BIT_RIGHT_SHIFT,
  [TK_GREATER_GREATER_EQUAL]=TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN,

  /* arrow */
  [TK_NEGATIVE_GREATER]=TKT_BINARY_OP_MEMBER_ARROW,
};

bool is_oper_type (tknzr_state state);
// caller should check state is
// a valid operator accepted one
token_type state2oper(tknzr_state state)
{
  return state2operator[state];
}

