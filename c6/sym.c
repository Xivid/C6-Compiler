#include "sym.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//only for debugging
//SCOPE_STACK* ss;

//scope 
SCOPE_STACK* init_scope()
{
	SCOPE_STACK* scope = (SCOPE_STACK*) (malloc (sizeof (SCOPE_STACK)));
	scope->count =0;
	return scope;
}

HASH_TABLE* global_ht(SCOPE_STACK* s)
{
	return s->stack[0];
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
unsigned int hash(char* name)
{
	unsigned int result=5381;
	int i=0;

	while (name[i] != '\0')
	{
		result = (result<<5) + result + name[i]; i++;
	}
	return result % TABLE_SIZE;
}

//djb2 hash algorithm for strings
unsigned long djb2(unsigned char *str)
    {
        unsigned long hash = 5381;
        int c;

        while ((c = *str++) != 0)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }

//allocate_ht for every function call
//entry size is now variable
void allocate_ht()
{
	HASH_TABLE* ht = (HASH_TABLE*) (malloc (sizeof(HASH_TABLE)));
	int i;
	for (i=0;i<TABLE_SIZE;i++)
	{
		(*ht)[i]=NULL;//pointing to no entry
	}
	push_ht(ss,ht);
	return;
}

void propagate_ht()
{
	HASH_TABLE* ht = (HASH_TABLE*) (malloc (sizeof(HASH_TABLE)));
	memcpy(ht, current_ht(ss), sizeof(HASH_TABLE));
	push_ht(ss,ht);
	return;
}

PARAMLIST* paramlist(){
	PARAMLIST* p = malloc(sizeof(PARAMLIST));
	p->no= 0;
	p->head=NULL;
	return p;
}

void add_param(PARAMLIST* p,char* name,typeEnum type){
	if (strlen(name)>=NAME_MAX){
		printf("Error:%s name too long\n",name);return;	
	}
	PARAM* pm = malloc(sizeof(PARAM));
	pm->name= name;
	pm->type = type;
	pm->next = p->head;
	p->head = pm;
	p->no++;	
}

PARAMLIST* cpypl(PARAMLIST* p){
	PARAMLIST* p2 = paramlist();
	PARAM* pm= p->head;
	while (pm!=NULL){
		add_param(p2,pm->name,pm->type);
		pm=pm->next;
	}
	return p2;
}

ENTRY* var_entry(char* name,int index){
	ENTRY* e;
	e = malloc(sizeof(ENTRY));
	e->type =typeVar;
	e->name = strdup(name);
	e->var.index = index;
	return e;
}
ENTRY* pointer_entry(char* name,int pos){
	ENTRY* e;
	e = malloc(sizeof(ENTRY));
	e->type =typePointer;
	e->name = strdup(name);
	e->pointer.pos = pos;
	return e;

}
ENTRY* array_pointer_entry(char* name, int basepos,int ndim, int* size){
	ENTRY* e;
	e = malloc(sizeof(ENTRY));
	e->type =typeArrayPointer;
	e->name = strdup(name);
	e->ap.basepos = basepos;
	e->ap.ndim = ndim;
	e->ap.size = size;
	return e;
}
ENTRY* func_entry(char* name,int label,PARAMLIST* params){
	ENTRY* e;
	e = malloc(sizeof(ENTRY));
	e->name =strdup(name);
	e->type = typeFunc;
	e->func.label = label;
	e->func.params = params;
	return e;

}

ENTRY* array_entry(char* name,int base,int ndim, int* size){
	ENTRY* e;
	e= malloc(sizeof(ENTRY));
	e->name = strdup(name);
	e->type = typeArray;
	e->array.base = base;
	e->array.ndim = ndim;
	e->array.size = size;
	return e;
}

void free_entry(ENTRY* e){
	if (e==NULL) return;
	if (e->type == typeFunc){
		free(e->func.params);//free_params?
	}
	free(e);
}
void free_ht()
{
	HASH_TABLE* ht = current_ht(ss);
	//free every entry
	int i=0;
	for (;i<TABLE_SIZE;i++)
	{	
		free_entry((*ht)[i]);
	}
	free(ht);
	pop_ht(ss);
	return;
}
void delete_ht() {
	// delete propagated hashtable (without removing the not-really-copied entries)
	free(current_ht(ss));
	pop_ht(ss);
}
void insert_entry(char* name,ENTRY* e){
	if (strlen(name)>=NAME_MAX) {
		printf("Error:%s name too long\n",name);return;
	}
	HASH_TABLE* ht = current_ht(ss);
	int h = hash(name);
	int original= h;
	while ((*ht)[h]!=NULL ) {
		h=(h+1)%TABLE_SIZE;
		if (h == original) printf("Error: sym table full!\n");
	}
	//create variable entry 
	(*ht)[h] = e;
	return;
}
//fp[index] = value
//variable
void insert_array_pointer(char* name, int basepos, int ndim, int* size){
	insert_entry(name,array_pointer_entry(name,basepos,ndim,size));
	return;
}
void insert_pointer(char* name,int pos){
	insert_entry(name,pointer_entry(name,pos));
	return;
}
void insert_var(char* name,int index)
{	insert_entry(name,var_entry(name,index));
	return;
}

void insert_func(char* name,int label,PARAMLIST* params){
	insert_entry(name,func_entry(name,label,params));
	return;
}

void insert_array(char* name,int base,int ndim,int* size)
{
	insert_entry(name,array_entry(name,base,ndim,size));
	return;
}


//for var,function,array
ENTRY* lookup(char* name,HASH_TABLE* ht)
{	
	int h = hash(name);
	int original= h;
	if ((*ht)[h]==NULL) {return NULL;}//not found 
	else {
		while (((*ht)[h]!=NULL) && (strcmp((*ht)[h]->name,name)!=0))//stores another entry
		{
			h=(h+1)%TABLE_SIZE;
			if (h == original) return NULL;//not found
		}

	}
	return (*ht)[h];
}

ENTRY* global_lookup(char* name)
{	
	return lookup(name,global_ht(ss));
}

ENTRY* local_lookup(char* name){
	return lookup(name,current_ht(ss));
}

