

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "main.h"

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
