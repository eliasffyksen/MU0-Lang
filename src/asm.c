
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "asm.h"

/*
 * This file contains functions that print mu0 assmebly
 * The functions are called by main() in main.c
 *
 * Prefixes:
 *  * "asm_" - Generates assembly
 *  * "get_vars" - Recursivly gets the list of local variables in a function
 *
 */

void get_vars_statements(Function func, List statements);

// Register integer n as constant in address space
void reg_num(int n) {
	
	if(n == 0){
		return;
	}

	if(n < 0){
		n = 0x10000 + n;
	}

	int len = list_len(&numbers);

	for(int i = 0; i < len; i++){
		int n2 = (int)(size_t) list_get(&numbers, i);
		if(n == n2)
			return;
	}

	list_push(&numbers, (void *)(size_t) n);
}

// Print all global integer declerations in address space
void asm_declerations() {
	int len = list_len(&declerations);

	for(int i = 0; i < len; i++){
		Decleration decl = list_get(&declerations, i);
		printf("%s\n\tDEFW &%x\n", decl->str, (unsigned short) decl->num);
	}
}

// Print assembly to load contents of accumilator from identifier
void asm_identifier_lda(Function func, struct Identifier identifier) {

	if(identifier.str == 0){
		printf("\tLDA &%x\n", identifier.addr);
		return;
	}

	int len = list_len(&func->vars);

	for(int i = 0; i < len; i++){
		char* var = list_get(&func->vars, i);
		if(strcmp(var, identifier.str) == 0){

			reg_num(++i);

			printf("\tLDA _base; load local variables\n");
			printf("\tSUB _I%d\n", i);
			printf("\tSTA _L%d\n", local_labels);
			printf("_L%d\n", local_labels++);
			printf("\tDEFW &0; dummy\n");
			return;
		}
	}

	printf("\tLDA %s\n", identifier.str); 

}

// Print assembly to store contents of accumilator to identifier 
void asm_identifier_sta(Function func, struct Identifier identifier){
	
	if(identifier.str == 0){
		printf("\tSTA &%x\n", identifier.addr);
		return;
	}

	int len = list_len(&func->vars);

	for(int i = 0; i < len; i++){
		char* var = list_get(&func->vars, i);
		if(strcmp(var, identifier.str) == 0){

			reg_num(++i);
			
			printf("\tSTA _RESVAL; store local variable\n");
			printf("\tLDA _base\n");
			printf("\tSUB _I%d\n", i);
			printf("\tADD _ISTA\n");
			printf("\tSTA _L%d\n", local_labels);
			printf("\tLDA _RESVAL\n");
			printf("_L%d\n", local_labels++);
			printf("\tDEFW &0; dummy\n");
			return;
		}
	}

	printf("\tSTA %s\n", identifier.str); 
}

// Print assembly to push content of accumilator to stack
void asm_push() {
	printf("\tSTA _RESVAL; push to stack\n");
	printf("\tLDA _stack;\n");
	printf("\tSUB _I1\n");
	printf("\tSTA _stack\n");
	printf("\tADD _ISTA\n");
	printf("\tSTA _L%d\n", local_labels);
	printf("\tLDA _RESVAL\n");
	printf("_L%d\n", local_labels++);
	printf("\tDEFW &0; dummy\n");
}

// Print assembly to pop the stack into accumilator
void asm_pop() {
	printf("\tLDA _stack; pop from stack\n");
	printf("\tSTA _L%d\n", local_labels);
	printf("\tADD _I1\n");
	printf("\tSTA _stack\n");
	printf("_L%d\n", local_labels++);
	printf("\tDEFW &0; dummy\n");
}

