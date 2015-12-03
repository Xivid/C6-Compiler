#include "sym.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//scope 
SCOPE_STACK* init_scope()
{
	SCOPE_STACK* scope = (SCOPE_STACK*) (malloc (sizeof (SCOPE_STACK)));
	scope->count =0;
	return scope;
}

HASH_TABLE* current_ht(SCOPE_STACK* s)
{	
	if (s->count ==0) 
	{
		printf("Error: scope stack empty\n");
	}
	int i=s->count -1;
	return s->stack[i];  
}

void push_ht(SCOPE_STACK* s,HASH_TABLE* ht)
{	if (s->count == STACK_SIZE-1) printf("scope stack is full!\n" );
	s->stack[s->count++] = ht;
	return;
}
void pop_ht(SCOPE_STACK* s)
{	if (s->count ==0) 
	{
		printf("Error: scope stack empty\n");
	}
	s->count--;
	return;
}
//symbol table 
//internal hash function
int hash(char* name)
{
	int result=5381;
	int i=0;

	while (name[i] != '\0')
	{
		result = (result<<5) +result+ name[i];i++;

	}
	return result % TABLE_SIZE;
}

//djb2 hash algorithm for strings
unsigned long djb2(unsigned char *str)
    {
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }

//allocate_ht for every function call
void allocate_ht()
{
	HASH_TABLE* ht = (HASH_TABLE*) (malloc ((sizeof(ENTRY)+sizeof(char)*NAME_MAX)*TABLE_SIZE));
	int i;
	for (i=0;i<TABLE_SIZE;i++)
	{
		(*ht)[i].status=0;//empty
		(*ht)[i].index=-1;
	}
	push_ht(ss,ht);
	return;
}
void free_ht()
{
	HASH_TABLE* ht = current_ht(ss);
	//free every entry
	int i=0;
	for (;i<TABLE_SIZE;i++)
	{	free((*ht)[i].name);
		free(ht[i]);
	}
	free(ht);
	pop_ht(ss);
	return;
}

//fp[index] = value
void insert_sym(char* name,int index,typeEnum type)
{	if (strlen(name)>=NAME_MAX) {
		printf("Error:%s name too long\n",name);return;
	}
	HASH_TABLE* ht = current_ht(ss);
	int h = hash(name);
	int original= h;
	while ((*ht)[h].status !=0 ) {
		h=(h+1)%TABLE_SIZE;
		if (h == original) printf("Error: sym table full!\n");
	}
	(*ht)[h].name = strdup(name);
	(*ht)[h].status = 1;
	(*ht)[h].index = index;
	(*ht)[h].type = type;
	return;

}

int lookup_sym_index(char* name)
{	HASH_TABLE* ht = current_ht(ss);
	int h = hash(name);
	int original= h;
	if ((*ht)[h].status==0) {return -1;}//not found 
	else {
		while ((strcmp((*ht)[h].name,name)!=0) && ((*ht)[h].status==1))//stores another entry
		{
			h=(h+1)%TABLE_SIZE;
			if (h == original) return -1;//not found
		}

	}
	return (*ht)[h].index;
}



