char rcsid_list[] = "$Id$";

#include "b.h"

IntList
newIntList(int x, IntList next)
{
	IntList l;

	l = (IntList) zalloc(sizeof(*l));
	assert(l);
	l->x = x;
	l->next = next;

	return l;
}

List
newList(void *x, List next)
{
	List l;

	l = (List) zalloc(sizeof(*l));
	assert(l);
	l->x = x;
	l->next = next;

	return l;
}

List
appendList(void *x, List l)
{
	List last;
	List p;

	last = 0;
	for (p = l; p; p = p->next) {
		last = p;
	}
	if (last) {
		last->next = newList(x, 0);
		return l;
	} else {
		return newList(x, 0);
	}
}

void
foreachList(ListFn f, List l)
{
	for (; l; l = l->next) {
		(*f)(l->x);
	}
}

void
reveachList(ListFn f, List l)
{
	if (l) {
		reveachList(f, l->next);
		(*f)(l->x);
	}
}

int
length(List l)
{
	int c = 0;

	for(; l; l = l->next) {
		c++;
	}
	return c;
}
