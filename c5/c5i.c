#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "sym.h"
#include "sym.c"

static int cont = 0; // = 1 if "continue" has been encountered
static int brk = 0; // = 1 if "break" has been encountered
static int scope = 0; // outermost scope - no continue/break allowed

int ex(nodeType *p,int l1,int l2) {
    int i;
    if (!p) return 0;
    switch(p->type) {
    case typeCon:       return p->con.value;
    case typeId:        return sym[p->id.name[0]];//lookup the value
    case typeOpr:
        switch(p->opr.oper) {
    case BREAK: if (scope !=0) 
                {brk=1;
                    //printf("Break set to 1\n");
                } 
                else printf("No break allowed in outer scope\n");
                return 0;
    case CONTINUE: if (scope !=0) 
                    {cont=1; 
                        //printf("Continue set to 1\n");
                    } 
                    else printf("No continue allowed in outer scope\n");
                    return 0;
	case FOR:	if ((brk==0) && (cont==0)){
                ex(p->opr.op[0],-1,-1);
			    while (ex(p->opr.op[1],-1,-1)) {
                scope++;
				ex(p->opr.op[3],-1,-1);//break or continue in here
                scope--;
                cont =0;
                if (brk==1) break; 
				ex(p->opr.op[2],-1,-1);
			    }
                brk=0;
                }    
			return 0;
    case WHILE: if ((brk==0) && (cont==0)){
                    while(ex(p->opr.op[0],-1,-1)) {
                        scope++;
                        ex(p->opr.op[1],-1,-1);//break or continue
                        scope--;
                        cont=0;
                        if (brk==1) break; 
                    }
                    brk=0;
                }
                return 0;
    case DO:    if ((brk==0) && (cont==0)){
                    do {
                        scope++;
                        ex(p->opr.op[0],-1,-1);//break or continue
                        scope--;
                        cont=0;
                        if (brk==1) break; 
                    } 
                    while (ex(p->opr.op[1],-1,-1));
                    brk=0;
                }
                return 0; 
    case IF:    if ((brk==0) && (cont==0)){
                    if (ex(p->opr.op[0],-1,-1)) ex(p->opr.op[1],-1,-1);
                    else if (p->opr.nops > 2) ex(p->opr.op[2],-1,-1);
                }   
                return 0;

    case PRINT: if ((brk==0) && (cont==0)) {
                    printf("%d\n", ex(p->opr.op[0],-1,-1));
                } 
                return 0;

	case READ:	if ((brk==0) && (cont==0)) {
                printf("? "); scanf("%d", &i);
			     return sym[p->opr.op[0]->id.name[0]] = i;
                }
                else return 0;
    case ';':   if ((brk==0) && (cont==0)) {ex(p->opr.op[0],-1,-1);} return ex(p->opr.op[1],-1,-1);
    case '=':   if ((brk==0) && (cont==0)) {return sym[p->opr.op[0]->id.name[0]] = ex(p->opr.op[1],-1,-1);}
                else return 0;
    //expr
    case UMINUS:    return -ex(p->opr.op[0],-1,-1);
    case '+':       return ex(p->opr.op[0],-1,-1) + ex(p->opr.op[1],-1,-1);
    case '-':       return ex(p->opr.op[0],-1,-1) - ex(p->opr.op[1],-1,-1);
    case '*':       return ex(p->opr.op[0],-1,-1) * ex(p->opr.op[1],-1,-1);
    case '/':       return ex(p->opr.op[0],-1,-1) / ex(p->opr.op[1],-1,-1);
    case '%':       return ex(p->opr.op[0],-1,-1) % ex(p->opr.op[1],-1,-1);
    case '<':       return ex(p->opr.op[0],-1,-1) < ex(p->opr.op[1],-1,-1);
    case '>':       return ex(p->opr.op[0],-1,-1) > ex(p->opr.op[1],-1,-1);
    case GE:        return ex(p->opr.op[0],-1,-1) >= ex(p->opr.op[1],-1,-1);
    case LE:        return ex(p->opr.op[0],-1,-1) <= ex(p->opr.op[1],-1,-1);
    case NE:        return ex(p->opr.op[0],-1,-1) != ex(p->opr.op[1],-1,-1);
    case EQ:        return ex(p->opr.op[0],-1,-1) == ex(p->opr.op[1],-1,-1);
	case AND:	    return ex(p->opr.op[0],-1,-1) && ex(p->opr.op[1],-1,-1);
	case OR:	    return ex(p->opr.op[0],-1,-1) || ex(p->opr.op[1],-1,-1);
        }
    }
    return 0;
}
