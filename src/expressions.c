
#include "expressions.h"


Expression expression_num(int num){
	Expression e = malloc(sizeof(struct Expression));
	
	e->type = EXP_NUM;
	e->num = num;

	return e;
}


Expression expression_assignment(Expression target, Expression value){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_ASSIGNEMENT;
	e->assignment.target = target;
	e->assignment.expression = value;

	return e;
}

Expression expression_identifier(char* str, int addr){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_IDENTIFIER;
	e->identifier.str = str;
	e->identifier.addr = addr;

	return e;
}

Expression expression_pls(Expression a, Expression b){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_PLS;
	e->e[0] = a;
	e->e[1] = b;

	return e;
}

Expression expression_mns(Expression a, Expression b){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_MNS;
	e->e[0] = a;
	e->e[1] = b;

	return e;
}

Expression expression_if(Expression condition, List block, List _else){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_IF;
	e->_if.expression = condition;
	e->_if.block = block;
	e->_if._else = _else;

	return e;
}

Expression expression_while(Expression condition, List block){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_WHILE;
	e->_while.expression = condition;
	e->_while.block = block;

	return e;
}

Expression expression_call_function(char* func_name, List expressions){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_CALL_FUNCTION;
	e->call_func.func_name = name;
	e->call_func.expressions = expressions;

	return e;
}

Expression expression_gt(Expression a, Expression b){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_GT;
	e->e[0] = a;
	e->e[1] = b;
	
	return e;
}

Expression expression_ge(Expression a, Expression b){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_GE;
	e->e[0] = a;
	e->e[1] = b;

	return e;
}

Expression expression_and(Expression a, Expression b){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_AND;
	e->e[0] = a;
	e->e[1] = b;

	return e;
}

Expression expression_or(Expression a, Expression b){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_OR;
	e->e[0] = a;
	e->e[1] = b;

	return e;
}

Expression expression_not(Expression a){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_NOT;
	e->e[0] = a;

	return a;
}

Expression expression_ret(Expression a){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_RET;
	e->e[0] = a;

	return a;
}

Expression expression_inline(char* str){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_INLINE;
	e->str = str;

	return e;
}

Expression expression_peql(Expression target, Expression expression){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_PEQL;
	e->assignment.target = target;
	e->assignment.expression = expression;

	return e;
}

Expression expression_meql(Expression target, Expression expression){
	Expression e = malloc(sizeof(struct Expression));

	e->type = EXP_MEQL;
	e->assignment.target = target;
	e->assignment.expression = expression;

	return e;
}	

