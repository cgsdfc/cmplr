#include "lookahead_set.h"

// input: set: DR, relation: reads.
// apply: digraph
// output: set Read
// 

// X: non_terminal transition (p,A)
// state p(vertex), edge A(non_terminal symbol)
// DR(p,A) = {t in T | p [A] r [t] }.
// (DR) are simply those that label terminal transitions
// of the successor state r of the (p, A) transition.


// precond: init_digraph
// precond: build_item_set
int
compute_direct_read_relation(digraph_struct * d, grammar * g)
{
  int num_state;
  int symbol;
  int from, to;
  int num_terminal;
  void ** bit;

  num_state = g->item_set_id;
  num_terminal = get_num_terminal(g);

  for (int i=0;i<num_state;++i)
  {
    for (int j=0;j<num_state;++j)
    {
      symbol = g->set2symbols[i][j];
      if (symbol == -1 || is_terminal(g,symbol)) // no transition 
        continue;

      from = alloc_nonterm_tran_node(d,i,j);
      bit = &d->m_data[from];
      if(!*bit)
      {
        *bit = create_bitset(num_terminal);
      }
      for (int k=0;k<num_state;++k) 
      {
        symbol = g->set2symbols[j][k];
        if (symbol == -1) // no transition
          continue;
        if (is_terminal(g, symbol))
        {
          set_bitset(d->m_data[from], symbol);
          // set DR, direct read
        }
      }
    }
  }
  for (int i=0;i<num_state;++i)
  {
    for (int j=0;j<num_state;++j)
    {
      symbol = g->set2symbols[i][j];
      if (symbol == -1 || is_terminal(g,symbol)) // no transition
        continue;

      from = alloc_nonterm_tran_node(d,i,j);
      for (int k=0;k<num_state;++k) 
      {
        symbol = g->set2symbols[j][k];
        if (symbol == -1 || is_terminal(g,symbol)) // no transition
          continue;
        if (is_nullable(g,symbol))
        {
          to = alloc_nonterm_tran_node(d,j,k);
          DIGRAPH_SET_RELATION(d, from, to);
          ++d->m_edges;
          // relation reads
        }
      }
    }
  }
  printf("m_dimension=%d, m_edges=%d\n",d->m_dimension,d->m_edges);
  /* m_dimension=2757, m_edges=14525 */
  return d->m_dimension;
}

// 2564 union
// 5323 operation(-> and |=)

int compute_include_relation(digraph_struct * d, grammar * g)
{


}

/* A nullable nonterminal is one that produces null */

