
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "listmap.h"

#define DEBUG 0

/*
 * Oh, 
 * blessed be thee,
 * who dare to dive 
 * down this rabiit whole
 *
 * Well, good luck at least ;)
 *
 * main() is at the bottom, its a good place to start
*/



unsigned int local_labels = 0;

List declerations = 0;
List arrs = 0;
List functions = 0;
List numbers = 0;

void asm_statements(Function func, List statements);
void asm_call_function(Function func, char* name, List args);

void get_vars_statements(Function func, List statements);

void reg_num(int n) {
	
	if(n == 0){
		return;
	}

	if(n < 0){
		n = 0x10000 + n;
	}

	int len = list_len(&numbers);

	for(int i = 0; i < len; i++){
		int n2 = (int) list_get(&numbers, i);
		if(n == n2)
			return;
	}

	list_push(&numbers, n);
}


void var_decl(char* str, int num){
	Decleration decl = malloc(sizeof(struct Decleration));
	decl->str = str;
	decl->num = num;
	list_push(&declerations, decl);
}

void arr_decl(char* str, int len, List numbers){
	Array arr = malloc(sizeof(struct Array));

	arr->str = str;
	arr->len = len;
	arr->nums = numbers;

	list_push(&arrs, arr);
}

void arr_push(List* arr, int num){
	int* n = malloc(sizeof(int));
	*n = num;
	list_push(arr, n);
}

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

void get_vars_expression(Function func, Expression e) {
	
	if(e->type == EXP_NUM){
		reg_num(e->num);

	}else if(e->type == EXP_ASSIGNMENT || e->type == EXP_PEQL || e->type == EXP_MEQL){
		if(e->assignment.target->identifier.str)
			reg_var(func, e->assignment.target->identifier.str);
		get_vars_expression(func, e->assignment.expression);

	}else if(e->type == EXP_NOT
			|| e->type == EXP_MEQL
			|| e->type == EXP_PEQL
			|| e->type == EXP_RET){
		get_vars_expression(func, e->e[0]);

	}else if(e->type == EXP_PLS 
			|| e->type == EXP_MNS
			|| e->type == EXP_GT
			|| e->type == EXP_GE
			|| e->type == EXP_AND
			|| e->type == EXP_OR){
		get_vars_expression(func, e->e[0]);
		get_vars_expression(func, e->e[1]);

	}else if(e->type == EXP_IF){
		get_vars_expression(func, e->_if.expression);
		get_vars_statements(func, e->_if.block);

		if(e->_if._else){
			get_vars_statements(func, e->_if._else);
		}

	}else if(e->type == EXP_WHILE){
		get_vars_expression(func, e->_while.expression);
		get_vars_statements(func, e->_while.block);

	}else if(e->type == EXP_CALL_FUNCTION){
		
		int len = list_len(&e->call_func.expressions);
		
		for(int i = 0; i < len; i++){
			Expression expression = list_get(&e->call_func.expressions, i);
			get_vars_expression(func, expression);
		}
	}else if(e->type == EXP_INLINE){
		
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
	}
}

void get_vars_statements(Function func, List statements) {
	
	int len = list_len(&statements);

	for(int i = 0; i < len; i++){
		Expression e = list_get(&statements, i);
		get_vars_expression(func, e);
	}
}

void func_decl(char* name, List args, List block, int _inline) {
	Function func = malloc(sizeof(struct Function));
	
	func->name = name;
	
	func->vars = args;
	func->argc = list_len(&func->vars);
	func->block = block;
	func->_inline = _inline;
	
	list_push(&functions, func);
}

void print_declerations() {
	int len = list_len(&declerations);

	for(int i = 0; i < len; i++){
		Decleration decl = list_get(&declerations, i);
		printf("Decl: %s = %d\n", decl->str, decl->num);
	}
}

void asm_declerations() {
	int len = list_len(&declerations);

	for(int i = 0; i < len; i++){
		Decleration decl = list_get(&declerations, i);
		printf("%s\n\tDEFW &%x\n", decl->str, (short) decl->num);
	}
}

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

			printf("\tLDA _base\n");
			printf("\tSUB _I%d\n", i);
			printf("\tSTA _L%d\n", local_labels);
			printf("_L%d\n", local_labels++);
			printf("\tDEFW &0; dummy\n");
			return;
		}
	}

	printf("\tLDA %s\n", identifier.str); 

}

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
			
			printf("\tSTA _RESVAL\n");
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

