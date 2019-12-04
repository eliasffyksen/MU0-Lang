
# MU0-Lang

This project is a language designed for the (MU0 architecture)[http://www.cs.man.ac.uk/~pjj/cs1001/arch/node1.html], as well as a compiler for that architecture, written in C, with the help of flex and bison. By Elias F. Fyksen

The language is largely based on C, with some querks. The language is specified below.

## Complier flow

The compiler starts in `main()` at the bottom of the `src/main.c` file, and executes the following steps:

1. Parse `stdin` using bison specified in the `src/bison.y` file
  1. Use lexical analyzer flex specified in the `src/flex.l` file
  2. Parse functions and global variable declerations, using conversion functions in `src/expressions.c`
    1. Global variable declerations gets added to the `List declerations` in `src/main.c`
    2. Functions gets paresed into abstract syntax trees which are linked together by `Expression`'s and added to the `List functions` in `src/main.c`
2. The assembly of the functions gets constructed and outputted to `stdout`, in the functions in `src/asm.c`
3. The assembly for the global variable declerations gets made, and outputted to `stdout`
4. Main will also output some constants that are necessary for stacks and returns in MU0

## The language
Some small things to note:
* `number` is numbers in decimal or hex denoted by `0x`, f.ex `0xff` or `0xFF`
* `identifier` is strings that name functions and variables
* `*number` is a reference to a physical memmory address
* `terminal` can be all of thee above or `(` `expression` `)`
* `asignee` can be `identifier` or `*number`

### Global variable declerations
```
identifier = number;
```

### Function defenitions
```
identifer ( arg1 , arg2 , ... , argn ) {
  statement_1
  statement_2
  ...
  statement_n
}
```

### Statement

A statement can be any of thee below:

```
expression ;
```
```
if expression {
  statement_1
  statement_2
  ...
  statement_n
} else {
  statement_1
  statement_2
  ...
  statement_n
}
```
NB: else is optional
```
while expression {
  statement_1
  statement_2
  ...
  statement_n
}
```
```
return expression ;
```
NB: return-expression is optional


### Expressions
Expressions can be any of thee below:
* `asignee = expression`
* `asignee += expression`
* `asignee -= expression`
* `! expression`
* `expression opperator terminal`
* `terminal`

NB: all opperations have the same precidence, and so if you want to write `expression && expression` you should write `expression && (expression)`

### Operators
Operators can be any of: `+, -, >, <, >=, <=, && or ||`
