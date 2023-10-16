# BURG

This is the source code of the paper: [BURG - fast optimal instruction selection and tree parsing](https://dl.acm.org/doi/10.1145/131080.131089).
The code is imported and be modified as:

1. Rewrite to remove old C syntaxs like old-style function definitions.
2. Use GNU Bison to generate parser files gram.tab.c and gram.tab.h
   from the YACC specification file gram.y, and include the generated
   parser files into the repository.
3. Fix several compiler warnings and errors.
4. Use CMake as build system, remove the Makefile.
5. Fix some BUGs (for details, see the log of this repository).


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
