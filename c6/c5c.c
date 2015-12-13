#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "sym.h"
#include "sym.c"

//the number of labels
static int lbl;
int local;
//in the global scope sb = fp
char* name;
PARAMLIST* pl;
int i;
//l1 for continue 
//l2 for break
int ex(nodeType *p,int l1,int l2,int* fp) {
    int lblx, lbly,lblz, lbl1, lbl2;
    if (!p) return 0;
    switch(p->type) {
    case typeCh:
        printf("\tpush\t'%c'\n",p->ch.value);
        (*fp)++;
        break;
    case typeCon:       
        printf("\tpush\t%d\n", p->con.value); 
        (*fp)++;
        break;
    case typeStr:
        printf("\tpush\t\"%s\"\n",p->str.value);
        (*fp)++;
        break;
    case typeId:    
        printf("\tpush\tfp[%d]\n", local_lookup(p->id.name)->var.index); 
        (*fp)++;
        break;
    case typeOpr:
        switch(p->opr.oper) {
    case '@'://global variable
        name = (p->opr.op[0])->id.name;
        printf("\tpush\tsb[%d]\n", global_lookup(p->id.name)->var.index); 
        (*fp)++;
        break;
    case '$'://function def
    printf("\tjmp\tL%03d\n", lbl1 = lbl++);
    if (p->opr.nops>2){
        nodeType* args = p->opr.op[1];
        //create param list from argument op[1]
        pl = paramlist();
        for (i=0;i<args->opr.nops;i++)
        {
            name = args->opr.op[i]->id.name;
            add_param(pl,name);
        }
    }
    printf("L%03d:\n", lbl2 = lbl++);
    //labels are globally seen so it doesn't change here
    insert_func((p->opr.op[0])->id.name,lbl,pl);
    //insert param list into scope 
    //new scope 
    allocate_ht();//already pushed to stack
    local =1;
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
    i =0;
    int* newfp = &i;
    ex(p->opr.op[2],l1,l2,newfp);//body
    free_ht();//pop ht,error checking?
    if (current_ht(ss) == global_ht(ss)) local =0;
    //var should be restored to the value of fp 
    printf("L%03d:\n",lbl1);
    break;
    case RETURN:
        //RETURN expr ';' {$$=opr(RETURN,1,$2);}
        ex(p->opr.op[0],-1,-1,fp);
        printf("\tret\n");
        (*fp)--;
        break;
    case '|'://argument list  
        for (i=0;i<p->opr.nops;i++)
            ex(p->opr.op[i],-1,-1,fp);
        break;
    case '#':
        ex(p->opr.op[1],l1,l2,fp);//execute arguments (push)
        //VARIABLE '(' arguments ')'  {$$ = opr('#',2,id($1),$3)
        //lookup function
        ENTRY* f;
        f= local_lookup(p->opr.op[0]->id.name);
        if (f == NULL) {
            printf("function %s not defined!",p->opr.op[0]->id.name);
            return;
        }
        printf("\tcall L%03d, %d\n",f->func.label,f->func.params->no);
        (*fp) = (*fp) - (p->opr.op[1])->opr.nops;
        //end of function
        //add return value to scope
        (*fp)++;
        insert_var(name,(*fp)-1,typeInt);
        printf("//inserted return value at fp[%d]\n",(*fp)-1);
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
		ex(p->opr.op[0],l1,l2,fp);
		printf("L%03d:\n", lblx = lbl++);
		ex(p->opr.op[1],l1,l2,fp);
		printf("\tj0\tL%03d\n", lbly = lbl++);
        (*fp)--;
        lblz = lbl++;
		ex(p->opr.op[3],lblz,lbly,fp);
        printf("L%03d:\n", lblz);
		ex(p->opr.op[2],l1,l2,fp);
		printf("\tjmp\tL%03d\n", lblx);
		printf("L%03d:\n", lbly);
		break;
    case WHILE:
        printf("L%03d:\n", lbl1 = lbl++);
        ex(p->opr.op[0],l1,l2,fp);
        printf("\tj0\tL%03d\n", lbl2 = lbl++);
        (*fp)--;
        ex(p->opr.op[1],lbl1,lbl2,fp);
        printf("\tjmp\tL%03d\n", lbl1);
        printf("L%03d:\n", lbl2);
        break;
    case DO:
    //do stmt while (expr)
        printf("L%03d:\n", lbl1 = lbl++);
        lbl2 = lbl++;
        ex(p->opr.op[0],lbl1,lbl2,fp);
        ex(p->opr.op[1],l1,l2,fp);
        printf("\tj0\tL%03d\n", lbl2);
        (*fp)--;
        printf("\tjmp\tL%03d\n", lbl1);
        printf("L%03d:\n", lbl2);
        break;
    case IF:
        ex(p->opr.op[0],l1,l2,fp);
        if (p->opr.nops > 2) {
            /* if else */
            printf("\tj0\tL%03d\n", lbl1 = lbl++);
            (*fp)--;
            ex(p->opr.op[1],l1,l2,fp);
            printf("\tjmp\tL%03d\n", lbl2 = lbl++);
            printf("L%03d:\n", lbl1);
            ex(p->opr.op[2],l1,l2,fp);
            printf("L%03d:\n", lbl2);
        } else {
            /* if */
            printf("\tj0\tL%03d\n", lbl1 = lbl++);
            (*fp)--;
            ex(p->opr.op[1],l1,l2,fp);
            printf("L%03d:\n", lbl1);
        }
        break;
    case GETI:
        printf("\tgeti\n");
        (*fp)++;
        name = p->opr.op[0]->id.name;
        if (p->opr.nops ==1){
            if (local_lookup(name)==NULL){
                 //redundant push
                printf("\tpush\tsp[-1]\n");
                (*fp)++;
                printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                insert_var(name,(*fp)-2,typeInt);
            }
            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
            (*fp)--;
        }
        break;
    case GETC:
        printf("\tgetc\n");
        (*fp)++;
        name = p->opr.op[0]->id.name;
        if (p->opr.nops ==1){
            if (local_lookup(name)==NULL){
                 //redundant push
                printf("\tpush\tsp[-1]\n");
                (*fp)++;
                printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                insert_var(name,(*fp)-2,typeChar);
            }
            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
            (*fp)--;
        }
        break;
    case GETS:
        printf("\tgets\n");
        (*fp)++;
        name = p->opr.op[0]->id.name;
        if (p->opr.nops ==1){
            if (local_lookup(name)==NULL){
                 //redundant push
                printf("\tpush\tsp[-1]\n");
                (*fp)++;
                printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                insert_var(name,(*fp)-2,typeString);
            }
            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
            (*fp)--;
        }
        break;
	case READ:
        printf("\tgeti\n");
        (*fp)++;
        name = p->opr.op[0]->id.name;
        if (p->opr.nops ==1){
            if (local_lookup(name)==NULL){
                 //redundant push
                printf("\tpush\tsp[-1]\n");
                (*fp)++;
                printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                insert_var(name,(*fp)-2,typeInt);
            }
            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
            (*fp)--;
        }
        else {
            //read into a global variable
        }
	    break;
    case PRINT:     
        ex(p->opr.op[0],l1,l2,fp);
        printf("\tputi\n");
        (*fp)--;
        break;
    case PUTI:
        ex(p->opr.op[0],l1,l2,fp);
        if (p->opr.nops ==1) printf("\tputi\n");//with newline
        else printf("\tputi_\n");//without newline
        (*fp)--;
        break;
    case PUTC:
        ex(p->opr.op[0],l1,l2,fp);
        if (p->opr.nops == 1) printf("\tputc\n");
        else printf("\tputc_\n");
        (*fp)--;
        break;
    case PUTS:
        ex(p->opr.op[0],l1,l2,fp);
        if (p->opr.nops == 1) printf("\tputs\n");
        else printf("\tputs_\n");
        (*fp)--;
        break;
    case '=':       
        ex(p->opr.op[1],l1,l2,fp);
        name = p->opr.op[0]->id.name;
        if (p->opr.nops == 2){
            if (local_lookup(name) ==NULL){
                //redundant push
                printf("\tpush\tsp[-1]\n");
                (*fp)++;
                printf("//variable %s not defined, saved at fp[%d]\n",name,(*fp)-2);
                insert_var(name,(*fp)-2,typeInt);
            } 
            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
            (*fp)--;
        }
        else{
            //global variable 
            if (global_lookup(name) == NULL){
                if (local ==1) printf("global variable %s not defined!\n",name);
                else {
                    //redundant push
                    printf("\tpush\tsp[-1]\n");
                    (*fp)++;
                    printf("//variable %s not defined, saved at fp[%d]\n",name,(*fp)-2);
                    insert_var(name,(*fp)-2,typeInt);
                }
            }
            printf("\tpop\tsb[%d]\n", global_lookup(name)->var.index);
            (*fp)--;

        }
        break;
    case UMINUS:    
        ex(p->opr.op[0],l1,l2,fp);
        printf("\tneg\n");
        break;
    default:
        ex(p->opr.op[0],l1,l2,fp);
        ex(p->opr.op[1],l1,l2,fp);
        switch(p->opr.oper) {
        case '+':   printf("\tadd\n"); (*fp)--;break;
        case '-':   printf("\tsub\n"); (*fp)--;break; 
        case '*':   printf("\tmul\n"); (*fp)--;break;
        case '/':   printf("\tdiv\n"); (*fp)--;break;
        case '%':   printf("\tmod\n"); (*fp)--;break;
        case '<':   printf("\tcomplt\n"); (*fp)--;break;
        case '>':   printf("\tcompgt\n"); (*fp)--;break;
        case GE:    printf("\tcompge\n"); (*fp)--;break;
        case LE:    printf("\tcomple\n"); (*fp)--;break;
        case NE:    printf("\tcompne\n"); (*fp)--;break;
        case EQ:    printf("\tcompeq\n"); (*fp)--;break;
        case AND:   printf("\tand\n"); (*fp)--;break;
        case OR:    printf("\tor\n"); (*fp)--;break;
        }
        }
    }
    return 0;
}
