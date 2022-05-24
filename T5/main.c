//Pedro Wachsmann Schanzer de Oliveira 208612

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>


//lex.yy.h
int yylex();
int yyparse();
extern char *yytext;
extern FILE *yyin;
extern FILE* file();
extern int yylex();
extern int isRunning();
extern int getLineNumber();
extern int yyparse();
extern void initMe();
extern void hashPrint();
extern Ast* getAst();

FILE *out;

void uncompile(Ast *ast){
  if(!ast) return;
  if(ast){
    printf("Node type: %d\n",ast->type);
    switch(ast->type){      
                case AST_SYMBOL:
                    fprintf(out,"%s",ast->symbol->text);
                    break;
                case AST_AND:
                    uncompile(ast->son[0]); fprintf(out,"&"); uncompile(ast->son[1]);
                    break;
                case AST_OR:
                    uncompile(ast->son[0]); fprintf(out,"|"); uncompile(ast->son[1]);
                    break;
                case AST_ADD:
                    uncompile(ast->son[0]); fprintf(out,"+"); uncompile(ast->son[1]);
                    break;
                case AST_SUB:
                    uncompile(ast->son[0]); fprintf(out,"-"); uncompile(ast->son[1]);
                    break;
                case AST_MULT:
                    uncompile(ast->son[0]); fprintf(out,"*"); uncompile(ast->son[1]);
                    break;
                case AST_DIV:
                    uncompile(ast->son[0]); fprintf(out,"/"); uncompile(ast->son[1]);
                    break;
                case AST_LESS:
                    uncompile(ast->son[0]); fprintf(out,"<"); uncompile(ast->son[1]);
                    break;
                case AST_GREATER:
                    uncompile(ast->son[0]); fprintf(out,">"); uncompile(ast->son[1]);
                    break;
                case AST_LE:
                    uncompile(ast->son[0]); fprintf(out,"<="); uncompile(ast->son[1]);
                    break;
                case AST_GE:
                    uncompile(ast->son[0]); fprintf(out,">="); uncompile(ast->son[1]);
                    break;
                case AST_EQUAL:
                    uncompile(ast->son[0]); fprintf(out,"=="); uncompile(ast->son[1]);
                    break;
                case AST_NEG:
                    fprintf(out,"~"); uncompile(ast->son[0]);  
                    break;
                case AST_DIF:
                    fprintf(out,"!="); uncompile(ast->son[0]);  
                    break;
                case AST_RANGE:
                    uncompile(ast->son[0]); fprintf(out,".."); uncompile(ast->son[1]); 
                    break;
                case AST_READ:
                    fprintf(out,"read ");uncompile(ast->son[0]);
                    break;
                case AST_RETURN:
                    fprintf(out,"return "); uncompile(ast->son[0]);
                    break;
                case AST_UNTIL:
                    fprintf(out,"until ("); uncompile(ast->son[0]); fprintf(out,")"); uncompile(ast->son[1]);
                    break;
                case AST_COMEFROM:
                    fprintf(out,"comefrom: "); uncompile(ast->son[0]);
                    break;
                case AST_CALL_FUNCTION:
                    uncompile(ast->son[0]); fprintf(out,"("); uncompile(ast->son[1]); fprintf(out,")");
                    break;
                case AST_ASSOCIATIVIDADE:
                    fprintf(out,"("); uncompile(ast->son[0]); fprintf(out,")");
                    break;
                case AST_BODY:
                    fprintf(out,"{\n"); fprintf(out," "); uncompile(ast->son[0]); fprintf(out,"}\n");
                    break;
                case AST_PARAM:
                    uncompile(ast->son[0]); fprintf(out,": "); uncompile(ast->son[1]);
                    break; 
                case AST_ATRIBUICAO:
                    uncompile(ast->son[0]); fprintf(out,"="); uncompile(ast->son[1]);
                    break;
                case AST_VECTOR:
                    uncompile(ast->son[0]); fprintf(out,"["); uncompile(ast->son[1]); fprintf(out,"]");
                    break;
                case AST_PRINT:
                    fprintf(out,"print "); uncompile(ast->son[0]); 
                    break;
                case AST_PRINTLIST:
                    if(ast->son[1] != NULL){
                          uncompile(ast->son[0]); fprintf(out,","); uncompile(ast->son[1]);
                    }else
                        uncompile(ast->son[0]);
                    break;
                case AST_LISTCMD:
                    if(ast->son[1] != NULL){
                          uncompile(ast->son[0]); fprintf(out,";\n"); uncompile(ast->son[1]);
                    }else
                          uncompile(ast->son[0]);
                    break;
                case AST_LISTDEC:    
                  if(ast->son[1] != NULL){
                            uncompile(ast->son[0]); fprintf(out,"\n"); uncompile(ast->son[1]);
                    }else
                            uncompile(ast->son[0]);
                    break;
                case AST_LISTPARAM:
                    if(ast->son[1] != NULL){
                          uncompile(ast->son[0]); fprintf(out,","); uncompile(ast->son[1]);
                    }else
                          uncompile(ast->son[0]);
                    break;
                case AST_LISTLIT:
                    if(ast->son[1] != NULL){
                          uncompile(ast->son[0]); fprintf(out," "); uncompile(ast->son[1]);
                    }else
                          uncompile(ast->son[0]);
                    break;
                case AST_VARIAVEL_DEC:
                    uncompile(ast->son[0]); fprintf(out,": "); uncompile(ast->son[1]); fprintf(out,"="); uncompile(ast->son[2]);fprintf(out,";");
                    break;
                case AST_DATA:
                     fprintf(out,"data{\n");uncompile(ast->son[0]); uncompile(ast->son[1]); fprintf(out,"\n}"); 
                    break;
                case AST_INT:
                    fprintf(out,"int "); 
                    break;
                case AST_CHAR:
                    fprintf(out,"char "); 
                    break;
                case AST_HEADER:
                     uncompile(ast->son[0]); fprintf(out,": ");uncompile(ast->son[1]);fprintf(out,"(");uncompile(ast->son[2]);fprintf(out,")\n");
                    break;
                case AST_FUNCAO:
                     uncompile(ast->son[0]); uncompile(ast->son[1]); fprintf(out,"\n");
                    break;
                case AST_LISTFUN:    
                    if(ast->son[1] != NULL){
                            fprintf(out,"\n"); uncompile(ast->son[0]); uncompile(ast->son[1]);
                    }else
                        uncompile(ast->son[0]);
                    break;
                case AST_LISTEXP:
                    if(ast->son[1] != NULL){
                    uncompile(ast->son[0]); fprintf(out,","); uncompile(ast->son[1]);;
                    }else
                          uncompile(ast->son[0]);
                    break;
                case AST_FLOAT:
                    fprintf(out,"float ");
                    break;
                case AST_ATRIBUICAO_VEC:
                    uncompile(ast->son[0]);fprintf(out,"[");uncompile(ast->son[1]);fprintf(out,"]");fprintf(out,"=");uncompile(ast->son[2]);
                    break;
                case AST_IF:
                    fprintf(out,"if ("); uncompile(ast->son[0]); fprintf(out,")");fprintf(out," \n"); uncompile(ast->son[1]);
                    if(ast->son[2] != NULL){
                    	uncompile(ast->son[2]);
                    }
                    break;
                case AST_ELSE:
                    fprintf(out," else \n"); uncompile(ast->son[0]);
                    break;  
                case AST_VEC_DEC:
                    uncompile(ast->son[0]); fprintf(out,"["); uncompile(ast->son[1]); fprintf(out,"]: ");uncompile(ast->son[2]); fprintf(out,"= ");uncompile(ast->son[3]);
                    fprintf(out,";");
                    break;
                case AST_VEC_DEC_INIT:
                     uncompile(ast->son[0]); fprintf(out,"["); uncompile(ast->son[1]); fprintf(out,"]: ");uncompile(ast->son[2]);
                    if(ast->son[3]!=NULL)
                    fprintf(out,":"); uncompile(ast->son[3]);
                    fprintf(out,";");
                    break;
                case AST_TOP:
                    uncompile(ast->son[0]); uncompile(ast->son[1]);
                    break;
                default:
                  printf("ERROR\n");
                break;
    }
  }
  return;
}

int isRunning(void);
void initMe(void);

int main(int argc, char** argv)
  {
FILE *gold = 0;
  int tok = 0;
  int answar = 0;
  int nota = 0;
  int i=1;
  int fecha;
  if (argc < 1)
    {
    printf("call: ./etapa5 input.txt output.txt \n");
    exit(1);
    }
  if (0==(yyin = fopen(argv[1],"r")))
    {
    printf("Cannot open file %s... \n",argv[1]);
    exit(2);
    }
  initMe();
  yyparse();
  
Ast *ast = getAst();
  out = fopen(argv[2], "w");
  if(out == NULL){
          fprintf(stderr,"Cannot open file %s.\n",argv[2]);
    exit(2);
    }else{
        
     uncompile(ast);}
      
  if(fecha = fclose(out) != 0)
    fprintf(stderr,"Error closing file %s.\n",argv[2]);
  exit(0);
} 