// Print assembly for a expression
void asm_expression(Function func, Expression e){

	switch(e->type){

		case EXP_NUM:
			printf("\tLDA _I%d; load integer\n", 
					e->num < 0 ? 0x10000 - e->num : e->num);

			break;

		case EXP_IDENTIFIER:
			asm_identifier_lda(func, e->identifier);
			break;

		case EXP_ASSIGNMENT:
			asm_expression(func, e->assignment.expression);
			asm_identifier_sta(func, e->assignment.target->identifier);
			break;

		// All the opperations +, -, > and >= have similar instruction flow
		case EXP_PLS:
		case EXP_MNS:
		case EXP_GT:
		case EXP_GE:
			asm_expression(func, e->e[0]);
			asm_push();
			asm_expression(func, e->e[1]);
			printf("\tSTA _RESVAL\n");
			asm_pop();

			if(e->type == EXP_PLS){
				printf("\tADD _RESVAL; + opperation\n");
			}else{
				printf("\tSUB _RESVAL; - opperation\n");

				if(e->type == EXP_GE){
					printf("\tJGE _L%d; GE opperation\n", local_labels);
					printf("\tLDA _I0\n");
					printf("\tJMP _L%d\n", local_labels + 1);
					printf("_L%d\n", local_labels++);
					printf("\tSTA _I1\n");
					printf("_L%d\n", local_labels++);
				}

				if(e->type == EXP_GT){
					printf("\tJGE _L%d; GT opperation\n", local_labels);
					printf("\tLDA _I0\n");
					printf("_L%d\n", local_labels++);
				}
			}

			break;

		case EXP_IF:
		{
			int label1 = local_labels++;
			int label2 = local_labels++;
		
			asm_expression(func, e->_if.expression);
		
			printf("\tJNE _L%d; if else:\n", label1);

			if(e->_if._else) {
				asm_statements(func, e->_if._else);
			}

			printf("\tJMP _L%d; jmp to if end\n", label2);

			printf("_L%d; if block:\n", label1);
		
			asm_statements(func, e->_if.block);

			printf("_L%d; if end\n", label2);

			break;
		}

		case EXP_WHILE:
		{
			asm_expression(func, e->_while.expression);

			int label1 = local_labels++;
			int label2 = local_labels++;

			printf("\tJMP _L%d; while\n", label2);
			printf("_L%d\n", label1);

			asm_statements(func, e->_while.block);

			printf("_L%d; while condition:\n", label2);

			asm_expression(func, e->_while.expression);

			printf("\tJNE _L%d; jmp to while\n", label1);

			break;
		}

		case EXP_CALL_FUNCTION:
			asm_call_function(func, e->call_func.func_name,
					e->call_func.expressions);
			break;

		case EXP_OR:
		{
			int label = local_labels++;

			asm_expression(func, e->e[0]);
			printf("\tJNE _L%d; or opperation\n", label);
			asm_expression(func, e->e[1]);
			printf("_L%d; or end\n", label);

			break;
		}

		case EXP_AND:
		{	
			int label1 = local_labels++;
			int label2 = local_labels++; 

			asm_expression(func, e->e[0]);
			printf("\tJNE _L%d; and operation\n", label1);
			printf("\tJMP _L%d\n", label2);
			printf("_L%d\n", label1);
			asm_expression(func, e->e[1]);
			printf("_L%d\n; and end", label2);

			break;
		}

		case EXP_NOT:

			asm_expression(func, e->e[0]);

			printf("\tJNE _L%d; not\n", local_labels);
			printf("\tLDA _I1\n");
			printf("\tJMP _L%d\n", local_labels + 1);
			printf("_L%d\n", local_labels++);
			printf("\tLDA _I0\n");
			printf("_L%d\n", local_labels++);

			break;

		case EXP_RET:
			
			if(e->e[0]){
				asm_expression(func, e->e[0]);
				printf("\tSTA _RESVAL\n");
			}
			
			printf("\tJMP _L%d; return\n", func->retptr);

			break;

		case EXP_INLINE:
		{
			// simulate return inside inline functions
			int oldretptr = func->retptr;
			func->retptr = local_labels++;

			int len = list_len(&functions);

			for(int i = 0; i < len; i++){
				Function inline_func = list_get(&functions, i);

				if(strcmp(e->str, inline_func->name) == 0){
					asm_statements(func, inline_func->block);
					printf("_L%d; inline return\n", func->retptr);
					break;
				}
			}

			func->retptr = oldretptr;

			break;
		}

		case EXP_PEQL:
		case EXP_MEQL:
		
			asm_expression(func, e->assignment.expression);
			printf("\tSTA _RESVAL\n");
			asm_identifier_lda(func, e->assignment.target->identifier);
			
			if(e->type == EXP_PEQL)
				printf("\tADD _RESVAL; += opperation\n");
			else
				printf("\tSUB _RESVAL; -= opperation\n");
		
			asm_identifier_sta(func, e->assignment.target->identifier);

			break;
	}
}

