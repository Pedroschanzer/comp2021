%option nounput yylineno

%{

//#include "main.c"
#include "ast.h"
#include "y.tab.h"



int lineNumbers = 1;
int running = 1;
void initMe()
{
	hashInit(hashTable);
}

int getLineNumber()
{
	if(yylineno - lineNumbers == 0)
		return yylineno;
	return lineNumbers;
}

int isRunning()
{
	return running;
} 

%}

%x COMMENT

%%

"data"							{return KW_DATA;}
"char"							{return KW_CHAR;}
"int"							{return KW_INT;}
"float"							{return KW_FLOAT;}
"if"							{return KW_IF;}
"else"							{return KW_ELSE;}
"until"							{return KW_UNTIL;}
"comefrom"				        {return KW_COMEFROM;}
"read"							{return KW_READ;}
"print"							{return KW_PRINT;}
"return"						{return KW_RETURN;}

[-,;:()[\]{}+*/<>|&~=]       	{return yytext[0];}

"<="							{return OPERATOR_LE;}
">="							{return OPERATOR_GE;}
"=="							{return OPERATOR_EQ;}
"!="							{return OPERATOR_DIF;}
".."							{return OPERATOR_RANGE;}


[a-zA-Z][a-zA-Z0-9 ]*[a-zA-Z0-9]+|[a-zA-Z]+		{lineNumbers = yylineno; hashInsert(yytext, SYMBOL_IDENTIFIER , getLineNumber()); yylval.symbol=hashFind(yytext); return TK_IDENTIFIER;}
[0-9]+						{lineNumbers = yylineno; hashInsert(yytext, SYMBOL_INTEGER , getLineNumber()); yylval.symbol=hashFind(yytext); return LIT_INTEGER;}
'.'				{lineNumbers = yylineno; hashInsert(yytext, SYMBOL_CHAR , getLineNumber()); yylval.symbol=hashFind(yytext); return LIT_CHAR;}
\"([^\"]|\n)+\"		{lineNumbers = yylineno; hashInsert(yytext, SYMBOL_STRING , getLineNumber()); yylval.symbol=hashFind(yytext); return LIT_STRING;}
[ \t]							
"\n"							{lineNumbers = yylineno;}

"##".*	
"#/"      {  BEGIN(COMMENT); }      
.								{return TOKEN_ERROR;}
<COMMENT>"/#" {BEGIN(INITIAL);}
<COMMENT>"\n" {lineNumbers++; }
<COMMENT>.                      


%%

int yywrap()
{
	running = 0;
	return 1;
}













