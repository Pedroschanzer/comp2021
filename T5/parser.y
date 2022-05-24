//Pedro Wachsmann Schanzer de Oliveira 208612

%{

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "hash.h"
#include "semantic.h"
#include "tac.h"


extern void print_expression(Ast *ast);
extern Node* hashTable[HASH_SIZE];


int yyerror(char* msg);
int yylex();
int getLineNumber();
int erro=0;
Ast* root;
%}

%union
{
	Node* symbol;
	Ast* ast;
}

%token KW_DATA
%token KW_CHAR
%token KW_INT        
%token KW_FLOAT      
%token KW_IF          
%token KW_ELSE       
%token KW_UNTIL      
%token KW_COMEFROM       
%token KW_READ         
%token KW_PRINT      
%token KW_RETURN  
%token OPERATOR_LE   
%token OPERATOR_GE   
%token OPERATOR_EQ   
%token OPERATOR_DIF
%token OPERATOR_RANGE  
%token TK_IDENTIFIER 
%token LIT_INTEGER    
%token LIT_CHAR      
%token LIT_STRING    
%token TOKEN_ERROR   

%type <ast>literal
%type <ast>expr
%type <ast>identifier
%type <ast>atribuicao
%type <ast>listexp
%type <ast>ifcmd
%type <ast>elsecmd
%type <ast>cmd
%type <ast>untilcmd
%type <ast>comefromcmd
%type <ast>body
%type <ast>lcmd
%type <ast>print
%type <ast>to_print
%type <ast>param
%type <ast>tipo
%type <ast>listparam
%type <ast>header
%type <ast>funcao
%type <ast>var
%type <ast>dados
%type <ast>listlit
%type <ast>declaracao
%type <ast>listdec
%type <ast>listfun
%type <ast>program
%type <ast>init

%type <symbol>LIT_INTEGER
%type <symbol>TK_IDENTIFIER
%type <symbol>LIT_CHAR
%type <symbol>LIT_STRING

%left '|'
%left '&'
%left OPERATOR_EQ
%left '<' '>' OPERATOR_LE OPERATOR_GE
%left '+' '-'
%left '*' '/'
%left OPERATOR_RANGE

%right '~'

%start program

%%


program: init {root=$$; 
					semantic(root);
			        if(root != 0)
		            	tacPrintFoward(tacReverse(tacGenerateCode($1)));
					}


init: dados listfun   {$$=astCreate(AST_TOP,0,$1,$2,0,0);}
;

dados: KW_DATA '{' listdec '}' {$$=astCreate(AST_DATA,0,$3,0,0,0);}
;


listdec: declaracao listdec  {$$=astCreate(AST_LISTDEC,0,$1,$2,0,0);}
|		                     {$$=0;}
;

declaracao: var ';'          {$$=$1;}
	|listfun ';'             {$$=$1;}
;

tipo: KW_INT 				{$$=astCreate(AST_INT,0,0,0,0,0);}
	| KW_CHAR				{$$=astCreate(AST_CHAR,0,0,0,0,0);}
	| KW_FLOAT				{$$=astCreate(AST_FLOAT,0,0,0,0,0);}
;

