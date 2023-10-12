char rcsid_pattern[] = "$Id$";

#include <stdio.h>
#include "b.h"

Pattern
newPattern(Operator op)
{
	Pattern p;

	p = (Pattern) zalloc(sizeof(struct pattern));
	p->op = op;
	return p;
}

void
dumpPattern(Pattern p)
{
	int i;

	if (!p) {
		printf("[no-pattern]");
		return;
	}

	if (p->op) {
		printf("%s", p->op->name);
		if (p->op->arity > 0) {
			printf("(");
			for (i = 0; i < p->op->arity; i++) {
				printf(i == 0 ? "%s" : ",%s", p->children[i]->name);
			}
			printf(")");
		}
	} else {
		printf("%s", p->children[0]->name);
	}
}