// Invoke asm_expression on all statements in a statement list
void asm_statements(Function func, List statements) {
	int len = list_len(&statements);

	for(int i = 0; i < len; i++) {
		Expression e = list_get(&statements, i);
		asm_expression(func, e);
	}
}

// Prints the assembly for a function call
void asm_call_function(Function func, char* func_name, List args){
	
	int len = list_len(&args);
	int label_ret = local_labels++;
	int label_ret_def = local_labels++;
	int label_ret_store = local_labels++;
	int label_base_store = local_labels++;

	printf("\tLDA _stack; store return address\n"); //load stackpointer
	printf("\tADD _ISTAM1\n");
	printf("\tSTA _L%d\n", label_ret_store);
	printf("\tSUB _I1\n");
	printf("\tSTA _L%d; store current base pointer\n", label_base_store);
	printf("\tSUB _ISTAM1\n");
	printf("\tSTA _stack\n");
	printf("\tLDA _L%d\n", label_ret_def);
	printf("_L%d\n", label_ret_store);
	printf("\tDEFW &0; dummy\n");
	printf("\tLDA _base\n");
	printf("_L%d\n", label_base_store);
	printf("\tDEFW &0; dummy\n");
	printf("\tLDA _stack; update base pointer\n");
	printf("\tSTA _base\n; push variables\n");


	for(int i = 0; i < len; i++){
		Expression e = list_get(&args, i);
		asm_expression(func, e);
		asm_push();
	}

	printf("\tJMP %s; jump to the function\n", func_name);
	printf("_L%d\n", label_ret_def);
	printf("\tDEFW L%d\n", label_ret);
	printf("_L%d\n; return to here when function is done", label_ret);
}

// Register local variable in function if it is not a global variable
void reg_var(Function func, char* var){
	int len = list_len(&declerations);

	for(int i = 0; i < len; i++){
		Decleration decl = list_get(&declerations, i);
		if(strcmp(decl->str, var) == 0)
			return;
	}

	len = list_len(&func->vars);

	for(int i = 0; i < len; i++){
		char* fvar = list_get(&func->vars, i);
		if(strcmp(fvar, var) == 0)
			return;
	}

	list_push(&func->vars, var);
}