literal: LIT_CHAR			{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
		| LIT_INTEGER		{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
		| LIT_STRING		{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);} 
;

identifier: TK_IDENTIFIER   {$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
;


listlit: literal listlit 	{$$=astCreate(AST_LISTLIT,0,$1,$2,0,0);}
|							{$$=0;}
;

var: tipo ':' identifier '=' literal				{$$=astCreate(AST_VARIAVEL_DEC,0,$1,$3,$5,0);}
	| tipo '[' expr ']' ':' identifier				{$$=astCreate(AST_VEC_DEC_INIT,0,$1,$3,$6,0);}
	| tipo '[' expr ']' ':' identifier '=' listlit	{$$=astCreate(AST_VEC_DEC,0,$1,$3,$6,$8);}
;

listfun: funcao listfun		{$$=astCreate(AST_LISTFUN,0,$1,$2,0,0);}
|							{$$=0;}
;

funcao: header body			{$$=astCreate(AST_FUNCAO,0,$1,$2,0,0);}
;

header: tipo ':' identifier '(' listparam ')' {$$=astCreate(AST_HEADER,0,$1,$3,$5,0);} 
;

listparam: param listparam {$$=astCreate(AST_LISTPARAM,0,$1,$2,0,0);}
	| ',' param listparam   {$$=astCreate(AST_LISTPARAM,0,$2,$3,0,0);}
	|                          {$$=0;}
;

param: tipo ':' identifier  {$$=astCreate(AST_PARAM,0,$1,$3,0,0);}
;

lcmd: cmd ';' lcmd {$$=astCreate(AST_LISTCMD,0,$1,$3,0,0);}
	| cmd           {$$=astCreate(AST_LISTCMD,0,$1,0,0,0);}
;

body: '{' lcmd '}'  {$$=astCreate(AST_BODY,0,$2,0,0,0);}
;

to_print: print           {$$=astCreate(AST_PRINTLIST,0,$1,0,0,0);}
	| print ',' to_print  {$$=astCreate(AST_PRINTLIST,0,$1,$3,0,0);}
;

print: LIT_STRING    {$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
	| expr           {$$=$1;}
;

cmd: atribuicao      {$$=$1;}
	| ifcmd          {$$=$1;}
	| KW_PRINT to_print {$$=astCreate(AST_PRINT,0,$2,0,0,0);}
	| body           {$$=$1;}
	| KW_RETURN expr {$$=astCreate(AST_RETURN,0,$2,0,0,0);}
	| untilcmd       {$$=$1;}
	| comefromcmd    {$$=$1;}
	|                {$$=0;}
;

atribuicao: identifier '=' expr  {$$=astCreate(AST_ATRIBUICAO,0,$1,$3,0,0);}
	| identifier '[' expr ']' '=' expr  {$$=astCreate(AST_ATRIBUICAO_VEC,0,$1,$3,$6,0);}
;

ifcmd: KW_IF '(' expr ')' cmd elsecmd   {$$=astCreate(AST_IF,0,$3,$5,$6,0);}
;

elsecmd: KW_ELSE cmd		            {$$=astCreate(AST_ELSE,0,$2,0,0,0);}											
	|                                   {$$=0;}
;

untilcmd: KW_UNTIL '(' expr ')' cmd     {$$=astCreate(AST_UNTIL,0,$3,$5,0,0);}
;

comefromcmd: KW_COMEFROM ':' identifier {$$=astCreate(AST_COMEFROM,0,$3,0,0,0);}
		|identifier  					{$$=$1;}
;


expr: LIT_INTEGER                       {$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}	
	| LIT_CHAR                          {$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
	| identifier                    	{$$=$1;}
	| identifier '[' expr ']'       	{$$=astCreate(AST_VECTOR,0,$1,$3,0,0);}
	| identifier '(' listexp ')'    	{$$=astCreate(AST_CALL_FUNCTION,0,$1,$3,0,0);}
	| '(' expr ')'                      {$$=astCreate(AST_ASSOCIATIVIDADE,0,$2,0,0,0);}
	| KW_READ                           {$$=astCreate(AST_READ,0,0,0,0,0);}
	| expr OPERATOR_EQ expr             {$$=astCreate(AST_EQUAL,0,$1,$3,0,0);}
	| expr OPERATOR_LE expr             {$$=astCreate(AST_LE,0,$1,$3,0,0);}
	| expr OPERATOR_GE expr             {$$=astCreate(AST_GE,0,$1,$3,0,0);}
	| expr '+' expr                     {$$=astCreate(AST_ADD,0,$1,$3,0,0);}              
	| expr '-' expr                     {$$=astCreate(AST_SUB,0,$1,$3,0,0);}
	| expr '/' expr                     {$$=astCreate(AST_DIV,0,$1,$3,0,0);}
	| expr '*' expr                     {$$=astCreate(AST_MULT,0,$1,$3,0,0);}
	| expr '<' expr                     {$$=astCreate(AST_LESS,0,$1,$3,0,0);}
	| expr '>' expr                     {$$=astCreate(AST_GREATER,0,$1,$3,0,0);}
	| expr '&' expr                     {$$=astCreate(AST_AND,0,$1,$3,0,0);} 
	| expr '|' expr                     {$$=astCreate(AST_OR,0,$1,$3,0,0);}
	| '~' expr                          {$$=astCreate(AST_NEG,0,$2,0,0,0);}
	| expr OPERATOR_DIF expr            {$$=astCreate(AST_DIF,0,$1,$3,0,0);}
	| expr OPERATOR_RANGE expr          {$$=astCreate(AST_RANGE,0,$1,$3,0,0);}
;

listexp: expr listexp                   {$$=astCreate(AST_LISTEXP,0,$1,$2,0,0);}
	| ',' expr listexp                  {$$=astCreate(AST_LISTEXP,0,$2,$3,0,0);}
	|                            		{$$=0;}
;


%%
Ast* getAst(){
	return root;
}

int yyerror(char* msg)
{
	int linecounter = getLineNumber(); 
	fprintf(stderr, "Syntax error at line %d\n",linecounter);
	exit(3);
}