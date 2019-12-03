
%{
#include "listmap.h"
#include "main.h"
%}

%union {
	char* str;
	int num;
	List list;
	Expression expression;
}

%token IF WHILE ELSE
%token EQL SM_COL OPN_PRN CLS_PRN OPN_CRL CLS_CRL CMA PLS MNS GT LT GE LE AND OR AMP NOT RET INLINE PEQL MEQL
%token <num> NUM
%token <str> IDENTIFIER

%type <list> arg_list args statement_list block expression_list
%type <expression> expression statement terminal identifier

%%

list: /* nothing */
    | list entry
    ;

entry: var_decl
     | func
     ;

var_decl: IDENTIFIER EQL NUM SM_COL { var_decl($1, $3); }
	;

func: IDENTIFIER args block { func_decl($1, $2, $3, 0); }
    | INLINE IDENTIFIER args block { func_decl($2, $3, $4, 1); }
    ;

args: OPN_PRN CLS_PRN { $$ = 0; }
    | OPN_PRN arg_list CLS_PRN { $$ = $2; }
    ;

arg_list: IDENTIFIER { $$ = 0; list_push(&$$, $1); }
	| arg_list CMA IDENTIFIER { $$ = $1; list_push(&$$, $3); }
	;

block: OPN_CRL statement_list CLS_CRL { $$ = $2; }
     ;

statement_list: /* nothing */ { $$ = 0; }
	      | statement_list statement { $$ = $1; list_push(&$$, $2); }
	      ;

statement: expression SM_COL { $$ = $1; }
	 | RET expression SM_COL { $$ = expression_ret($2); }
	 | WHILE expression block { $$ = expression_while($2, $3); }
	 | IF expression block ELSE block { $$ = expression_if($2, $3, $5); }
	 | IF expression block { $$ = expression_if($2, $3, 0); }
	 ;

expression: expression MNS terminal { $$ = expression_mns($1, $3); }
	  | expression PLS terminal { $$ = expression_pls($1, $3); }
	  | expression GT terminal { $$ = expression_gt($1,$3); }
	  | expression LT terminal { $$ = expression_gt($3, $1); }
	  | expression GE terminal { $$ = expression_ge($1, $3); }
	  | expression LE terminal { $$ = expression_ge($3, $1); }
	  | expression OR terminal { $$ = expression_or($1, $3); }
	  | NOT expression { $$ = expression_not($2); }
	  | expression AND terminal { $$ = expression_and($1, $3); }
	  | identifier EQL expression { $$ = expression_assignment($1, $3); }
	  | identifier PEQL expression { $$ = expression_peql($1, $3); }
	  | identifier MEQL expression { $$ = expression_meql($1, $3); }
	  | IDENTIFIER OPN_PRN CLS_PRN { $$ = expression_call_function($1, 0); }
	  | IDENTIFIER OPN_PRN expression_list CLS_PRN { $$ = expression_call_function($1, $3); }
	  | terminal { $$ = $1; }
	  ;

expression_list: expression { $$ = 0; list_push(&$$, $1); }
	       | expression_list CMA expression { $$ = $1; list_push(&$$, $3); }
	       ;

terminal: identifier { $$ = $1; }
	| NUM { $$ = expression_num($1); }
	| OPN_PRN expression CLS_PRN { $$ = $2; }
	| INLINE IDENTIFIER { $$ = expression_inline($2); }
	;

identifier: IDENTIFIER { $$ = expression_identifier($1, 0); }
	  | AMP NUM { $$ = expression_identifier(0, $2); }
	  ;




