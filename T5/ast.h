//Pedro Wachsmann Schanzer de Oliveira 208612

#ifndef AST_H
#define AST_H

#include "hash.h"

#define MAX_SONS 4

#define AST_SYMBOL 				1
#define AST_VECTOR 				2
#define AST_EXP 				3
#define AST_ATRIBUICAO 			4
#define AST_ATRIBUICAO_VEC 		5
#define AST_FUNCAO		 		6
#define AST_LISTEXP		 		7
#define AST_CALL_FUNCTION		8
#define AST_EQUAL				9
#define AST_IF					10
#define AST_ELSE				11
#define AST_OR					12
#define AST_AND					13
#define AST_LE					14
#define AST_GE					15
#define AST_ADD					16
#define AST_SUB					17
#define AST_DIV					18
#define AST_MULT				19
#define AST_LESS				20
#define AST_GREATER				21
#define AST_ASSOCIATIVIDADE		22
#define AST_NEG					23
#define AST_DIF					24
#define AST_RANGE				25
#define AST_RETURN				26
#define AST_READ				27
#define AST_UNTIL				28
#define AST_COMEFROM			29
#define AST_BODY				30
#define AST_LISTCMD				31
#define AST_PRINT				32
#define AST_PRINTLIST	 		33
#define AST_PARAM				34
#define AST_DATA				35
#define AST_INT 				36
#define AST_FLOAT 				37
#define AST_CHAR				38
#define AST_LISTPARAM 			39
#define AST_HEADER 			    40
#define AST_VARIAVEL_DEC		41
#define AST_VEC_DEC 			42
#define AST_LISTLIT		 		43
#define AST_VEC_DEC_INIT 		44
#define AST_LISTDEC 	 		45
#define AST_LISTFUN				46
#define AST_TOP				    47

#define AST_DATATYPE_FLUTUANTE			1
#define AST_DATATYPE_INTEIRO			2
#define AST_DATATYPE_BOOLEANO			3

typedef struct ast_node
{
	int type;
	int dataType;
	Node *symbol;
	struct ast_node *son[MAX_SONS];
} Ast;

Ast *astCreate(int type, Node* symbol, Ast *s0, Ast *s1, Ast *s2, Ast *s3);

void astPrint(Ast *node, int nivel);
 
#endif