void asm_push() {
	printf("\tSTA _RESVAL\n");
	printf("\tLDA _stack; pushing\n");
	printf("\tSUB _I1\n");
	printf("\tSTA _stack\n");
	printf("\tADD _ISTA\n");
	printf("\tSTA _L%d\n", local_labels);
	printf("\tLDA _RESVAL\n");
	printf("_L%d\n", local_labels++);
	printf("\tDEFW &0; dummy\n");
}

void asm_pop() {
	printf("\tLDA _stack; popping\n");
	printf("\tSTA _L%d\n", local_labels);
	printf("\tADD _I1\n");
	printf("\tSTA _stack\n");
	printf("_L%d\n", local_labels++);
	printf("\tDEFW &0; dummy\n");
}

void asm_expression(Function func, Expression e){

	if(e->type == EXP_NUM){

		printf("\tLDA _I%d\n", e->num);

	}else if(e->type == EXP_IDENTIFIER) {
	
		asm_identifier_lda(func, e->identifier);

	}else(e->type == EXP_ASSIGNMENT){
		asm_expression(func, e->assignment.expression);
		asm_identifier_sta(func, e->assignment.target->identifier);

	}else if(e->type == EXP_PLS
			|| e->type == EXP_MNS
			|| e->type == EXP_GT
			|| e->type == EXP_GE) {
		
		asm_expression(func, e->e[0]);
		asm_push();
		asm_expression(func, e->e[1]);
		printf("\tSTA _RESVAL\n");
		asm_pop();

		if(e->type == EXP_PLS){
			printf("\tADD _RESVAL\n");
		}else{
			printf("\tSUB _RESVAL\n");

			if(e->type == EXP_GE){
				printf("\tJGE _L%d\n", local_labels);
				printf("\tLDA _I0\n");
				printf("\tJMP _L%d\n", local_labels + 1);
				printf("_L%d\n", local_labels++);
				printf("\tSTA _I1\n");
				printf("_L%d\n", local_labels++);
			}


			if(e->type == EXP_GT){
				printf("\tJGE _L%d\n", local_labels);
				printf("\tLDA _I0\n");
				printf("_L%d\n", local_labels++);
			}
		}

	}else if(e->type == EXP_IF) {

		int label1 = local_labels++;
		int label2 = local_labels++;
		
		asm_expression(func, e->_if.expression);
		
		printf("\tJNE _L%d; if\n", label1);

		if(e->_if._else) {
			asm_statements(func, e->_if._else);
		}

		printf("\tJMP _L%d\n", label2);

		printf("_L%d\n", label1);
		
		asm_statements(func, e->_if.block);

		printf("_L%d\n", label2);

	}else if(e->type == EXP_WHILE) {
		
		asm_expression(func, e->_while.expression);

		int label1 = local_labels++;
		int label2 = local_labels++;

		printf("\tJMP _L%d; while\n", label2);
		printf("_L%d\n", label1);

		asm_statements(func, e->_while.block);

		printf("_L%d\n", label2);

		asm_expression(func, e->_while.expression);

		printf("\tJNE _L%d\n", label1);

	}else if(e->type == EXP_CALL_FUNCTION){
		asm_call_function(func, e->call_func.func_name, e->call_func.expressions);

	}else if(e->type == EXP_OR){

		int label = local_labels++;

		asm_expression(func, e->e[0]);
		printf("\tJNE _L%d\n", label);
		asm_expression(func, e->e[1]);
		printf("_L%d\n", label);

	}else if(e->type == EXP_AND){
		
		int label1 = local_labels++;
		int label2 = local_labels++; 

		asm_expression(func, e->e[0]);
		printf("\tJNE _L%d\n", label1);
		printf("\tJMP _L%d\n", label2);
		printf("_L%d\n", label1);
		asm_expression(func, e->e[1]);
		printf("_L%d\n", label2);

	}else if(e->type == EXP_NOT){
		asm_expression(func, e->e[0]);
		printf("\tJNE _L%d\n", local_labels);
		printf("\tLDA _I1\n");
		printf("\tJMP _L%d\n", local_labels + 1);
		printf("_L%d\n", local_labels++);
		printf("\tLDA _I0\n");
		printf("_L%d\n", local_labels++);

	}else if(e->type == EXP_RET){
		asm_expression(func, e->e[0]);
		printf("\tSTA _RESVAL\n");
		printf("\tJMP _L%d\n", func->retptr);

	}else if(e->type == EXP_INLINE){

		int oldretptr = func->retptr;
		func->retptr = local_labels++;

		int len = list_len(&functions);

		for(int i = 0; i < len; i++){
			Function inline_func = list_get(&functions, i);

			if(strcmp(e->str, inline_func->name) == 0){
				asm_statements(func, inline_func->block);
				printf("_L%d\n", func->retptr);
				break;
			}
		}

		func->retptr = oldretptr;

	}else if(e->type == EXP_PEQL || e->type == EXP_MEQL){
		
		asm_expression(func, e->assignment.expression);
		printf("\tSTA _RESVAL\n");
		asm_identifier_lda(func, e->assignment.target->identifier);
		
		if(e->type == EXP_PEQL)
			printf("\tADD _RESVAL\n");
		else
			printf("\tSUB _RESVAL\n");
		
		asm_identifier_sta(func, e->assignment.target->identifier);
	}
}

