#ifndef SYM_H
#define SYM_H

#include <string.h>
#define TABLE_SIZE 223 
#define STACK_SIZE 50
#define NAME_MAX 13
#define PARAM_MAX 10
typedef enum {typeVar,typeFunc,typeArray,typePointer,typeArrayPointer} typeEnum;

typedef struct PARAM{
	typeEnum type;
	char* name;
	struct PARAM* next;
}PARAM;

typedef struct{
	int no;
	PARAM* head;
}PARAMLIST;

typedef struct {
	int index;
}variableType;

typedef struct {
	int pos;
}pointerType;

typedef struct{
	int basepos;
	int ndim;
	int* size;
}arraypointerType;

typedef struct {
	int label;
	PARAMLIST* params;
}functionType;

typedef struct {
	int* size;
	int base;
	int ndim;
}arrayType;

typedef struct{
	char* name;
	typeEnum type;
	union{
		variableType var; 
		functionType func; 
		arrayType array;
		pointerType pointer;
		arraypointerType ap;
	};	
}ENTRY;

typedef ENTRY* HASH_TABLE[TABLE_SIZE];  

typedef struct{
	HASH_TABLE* stack[STACK_SIZE];
	int count;

}SCOPE_STACK;

extern SCOPE_STACK* ss;

//function prototypes
SCOPE_STACK* init_scope();
void allocate_ht();
void free_ht();

#endif 