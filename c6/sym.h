#ifndef SYM_H
#define SYM_H

#include <string.h>
#define TABLE_SIZE 101 
#define STACK_SIZE 10
#define NAME_MAX 12
typedef enum {typeInt,typeChar,typeString} typeEnum;
typedef struct{
	char* name;
	int index;
	typeEnum type;
	int status;//0 empty 1 full
}ENTRY;

typedef ENTRY HASH_TABLE[TABLE_SIZE];  

typedef struct{
	HASH_TABLE* stack[STACK_SIZE];
	int count;

}SCOPE_STACK;

extern SCOPE_STACK* ss;

#endif 