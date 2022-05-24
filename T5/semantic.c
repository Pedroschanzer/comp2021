//Pedro Wachsmann Schanzer de Oliveira 208612

#include "semantic.h"
#include "ast.h"

void semantic(Ast *node)
{
	erroSemantico =0;
	functions_ = 0;
	function_calls = 0;
	//astPrint(node, 0);
	check_and_set_declarations(node);
	check_undeclared(node);
	check_operands(node);
	check_call_function();
	check_ID_type(node);
	

	if(erroSemantico)
		exit(4);
}



void delete_list(Functions *node)
{
	Functions *current = node;
	Functions *next;

	while(current != 0)
	{
		next = current->next;
		free(current);
		current = next;
	}

	node = 0;
}



void assign_datatype(Ast *node)
{
	if(node->son[0]->type == AST_CHAR)
		node->son[1]->symbol->datatype = SYMBOL_DATATYPE_CHAR;
	else if(node->son[0]->type == AST_INT)
		node->son[1]->symbol->datatype = SYMBOL_DATATYPE_INT;
	else
		node->son[1]->symbol->datatype = SYMBOL_DATATYPE_FLOAT;
}




void insert_function(Ast *node, int type, char *name)
{
	Functions *elementoReturn = function_return;
	Functions *elementoCall = function_calls;
	Functions *elemento = functions_;
	Functions *newNode;
	newNode = (Functions*) calloc(1, sizeof(Functions));
	newNode->listParam = node;
	newNode->name = name;
	newNode->next = 0;

	if(type == FUNC_DEC)
	{
		if(functions_ == 0) 
	        functions_ = newNode;
	    else
	    {
	        while(elemento->next != 0)
	            elemento = elemento->next;

	        elemento->next = newNode;
	    }
	}
	else if(type == FUNC_CALL)
	{
		if(function_calls == 0) 
	        function_calls = newNode;
	    else
	    {
	        while(elementoCall->next != 0)
	            elementoCall = elementoCall->next;

	        elementoCall->next = newNode;
	    }
	}
	else
	{
		if(function_return == 0) 
	        function_return = newNode;
	    else
	    {
	        while(elementoReturn->next != 0)
	            elementoReturn = elementoReturn->next;

	        elementoReturn->next = newNode;
	    }
	}
}


void check_call_function()
{
	int i, j;

	Functions *call = function_calls;
	Functions *func = functions_;

	int isCallWrong = 0;

	while(call != 0)
	{
		while(func != 0)
		{
			if(strcmp(call->name, func->name) == 0)
			{
				isCallWrong = check_parameters(call->listParam, func->listParam, call->name, call->listParam, func->listParam, 0, 0);
				if(isCallWrong != 0)
				{
					fprintf(stderr, "		Function -> %s(", func->name);
					print_expression(func->listParam);
					fprintf(stderr, ")\n");
					fprintf(stderr, "		Call -> %s(", call->name);
					print_expression(call->listParam);
					fprintf(stderr, ")\n");
				}
				break;
			}
			func = func->next;
		}
		func = functions_;
		call = call->next;
		isCallWrong = 0;
	}
	delete_list(call);
	delete_list(func);
}

void different_argument_message(Ast *listParamCall, Ast *listParamFunc, char *name)
{
	fprintf(stderr, "Semantic ERROR: Function call with arguments not compatible with the statement\n");
	fprintf(stderr, "		Function -> %s(", name);
	print_expression(listParamFunc);
	fprintf(stderr, ")\n");
	fprintf(stderr, "		Call -> %s(", name);
	print_expression(listParamCall);
	fprintf(stderr, ")\n");
	erroSemantico = 1;
}