// Recursivly go trough expressions to find all local variables
void get_vars_expression(Function func, Expression e) {
	
	switch(e->type){
		case EXP_NUM:
			reg_num(e->num);
			break;

		case EXP_ASSIGNMENT:
			
			if(e->assignment.target->identifier.str)
				reg_var(func, e->assignment.target->identifier.str);
			
			get_vars_expression(func, e->assignment.expression);

			break;

		case EXP_RET:
			if(e->e[0])
				get_vars_expression(func, e->e[0]);
			break;

		case EXP_MEQL:
		case EXP_PEQL:
		case EXP_NOT:
			get_vars_expression(func, e->e[0]);
			break;

		case EXP_PLS:
		case EXP_MNS:
		case EXP_GT:
		case EXP_GE:
		case EXP_AND:
		case EXP_OR:
			get_vars_expression(func, e->e[0]);
			get_vars_expression(func, e->e[1]);
			break;

		case EXP_IF:

			get_vars_expression(func, e->_if.expression);
			get_vars_statements(func, e->_if.block);

			if(e->_if._else){
				get_vars_statements(func, e->_if._else);
			}

		case EXP_WHILE:
			get_vars_expression(func, e->_while.expression);
			get_vars_statements(func, e->_while.block);
			break;

		case EXP_CALL_FUNCTION:
		{	
			int len = list_len(&e->call_func.expressions);
			
			for(int i = 0; i < len; i++){
				Expression expression = list_get(&e->call_func.expressions, i);
				get_vars_expression(func, expression);
			}

			break;
		}
		case EXP_INLINE:
		{
			if(strcmp(func->name, e->str) == 0){
				printf("RECURSIVE INLINE CALL DETECTED\nIN FUNC %s!!!\n", func->name);
				exit(1);
			}

			int len = list_len(&functions);

			for(int i = 0; i < len; i++){
				Function f = list_get(&functions, i);

				if(strcmp(e->str, f->name) == 0){	
					get_vars_statements(func, f->block);
					break;
				}
			}

			break;
		}
	}
}

// Invoke get_vars_expression on all expressions in a statement list
void get_vars_statements(Function func, List statements) {
	
	int len = list_len(&statements);

	for(int i = 0; i < len; i++){
		Expression e = list_get(&statements, i);
		get_vars_expression(func, e);
	}
}

// Print the assembly for all the functions
void asm_functions() {
	int len = list_len(&functions);

	for(int i = 0; i < len; i++){

		Function func = list_get(&functions, i);
		
		// Skip printing of function if it is decleared as inline
		if(func->_inline)
			continue;
		
		func->retptr = local_labels++;

		int label_0 = local_labels++;
		int label_1 = local_labels++;
		int label_2 = local_labels++;

		printf("%s\n", func->name);

		// Get all local variables in the function
		get_vars_statements(func, func->block);

		int lcl_varc = list_len(&func->vars) - func->argc;
	
		// Make space for local variables
		if(lcl_varc) {
			reg_num(lcl_varc);

			printf("\tLDA _stack; make space for local variables in stack\n");
			printf("\tSUB _I%d\n", lcl_varc);
			printf("\tSTA _stack\n");
		}

		asm_statements(func, func->block);

		printf("_L%d\n", func->retptr);

		if(i == 0){
			printf("\tSTP\n");
			printf("\tJMP &0\n");
			continue;
		}
		
		// END OF FUNCTION, LOAD BASE POINTER
		printf("\tLDA _base; returning now\n");
		
		// GET ADDRESS OF RETURN ADDRESS AND OLD BASE POINTER
		printf("\tSTA _L%d\n", label_0);
		printf("\tADD _I1\n");
		printf("\tSTA _L%d\n", label_1);
		printf("\tADD _I1\n");
		// SAVE OLD STACK POINTER
		printf("\tSTA _stack\n");

		// SAVE OLD BASE POINTER
		printf("_L%d\n", label_0);
		printf("\tDEFW &0; dummy\n");
		printf("\tSTA _base\n");

		// RETURN TO STORED ADDRESS
		printf("_L%d\n", label_1); // LABEL_1
		printf("\tDEFW &0; dummy\n");
		printf("\tADD _IJMP\n");
		printf("\tSTA _L%d\n", label_2);
		printf("_L%d\n", label_2); // LABEL_2
		printf("\tDEFW &0; dummy\n");

	}
}

void asm_numbers(){
	printf("_I0\n\tDEFW &0\n");

	int len = list_len(&numbers);

	for(int i = 0; i < len; i++){
		int n = (int)(size_t) list_get(&numbers, i);

		printf("_I%d\n\tDEFW &%x\n", n, n);
	}
}

