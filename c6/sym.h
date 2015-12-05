#ifndef SYM_H
#define SYM_H

#include <string.h>
#define TABLE_SIZE 101 
#define STACK_SIZE 10
#define NAME_MAX 13
typedef enum {typeInt,typeChar,typeString,typeFunc,typeArray} typeEnum;

typedef struct{
	int no;
	char** paramlist;
}PARAMLIST;

typedef struct {
	int index;
}variableType;

typedef struct {
	int label;
	PARAMLIST* params;
}functionType;

typedef struct {
	typeEnum type;
	int size;
	int index;//base index
}arrayType;
typedef struct{
	char* name;
	typeEnum type;
	union{
		variableType var; 
		functionType func; 
		arrayType array;
	};	
}ENTRY;

typedef ENTRY* HASH_TABLE[TABLE_SIZE];  

typedef struct{
	HASH_TABLE* stack[STACK_SIZE];
	int count;

}SCOPE_STACK;

extern SCOPE_STACK* ss;

#endif 