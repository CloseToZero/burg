# BURG

This is the source code of the paper: [BURG - fast optimal instruction selection and tree parsing](https://dl.acm.org/doi/10.1145/131080.131089).
The code is imported and be modified as:

1. Rewrite to remove old C syntaxs like old-style function definitions.
2. Use GNU Bison to generate parser files gram.tab.c and gram.tab.h
   from the YACC specification file gram.y, and include the generated
   parser files into the repository.
3. Fix several compiler warnings, errors and unnecessary ways of
   writing code.
4. Use CMake as build system, remove the Makefile.


See README_ORIGINAL for the original README file, checkout the first
commit of this repository if you want the original source code.

# How to build?

1. Install [CMake](https://cmake.org/)
2. Create a directory `build` in the project's root directory
3. Open a terminal and cd to the `build` directory we just created
4. Execute the command `cmake -S .. -B .`
5. Build the executable by command `cmake --build .` (or open the
   `burg.sln` solution file inside the `build` directory and use
   Visual Studio to build if you are on Windows)

# Why the instruction selector generated from `sample.gr` can't be compiled?

`sample.gr` use `burm_string`, `burm_op_label` and `burm_arity` etc to
produce diagnostics, these `burm_xx` variables only get generated when you
pass `-I` option to the instruction selector generator.

# How does BURG work? How to understand the source code?

The most useful papers that helped me understand the code are the
following (I will talk about the details after the list):

1. [Code Generation Using Tree Matching and Dynamic Programming](https://dl.acm.org/doi/10.1145/69558.75700)
2. [Engineering a simple, efficient code-generator generator](https://dl.acm.org/doi/abs/10.1145/151640.151642)
3. [Simple and efficient BURS table generation](https://dl.acm.org/doi/10.1145/143095.143145)
4. [Efficient retargetable code generation using bottom-up tree pattern matching](https://dl.acm.org/doi/10.1016/0096-0551%2890%2990006-B)
5. [Optimal code generation for expression trees: an application BURS theory](https://dl.acm.org/doi/10.1145/73560.73586)


[Code Generation Using Tree Matching and Dynamic Programming](https://dl.acm.org/doi/10.1145/69558.75700)
talks about TWIG, just like BURG, TWIG is an instruction selector generator.
[Engineering a simple, efficient code-generator generator](https://dl.acm.org/doi/abs/10.1145/151640.151642)
talks about IBURG, also an instruction selector generator.
Both TWIG, IBURG and BURG use the same dynamic programming algorithm
to find a minimum-cost cover (that's why read one can help you to understand the others),
but their implementations are very different.
IBURG is the most simple and easy one to understand, but it doesn't talk much about
the used dynamic programming algorithm, so I recommend you read
[Code Generation Using Tree Matching and Dynamic Programming](https://dl.acm.org/doi/10.1145/69558.75700) **first**.
Unlike IBURG which generates a hard-coded instruction selector, TWIG
assume or encode the input IR trees in prefix form, use automaton to
fast match tree patterns in parallel, use bit-strings/vectors to
verify the subpattern matches. Note that TWIG assumes those operators
with the same name in both input and output IR have same arity, for
eample, if both input and output IR have the operator PLUS, and one of
the PLUS operator has arity 2, the other must also have arity 2, this
is because instead of using counts to count the number of successful
subpattern matches, TWIG use only the first bit of a bit-string to
record whether the current node get matched or not, and the first bit
is set by bit-and operations, so you only know all subpatterns get
matched, but you don't know how many subpatterns get matched, that's
why TWIG need to assume that operators with the same name in both
input and output IR must have same arity. You may ask: why not simply
use counts to count the number of subpattern matches? TWIG do this for
its flexible extensions to the basic algorithm (various modes, dynamic
cost, replace a node by calling user supplied function and trigger
again the matching process), if use counts, then more information need
to be updated during the matching, which is less efficient, if you drop
some extensions and use counts, then the requirement of same arity can
be dropped.

The last three papers are all about BURS (stands for bottom-up rewrite
system, also note that BURG stands for BURS generator, so the
underlying system is actually BURS, not BURG), BURS use automaton to
simulate the bottom up dynamic programming algorithm that is used to find
a minimum-cost cover. Starts from the leaves of the input IR tree,
ask ourselves: what rules (or patterns, the right hand sides of rules)
can match those leaves? The answer to this question is those rules
whose right hand side are one of those leaves, we partition the rules
by the right hand side, so the rules in a partation will both match
the same leave, like `{lhs1 -> leave1, lhs2 -> leave1 }, { lhs3 -> leave2 }`.
Each partation can be seen as a state, it encode the information about the matches
(what non-terminals can match what leaves).
Here `{lhs1 -> leave1, lhs2 -> leave1 }` is a state, `{ lhs3 -> leave2 }` is also a state.
Because we also need costs information to find a minimum-cost cover,
so the state should also encode information about costs,
this means that the above states should be modified to be
`{(lhs1 -> leave1, cost1), (lhs2 -> leave1, cost2) }, { (lhs3 -> leave2, cost3) }`,
we call these states: leaf states, and call the corresponding left hand sides: leaf non-terminals,
After matching leaves, we can climb one step up the input IR tree,
start matching those interior nodes whose children are all leaves,
use the information encoded in the leaf states, again, we will get
new states like
`{(lhs4 -> op1(leave-non-terminal1, leave-non-terminal2), cost4), (lhs5 -> op1(leave-non-terminal3, leave-non-terminal4), cost5) }, { (lhs6 -> op2(leave-non-terminal1, leave-non-terminal2), cost6) }`,
here `op1` and `op2` are operators whose children are all leaf non-terminals.
We can continue this process to find a bunch of new states, but we quickly realized a problem:
as the input IR tree we received get larger and larger, there
generally will have infinite possible costs, so the possible states are also infinite,
we can't represent infinite states (unless they have common patterns), so rather
than store `(rule, cost)` pairs inside a state, we store `(rule, normalized_cost)`,
the `normalized_cost` is defined to be the cost minus the minimum cost in the same state,
in practice, most machine's specification will generate finite states if use normalized costs,
but this is not guaranteed, the number of states may still be infinite, and our BURG
may run forever until out of memory, see
[BURG - fast optimal instruction selection and tree parsing](https://dl.acm.org/doi/10.1145/131080.131089)
figure 3 for an example, in this case, you can fallback to use IBURG.

In the above, I had given a brief overview of how BURS work, now we are
going to talk about the last three papers.

[Simple and efficient BURS table generation](https://dl.acm.org/doi/10.1145/143095.143145)
explains how table generation work in BURG and various optimizations,
it's pretty much the same as the repository's code. The table generation
is the core part of this repository, so if you understand the table generation,
then you already understand most of the repository's code.
The following table is the mapping between function names in repository's code and
[Simple and efficient BURS table generation](https://dl.acm.org/doi/10.1145/143095.143145):

| function name in the paper | function name in the repository |
|----------------------------|---------------------------------|
| Main                       | main                            |
| NormalizeCosts             | zero                            |
| Closure                    | closure                         |
| ComputeLeafStates          | doLeaf                          |
| Project                    | restrict_                       |
| Triangle                   | siblings                        |
| Trim                       | trim                            |
| ComputeTransitions         | build and addToTable            |


[Efficient retargetable code generation using bottom-up tree pattern matching](https://dl.acm.org/doi/10.1016/0096-0551%2890%2990006-B)
gives more details about the underlying algorithms of the repository's code
and also give a formalization of the problem.

[Optimal code generation for expression trees: an application BURS theory](https://dl.acm.org/doi/10.1145/73560.73586) gives the underlying theory of BURS.

Additionally, I found Gabriel Hjort Blindell's dissertation and book particularly helpful:

1. Book: [Instruction Selection: Principles, Methods, and Applications](https://www.amazon.sg/Instruction-Selection-Principles-Methods-Applications/dp/3319816586)
   gives a broad survey over the large body of literature on instruction selection.
2. Dissertation: [Universal Instruction Selection](http://kth.diva-portal.org/smash/record.jsf?pid=diva2%3A1185339&dswid=-2845),
   just like the above book, this dissertation also gives a broad
   survey over the large body of literature on instruction selection,
   and more importantly, introduces a new approach called universal
   instruction selection that integrates global instruction selection
   with global code motion and block ordering.
