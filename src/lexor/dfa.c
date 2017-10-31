#include "lexer/dfa.h"
#include <assert.h>
#include <ctype.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int add_state(int from, int to);

typedef struct dfa_config {
	int from;
	int to;
	int action;
	int cond;
	int usrd;
	int idcnt;
	int nrows;

} dfa_config;
enum {
	DFA_USR_HANDLER = 1,
	DFA_DEF_HANDLER,
};

static dfa_table *cur_dfa;
static dfa_config cur_config;
static dfa_table all_dfas[DFA_TAB_MAX_LEN];
static int dfa_count;

int init_dfa(dfa_table * dfa, int nrows, transfer_func func)
{
	if (!func)
		return -1;
	if (!dfa)
		return -1;
	if (nrows <= 0)
		return -1;

	int *len = malloc(sizeof(int) * nrows);
	if (!len)
		return -1;

	int *max = malloc(sizeof(int) * nrows);
	if (!max)
		return -1;

	dfa_state **states = malloc(sizeof(dfa_state *) * nrows);
	if (!states)
		return -1;

	// the rows are not alloced by now
	memset(max, 0, sizeof(int) * nrows);
	memset(len, 0, sizeof(int) * nrows);
	dfa->nrows = nrows;
	dfa->func = func;
	dfa->diagram = states;
	dfa->len = len;
	dfa->max = max;

	return 0;
}

dfa_table *alloc_dfa(int nrows, transfer_func func)
{
	int count = dfa_count;
	if (count == DFA_TAB_MAX_LEN)
		return NULL;

	cur_dfa = &all_dfas[count];
	init_dfa(cur_dfa, nrows, func);

	memset(&cur_config, 0, sizeof cur_config);
	cur_config.nrows = nrows;
	cur_config.idcnt = 1;
	/* important! 0 is reserved for *init* */

	++dfa_count;
	return cur_dfa;
}

void config_from(int from)
{
	cur_config.from = from;
}

void config_to(int to)
{
	cur_config.to = to;
}

void config_condition(int cond)
{
	cur_config.cond = cond;
}

void config_action(int action)
{
	cur_config.action = action;
}

void config_usrd(int usrd)
{
	cur_config.usrd = usrd;
}

void config_end(void)
{
	int idcnt = cur_config.idcnt;
	int nrows = cur_config.nrows;
	memset(&cur_config, 0, sizeof(dfa_config));
	cur_config.idcnt = idcnt;
	cur_config.nrows = nrows;
}

// when add_** finds that one
// row of entries is full, resize_row
// will be called to enlarge the row
// it takes in the old size `len`
// and return the len of the newly
// alloc array
static dfa_state *resize_row(dfa_state * ds, int len, int *newlen)
{
	dfa_state *newspace = NULL;
	*newlen = (len & 1) ? len << 1 : len * 3 + 5;
	// newlen = len*2 if len is odd else 3len+5
	newspace = calloc(*newlen, sizeof(dfa_state));
	if (!newspace)
		return NULL;

	// when initial len==0 no problem!
	// check if **the standard** make it sure.
	memcpy(newspace, ds, sizeof(dfa_state) * len);
	free(ds);
	return newspace;
}

int add_config(void)
{
	add_state(cur_config.from, cur_config.to);
}

static int add_state(int from, int to)
{
	int newlen = 0;
	int len = cur_dfa->len[from];
	dfa_state *ds = cur_dfa->diagram[from];
	if (len == cur_dfa->max[from]) {
		ds = resize_row(ds, len, &newlen);
		if (!ds) {
			return -1;
		}
		cur_dfa->diagram[from] = ds;
		cur_dfa->max[from] = newlen;
	}

	ds = cur_dfa->diagram[from] + len;
	ds->state = to;
	ds->cond = cur_config.cond;
	ds->action = cur_config.action;
	ds->usrd = cur_config.usrd;
	cur_dfa->len[from]++;
	return 0;
}

// after calling config_to,
// call add_from will add to,
// which was configed, to `from`'s
// entries
int add_from(int from)
{
	return add_state(from, cur_config.to);
}

// after calling config_from,
// call add_to will add `to`
// into `from`'s entries configed
// before
int add_to(int to)
{
	return add_state(cur_config.from, to);
}

int alloc_state(bool is_non_terminal)
{
	// bad practice if they overlap
	if (is_non_terminal) {
		if (cur_config.idcnt == cur_config.nrows - 1) {
			// TODO write a little logger
			printf("[FATAL] alloc_state: they overlap\n");
			exit(1);
		}
		return cur_config.idcnt++;
	}

	return cur_config.nrows++;
}

// this function look up the diagram inside
// `dfa`, to find a suitabl state to go to
// the return value depends on the usr `func`
// `func` returns 1 if can go, 0 if can not
// (even if error happened).
// this function does a little diff than the
// usr func does: if success, it returns 0,
// if fail, it return 1,
int transfer(dfa_table * dfa, int from, int cond, dfa_state ** to)
{
	int len = dfa->len[from];
	dfa_state *ds = dfa->diagram[from];

	for (int i = 0; i < len; ++i) {
		*to = (ds + i);
		switch (dfa->func(*to, cond)) {
		case 1:
			return 0;
		case 0:
			continue;
		}
	}
	return 1;
}

static void destroy_dfa(struct dfa_table *dfa)
{
	for (int i = 0; i < dfa->nrows; ++i) {
		free(dfa->diagram[i]);
	}
	free(dfa->diagram);
	free(dfa->max);
	free(dfa->len);
}

void destroy_all_dfa(void)
{
	for (int i = 0; i < dfa_count; ++i) {
		destroy_dfa(&all_dfas[i]);
	}
}
