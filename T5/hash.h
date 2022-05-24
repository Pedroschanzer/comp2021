//Pedro Wachsmann Schanzer de Oliveira 208612

#ifndef HASH_H
#define HASH_H

#define HASH_SIZE 997

#define SYMBOL_INTEGER 		1
#define SYMBOL_CHAR 		2
#define SYMBOL_FLOAT 		3
#define SYMBOL_STRING 		4
#define SYMBOL_IDENTIFIER 	5

#define SYMBOL_VARIABLE		6
#define SYMBOL_FUNCTION		7
#define SYMBOL_VECTOR		8


#define SYMBOL_DATATYPE_INT			1
#define SYMBOL_DATATYPE_BOOL		2
#define SYMBOL_DATATYPE_FLOAT		3
#define SYMBOL_DATATYPE_CHAR        4

typedef struct element
{
    char* text;
    int type;
    int datatype;
    int line;
    struct element *next;
} Node;


Node* hashTable[HASH_SIZE];

void hashInit(Node** ht);
Node* hashInsert(char* key, int type, int line);
Node* hashFind(const char* key);

void hashCheckUndeclared();

Node* makeTemp();
Node* makeLabel();



#endif 