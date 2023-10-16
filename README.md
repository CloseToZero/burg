# BURG

This is the source code of the paper: [BURG - fast optimal instruction selection and tree parsing](https://dl.acm.org/doi/10.1145/131080.131089).
The code is imported and be modified as:

1. Rewrite to remove old C syntaxs like old-style function definitions.
2. Use GNU Bison to generate parser files gram.tab.c and gram.tab.h
   from the YACC specification file gram.y, and include the generated
   parser files into the repository.
3. Fix several compiler warnings and errors.
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

**TODO** talk about the last three papers, what BURG and BURS stands for, the
mapping between functions in our code and [Simple and efficient BURS table generation](https://dl.acm.org/doi/10.1145/143095.143145).
