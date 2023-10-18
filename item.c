char rcsid_item[] = "$Id$";

#include "b.h"
#include <stdio.h>
#include <string.h>
#include "fe.h"

static Item_Set fptr;

ItemArray
newItemArray()
{
	ItemArray ia;
	ia = (ItemArray) zalloc(max_nonterminal *sizeof(*ia));
	return ia;
}

ItemArray
itemArrayCopy(ItemArray src)
{
	ItemArray dst;

	dst = newItemArray();
	memcpy(dst, src, max_nonterminal * sizeof(*dst));
	return dst;
}

Item_Set
newItem_Set(Relevant relevant)
{
	Item_Set ts;
	
	if (fptr) {
		ts = fptr;
		fptr = 0;
		memset(ts->virgin, 0, max_nonterminal * sizeof(struct item));
		if (ts->closed) {
			zfree(ts->closed);
			ts->closed = 0;
		}
		ts->num = 0;
		ts->op = 0;
	} else {
		ts = (Item_Set) zalloc(sizeof(struct item_set));
		ts->virgin = newItemArray();
	}
	ts->relevant = relevant;
	return ts;
}

void
freeItem_Set(Item_Set ts)
{
	assert(!fptr);
	fptr = ts;
}

int
equivSet(Item_Set a, Item_Set b)
{
	Relevant r;
	int nt;
	Item *aa = a->virgin;
	Item *ba = b->virgin;

	/*
	return !bcmp(a->virgin, b->virgin, max_nonterminal * sizeof(Item));
	*/

	r = a->relevant ? a->relevant : b->relevant;
	assert(r);

	if (a->op != b->op) {
		return 0;
	}
	for (; (nt = *r) != 0; r++) {
		if (aa[nt].rule != ba[nt].rule || !EQUALCOST(aa[nt].delta, ba[nt].delta)) {
			return 0;
		}
	}
	return 1;
}

void
printRepresentative(FILE *f, Item_Set s)
{
	if (!s) {
		return;
	}
	fprintf(f, "%s", s->op->name);
	switch (s->op->arity) {
	case 1:
		fprintf(f, "(");
		printRepresentative(f, s->kids[0]);
		fprintf(f, ")");
		break;
	case 2:
		fprintf(f, "(");
		printRepresentative(f, s->kids[0]);
		fprintf(f, ", ");
		printRepresentative(f, s->kids[1]);
		fprintf(f, ")");
		break;
	}
}

void
dumpItem(Item *t)
{
	printf("[%s #%d]", t->rule->lhs->name, t->rule->num);
	dumpCost(t->delta);
}

void
dumpItem_Set(Item_Set ts)
{
	int i;

	printf("Item_Set #%d: [", ts->num);
	for (i = 1; i < max_nonterminal; i++) {
		if (ts->virgin[i].rule) {
			printf(" %d", i);
			dumpCost(ts->virgin[i].delta);
		}
	}
	printf(" ]\n");
}

void
dumpCost(DeltaCost dc)
{
#ifndef NOLEX
	printf("(");
	for (int i = 0; i < DELTAWIDTH; i++) {
		if (i != 0) {
			printf(",");
		}
		printf("%d", dc[i]);
	}
	printf(")");
#else NOLEX
	printf("(%d)", dc);
#endif
}