int check_parameters(Ast *listParamCall, Ast *listParamFunc, char *nome, Ast *call, Ast *func, int param, int stop)
{
	int i;
	int isCallWrong;

	if((call == 0 && func->type == AST_PARAM) || (func == 0 && call->son[1] != 0 && stop != 0) ||
		(!listParamFunc && listParamCall) || (listParamFunc && !listParamCall))
	{
		fprintf(stderr, "Semantic ERROR: Function Call with number of parameters different from the declaration\n");
		erroSemantico = 1;
		return 1;
	}

	if(call == 0 || func == 0)
		return 0; 

	if(func->type == AST_PARAM)
	{
		switch(func->son[0]->type)
		{
			case AST_CHAR:
			case AST_INT:
				if(call->son[0]->symbol != 0)
				{
					if(call->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || call->son[0]->symbol->type == SYMBOL_FLOAT)
						different_argument_message(listParamCall, listParamFunc, nome);
				}
				else
					if(call->son[0]->dataType == AST_DATATYPE_FLUTUANTE || call->son[0]->dataType == AST_DATATYPE_BOOLEANO)
						different_argument_message(listParamCall, listParamFunc, nome);
				break;

			case AST_FLOAT:
				if(call->son[0]->symbol != 0)
				{
					if((call->son[0]->symbol->datatype != 0 &&call->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT) || 
						call->son[0]->symbol->type == SYMBOL_FLOAT)
					{}
					else
						different_argument_message(listParamCall, listParamFunc, nome);
				}
				else
					if(call->son[0]->dataType != AST_DATATYPE_FLUTUANTE)
						different_argument_message(listParamCall, listParamFunc, nome);
				break;
		}
	}

	for(i=0; i<2; i++)
	{
		if(func->type == AST_LISTPARAM && param == 0 && func->son[i] != 0 && func->son[i]->type != AST_LISTPARAM)
			isCallWrong = check_parameters(listParamCall, listParamFunc, nome, call, func->son[i], 1, 1);
		else if(func->type == AST_LISTPARAM && param != 0  && func->son[i] != 0 &&  func->son[i]->type != AST_LISTPARAM)
		{
			isCallWrong = check_parameters(listParamCall, listParamFunc, nome, call->son[1], func->son[i], 1, 1);
			if(call->son[1] != 0)
				call = call->son[1];
		}
		else
			if(func->type != AST_LISTPARAM)
				isCallWrong = check_parameters(listParamCall, listParamFunc, nome, call, func->son[i], 1, 0);
			else 
				isCallWrong = check_parameters(listParamCall, listParamFunc, nome, call, func->son[i], 1, 1);

		if(isCallWrong)
			return 1;
	}

	return 0;
}

void different_return_message(Ast *decFunction, Functions *elementoReturn)
{
	fprintf(stderr, "Semantic ERROR: Function return with different type from the declaration\n");
	fprintf(stderr, "		Function -> ");
	print_expression(decFunction);
	fprintf(stderr, "\n");
	fprintf(stderr, "		Retorno -> ");
	print_expression(elementoReturn->listParam);
	fprintf(stderr, "\n");
	erroSemantico = 1;
}



