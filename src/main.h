
#ifndef MAIN_H
#define MAIN_H

struct Function;
typedef struct Function* Function;

struct Decleration;
typedef struct Decleration* Decleration;

#include "listmap.h"
#include "expressions.h"
#include "asm.h"

List declerations;
List functions;
List numbers;
unsigned int local_labels;

struct Decleration {
	char* str;
	int num;
};

struct Function {
	char* name;
	int argc;
	List vars;
	List block;
	int retptr;
	int _inline;
};

void var_decl(char* str, int num);
void func_decl(char* str, List args, List statements, int _inline);

void yyerror();

#endif
