C-- Compiler
---

> This project was built for Compiler-Principles course.

The C-- compiler is based on the "C--" language, supports partial C89 language features and allow user not only declare a structure but also define and call a recursive function.  

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
- Flex 2.6.0
- Bison 3.0.4
- QtSpim 9.1.20
## Build 
```shell
 git clone https://github.com/jimleungjing/Compiler-Principles-Lab
 cd Compiler-Principles-Lab
 make
```

## Run 
```shell
./build/bin/parser <your-test-filename>
```
    
## Test Assembly file
- Run the "C--" comiler, and it will leave a assembly file named "code.s" in the curreny directory.
- Open the QtSpim, and load the "code.s" file.
- Just run and enjoy with my mini-complier!

BTW, any advise would be appreciated :)
