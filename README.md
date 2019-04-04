C-- Compiler
---

> This project was built for Compiler-Principles course.

C-- compiler is a compiler based on the "C--" language.
It supports partial C89 language features .
It will leave the MIPS instruction file in current directory after compiling your test file; 

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

## Prerequisites
- flex 2.6.0
- bison 3.0.4
## Build 
'''shell
$make 
'''

## Run 
'''shell
./build/bin/parser <your-test-filename>
'''
    
## Test Assembly file
    To begin with I suggest you to install the QtSpim (a MIPS simulator).
    Next, run the "C--" comiler, it will leave a assembly file named "code.s" in the curreny directory.
    Open the QtSpim, and load the "code.s" file.
    Run and enjoy it!

## Any advise would be appreciated :)
