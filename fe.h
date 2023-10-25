/* $Id$ */

#include <stdio.h>

struct binding {
	char	*name;
	int	opnum;
};
typedef struct binding	*Binding;

struct arity {
	int	arity;
	List	bindings;
};
typedef struct arity	*Arity;

struct patternAST {
	struct symbol *sym;
	char	*op;
	List	children;
};
typedef struct patternAST	*PatternAST;

struct ruleAST {
	char			*lhs;
	PatternAST		pat;
	int			erulenum;
	IntList			cost;
	struct rule		*rule;
	struct strTableElement	*kids;
	struct strTableElement	*nts;
};
typedef struct ruleAST	*RuleAST;

typedef enum {
	UNKNOWN,
	OPERATOR,
	NONTERMINAL
} TagType;

struct symbol {
	char	*name;
	TagType	tag;
	union {
		NonTerminal	nt;
		Operator	op;
	} u;
};
typedef struct symbol	*Symbol;

struct strTableElement {
	char *str;
	IntList erulenos;
	char *ename;
};
typedef struct strTableElement	*StrTableElement;

struct strTable {
	List elems;
};
typedef struct strTable	*StrTable;

extern StrTable newStrTable(void);
extern StrTableElement addString(StrTable, char *, int, int *);

extern void doSpec(List, List);
extern Arity newArity(int, List);
extern Binding newBinding(char *, int);
extern PatternAST newPatternAST(char *, List);
extern RuleAST newRuleAST(char *, PatternAST, int, IntList);
extern Symbol enter(char *, int *);
extern Symbol newSymbol(char *);

extern void makeDebug(void);
extern void makeSimple(void);
extern void makePlanks(void);
extern void makeOpLabel(void);
extern void makeChild(void);
extern void makeOperators(void);
extern void makeLabel(void);
extern void makeString(void);
extern void makeString(void);
extern void makeReduce(void);
extern void makeRuleTable(void);
extern void makeTables(void);
extern void makeTreecost(void);
extern void makePrint(void);
extern void makeRule(void);
extern void makeNts(void);
extern void makeKids(void);
extern void startBurm(void);
extern void startOptional(void);
extern void makePlankLabel(void);
extern void makeStateLabel(void);
extern void makeStringArray(void);
extern void makeNonterminalArray(void);
extern void makeCostArray(void);
extern void makeLHSmap(void);
extern void makeClosureArray(void);
extern void makeOperatorVector(void);
extern void endOptional(void);
extern void reportDiagnostics(void);
extern void makeNonterminals(void);
extern int opsOfArity(int);

extern void yypurge(void);
extern void yyfinished(void);

extern void printRepresentative(FILE *, Item_Set);

extern void dumpRules(List);
extern void dumpDecls(List);
extern void dumpRuleAST(RuleAST);
extern void dumpPatternAST(PatternAST);
extern void dumpArity(Arity);
extern void dumpBinding(Binding);
extern void dumpStrTable(StrTable);

extern int yylex(void);
extern int yyparse(void);

extern int	max_ruleAST;
extern List	ruleASTs;

extern FILE	*outfile;
extern char	*prefix;
extern int 	trimflag;
