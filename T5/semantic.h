//Pedro Wachsmann Schanzer de Oliveira 208612

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

#define FUNC_DEC 	1
#define FUNC_CALL 	2
#define FUNC_RETURN 3
#define FUNC_COMEFROM 4


typedef struct Func
{
	char *name;
	Ast* listParam;
	struct Func *next;
}Functions;

Functions *functions_;
Functions *function_calls;
Functions *function_return;

int erroSemantico;

void insert_function(Ast* node, int type, char *name);
void check_call_function();
int check_parameters(Ast *listParamCall, Ast *listParamFunc, char *name, Ast *call, Ast *func, int param, int stop);

void semantic(Ast *node);


void assign_datatype(Ast *node);
void check_and_set_declarations(Ast *node);
void check_undeclared(Ast *node);
void check_operands(Ast *node);
void check_ID_type(Ast *node);

void print_expression(Ast *ast);
