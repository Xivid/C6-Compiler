#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "sym.h"
#include "sym.c"

//the number of labels
static int lbl;
//the number of variables on stack
static int var;
static int oldvar;
char* name;
PARAMLIST* pl;
int i;
//l1 for continue 
//l2 for break
int ex(nodeType *p,int l1,int l2) {
    int lblx, lbly,lblz, lbl1, lbl2;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:       
        printf("\tpush\t%d\n", p->con.value); 
        var++;
        break;
    case typeId: 
        //push lookup id.name       
        printf("\tpush\tfp[%d]\n", lookup(p->id.name)->var.index); 
        var++;
        break;
    case typeOpr:
        switch(p->opr.oper) {
    case '$':
    printf("\tjmp\tL%03d\n", lbl1 = lbl++);
    if (p->opr.nops>3){
        ex(p->opr.op[1],l1,l2);
       
    }
    printf("L%03d:\n", lbl2 = lbl++);
    insert_func((p->opr.op[0])->id.name,lbl,pl);
    //insert param list into scope 
    //new scope 
    allocate_ht();//already pushed to stack
    //insert params 
    char** varl = pl->paramlist;
    i= pl->no;
    while (1){
    insert_var(*varl,-(3+i),typeInt);
    printf("//inserted %s at fp[%d]\n",*varl,-(3+i));
    i--;
    if (i==0) break;
    varl++;
    }

    pl=NULL;//release param list
    oldvar = var;
    var =0;//fp starts from 0
    ex(p->opr.op[2],l1,l2);//body
    ex(p->opr.op[3],l1,l2);//return
    printf("L%03d:\n",lbl1);
    break;
    case ':':
        if (p->opr.nops==1){
            name = p->opr.op[0]->id.name;
            pl = paramlist();
            add_param(pl,name);
        }
        else{
            ex(p->opr.op[0],-1,-1);
            name = p->opr.op[1]->id.name;
            add_param(pl,name);
        }
        break;
    case RETURN:
        //RETURN VARIABLE ';' {$$=opr(RETURN,1,id($2));}
        name = p->opr.op[0]->id.name;
        printf("\tpush\tfp[%d]\n", lookup(name)->var.index);
        var ++;
        //pass the type of return value 
        printf("\tret\n");
        free_ht();//pop ht
        //var should be restored to the value of fp 
        var =oldvar;
        break;
    case '#':
        ex(p->opr.op[1],l1,l2);//create pl
        //VARIABLE '(' params ')'  {$$ = opr('#',2,id($1),$3)
        //lookup function
        ENTRY* f;
        f= lookup(p->opr.op[0]->id.name);
        if (f == NULL) {
            printf("function %s not defined!",p->opr.op[0]->id.name);
            return;
        }
        //push arguments 
        char** al = pl->paramlist;
        int paramn = pl->no;
        i=paramn;
        while (1){

            printf("\tpush\tfp[%d]\n",lookup(*al)->var.index);
            var ++;
            i--;
            if (i==0) break;
            al++;
        }
        
        pl=NULL;//release param list
        printf("\tcall L%03d, %d\n",f->func.label,f->func.params->no);
        var = var -paramn;
        insert_var(name,var,typeInt);
        var++;
        //add return value to scope
        break;
    case CONTINUE:
        if (l1 != -1) printf("\tjmp\tL%03d\n", l1);
        else printf("invalid continue !\n");
        return;
    case BREAK:
        if (l2!= -1) printf("\tjmp\tL%03d\n", l2);
        else printf("invalid break !\n");
        return;
	case FOR:
		ex(p->opr.op[0],l1,l2);
		printf("L%03d:\n", lblx = lbl++);
		ex(p->opr.op[1],l1,l2);
		printf("\tj0\tL%03d\n", lbly = lbl++);
        var--;
        lblz = lbl++;
		ex(p->opr.op[3],lblz,lbly);
        printf("L%03d:\n", lblz);
		ex(p->opr.op[2],l1,l2);
		printf("\tjmp\tL%03d\n", lblx);
		printf("L%03d:\n", lbly);
		break;
    case WHILE:
        printf("L%03d:\n", lbl1 = lbl++);
        ex(p->opr.op[0],l1,l2);
        printf("\tj0\tL%03d\n", lbl2 = lbl++);
        var--;
        ex(p->opr.op[1],lbl1,lbl2);
        printf("\tjmp\tL%03d\n", lbl1);
        printf("L%03d:\n", lbl2);
        break;
    case DO:
    //do stmt while (expr)
        printf("L%03d:\n", lbl1 = lbl++);
        lbl2 = lbl++;
        ex(p->opr.op[0],lbl1,lbl2);
        ex(p->opr.op[1],l1,l2);
        printf("\tj0\tL%03d\n", lbl2);
        var--;
        printf("\tjmp\tL%03d\n", lbl1);
        printf("L%03d:\n", lbl2);
        break;
    case IF:
        ex(p->opr.op[0],l1,l2);
        if (p->opr.nops > 2) {
            /* if else */
            printf("\tj0\tL%03d\n", lbl1 = lbl++);
            var--;
            ex(p->opr.op[1],l1,l2);
            printf("\tjmp\tL%03d\n", lbl2 = lbl++);
            printf("L%03d:\n", lbl1);
            ex(p->opr.op[2],l1,l2);
            printf("L%03d:\n", lbl2);
        } else {
            /* if */
            printf("\tj0\tL%03d\n", lbl1 = lbl++);
            var--;
            ex(p->opr.op[1],l1,l2);
            printf("L%03d:\n", lbl1);
        }
        break;
	case READ:
        printf("\tgeti\n");
        var++;
        name = p->opr.op[0]->id.name;
        printf("//variable %s from input, saved at fp[%d]\n",name,var-1);
        insert_var(name,var-1,typeInt);
	    break;
    case PRINT:     
        ex(p->opr.op[0],l1,l2);
        //case of the result type
        printf("\tputi\n");
        var--;
        break;
    case '=':       
        ex(p->opr.op[1],l1,l2);
        name = p->opr.op[0]->id.name;
        if (lookup(name) ==NULL){
            //redundant push
            printf("\tpush\tsp[-1]\n");
            var++;
            printf("//variable %s not defined, saved at fp[%d]\n",name,var-1);
            insert_var(name,var-1,typeInt);
        } 
        printf("\tpop\tfp[%d]\n", lookup(name)->var.index);
        var--;
        break;
    case UMINUS:    
        ex(p->opr.op[0],l1,l2);
        printf("\tneg\n");
        break;
    default:
        ex(p->opr.op[0],l1,l2);
        ex(p->opr.op[1],l1,l2);
        switch(p->opr.oper) {
        case '+':   printf("\tadd\n"); var--;break;
        case '-':   printf("\tsub\n"); var--;break; 
        case '*':   printf("\tmul\n"); var--;break;
        case '/':   printf("\tdiv\n"); var--;break;
        case '%':   printf("\tmod\n"); var--;break;
        case '<':   printf("\tcomplt\n"); var--;break;
        case '>':   printf("\tcompgt\n"); var--;break;
        case GE:    printf("\tcompge\n"); var--;break;
        case LE:    printf("\tcomple\n"); var--;break;
        case NE:    printf("\tcompne\n"); var--;break;
        case EQ:    printf("\tcompeq\n"); var--;break;
        case AND:   printf("\tand\n"); var--;break;
        case OR:    printf("\tor\n"); var--;break;
        }
        }
        printf("//fp now at %d\n",var);
    }
    return 0;
}
