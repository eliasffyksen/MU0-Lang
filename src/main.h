
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

struct Expression;
typedef struct Expression* Expression;

struct Assignment {
	Expression target;
	Expression expression;
};

struct If {
	Expression expression;
	List block;
	List _else;
};

struct While {
	Expression expression;
	List block;
};

struct CallFunction {
	char* func_name;
	List expressions;
};

struct Identifier {
	int addr;
	char* str;
};

struct Expression {
	enum {
		EXP_NUM,
		EXP_ASSIGNMENT,
		EXP_IDENTIFIER,
		EXP_PLS,
		EXP_MNS,
		EXP_IF,
		EXP_WHILE,
		EXP_CALL_FUNCTION,
		EXP_GT,
		EXP_GE,
		EXP_AND,
		EXP_OR,
		EXP_NOT,
		EXP_PEQL,
		EXP_MEQL,
		EXP_RET,
		EXP_INLINE
	} type;

	union {
		int num;
		char* str;
		Expression e[2];
		struct Assignment assignment;
		struct Identifier identifier;
		struct If _if;
		struct While _while;
		struct CallFunction call_func;
	};
};

void var_decl(char* str, int num);
void arr_decl(char* str, int len, List nums);
void arr_push(List* arr, int num);
void func_decl(char* str, List args, List statements, int _inline);

void yyerror();

#endif
