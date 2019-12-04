
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "bison.h"

#define DEBUG 0

/*
 * main() is at the bottom, its a good place to start
*/



unsigned int local_labels = 0;

List declerations = 0; // A list of all global variable declerations
List functions = 0; // A list of all function declerations
List numbers = 0; // A list of all constants to be decleared in the assembly


// Create var decleration, called from bison.y
void var_decl(char* str, int num){
	Decleration decl = malloc(sizeof(struct Decleration));
	decl->str = str;
	decl->num = num;
	list_push(&declerations, decl);
}

// Create function decleration, called from bison.y
void func_decl(char* name, List args, List block, int _inline) {
	Function func = malloc(sizeof(struct Function));
	
	func->name = name;
	
	func->vars = args;
	func->argc = list_len(&func->vars);
	func->block = block;
	func->_inline = _inline;
	
	list_push(&functions, func);
}

// The main() function
int main(){

	// Parse input using bison.y, with lexical analysis from flex.l
	yyparse();
	
	// The number 1 must be registered since its used to push and pop the stack
	reg_num(1);
	
	asm_functions();
	asm_declerations();

	asm_numbers();

	// Some constants that needs to be defined
	printf("_IJMP\n\tDEFW &4000\n");
	printf("_ISTA\n\tDEFW &1000\n");
	printf("_ISTAM1\n\tDEFW &FFF\n");
	printf("_RESVAL\n\tDEFW &0\n");
	printf("_base\n\tDEFW &F00\n");
	printf("_stack\n\tDEFW &FED\n"); // Start the stack at just bellow board periphirals


}

// Error function for bison
void yyerror(){
	printf("syntax error!\nI give up!\n");
	exit(1);
}
