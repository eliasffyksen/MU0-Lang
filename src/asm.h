
#ifndef ASM_H
#define ASM_H

#include "main.h"

void reg_num(int n);
void asm_declerations();
void asm_identifier_lda(Function func, struct Identifier identifier);
void asm_identifier_sta(Function func, struct Identifier identifier);
void asm_push();
void asm_pop();
void asm_expression(Function func, Expression e);
void asm_statements(Function func, List statements);
void asm_call_function(Function func, char* func_name, List args);
void asm_functions();
void asm_numbers();

#endif