void asm_statements(Function func, List statements) {
	int len = list_len(&statements);

	for(int i = 0; i < len; i++) {
		Expression e = list_get(&statements, i);
		asm_expression(func, e);
	}
}

void asm_call_function(Function func, char* func_name, List args){
	
	int len = list_len(&args);
	int label_ret = local_labels++;
	int label_ret_def = local_labels++;
	int label_ret_store = local_labels++;
	int label_base_store = local_labels++;

	printf("\tLDA _stack\n"); //load stackpointer
	printf("\tADD _ISTAM1\n");
	printf("\tSTA _L%d\n", label_ret_store);
	printf("\tSUB _I1\n");
	printf("\tSTA _L%d\n", label_base_store);
	printf("\tSUB _ISTAM1\n");
	printf("\tSTA _stack\n");
	printf("\tLDA _L%d\n", label_ret_def);
	printf("_L%d\n", label_ret_store);
	printf("\tDEFW &0; dummy\n");
	printf("\tLDA _base\n");
	printf("_L%d\n", label_base_store);
	printf("\tDEFW &0; dummy\n");
	printf("\tLDA _stack\n");
	printf("\tSTA _base\n");


	for(int i = 0; i < len; i++){
		Expression e = list_get(&args, i);
		asm_expression(func, e);
		asm_push();
	}

	printf("\tJMP %s\n", func_name);
	printf("_L%d\n", label_ret_def);
	printf("\tDEFW L%d\n", label_ret);
	printf("_L%d\n", label_ret);
}


void asm_functions() {
	int len = list_len(&functions);

	for(int i = 0; i < len; i++){

		Function func = list_get(&functions, i);

		if(func->_inline)
			continue;
		
		func->retptr = local_labels++;

		int label_0 = local_labels++;
		int label_1 = local_labels++;
		int label_2 = local_labels++;

		printf("%s\n", func->name);

		get_vars_statements(func, func->block);

		int lcl_varc = list_len(&func->vars) - func->argc;
		
		if(lcl_varc) {
			reg_num(lcl_varc);

			printf("\tLDA _stack\n");
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

asm_numbers(){
	printf("_I0\n\tDEFW &0\n");

	int len = list_len(&numbers);

	for(int i = 0; i < len; i++){
		int n = list_get(&numbers, i);

		printf("_I%d\n\tDEFW &%x\n", n, n);
	}
}

asm_arrs(){
	
	int arrs_len = list_len(&arrs);
	
	for(int i = 0; i < arrs_len; i++){

		Array arr = list_get(&arrs, i);

		printf("%s_len DEFW &%x\n", arr->str, arr->len);
		printf("%s_addr DEFW %s\n", arr->str, arr->str);
		printf("%s\n", arr->str);

		int len = list_len(&(arr->nums));

		for(int j = 0; j < arr->len; j++){
			
			if(j < len){
				int num = *(int*)list_get(&(arr->nums), j);
				printf("\tDEFW &%x\n", num);
			}else{
				printf("\tDEFW &0\n");
			}
		}
	}
}

int main(){

	yyparse();

	if(DEBUG){
		print_declerations();
		printf("\nASM:\n\n");
	}
	
	reg_num(1);
	
	asm_functions();
	asm_declerations();
	asm_arrs();

	asm_numbers();

	printf("_IJMP\n\tDEFW &4000\n");
	printf("_ISTA\n\tDEFW &1000\n");
	printf("_ISTAM1\n\tDEFW &FFF\n");
	printf("_RESVAL\n\tDEFW &0\n");
	printf("_base\n\tDEFW &F00\n");
	printf("_stack\n\tDEFW &F00\n");


	printf("\tORG &F00\n");
}

void yyerror(){
	printf("syntax error!\nI give up!\n");
	exit(1);
}
