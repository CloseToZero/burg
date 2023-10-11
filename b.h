/* $Id$ */

#define MAX_ARITY	2

#include <stdio.h>
#include <stdlib.h>

typedef int ItemSetNum;
typedef int OperatorNum;
typedef int NonTerminalNum;
typedef int RuleNum;
typedef int ArityNum;
typedef int ERuleNum;

extern NonTerminalNum	last_user_nonterminal;
extern NonTerminalNum	max_nonterminal;
extern RuleNum		max_rule;
extern ERuleNum		max_erule_num;
extern int		max_arity;

#ifndef NOLEX
#define DELTAWIDTH	4
typedef short DeltaCost[DELTAWIDTH];
typedef short *DeltaPtr;
extern void ASSIGNCOST(DeltaPtr, DeltaPtr);
extern void ADDCOST(DeltaPtr, DeltaPtr);
extern void MINUSCOST(DeltaPtr, DeltaPtr);
extern void ZEROCOST(DeltaPtr);
extern int LESSCOST(DeltaPtr, DeltaPtr);
extern int EQUALCOST(DeltaPtr, DeltaPtr);
#define PRINCIPLECOST(x)	(x[0])
#else
#define DELTAWIDTH	1
typedef int DeltaCost;
typedef int DeltaPtr;
#define ASSIGNCOST(l, r)	((l) = (r))
#define ADDCOST(l, r)		((l) += (r))
#define MINUSCOST(l, r)		((l) -= (r))
#define ZEROCOST(x)		((x) = 0)
#define LESSCOST(l, r)		((l) < (r))
#define EQUALCOST(l, r)		((l) == (r))
#define PRINCIPLECOST(x)	(x)
#endif /* NOLEX */
#define NODIVERGE(c,state,nt,base)		if (prevent_divergence > 0) CHECKDIVERGE(c,state,nt,base);

struct list {
	void		*x;
	struct list	*next;
};
typedef struct list	*List;

struct intlist {
	int		x;
	struct intlist	*next;
};
typedef struct intlist	*IntList;

struct operator {
	char		*name;
	unsigned int	ref:1;
	OperatorNum	num;
	ItemSetNum	baseNum;
	ItemSetNum	stateCount;
	ArityNum	arity;
	struct table	*table;
};
typedef struct operator	*Operator;

struct nonterminal {
	char		*name;
	NonTerminalNum	num;
	ItemSetNum	baseNum;
	ItemSetNum	ruleCount;
	struct plankMap *pmap;

	struct rule	*sampleRule; /* diagnostic---gives "a" rule that with this lhs */
};
typedef struct nonterminal	*NonTerminal;

struct pattern {
	NonTerminal	normalizer;
	Operator	op;		/* NULL if NonTerm -> NonTerm */
	NonTerminal	children[MAX_ARITY];
};
typedef struct pattern	*Pattern;

struct rule {
	DeltaCost	delta;
	ERuleNum	erulenum;
	RuleNum		num;
	RuleNum		newNum;
	NonTerminal	lhs;
	Pattern		pat;
	unsigned int	used:1;
};
typedef struct rule	*Rule;

struct item {
	DeltaCost	delta;
	Rule		rule;
};
typedef struct item	Item;

typedef short 	*Relevant;	/* relevant non-terminals */

typedef Item	*ItemArray;

struct item_set {	/* indexed by NonTerminal */
	ItemSetNum	num;
	ItemSetNum	newNum;
	Operator	op;
	struct item_set *kids[2];
	struct item_set *representative;
	Relevant	relevant;
	ItemArray	virgin;
	ItemArray	closed;
};
typedef struct item_set	*Item_Set;

#define DIM_MAP_SIZE	(1 << 8)
#define GLOBAL_MAP_SIZE	(1 << 15)

struct mapping {	/* should be a hash table for TS -> int */
	List		*hash;
	int		hash_size;
	int		max_size;
	ItemSetNum	count;
	Item_Set	*set;	/* map: int <-> Item_Set */
};
typedef struct mapping	*Mapping;

struct index_map {
	ItemSetNum	max_size;
	Item_Set	*class;
};
typedef struct index_map	Index_Map;

struct dimension {
	Relevant	relevant;
	Index_Map	index_map;
	Mapping		map;
	ItemSetNum	max_size;
	struct plankMap *pmap;
};
typedef struct dimension	*Dimension;


