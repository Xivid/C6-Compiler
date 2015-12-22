#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "sym.h"
#include "sym.c"

//the number of labels
static int lbl = 0;
int local;
//in the global scope sb = fp
char* name;
PARAMLIST* pl;
ENTRY* ep;
//l1 for continue 
//l2 for break
int ex(nodeType *p,int l1,int l2,int* fp) {
    int lblx, lbly,lblz, lbl1, lbl2,i,argn;
    int* size;
    int arrlength, initval, arrbase;
    char* initstr;
    nodeType* temp;
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
            if (local_lookup(p->id.name) == NULL)
            {
                printf("Variable %s not defined!\n",p->id.name);
                return 1;
            }
            else {     
                printf("\tpush\tfp[%d]\n", local_lookup(p->id.name)->var.index); 
                (*fp)++;
            }
            break;
        case typeOpr:
            switch(p->opr.oper) {
                case ',': //array definition list
                    //printf("//execute %d array definitions\n", p->opr.nops);
                    for (i = 0; i < p->opr.nops; ++i) {
                        ex(p->opr.op[i], l1, l2,fp);
                    }
                    break;
                case ARRAY: //array definition
                    arrlength = 1;
                    temp = p->opr.op[0]; // the '[' node (array)
                    name = temp->opr.op[0]->id.name;
                    if (local_lookup(name) == NULL) {
                        temp = temp->opr.op[1]; // the '|' node (arguments)
                        size = (int*) malloc(sizeof(int) * temp->opr.nops);
                        for (i = 0; i < temp->opr.nops; ++i) {
                            if (temp->opr.op[i]->type != typeCon) {
                                printf("error: array size not a constant\n");
                                return 1;
                            }
                            else {
                                if (temp->opr.op[i]->con.value < 1) {
                                    printf("error: array size not positive\n");
                                    return 1;
                                }
                                size[i] = temp->opr.op[i]->con.value;
                                //printf("//size[%d] = %d\n", i, size[i]);
                                arrlength *= size[i];
                            }
                        }
                        //printf("//insert_array(%s, %d, %d, %p, Int)\n", name, var, p->opr.nops, size);
                        insert_array(name, *fp, temp->opr.nops, size, typeVar);
                        } 
                    else {
                        printf("error: identifier %s has been used.\n", name);
                        return 1;
                        }
                    if (p->opr.nops == 1) {
                        printf("//uninitialized definition\n");
                        printf("\tpush\tsp; push\t%d; add; pop\tsp\n", arrlength);
                        (*fp) += arrlength;
                    } 
                    else {
                        printf("//initialized definition: %s\n", name);
                         if (p->opr.op[1]->type == typeCon) {
                            initval = p->opr.op[1]->con.value;
                            printf("\tpush\t%d\n", arrlength - 1);
                            printf("L%03d:\n", lblx = lbl++);
                            printf("\tpush\tfp[%d]\n", *fp);
                            printf("\tj0\tL%03d\n", lbly = lbl++);
                            printf("\tpush\t%d\n", initval);
                            printf("\tpush\tfp[%d]; push\t1; sub; pop\tfp[%d]\n", *fp, *fp);
                            printf("\tjmp\tL%03d\n", lblx);
                            printf("L%03d:\n", lbly);
                            printf("\tpush\t%d; pop\tfp[%d]\n", initval, *fp);
                        } 
                        else { //initialize with char
                            if(p->opr.op[1]->type == typeCh){
                                initval = p->opr.op[1]->ch.value;
                                printf("\tpush\t%d\n", arrlength - 1);
                                printf("L%03d:\n", lblx = lbl++);
                                printf("\tpush\tfp[%d]\n", *fp);
                                printf("\tj0\tL%03d\n", lbly = lbl++);
                                printf("\tpush\t'%c'\n", initval);
                                printf("\tpush\tfp[%d]; push\t1; sub; pop\tfp[%d]\n", *fp, *fp);
                                printf("\tjmp\tL%03d\n", lblx);
                                printf("L%03d:\n", lbly);
                                printf("\tpush\t'%c'; pop\tfp[%d]\n", initval, *fp);

                            }
                            else{//initialize with string
                                initstr = p->opr.op[1]->str.value;
                                i=0;
                                for (i;i<strlen(initstr);i++)
                                {
                                    printf("\tpush\t'%c'\n", initstr[i]);
                                }
                                printf("\tpush\t%d\n", 0);
                            }
                        }
                        (*fp) += arrlength;
                        printf("//fp now at %d\n",*fp);
                        } 
                    break;
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
                    local = 1;
                    //insert params 
                    char** varl = pl->paramlist;
                    i= pl->no;
                    while (1){
                        insert_var(*varl,-(3+i),typeVar);
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
                        return 1;
                    }
                    printf("\tcall L%03d, %d\n",f->func.label,f->func.params->no);
                    (*fp) = (*fp) - (p->opr.op[1])->opr.nops;
                    //end of function
                    //add return value to scope
                    (*fp)++;
                    insert_var(name,(*fp)-1,typeVar);
                    printf("//inserted return value at fp[%d]\n",(*fp)-1);
                    break;
                case CONTINUE:
                    if (l1 != -1) printf("\tjmp\tL%03d\n", l1);
                    else printf("invalid continue !\n");
                    return 1;
                case BREAK:
                    if (l2!= -1) printf("\tjmp\tL%03d\n", l2);
                    else printf("invalid break !\n");
                    return 1;
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
                // | GETI '(' arguments ')'     { $$ = opr(GETI, 1, $3); }
                    argn = p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++){
                        printf("\tgeti\n");
                        (*fp)++;
                        name = p->opr.op[0]->opr.op[i]->id.name;
                        
                        if (local_lookup(name)==NULL){
                             //redundant push
                            printf("\tpush\tsp[-1]\n");
                            (*fp)++;
                            printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                            insert_var(name,(*fp)-2,typeVar);
                        }
                        printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                        (*fp)--;
                    }
                    
                    break;
                case GETC:
                    argn=p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++){
                        printf("\tgetc\n");
                        (*fp)++;
                        name = p->opr.op[0]->id.name;
                    
                        if (local_lookup(name)==NULL){
                             //redundant push
                            printf("\tpush\tsp[-1]\n");
                            (*fp)++;
                            printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                            insert_var(name,(*fp)-2,typeVar);
                        }
                        printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                        (*fp)--;
                    }
                    break;
                case GETS:
                    argn=p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++){
                         printf("\tgets\n");
                        (*fp)++;
                        name = p->opr.op[0]->id.name;
                        if (local_lookup(name)==NULL){
                             //redundant push
                            printf("\tpush\tsp[-1]\n");
                            (*fp)++;
                            printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                            insert_var(name,(*fp)-2,typeVar);
                        }
                        printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                        (*fp)--;
                    }
                    break;
                case READ:
                    printf("\tgeti\n");
                    (*fp)++;
                    name = p->opr.op[0]->id.name;
                    if (local_lookup(name)==NULL){
                             //redundant push
                            printf("\tpush\tsp[-1]\n");
                            (*fp)++;
                            printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                            insert_var(name,(*fp)-2,typeVar);
                        }
                    printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                    (*fp)--;
                    break;
                case PRINT:     
                    ex(p->opr.op[0],l1,l2,fp);
                    printf("\tputi\n");
                    (*fp)--;
                    break;
                case PUTI:
                    argn = p->opr.op[0]->opr.nops;
                    if (argn ==1 || p->opr.op[0]->opr.op[0]->type != typeStr) {
                        for (i=0;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputi\n");//with newline
                        }
                    }
                    else {//format string 
                        char* format = p->opr.op[0]->opr.op[0]->str.value;
                        for (i=1;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputi\t\"%s\"\n",format);//with newline
                        }
                    }
                    (*fp)--;
                    break;
                case PUTI_:
                    argn = p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++) {
                        ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                        printf("\tputi_\n");//without newline
                        }
                     (*fp)--;
                    break;
                case PUTC:
                    argn = p->opr.op[0]->opr.nops;
                    if (argn == 1 || p->opr.op[0]->opr.op[0]->type != typeStr) {
                        for (i=0;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputc\n");//with newline
                        }
                    }
                    else {
                         char* format = p->opr.op[0]->opr.op[0]->str.value;
                         for (i=1;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputc\t\"%s\"\n",format);//with newline
                        }
                    }
                    (*fp)--;
                    break;
                case PUTC_:
                    argn = p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++) {
                        ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                        printf("\tputc_\n");//without newline
                        }
                    (*fp)--;
                    break;
                case PUTS:
                    argn = p->opr.op[0]->opr.nops;
                    if (argn ==1 || p->opr.op[0]->opr.op[0]->type != typeStr) {
                        for (i=0;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputs\n");//with newline
                        }
                    }
                    else {
                         char* format = p->opr.op[0]->opr.op[0]->str.value;
                         for (i=1;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputs\t\"%s\"\n",format);//with newline
                        }
                    }
                    (*fp)--;
                    break;
                case PUTS_:
                    argn = p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++) {
                        ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                        printf("\tputs_\n");//without newline
                        }
                    (*fp)--;
                    break;
                case '[': // array element
                    name = p->opr.op[0]->id.name;
                    if ((ep = local_lookup(name)) == NULL) {
                        printf("error: array %s undeclared.\n", name);
                    } else if (ep->type != typeArray) {
                        printf("error: %s is not an array\n", name);
                    } else {
                        p = p->opr.op[1]; // the '|' node (arguments)
                        if (p->opr.nops != ep->array.ndim) {
                            printf("error: array %s dimensions do not match!\n", name);
                            return 1;
                        }
                        size = ep->array.size;
                        arrbase = ep->array.base;
                        ex(p->opr.op[0], l1, l2,fp); //push index(0)
                        if (p->opr.nops > 1) {
                            printf("\tpush\t%d; mul\n", size[1]);
                            for (i = 1; i < p->opr.nops - 1; ++i) {
                                ex(p->opr.op[i], l1, l2,fp); // push index(i)
                                printf("\tadd; push\t%d; mul\n", size[i+1]);
                                (*fp)--;
                            }
                            ex(p->opr.op[p->opr.nops - 1], l1, l2,fp);
                            printf("\tadd\n");
                            (*fp)--;
                        }
                        printf("\tpush\t%d; add\n", arrbase);
                        printf("\tpop\tin; push\tfp[in]\n");
                    }
                    break;
                case '=':       
                    ex(p->opr.op[1],l1,l2,fp);
                    if (p->opr.op[0]->type == typeId){
                        name = p->opr.op[0]->id.name;
                        if (p->opr.nops == 2){
                            if (local_lookup(name) == NULL){
                                //redundant push
                                printf("\tpush\tsp[-1]\n");
                                (*fp)++;
                                printf("//variable %s not defined, saved at fp[%d]\n",name,(*fp)-2);
                                insert_var(name,(*fp)-2,typeVar);
                            } 
                            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                            (*fp)--;
                        } else {
                            //global variable 
                            if (global_lookup(name) == NULL){
                                if (local ==1) printf("global variable %s not defined!\n",name);
                                else {
                                    //redundant push
                                    printf("\tpush\tsp[-1]\n");
                                    (*fp)++;
                                    printf("//variable %s not defined, saved at fp[%d]\n",name,(*fp)-2);
                                    insert_var(name,(*fp)-2,typeVar);
                                }
                            }
                            printf("\tpop\tsb[%d]\n", global_lookup(name)->var.index);
                            (*fp)--;

                        }
                    } else {
                        //array element 
                        name = p->opr.op[0]->opr.op[0]->id.name;
                        if ((ep = local_lookup(name)) == NULL) {
                            printf("error: array %s undeclared.\n", name);
                        } else if (ep->type != typeArray) {
                            printf("error: %s is not an array\n", name);
                        } else {
                            p = p->opr.op[0]->opr.op[1]; // the '|' node (arguments)
                            if (p->opr.nops != ep->array.ndim) {
                                printf("error: array %s dimensions do not match!\n", name);
                                return 1;
                            }
                            size = ep->array.size;
                            arrbase = ep->array.base;
                            ex(p->opr.op[0], l1, l2,fp); //push index(0)
                            if (p->opr.nops > 1) {
                                printf("\tpush\t%d; mul\n", size[1]);
                                for (i = 1; i < p->opr.nops - 1; ++i) {
                                    ex(p->opr.op[i], l1, l2,fp); // push index(i)
                                    printf("\tadd; push\t%d; mul\n", size[i+1]);
                                    (*fp)--;
                                }
                                ex(p->opr.op[p->opr.nops - 1], l1, l2,fp);
                                printf("\tadd\n");
                                (*fp)--;
                            }
                            printf("\tpush\t%d; add\n", arrbase);
                            printf("\tpop\tin; pop\tfp[in]\n");
                            (*fp) -=2;
                        }
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
