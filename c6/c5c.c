#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "sym.h"
#include "sym.c"

//the number of labels
static int lbl;
//the number of variables on stack
static int var;
char* name;
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
        printf("\tpush\tfp[%d]\n", lookup_sym_index(p->id.name)-1); 
        var++;
        break;
    case typeOpr:
        switch(p->opr.oper) {
    case '$':break;//function definition
    case ':':break;//params
    case '#':break;//function call
    case CONTINUE:
        if (l1 != -1) printf("\tjmp\tL%03d\n", l1);
        else printf("invalid continue !\n");
        break;
    case BREAK:
        if (l2!= -1) printf("\tjmp\tL%03d\n", l2);
        else printf("invalid break !\n");
        break;
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
        insert_sym(name,var,typeInt);
	    break;
    case PRINT:     
        ex(p->opr.op[0],l1,l2);
        //case of the result type
        printf("\tputi\n");
        var--;
        break;
    case '=':       
        name = p->opr.op[0]->id.name;
        if (lookup_sym_index(name) ==-1){
            //redundant push
            ex(p->opr.op[1],l1,l2);
            //printf("//variable %s not defined, saved at fp[%d]\n",name,var-1);
            insert_sym(name,var,typeInt);
        } 
        ex(p->opr.op[1],l1,l2);
        printf("\tpop\tfp[%d]\n", lookup_sym_index(name)-1);
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
    }
    return 0;
}
