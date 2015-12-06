%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "calc3.h"
#include "sym.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(char* x);
nodeType *con(int value);
void freeNode(nodeType *p);
int ex(nodeType *p,int l1,int l2);
int yylex(void);

void yyerror(char *s);
int sym[26];

SCOPE_STACK* ss;

%}

%union {
    int iValue;                 /* integer value */
    char* var;                /* symbol table variable name */
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <var> VARIABLE
%token FOR WHILE IF PRINT READ DO BREAK CONTINUE ARRAY RETURN
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list definition return params

%%

program: 
          function                { exit(0); }
        ;

definition:
         VARIABLE '(' params ')' '{' stmt_list return '}' 
         {$$= opr('$',4,id($1),$3,$6,$7); } 
         | VARIABLE '(' ')''{' stmt_list return '}' 
         {$$= opr('$',3,id($1),$5,$6); } 
         ;
params: 
        params',' VARIABLE          {$$=opr(':',2,$1,id($3));}
        | VARIABLE                  {$$=opr(':',1,id($1));}
        ;
return:
        RETURN VARIABLE ';' {$$=opr(RETURN,1,id($2));}
        ;

function:
          function stmt         { ex($2,-1,-1); freeNode($2); }
        | function definition   { ex($2,-1,-1); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | BREAK ';'                      { $$ = opr(BREAK,0);}
        | CONTINUE ';'                   { $$ = opr(CONTINUE,0);}
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
	    | READ VARIABLE ';'		         { $$ = opr(READ, 1, id($2)); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, id($1), $3); }
	    | FOR '(' stmt stmt stmt ')' stmt { $$ = opr(FOR, 4, $3, $4,
$5, $7); }
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | DO stmt WHILE '(' expr ')'     { $$ = opr(DO, 2, $2, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'              { $$ = $2; }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | VARIABLE              { $$ = id($1); }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '%' expr         { $$ = opr('%', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
	    | expr AND expr		{ $$ = opr(AND, 2, $1, $3); }
	    | expr OR expr		{ $$ = opr(OR, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        |  VARIABLE '(' params ')'  {$$ = opr('#',2,id($1),$3);}
        ;

%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType *con(int value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *id(char* x) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.name = x;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeId){
    free(p->id.name);
    }
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

int main(int argc, char **argv) {
extern FILE* yyin;
    yyin = fopen(argv[1], "r");
    ss= init_scope();//global scope stack
    allocate_ht();
    yyparse();
    free_ht();
    return 0;
}