void check_return(Ast *decFunction)
{
	Node *node = decFunction->son[1]->symbol;
	Functions *elementoReturn = function_return;

	while(elementoReturn != 0)
	{
		switch(node->datatype)
		{
			case SYMBOL_DATATYPE_INT:
			case SYMBOL_DATATYPE_CHAR:
				if(elementoReturn->listParam->son[0]->symbol != 0)
				{
					if(elementoReturn->listParam->son[0]->symbol->type == SYMBOL_FLOAT || elementoReturn->listParam->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
					{
						different_return_message(decFunction, elementoReturn);
					}
				}
				else
				{
					if(elementoReturn->listParam->son[0]->dataType == AST_DATATYPE_FLUTUANTE || elementoReturn->listParam->son[0]->dataType == AST_DATATYPE_BOOLEANO)
					{
						different_return_message(decFunction, elementoReturn);
					}
				}
				break;

			case SYMBOL_DATATYPE_FLOAT:
				if(elementoReturn->listParam->son[0]->symbol != 0)
				{
					if(elementoReturn->listParam->son[0]->symbol->type == SYMBOL_FLOAT || elementoReturn->listParam->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
					{}
					else
					{
						different_return_message(decFunction, elementoReturn);
					}
				}
				else
				{
					if(elementoReturn->listParam->son[0]->dataType == AST_DATATYPE_FLUTUANTE)
					{}
					else
					{
						different_return_message(decFunction, elementoReturn);
					}
				}
				break;
		}
		elementoReturn = elementoReturn->next;
	}
	delete_list(elementoReturn);
	function_return = 0;
}

void check_and_set_declarations(Ast *node)
{
	int i;

	if(node==0)
		return;

	for(i=0; i<MAX_SONS; i++)
		check_and_set_declarations(node->son[i]);

	switch(node->type)
	{
		case AST_VEC_DEC_INIT: 
		case AST_VEC_DEC:
			if(node->son[0]->symbol->type == SYMBOL_IDENTIFIER) // CORRIGIR RANGE - Professor, não consegui fazer o tratamento semantico do range e de vetores utilizando os valores de AST_RANGE
			{
			//node = node->son[1]->son[0];
			// int indexnode1 = atoi(node->son[1]->son[0]);
			// int indexnode2 = atoi(node->son[1]->son[1]);
			// int size_array = calculate_array_size(node);
			// int range = abs(indexnode2 - indexnode1) + 1;
			// if (ramge > indexnode2)
			// {
			// fprintf(stderr, "Semantic ERROR: Range declaration error\n";
			// erroSemantico = 1;
			// }
			// else	{
			// node->son[2]->symbol->type = SYMBOL_VECTOR;
			// assign_datatype(node);
			// }
			// else{
			//  	fprintf(stderr, "Semantic ERROR: Identifier %s reset. First statement on line %d\n", node->son[1]->symbol->text, node->son[1]->symbol->line);
			//  	erroSemantico = 1;
			 }
			break;

		case AST_VARIAVEL_DEC:
		case AST_PARAM:
			if(node->son[1]->symbol->type == SYMBOL_IDENTIFIER)
			{
				node->son[1]->symbol->type = SYMBOL_VARIABLE;
				assign_datatype(node);
			}
			else{
				fprintf(stderr, "Semantic ERROR: Identifier %s re-declared. First statement on line %d\n", node->son[1]->symbol->text, node->son[1]->symbol->line);
				erroSemantico = 1;
			}
			break;

		case AST_HEADER:
			if(node->son[1]->symbol->type == SYMBOL_IDENTIFIER)
			{
				node->son[1]->symbol->type = SYMBOL_FUNCTION;
				assign_datatype(node);
				insert_function(node->son[2], FUNC_DEC, node->son[1]->symbol->text);
			}
			else{
				fprintf(stderr, "Semantic ERROR: Identifier %s reset. First statement on line %d\n", node->son[1]->symbol->text, node->son[1]->symbol->line);
				erroSemantico = 1;
			}
			break;	

		case AST_CALL_FUNCTION:
			insert_function(node->son[1], FUNC_CALL, node->son[0]->symbol->text);
			break;
	}
}

void check_operands(Ast *node)
{
	int i;
	
	if(node == 0)
		return;

	for(i=0; i<MAX_SONS; i++)
		check_operands(node->son[i]);

	switch(node->type)
	{
		case AST_DIV:
		case AST_MULT:
		case AST_SUB:
		case AST_ADD:
			if(node->son[0]->type == AST_NEG || (node->son[0]->type == AST_ASSOCIATIVIDADE && node->son[0]->dataType == AST_DATATYPE_BOOLEANO))
			{
				fprintf(stderr, "Semantic ERROR: First operand, of the arithmetic operation, invalid\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
				break;
			}
			if(node->son[1]->type == AST_NEG || (node->son[1]->type == AST_ASSOCIATIVIDADE && node->son[1]->dataType == AST_DATATYPE_BOOLEANO))
			{
				fprintf(stderr, "Semantic ERROR: Second operand, of the arithmetic operation, invalid\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
				break;
			}

			if(node->son[0]->type == AST_SUB || node->son[0]->type == AST_DIV || node->son[0]->type == AST_MULT || node->son[0]->type == AST_ADD || 
				node->son[0]->type == AST_ASSOCIATIVIDADE || node->son[1]->type == AST_SUB || node->son[1]->type == AST_DIV || node->son[1]->type == AST_MULT || 
				node->son[1]->type == AST_ADD || node->son[1]->type == AST_ASSOCIATIVIDADE )
			{
				if((node->son[0]->type != AST_ASSOCIATIVIDADE && node->son[0]->dataType == AST_DATATYPE_FLUTUANTE) ||
					(node->son[1]->type != AST_ASSOCIATIVIDADE && node->son[1]->dataType == AST_DATATYPE_FLUTUANTE) ||
					(node->son[0]->type == AST_ASSOCIATIVIDADE && node->son[0]->dataType == AST_DATATYPE_FLUTUANTE) || 
					(node->son[1]->type == AST_ASSOCIATIVIDADE && node->son[1]->dataType == AST_DATATYPE_FLUTUANTE))
				{
					node->dataType = AST_DATATYPE_FLUTUANTE;
					break;
				}				
			}

			if((node->son[0]->symbol != 0 && (node->son[0]->symbol->type == SYMBOL_FLOAT || node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)) || 
				(node->son[1]->symbol != 0 && (node->son[1]->symbol->type == SYMBOL_FLOAT || node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)))
			{	
				node->dataType = AST_DATATYPE_FLUTUANTE;
				break;
			}

			if(node->son[0]->type == AST_VECTOR || node->son[0]->type == AST_CALL_FUNCTION ||
				node->son[1]->type == AST_VECTOR || node->son[1]->type == AST_CALL_FUNCTION)
			{
				if(node->son[0]->dataType == AST_DATATYPE_FLUTUANTE || node->son[1]->dataType == AST_DATATYPE_FLUTUANTE)
				{
					node->dataType = AST_DATATYPE_FLUTUANTE;
					break;
				}
			}

			node->dataType = AST_DATATYPE_INTEIRO;
			break;

		case AST_ASSOCIATIVIDADE:
			if(node->son[0]->symbol != 0 && (node->son[0]->symbol->type == SYMBOL_FLOAT || node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT))
				node->dataType = AST_DATATYPE_FLUTUANTE;
			else
				node->dataType = node->son[0]->dataType;
			break;

		case AST_VECTOR:
			if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
				node->dataType = AST_DATATYPE_FLUTUANTE;
			else 
				node->dataType = AST_DATATYPE_INTEIRO;
		case AST_ATRIBUICAO_VEC:
			if(node->son[1]->symbol != 0)
			{
				if(node->son[1]->symbol->type == SYMBOL_FLOAT || node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
				{
					fprintf(stderr, "Semantic ERROR: Array index must be an integer value\n");
					fprintf(stderr, "		-> ");
					print_expression(node);
					fprintf(stderr, "\n");
					erroSemantico = 1;
				}
			}
			else
			{
				if(node->son[1]->dataType == AST_DATATYPE_FLUTUANTE || node->son[1]->dataType == AST_DATATYPE_BOOLEANO)
				{
					fprintf(stderr, "Semantic ERROR: Vector index must be an integer value\n");
					fprintf(stderr, "		-> ");
					print_expression(node);
					fprintf(stderr, "\n");
					erroSemantico = 1;
				}
			}

			break;

		case AST_CALL_FUNCTION:
			if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
				node->dataType = AST_DATATYPE_FLUTUANTE;
			else 
				node->dataType = AST_DATATYPE_INTEIRO;
			break;

		case AST_EQUAL:
		case AST_LE:
		case AST_GE:
		case AST_LESS:
		case AST_GREATER:
		case AST_AND:
		case AST_OR:
		case AST_NEG:
			node->dataType = AST_DATATYPE_BOOLEANO;
			break;

		case AST_FUNCAO:
			if(function_return != 0)
				check_return(node->son[0]);
			break;
		case AST_RETURN:
			insert_function(node, FUNC_RETURN, "");
			break;
		// case AST_COMEFROM: //Professor, não consegui fazer o tratamento semantico de Comefrom
		// 	if(node->son[0] != NULL){
		// 	}
		// 	else 
		// 		fprintf(stderr, "Semantic ERROR: Command Comefrom without label\n");
		// 	break;
	}
}


void check_undeclared(Ast *node)
{
	int i;

	if(node==0)
		return;

	if(node->symbol == 0){

	}
	else if(node->symbol->type == SYMBOL_IDENTIFIER)
	{
		fprintf(stderr, "Semantic ERROR: Identifier %s not declared on line %d\n", node->symbol->text, node->symbol->line);
		erroSemantico = 1;
	}

	for(i=0; i<MAX_SONS; i++)
		check_undeclared(node->son[i]);
}

int check_listlit(Ast *node, int dataType)
{
	int erro = 0;

	if(node->son[1] != 0 && node->son[1]->symbol != 0)
	{
		if(node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
			erro = check_listlit(node->son[3], SYMBOL_DATATYPE_FLOAT);
		else
			erro = check_listlit(node->son[3], SYMBOL_DATATYPE_INT);
	}

	if(dataType == SYMBOL_DATATYPE_INT)
	{
		if(node->son[0] != 0 && node->son[0]->symbol != 0 && node->son[0]->symbol->type == SYMBOL_FLOAT)
		{
			fprintf(stderr, "Semantic ERROR: Vector must receive an integer value\n");
			fprintf(stderr, "		-> ");
			erroSemantico = 1;
			erro = 1;
		}
	}
	else
	{
		if(node->son[0] != 0 && node->son[0]->symbol != 0 && node->son[0]->symbol->type != SYMBOL_FLOAT)
		{
			fprintf(stderr, "Semantic ERROR: Vector must receive a floating value\n");
			fprintf(stderr, "		-> ");
			erroSemantico = 1;
			erro = 1;
		}
	}

	if(erro == 0 && node->son[1] != 0)
		erro = check_listlit(node->son[1], dataType);

	return erro;
}

void check_ID_type(Ast *node)
{
	int i;
	Ast *listlit;

	if(node == 0)
		return;

	for(i=0; i<MAX_SONS; i++)
		check_ID_type(node->son[i]);

	if(node->type != AST_VECTOR && node->type != AST_VEC_DEC_INIT && node->type != AST_VEC_DEC && node->type != AST_ATRIBUICAO_VEC)
	{
		if((node->son[0] != 0 && node->son[0]->symbol != 0 &&  node->son[0]->symbol->type == SYMBOL_VECTOR) ||
			(node->son[1] != 0 && node->son[1]->symbol != 0 &&  node->son[1]->symbol->type == SYMBOL_VECTOR) || 
			(node->son[2] != 0 && node->son[2]->symbol != 0 &&  node->son[2]->symbol->type == SYMBOL_VECTOR) ||
			(node->son[3] != 0 && node->son[3]->symbol != 0 &&  node->son[3]->symbol->type == SYMBOL_VECTOR))
		{
			fprintf(stderr, "Semantic ERROR: Vector could not be indexed\n");
			fprintf(stderr, "		-> ");
			print_expression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}
	else
	{
		if(node->son[0]->symbol != 0 &&  node->son[0]->symbol->type != SYMBOL_VECTOR)
		{
			fprintf(stderr, "Semantic ERROR: Variable could not be indexed\n");
			fprintf(stderr, "		-> ");
			print_expression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}

	if(node->type != AST_HEADER && node->type != AST_CALL_FUNCTION)
	{
		if((node->son[0] != 0 && node->son[0]->symbol != 0 &&  node->son[0]->symbol->type == SYMBOL_FUNCTION) ||
			(node->son[1] != 0 && node->son[1]->symbol != 0 &&  node->son[1]->symbol->type == SYMBOL_FUNCTION) || 
			(node->son[2] != 0 && node->son[2]->symbol != 0 &&  node->son[2]->symbol->type == SYMBOL_FUNCTION) ||
			(node->son[3] != 0 && node->son[3]->symbol != 0 &&  node->son[3]->symbol->type == SYMBOL_FUNCTION))
		{
			fprintf(stderr, "Semantic ERROR: Functions should only be used with calls\n");
			fprintf(stderr, "		-> ");
			print_expression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}
	else
	{
		if(node->son[0]->symbol != 0 &&  node->son[0]->symbol->type != SYMBOL_FUNCTION && node->son[0]->symbol->type != SYMBOL_VECTOR)
		{
			fprintf(stderr, "Semantic ERROR: Variable cannot be used as a function\n");
			fprintf(stderr, "		-> ");
			print_expression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}

	if(node->type == AST_ATRIBUICAO)
	{
		if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
		{
			if(node->son[1]->symbol != 0 && (node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[1]->symbol->type == SYMBOL_FLOAT))
			{}
			else if(node->son[1]->symbol == 0 && node->son[1]->dataType == AST_DATATYPE_FLUTUANTE)
			{}
			else
			{
				fprintf(stderr, "Semantic ERROR: Variable must receive a floating value\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
		else
		{
			if((node->son[1]->symbol != 0 && (node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[1]->symbol->type == SYMBOL_FLOAT)) ||
				(node->son[1]->symbol == 0 && (node->son[1]->dataType == AST_DATATYPE_FLUTUANTE || node->son[1]->dataType == AST_DATATYPE_BOOLEANO)))
			{
				fprintf(stderr, "Semantic ERROR: Variable must receive an integer value\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
	}
	else if(node->type == AST_ATRIBUICAO_VEC)
	{
		if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
		{
			if(node->son[2]->symbol != 0 && (node->son[2]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[2]->symbol->type == SYMBOL_FLOAT))
			{}
			else if(node->son[2]->symbol == 0 && node->son[2]->dataType == AST_DATATYPE_FLUTUANTE)
			{}
			else
			{
				fprintf(stderr, "Semantic ERROR: Variable must receive a floating value\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
		else
		{
			if((node->son[2]->symbol != 0 && (node->son[2]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[2]->symbol->type == SYMBOL_FLOAT)) ||
				(node->son[2]->symbol == 0 && (node->son[2]->dataType == AST_DATATYPE_FLUTUANTE || node->son[2]->dataType == AST_DATATYPE_BOOLEANO)))
			{
				fprintf(stderr, "Semantic ERROR: Variable must receive an integer value\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
	}
	else if(node->type == AST_VARIAVEL_DEC)
	{
		if(node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
		{
			if(node->son[2]->symbol->type == SYMBOL_FLOAT)
			{}
			else
			{
				fprintf(stderr, "Semantic ERROR: Variable must receive a floating value\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
		else
		{
			if(node->son[2]->symbol->type == SYMBOL_FLOAT)
			{
				fprintf(stderr, "Semantic ERROR: Variable must receive an integer value\n");
				fprintf(stderr, "		-> ");
				print_expression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
	}
	else if(node->type == AST_VEC_DEC_INIT)
	{
		node = node->son[1]->son[0];
		if(check_listlit(node, 0))
		{
			print_expression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
		}
 }


void print_expression(Ast *ast){
  if(!ast) return;
  if(ast){
    switch(ast->type){      
                case AST_SYMBOL:
                    fprintf(stderr,"%s",ast->symbol->text);
                    break;
                case AST_AND:
                    print_expression(ast->son[0]); fprintf(stderr," and "); print_expression(ast->son[1]);
                    break;
                case AST_OR:
                    print_expression(ast->son[0]); fprintf(stderr," or "); print_expression(ast->son[1]);
                    break;
                case AST_ADD:
                    print_expression(ast->son[0]); fprintf(stderr,"+"); print_expression(ast->son[1]);
                    break;
                case AST_SUB:
                    print_expression(ast->son[0]); fprintf(stderr,"-"); print_expression(ast->son[1]);
                    break;
                case AST_MULT:
                    print_expression(ast->son[0]); fprintf(stderr,"*"); print_expression(ast->son[1]);
                    break;
                case AST_DIV:
                    print_expression(ast->son[0]); fprintf(stderr,"/"); print_expression(ast->son[1]);
                    break;
                case AST_LESS:
                    print_expression(ast->son[0]); fprintf(stderr,"<"); print_expression(ast->son[1]);
                    break;
                case AST_GREATER:
                    print_expression(ast->son[0]); fprintf(stderr,">"); print_expression(ast->son[1]);
                    break;
                case AST_LE:
                    print_expression(ast->son[0]); fprintf(stderr,"<="); print_expression(ast->son[1]);
                    break;
                case AST_GE:
                    print_expression(ast->son[0]); fprintf(stderr,">="); print_expression(ast->son[1]);
                    break;
                case AST_EQUAL:
                    print_expression(ast->son[0]); fprintf(stderr,"=="); print_expression(ast->son[1]);
                    break;
                case AST_NEG:
                    fprintf(stderr,"~");print_expression(ast->son[0]);  
                    break;
                case AST_DIF:
                    fprintf(stderr,"!=");print_expression(ast->son[0]);  
                    break;
                case AST_RANGE:
                    print_expression(ast->son[0]); fprintf(stderr,".."); print_expression(ast->son[1]); 
                    break;
                case AST_READ:
                    fprintf(stderr,"read ");print_expression(ast->son[0]);
                    break;
                case AST_RETURN:
                    fprintf(stderr,"return "); print_expression(ast->son[0]);
                    break;
                case AST_UNTIL:
                    fprintf(stderr,"until ("); print_expression(ast->son[0]); fprintf(stderr,")"); print_expression(ast->son[1]);
                    break;
                case AST_COMEFROM:
                    fprintf(stderr,"comefrom");
                    break;
                case AST_CALL_FUNCTION:
                    print_expression(ast->son[0]); fprintf(stderr,"("); print_expression(ast->son[1]); fprintf(stderr,")");
                    break;
                case AST_ASSOCIATIVIDADE:
                    fprintf(stderr,"("); print_expression(ast->son[0]); fprintf(stderr,")");
                    break;
                case AST_BODY:
                    fprintf(stderr,"{\n"); fprintf(stderr," "); print_expression(ast->son[0]); fprintf(stderr,"}");
                    break;
                case AST_PARAM:
                    print_expression(ast->son[0]); print_expression(ast->son[1]);
                    break; 
                case AST_ATRIBUICAO:
                    print_expression(ast->son[0]); fprintf(stderr,"="); print_expression(ast->son[1]);
                    break;
                case AST_VECTOR:
                    print_expression(ast->son[0]); fprintf(stderr,"["); print_expression(ast->son[1]); fprintf(stderr,"]");
                    break;
                case AST_PRINT:
                    fprintf(stderr,"print "); print_expression(ast->son[0]); 
                    break;
                case AST_PRINTLIST:
                    if(ast->son[1] != NULL){
                          print_expression(ast->son[0]); fprintf(stderr,","); print_expression(ast->son[1]);
                    }else
                        print_expression(ast->son[0]);
                    break;
                case AST_LISTCMD:
                    if(ast->son[1] != NULL){
                          print_expression(ast->son[0]); fprintf(stderr,";\n"); print_expression(ast->son[1]);
                    }else
                          print_expression(ast->son[0]);
                    break;
                
                case AST_LISTDEC:    
                  if(ast->son[1] != NULL){
                            print_expression(ast->son[0]); fprintf(stderr,"\n"); print_expression(ast->son[1]);
                        }
                  else
                            print_expression(ast->son[0]);
        
                    break;
                case AST_LISTPARAM:
                    if(ast->son[1] != NULL){
                          print_expression(ast->son[0]); fprintf(stderr,","); print_expression(ast->son[1]);
                    }else
                          print_expression(ast->son[0]);
                    break;
                case AST_LISTLIT:
                  if(ast->son[1] != NULL){
                          print_expression(ast->son[0]); fprintf(stderr," "); print_expression(ast->son[1]);
                    }else
                          print_expression(ast->son[0]);
                    break;
                case AST_VARIAVEL_DEC:
                    print_expression(ast->son[0]); fprintf(stderr,": ");print_expression(ast->son[1]); fprintf(stderr,"="); print_expression(ast->son[2]);fprintf(stderr,";");
                    break;
    		case AST_DATA:
                     fprintf(stderr,"data{\n");print_expression(ast->son[0]); print_expression(ast->son[1]); fprintf(stderr,"\n}"); 
                    break;            
				case AST_CHAR:
                    fprintf(stderr,"char "); 
                    break;
                case AST_INT:
                    fprintf(stderr,"int "); 
                    break;
                case AST_HEADER:
                     print_expression(ast->son[0]); fprintf(stderr,": ");print_expression(ast->son[1]);fprintf(stderr,"(");print_expression(ast->son[2]);fprintf(stderr,")\n");
                    break;
                case AST_FUNCAO:
                     print_expression(ast->son[0]); print_expression(ast->son[1]); fprintf(stderr,";");
                    break;
    			case AST_LISTFUN:    
                    if(ast->son[1] != NULL){
                            fprintf(stderr,"\n"); print_expression(ast->son[0]); print_expression(ast->son[1]);
                    }else
                        print_expression(ast->son[0]);
                    break;            
				case AST_LISTEXP:
                    if(ast->son[1] != NULL){
                    print_expression(ast->son[0]); fprintf(stderr,","); print_expression(ast->son[1]);;
                    }else
                          print_expression(ast->son[0]);
                    break;
                case AST_FLOAT:
                    fprintf(stderr,"float ");
                    break;
                case AST_ATRIBUICAO_VEC:
                    print_expression(ast->son[0]);fprintf(stderr,"[");print_expression(ast->son[1]);fprintf(stderr,"]");fprintf(stderr,"=");print_expression(ast->son[2]);
                    break;
                case AST_IF:
                    fprintf(stderr,"if ("); print_expression(ast->son[0]); fprintf(stderr,")");fprintf(stderr," \n"); print_expression(ast->son[1]);
                    if(ast->son[2] != NULL){
                    	print_expression(ast->son[2]);
                    }
                    break;
                case AST_ELSE:
                    fprintf(stderr," else \n"); print_expression(ast->son[0]);
                    break;  
                 case AST_VEC_DEC:
                    print_expression(ast->son[0]); fprintf(stderr,"["); print_expression(ast->son[1]); fprintf(stderr,"]: ");print_expression(ast->son[2]); fprintf(stderr,"= ");print_expression(ast->son[3]);
                    fprintf(stderr,";");
                    break;
                case AST_VEC_DEC_INIT:
                     print_expression(ast->son[0]); fprintf(stderr,"["); print_expression(ast->son[1]); fprintf(stderr,"]: ");print_expression(ast->son[2]);
                    if(ast->son[3]!=NULL)
                    fprintf(stderr,":"); print_expression(ast->son[3]);
                    fprintf(stderr,";");
                    break;
                case AST_TOP:
                    print_expression(ast->son[0]); print_expression(ast->son[1]);
                    break;
                default:
                  printf("ERROR\n");
                break;
    }
  }
  return;
}

