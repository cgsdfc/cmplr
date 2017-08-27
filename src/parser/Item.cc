#include "item.h"

int parser_builder_init(parser_builder *pb, grammar *gr) {
  memset(pb, 0, sizeof(parser_builder));
  pb->gr = pb;
  return 0;
}

ParserBuilder::ParserBuilder(const Grammar &grammar, const ParserTable &table)
    : grammar(grammar), table(table) {}

int ParserBuilder::allocItem(const Item &item) {
  int itemID = itemSets.size();
  itemSets.emplace char *parser_builder_format_item(parser_builder * pb,
                                                    int item_id) {
    static char buf[1024];
    grammar *gr = pb->gr;
    item *it = pb->items + item_id;
    int ruleid = it->ruleid;
    rule *r = gr->rules + ruleid;
    int dot = it->dot;

    sprintf(buf, "%s := ", gr->symbol[r->lhs]);
    for (int i = 0; i < r->len; ++i) {
      if (i == dot) {
        strcat(buf, " ^");
      }
      strcat(buf, " ");
      strcat(buf, gr->symbol[r->rhs[i]]);
    }
    if (dot == r->len)
      strcat(buf, " ^");
    return buf;
  }

  int parser_builder_item_get_symbol(parser_builder * pb, int item_id) {
    item *it = pb->items + item_id;
    rule *r = pb->gr->rules + it->ruleid;
    return r->len == it->dot ? -1 : r->rhs[it->dot];
  }

  void item_init(item * itm, int dot, int ruleid) {
    itm->dot = dot;
    itm->ruleid = ruleid;
  }

  int parser_builder_item2id(parser_builder * pb, item * pitem) {
    int item_id, item_set_id;
    switch (parser_builder_lookup_item(pb, pitem, &item_id)) {
    case 0:
      return item_id;
    case 1:
      return parser_builder_alloc_item(pb, pitem);
    }
  }

  int parser_builder_lookup_item(parser_builder * pb, item * it, int *item_id) {
    item *pitem;
    for (int i = 0; i < pb->item_id; ++i) {
      pitem = pb->items + i;
      if (parser_builder_item_is_same(pitem, it)) {
        *item_id = i;
        return 0;
      }
    }

    return 1;
  }

  item_set *parser_builder_get_item_set(parser_builder * pb, int item_set_id) {
    if (0 <= item_set_id && item_set_id < pb->item_set_id) {
      return pb->item_sets + item_set_id;
    }
    return NULL;
  }

  int parser_builder_alloc_item(parser_builder * pb, item * it) {
    memcpy(pb->items + pb->item_id, it, sizeof(item));
    return pb->item_id++;
  }

  void item_set_init(item_set * set) { memset(set, 0, sizeof(item_set)); }

  void item_set_append(item_set * set, int item_id) {
    set->items[set->len++] = item_id;
  }

  int parser_builder_item_set2id(parser_builder * pb, item_set * set) {
    int item_set_id;
    // like item2id, but search for set
    switch (parser_builder_lookup_item_set(pb, set, &item_set_id)) {
    case 0:
      return item_set_id;
    case 1:
      return parser_builder_alloc_item_set(pb, set);
    }
  }

  int lookup_item_set(parser_builder * pb, item_set * set, int *item_set_id) {
    item_set *pset;
    for (int i = 0; i < pb->item_set_id; ++i) {
      pset = pb->item_sets + i;
      if (memcmp(pset, set, sizeof(item_set)) == 0) {
        *item_set_id = i;
        return 0;
      }
    }
    return 1;
  }

  void parser_builder_print_item_sets(parser_builder * pb, item_set * set) {
    for (int i = 0; i < set->len; ++i) {
      puts(parser_builder_format_item(pb, set->items[i]));
    }
  }

  void parser_builder_print_item_sets(parser_builder * pb) {
    for (int i = 0; i < pb->item_set_id; ++i) {
      printf("I[%d]\n", i);
      item_set_print(pb, pb->item_sets + i);
      printf("\n");
    }
  }

  void print_core(parser_builder * pb) {
    int symbol;
    char *rep;
    int nsymbol;

    nsymbol = pb->symbol_blk[N_SYMBOLS_IDX];

    for (int i = 0; i < nsymbol; ++i) {
      printf("\t%s", pb->symbol[i]);
    }
    puts("");
    for (int j = 0; j < pb->item_set_id; ++j) {
      printf("I[%d]", j);
      for (int i = 0; i < nsymbol; ++i) {
        symbol = pb->set2symbols[j][i];
        printf("\t%3d", symbol);
      }
      puts("");
    }
  }

  int int_cmp(const void *int0, const void *int1) {
    int i0 = *(int *)int0;
    int i1 = *(int *)int1;
    return i0 - i1;
  }

  int parser_builder_do_closure(parser_builder * pb, item_set * set) {
    bool accounted[1024];
    memset(accounted, 0, sizeof accounted);
    item temp;
    int iid;
    int symbol;
    int nrule;
    int *rules;

    for (int i = 0; i < set->len; ++i) {
      // get the symbol after dot
      symbol = item_get_symbol(pb, set->items[i]);
      switch (symbol) {
      case -1:
        // no symbol after dot
        continue;
      default:
        if (accounted[symbol])
          continue;
        nrule = pb->nnont_rule[symbol];
        rules = pb->nonterm[symbol];

        for (int j = 0; j < nrule; ++j) {
          item_init(&temp, 0, rules[j]);
          iid = item2id(pb, &temp);
          item_set_append(set, iid);
        }
        accounted[symbol] = true;
        break;
      }
    }
    qsort(set->items, set->len, sizeof(int), int_cmp);
    return 0;
  }

  int parser_builder_alloc_item_set(parser_builder * pb, item_set * set) {
    item_set *pset = pb->item_sets + pb->item_set_id;
    memcpy(pset, set, sizeof(item_set));
    return pb->item_set_id++;
  }

  item *parser_builder_get_item(parser_builder * pb, int item_id) {
    if (0 <= item_id && item_id < pb->item_id) {
      return pb->items + item_id;
    }
    return NULL;
  }

  bool parser_builder_item_is_same(item * s, item * t) {
    return (s->ruleid == t->ruleid && s->dot == t->dot);
  }

  int item_try_move(parser_builder * pb, int symbol, int item_id, item *temp) {
    item *old_item;
    int dot;
    int ruleid;
    rule *r;
    int iid;

    old_item = pb->items + item_id;
    r = pb->gr->rules + old_item->ruleid;
    if (r->len == old_item->dot)
      // reach end of rule
      return 0;
    if (r->rhs[old_item->dot] != symbol)
      // symbol after dot is not `symbol`
      return 0;

    item_init(temp, old_item->dot + 1, old_item->ruleid);
    return 1;
  }

  int build_item_set(parser_builder * pb, parse_table * table) {
    int queue[1024];
    bool visited[1024];
    int head = 0;
    int tail = 0;
    int nsymbol;
    int cur_set;
    int next_set;
    int item_id;
    item temp;
    item_set temp_set, *set;

    memset(visited, 0, sizeof visited);
    memset(queue, 0, sizeof queue);
    nsymbol = pb->gr->symbol_blk[N_SYMBOLS_IDX];

    // make the first item_set
    item_init(&temp, 0, 0);
    item_set_init(&temp_set);
    item_set_append(&temp_set, alloc_item(pb, &temp));
    parser_builder_do_closure(pb, &temp_set);
    cur_set = parser_builder_item_set2id(pb, &temp_set);
    queue[tail++] = cur_set;
    visited[cur_set] = true;

    while (head != tail) {
      cur_set = queue[head++];
      visited[cur_set] = true;
      set = pb->item_sets + cur_set;

      for (int symbol = 0; symbol < nsymbol; ++symbol) {
        item_set_init(&temp_set);
        for (int i = 0; i < set->len; ++i) {
          switch (item_try_move(pb, symbol, set->items[i], &temp)) {
          case 0:
            continue;
          case 1:
            item_id = parser_builder_item2id(pb, &temp);
            item_set_append(&temp_set, item_id);
            break;
          }
        }
        if (!temp_set.len) {
          continue;
        }

        parser_builder_do_closure(pb, &temp_set);
        next_set = parser_builder_item_set2id(pb, &temp_set);
        parse_table_add_edge(pb, cur_set, next_set, symbol);
        if (visited[next_set]) {
          continue;
        }
        queue[tail++] = next_set;
        visited[next_set] = true;
      }
    }
    return 0;
  }
