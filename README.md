C-- Compiler
=========

> This is a project created for Compiler-Principles course.

C-- compiler is a compiler based on the "C--" language.
It supports partial C89 language features .
It will leave the MIPS instruction file in current directory after compiling your test file.

## C-- language features
The "C--" language, which is cut from the C89 language, support the features as follow:

1.Types of variable: 
- interger, single-precision floating point
- array and structure

2.Basic syntax :
- if ( expression ) {statement}
- if ( expression ) {statement} else {statement}
- while ( expression ) {statment}

Attention! The "C--" grammer doesn't support pointer!

Please install the flex and bison before you try to run the compiler.
## Build 
    Run "make" to build 

## Run the "C--" compiler
    ./build/bin/parser \<your-test-filename\>

## Any advise would be appreciated :)
