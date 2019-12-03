

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "main.h"

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
	int _inline;
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

Expression expression_num(int num);
Expression expression_assignment(Expression target, Expression value);
Expression expression_identifier(char* str, int addr);
Expression expression_pls(Expression a, Expression b);
Expression expression_mns(Expression a, Expression b);
Expression expression_if(Expression condition, List block, List _else);
Expression expression_while(Expression condition, List block);
Expression expression_call_function(char* func_name, List expressions);
Expression expression_gt(Expression a, Expression b);
Expression expression_ge(Expression a, Expression b);
Expression expression_and(Expression a, Expression b);
Expression expression_or(Expression a, Expression b);
Expression expression_not(Expression a);
Expression expression_ret(Expression a);
Expression expression_inline(char* str);
Expression expression_peql(Expression target, Expression expression);
Expression expression_meql(Expression target, Expression expression);

#endif
