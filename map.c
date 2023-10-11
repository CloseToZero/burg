char rcsid_map[] = "$Id$";

#include <stdio.h>
#include <string.h>
#include "b.h"
#include "fe.h"

Mapping globalMap;

static void growMapping(Mapping);
static int hash(Item_Set, int);

Mapping
newMapping(int size)
{
	Mapping m;

	m = (Mapping) zalloc(sizeof(struct mapping));
	assert(m);

	m->count = 0;
	m->hash = (List*) zalloc(size * sizeof(List));
	m->hash_size = size;
	m->max_size = STATES_INCR;
	m->set = (Item_Set*) zalloc(m->max_size * sizeof(Item_Set));
	assert(m->set);

	return m;
}

static void
growMapping(Mapping m)
{
	Item_Set *tmp;

	m->max_size += STATES_INCR;
	tmp = (Item_Set*) zalloc(m->max_size * sizeof(Item_Set));
	memcpy(tmp, m->set, m->count * sizeof(Item_Set));
	zfree(m->set);
	m->set = tmp;
}

static int
hash(Item_Set ts, int mod)
{
	Item *p = ts->virgin;
	int v;
	Relevant r = ts->relevant;
	int nt;

	if (!ts->op) {
		return 0;
	}

	v = 0;
	for (; (nt = *r) != 0; r++) {
		v ^= ((int)((size_t)p[nt].rule)) + (PRINCIPLECOST(p[nt].delta)<<4);
	}
	v >>= 4;
	v &= (mod-1);
	return v;
}

Item_Set
encode(Mapping m, Item_Set ts, int *new)
{
	int h;
	List l;

	assert(m);
	assert(ts);
	assert(m->count <= m->max_size);

	if (grammarNts && errorState && m == globalMap) {
		List l;
		int found;

		found = 0;
		for (l = grammarNts; l; l = l->next) {
			Symbol s;
			s = (Symbol) l->x;

			if (ts->virgin[s->u.nt->num].rule) {
				found = 1;
				break;
			}
		}
		if (!found) {
			*new = 0;
			return errorState;
		}
	}

	*new = 0;
	h = hash(ts, m->hash_size);
	for (l = m->hash[h]; l; l = l->next) {
		Item_Set s = (Item_Set) l->x;
		if (ts->op == s->op && equivSet(ts, s)) {
			ts->num = s->num;
			return s;
		}
	}
	if (m->count >= m->max_size) {
		growMapping(m);
	}
	assert(m->count < m->max_size);
	m->set[m->count] = ts;
	ts->num = m->count++;
	*new = 1;
	m->hash[h] = newList(ts, m->hash[h]);
	return ts;
}

Item_Set
decode(Mapping m, ItemSetNum t)
{
	assert(m);
	assert(t);
	assert(m->count < m->max_size);
	assert(t < m->count);

	return m->set[t];
}

void
dumpMapping(Mapping m)
{
	int i;

	printf("BEGIN Mapping: Size=%d\n", m->count);
	for (i = 0; i < m->count; i++) {
		dumpItem_Set(m->set[i]);
	}
	printf("END Mapping\n");
}
