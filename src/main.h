
#ifndef MAIN_H
#define MAIN_H

#include "listmap.h"

struct Decleration {
	char* str;
	int num;
};
typedef struct Decleration* Decleration;

struct Array {
	char* str;
	int len;
	List nums;
};
typedef struct Array* Array;

struct Function {
	char* name;
	int argc;
	List vars;
	List block;
	int retptr;
	int _inline;
};
typedef struct Function* Function;

#include "expressions.h"

void var_decl(char* str, int num);
void arr_decl(char* str, int len, List nums);
void arr_push(List* arr, int num);
void func_decl(char* str, List args, List statements, int _inline);

void yyerror();

#endif