struct table {
	Operator	op;
	List		rules;
	Relevant	relevant;
	Dimension	dimen[MAX_ARITY];	/* 1 for each dimension */
	Item_Set	*transition;	/* maps local indices to global
						itemsets */
};
typedef struct table	*Table;

struct relation {
	Rule	rule;
	DeltaCost	chain;
	NonTerminalNum	nextchain;
	DeltaCost	sibling;
	int		sibFlag;
	int		sibComputed;
};
typedef struct relation	*Relation;

struct queue {
	List head;
	List tail;
};
typedef struct queue	*Queue;

struct plank {
	char *name;
	List fields;
	int width;
};
typedef struct plank	*Plank;

struct except {
	short index;
	short value;
};
typedef struct except	*Exception;

struct plankMap {
	List exceptions;
	int offset;
	struct stateMap *values;
};
typedef struct plankMap	*PlankMap;

struct stateMap {
	char *fieldname;
	Plank plank;
	int width;
	short *value;
};
typedef struct stateMap	*StateMap;

struct stateMapTable {
	List maps;
};

extern void CHECKDIVERGE(DeltaPtr, Item_Set, int, int);
extern void zero(Item_Set);
extern ItemArray newItemArray(void);
extern ItemArray itemArrayCopy(ItemArray);
extern Item_Set newItem_Set(Relevant);
extern void freeItem_Set(Item_Set);
extern Mapping newMapping(int);
extern NonTerminal newNonTerminal(char *);
extern int nonTerminalName(char *, int);
extern Operator newOperator(char *, OperatorNum, ArityNum);
extern Pattern newPattern(Operator);
extern Rule newRule(DeltaPtr, ERuleNum, NonTerminal, Pattern);
extern List newList(void *, List);
extern IntList newIntList(int, IntList);
extern int length(List);
extern List appendList(void *, List);
extern Table newTable(Operator);
extern Queue newQ(void);
extern void addQ(Queue, Item_Set);
extern Item_Set popQ(Queue);
extern int equivSet(Item_Set, Item_Set);
extern Item_Set decode(Mapping, ItemSetNum);
extern Item_Set encode(Mapping, Item_Set, int *);
extern void build(void);
extern Item_Set *transLval(Table, int, int);

typedef void *	(*ListFn)(void *);
extern void foreachList(ListFn, List);
extern void reveachList(ListFn, List);

extern void addToTable(Table, Item_Set);

extern void closure(Item_Set);
extern void trim(Item_Set);
extern void findChainRules(void);
extern void findAllPairs(void);
extern void addRelevant(Relevant, NonTerminalNum);

extern void *zalloc(size_t);
extern void zfree(void *);

extern NonTerminal	start;
extern List		rules;
extern List		chainrules;
extern List		operators;
extern List		leaves;
extern List		nonterminals;
extern List		grammarNts;
extern Queue		globalQ;
extern Mapping		globalMap;
extern int		exceptionTolerance;
extern int		prevent_divergence;
extern int		principleCost;
extern int		lexical;
extern struct rule	stub_rule;
extern Relation 	*allpairs;
extern Item_Set		*sortedStates;
extern Item_Set		errorState;

extern void dumpRelevant(Relevant);
extern void dumpOperator(Operator, int);
extern void dumpOperator_s(Operator);
extern void dumpOperator_l(Operator);
extern void dumpNonTerminal(NonTerminal);
extern void dumpRule(Rule);
extern void dumpRuleList(List);
extern void dumpItem(Item *);
extern void dumpItem_Set(Item_Set);
extern void dumpMapping(Mapping);
extern void dumpQ(Queue);
extern void dumpIndex_Map(Index_Map *);
extern void dumpDimension(Dimension);
extern void dumpPattern(Pattern);
extern void dumpTable(Table, int);
extern void dumpTransition(Table);
extern void dumpCost(DeltaCost);
extern void dumpAllPairs(void);
extern void dumpRelation(Relation);
extern void dumpSortedStates(void);
extern void dumpSortedRules(void);
extern int debugTrim;

#ifdef DEBUG
#define debug(a,b)	if (a) b
#else
#define debug(a,b)
#endif
extern int debugTables;

#define TABLE_INCR	8
#define STATES_INCR	64

#ifdef NDEBUG
#define assert(c) ((void) 0)
#else
#define assert(c) ((void) ((c) || fatal(__FILE__,__LINE__)))
#endif

extern void doStart(char *);
extern int fatal(char *, int);
extern void yyerror(char *);
extern void yyerror1(char *);

extern void makeRuleDescArray();
extern void makeDeltaCostArray();
extern void makeStateStringArray();
