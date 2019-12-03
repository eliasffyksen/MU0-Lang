
%option noyywrap

%{
#include <string.h>

#include "listmap.h"
#include "main.h"
#include "bison.h"
%}

%%

"//"[^\n]*	{}

"if"	{ return IF; }
"else"	{ return ELSE; }
"while"	{ return WHILE; }
"return" { return RET; }
"inline" { return INLINE; }

"&"	{ return AMP; }

"="	{ return EQL; }
"-="	{ return MEQL; }
"+="	{ return PEQL; }
";"	{ return SM_COL; }
"("	{ return OPN_PRN; }
")"	{ return CLS_PRN; }
","	{ return CMA; }
"{"	{ return OPN_CRL; }
"}"	{ return CLS_CRL; }
"+"	{ return PLS; }
"-"|"==" { return MNS; }
">="	{ return GE; }
"<="	{ return LE; }
">"	{ return GT; }
"<"	{ return LT; }
"!"	{ return NOT; }
"&&"	{ return AND; }
"||"	{ return OR; }

[a-zA-Z][a-zA-Z0-9_]*	{ yylval.str = strdup(yytext); return IDENTIFIER; }
-?[0-9]+		{ yylval.num = atoi(yytext); return NUM; }
0x[0-9a-fA-F]+		{ yylval.num = (int)strtol(yytext, NULL, 16); return NUM;}

[ \t\n]*
