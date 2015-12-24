#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "sym.h"
#include "sym.c"

//the number of labels
static int lbl = 0;
int local;
int offset;
//in the global scope sb = fp
//l1 for continue 
//l2 for break
int ex(nodeType *p,int l1,int l2,int* fp) {
    int lblx, lbly,lblz, lbl1, lbl2,i,j,argn;
    int* size;
    int arrlength, initval, arrbase;
    char* initstr;
    char* name;
    nodeType* temp;
    PARAMLIST* pl;
    ENTRY* ep;
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
            printf("\tpush\t\"");
            char* c;
            for(c = p->str.value; *c; c++)
                if (*c == '\n') printf("\\n");
                else if (*c == '\t') printf("\\t");
                else if (*c == '\\') printf("\\");
                else if (*c == '\"') printf("\\\"");
                else printf("%c", *c);
            printf("\"\n");
            (*fp)++;
            break;
        case typeId:
            ep = local_lookup(p->id.name);
            if (ep== NULL)
            {
                printf("Variable %s not defined!\n",p->id.name);
                return 1;
            }
            else {
                if(ep->type==typeVar){
                printf("\tpush\tfp[%d]\n", ep->var.index); 
                (*fp)++;
                }
                else{
                    if(ep->type==typePointer){
                        printf("\tpush\tfp[%d]\n",ep->pointer.pos);
                        printf("\tpop\tin\n");
                        printf("\tpush\tfp[in]\n");
                        (*fp)++;
                    }
                }
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
                        insert_array(name, *fp, temp->opr.nops, size);
                    }
                    else {
                        printf("error: identifier %s has been used.\n", name);
                        return 1;
                    }
                    if (p->opr.nops == 1) {
                        //printf("//uninitialized definition\n");
                        printf("\tpush\tsp; push\t%d; add; pop\tsp\n", arrlength);
                        (*fp) += arrlength;
                    }
                    else {
                        //printf("//initialized definition: %s\n", name);
                        if (p->opr.op[1]->type == typeCon) {
                            initval = p->opr.op[1]->con.value;
                            printf("\tpush\t%d\n", arrlength - 1);
                            printf("L%03d:\n", lblx = lbl++);
                            printf("\tpush\tfp[%d]\n", *fp); // loop counter
                            printf("\tj0\tL%03d\n", lbly = lbl++);
                            printf("\tpush\t%d\n", initval);
                            printf("\tpush\tfp[%d]; push\t1; sub; pop\tfp[%d]\n", *fp, *fp);
                            printf("\tjmp\tL%03d\n", lblx);
                            printf("L%03d:\n", lbly);
                            printf("\tpush\t%d; pop\tfp[%d]\n", initval, *fp);
                        } 
                        else if(p->opr.op[1]->type == typeCh) { //initialize with char
                                initval = p->opr.op[1]->ch.value;
                                printf("\tpush\t%d\n", arrlength - 1);
                                printf("L%03d:\n", lblx = lbl++);
                                printf("\tpush\tfp[%d]\n", *fp);
                                printf("\tj0\tL%03d\n", lbly = lbl++);
                                if (initval == '\n')
                                    printf("\tpush\t'\\n'\n");
                                else if (initval == '\t')
                                    printf("\tpush\t'\\t'\n");
                                else if (initval == '\'')
                                    printf("\tpush\t'\\''\n");
                                else
                                    printf("\tpush\t'%c'\n", initval);
                                printf("\tpush\tfp[%d]; push\t1; sub; pop\tfp[%d]\n", *fp, *fp);
                                printf("\tjmp\tL%03d\n", lblx);
                                printf("L%03d:\n", lbly);
                                printf("\tpush\t'%c'; pop\tfp[%d]\n", initval, *fp);

                        }
                        else {//initialize with string
                            initstr = p->opr.op[1]->str.value;
                            for (i=0;i<strlen(initstr);i++){
                                if (initstr[i] == '\n')
                                    printf("\tpush\t'\\n'\n");
                                else if (initstr[i] == '\t')
                                    printf("\tpush\t'\\t'\n");
                                else if (initstr[i] == '\'')
                                    printf("\tpush\t'\\''\n");
                                else
                                    printf("\tpush\t'%c'\n", initstr[i]);
                            }
                            for (i=0;i<arrlength-strlen(initstr);i++)
                                printf("\tpush\t''\n");
                        }
                        
                        (*fp) += arrlength;
                        
                    } 
                    break;
                case '@'://global variable
                    name = (p->opr.op[0])->id.name;
                    printf("\tpush\tsb[%d]\n", global_lookup(name)->var.index); 
                    (*fp)++;
                    break;
                case '&'://pointer
                    name = (p->opr.op[0])->id.name;
                    ep = local_lookup(name);
                    if (ep==NULL) {
                        printf("variable %s not declared!\n",name);
                        return 1;
                    }
                    else{
                        if (ep->type==typeVar){
                            printf("\tpush\t%d\n",ep->var.index-offset);
                            (*fp)++;
                            break;
                        }
                        if(ep->type==typeArray){
                            printf("\tpush\t%d\n",ep->array.base-offset);
                            (*fp)++;
                            break;
                        }
                    }
                    break;
                case '$'://function def
                    printf("\tjmp\tL%03d\n", lbl1 = lbl++);
                    if (p->opr.nops>2){
                        nodeType* args = p->opr.op[1];
                        //create param list from argument op[1]
                        pl = paramlist();
                        for (i=0;i<args->opr.nops;i++)
                        {   temp = args->opr.op[i];
                            if(temp->type==typeId)
                            {name = temp->id.name;
                            add_param(pl,name,typeVar);
                            }
                            else if(temp->type == typeOpr && temp->opr.nops == 1)
                                {
                                name = temp->opr.op[0]->id.name;
                                add_param(pl,name,typePointer);
                                }
                                else{//array pointer
                                    name = temp->opr.op[0]->opr.op[0]->id.name;
                                    add_param(pl,name,typeArrayPointer);
                                }
                        }
                    }
                    printf("L%03d:\n", lbl2 = lbl++);
                    //labels are globally seen so it doesn't change here
                    //printf("//number of params %d\n",pl->no);
                    insert_func((p->opr.op[0])->id.name,lbl2,pl);
                    //insert param list into scope 
                    //new scope 
                    allocate_ht();//already pushed to stack
                    //need a deep copy of pl!
                    insert_func((p->opr.op[0])->id.name,lbl2,cpypl(pl));
                    local = 1;
                    //insert params 
                    PARAM* pnode = pl->head;
                    i=1;
                    while (pnode!=NULL){
                        if (pnode->type == typeVar) insert_var(pnode->name,-(3+i));
                        else{
                            if (pnode->type==typePointer) insert_pointer(pnode->name,-(3+i));
                            else{//if type == typeArrayPointer
                                //covert [2,3,4] to size and ndim
                                temp = p->opr.op[1]->opr.op[pl->no-i]->opr.op[0]->opr.op[1]; // the '|' node (arguments)
                                size = (int*) malloc(sizeof(int) * temp->opr.nops);
                                for (j = 0; j < temp->opr.nops; ++j) {
                                    size[j] = temp->opr.op[j]->con.value;}
                            insert_array_pointer(pnode->name,-(3+i), temp->opr.nops, size);
                            }
                        } 
                        //printf("//inserted %s at fp[%d]\n",pnode->name,-(3+i));
                        i++;
                        pnode = pnode->next;
                    }
                    pl=NULL;//release param list?
                    i=0;
                    int* newfp = &i;
                    ex(p->opr.op[2],l1,l2,newfp);//body
                    free_ht();
                    if (current_ht(ss) == global_ht(ss)) local =0;
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
                    //VARIABLE '(' arguments ')'  {$$ = opr('#',2,id($1),$3)
                    //lookup function
                    ep= local_lookup(p->opr.op[0]->id.name);
                    if (ep == NULL) {
                        printf("function %s not defined!",p->opr.op[0]->id.name);
                        return 1;
                    }
                    offset = (*fp)+p->opr.op[1]->opr.nops +3;
                    ex(p->opr.op[1],l1,l2,fp);//execute arguments (push)
                    printf("\tcall L%03d, %d\n",ep->func.label,p->opr.op[1]->opr.nops);
                    (*fp) = (*fp) - (p->opr.op[1])->opr.nops;
                    //end of function
                    offset=0;
                    //add return value to scope
                    (*fp)++;
                    //printf("//inserted return value at fp[%d]\n",(*fp)-1);
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
                        temp = p->opr.op[0]->opr.op[i];
                        if (temp->type == typeOpr && temp->opr.oper == '[') {
                            name = temp->opr.op[0]->id.name;
                            if ((ep = local_lookup(name)) == NULL) {
                                printf("No such array!\n");
                                return 1;
                            }
                            temp = temp->opr.op[1]; // the '|' node (arguments)
                            if (ep->type == typeArray) {
                                if (temp->opr.nops != ep->array.ndim) {
                                    printf("error: array %s dimensions do not match!\n", name);
                                    return 1;
                                }
                                size = ep->array.size;
                                arrbase = ep->array.base;
                                ex(temp->opr.op[0], l1, l2,fp); //push index(0)
                                if (temp->opr.nops > 1) {
                                    printf("\tpush\t%d; mul\n", size[1]);
                                    for (i = 1; i < temp->opr.nops - 1; ++i) {
                                        ex(temp->opr.op[i], l1, l2,fp); // push index(i)
                                        printf("\tadd; push\t%d; mul\n", size[i+1]);
                                        (*fp)--;
                                    }
                                    ex(temp->opr.op[p->opr.nops - 1], l1, l2,fp);
                                    printf("\tadd\n");
                                    (*fp)--;
                                }
                                printf("\tpush\t%d; add\n", arrbase);
                                printf("\tpop\tin; pop\tfp[in]\n");
                                (*fp) -=2;
                            }
                        } else {
                            name = p->opr.op[0]->opr.op[i]->id.name;
                        
                            if (local_lookup(name)==NULL){
                                 //redundant push
                                printf("\tpush\tsp[-1]\n");
                                (*fp)++;
                                //printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                                insert_var(name,(*fp)-2);
                            }
                            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                            (*fp)--;
                        }
                    }
                    
                    break;
                case GETC:
                    argn=p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++){
                        printf("\tgetc\n");
                        (*fp)++;
                        temp = p->opr.op[0]->opr.op[i];
                        if (temp->type == typeOpr && temp->opr.oper == '[') {
                            name = temp->opr.op[0]->id.name;
                            if ((ep = local_lookup(name)) == NULL) {
                                printf("No such array!\n");
                                return 1;
                            }
                            temp = temp->opr.op[1]; // the '|' node (arguments)
                            if (ep->type == typeArray) {
                                if (temp->opr.nops != ep->array.ndim) {
                                    printf("error: array %s dimensions do not match!\n", name);
                                    return 1;
                                }
                                size = ep->array.size;
                                arrbase = ep->array.base;
                                ex(temp->opr.op[0], l1, l2,fp); //push index(0)
                                if (temp->opr.nops > 1) {
                                    printf("\tpush\t%d; mul\n", size[1]);
                                    for (i = 1; i < temp->opr.nops - 1; ++i) {
                                        ex(temp->opr.op[i], l1, l2,fp); // push index(i)
                                        printf("\tadd; push\t%d; mul\n", size[i+1]);
                                        (*fp)--;
                                    }
                                    ex(temp->opr.op[p->opr.nops - 1], l1, l2,fp);
                                    printf("\tadd\n");
                                    (*fp)--;
                                }
                                printf("\tpush\t%d; add\n", arrbase);
                                printf("\tpop\tin; pop\tfp[in]\n");
                                (*fp) -=2;
                            }
                        } else {
                            name = p->opr.op[0]->opr.op[i]->id.name;
                        
                            if (local_lookup(name)==NULL){
                                 //redundant push
                                printf("\tpush\tsp[-1]\n");
                                (*fp)++;
                                //printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                                insert_var(name,(*fp)-2);
                            }
                            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                            (*fp)--;
                        }
                    }
                    break;
                case GETS:
                    argn=p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++){
                         printf("\tgets\n");
                        (*fp)++;
                        temp = p->opr.op[0]->opr.op[i];
                        if (temp->type == typeOpr && temp->opr.oper == '[') {
                            name = temp->opr.op[0]->id.name;
                            if ((ep = local_lookup(name)) == NULL) {
                                printf("No such array!\n");
                                return 1;
                            }
                            temp = temp->opr.op[1]; // the '|' node (arguments)
                            if (ep->type == typeArray) {
                                if (temp->opr.nops != ep->array.ndim) {
                                    printf("error: array %s dimensions do not match!\n", name);
                                    return 1;
                                }
                                size = ep->array.size;
                                arrbase = ep->array.base;
                                ex(temp->opr.op[0], l1, l2,fp); //push index(0)
                                if (temp->opr.nops > 1) {
                                    printf("\tpush\t%d; mul\n", size[1]);
                                    for (i = 1; i < temp->opr.nops - 1; ++i) {
                                        ex(temp->opr.op[i], l1, l2,fp); // push index(i)
                                        printf("\tadd; push\t%d; mul\n", size[i+1]);
                                        (*fp)--;
                                    }
                                    ex(temp->opr.op[p->opr.nops - 1], l1, l2,fp);
                                    printf("\tadd\n");
                                    (*fp)--;
                                }
                                printf("\tpush\t%d; add\n", arrbase);
                                printf("\tpop\tin; pop\tfp[in]\n");
                                (*fp) -=2;
                            }
                        } else {
                            name = p->opr.op[0]->opr.op[i]->id.name;
                        
                            if (local_lookup(name)==NULL){
                                 //redundant push
                                printf("\tpush\tsp[-1]\n");
                                (*fp)++;
                                //printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                                insert_var(name,(*fp)-2);
                            }
                            printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                            (*fp)--;
                        }
                    }
                    break;
                case READ:
                    printf("\tgeti\n");
                    (*fp)++;
                    temp = p->opr.op[0];
                    if (temp->type == typeOpr && temp->opr.oper == '[') {
                        name = temp->opr.op[0]->id.name;
                        if ((ep = local_lookup(name)) == NULL) {
                            printf("No such array!\n");
                            return 1;
                        }
                        temp = temp->opr.op[1]; // the '|' node (arguments)
                        if (ep->type == typeArray) {
                            if (temp->opr.nops != ep->array.ndim) {
                                printf("error: array %s dimensions do not match!\n", name);
                                return 1;
                            }
                            size = ep->array.size;
                            arrbase = ep->array.base;
                            ex(temp->opr.op[0], l1, l2,fp); //push index(0)
                            if (temp->opr.nops > 1) {
                                printf("\tpush\t%d; mul\n", size[1]);
                                for (i = 1; i < temp->opr.nops - 1; ++i) {
                                    ex(temp->opr.op[i], l1, l2,fp); // push index(i)
                                    printf("\tadd; push\t%d; mul\n", size[i+1]);
                                    (*fp)--;
                                }
                                ex(temp->opr.op[p->opr.nops - 1], l1, l2,fp);
                                printf("\tadd\n");
                                (*fp)--;
                            }
                            printf("\tpush\t%d; add\n", arrbase);
                            printf("\tpop\tin; pop\tfp[in]\n");
                            (*fp) -=2;
                        }
                    } else {
                        name = p->opr.op[0]->opr.op[i]->id.name;
                    
                        if (local_lookup(name)==NULL){
                             //redundant push
                            printf("\tpush\tsp[-1]\n");
                            (*fp)++;
                            //printf("//variable %s from input, saved at fp[%d]\n",name,(*fp)-2);
                            insert_var(name,(*fp)-2);
                        }
                        printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                        (*fp)--;
                    }
                    break;
                case PRINT:     
                    ex(p->opr.op[0],l1,l2,fp);
                    printf("\tputi\n");
                    (*fp)--;
                    break;
                case PUTI:
                    temp = p->opr.op[0];
                    argn = temp->opr.nops;
                    if (argn ==1 || temp->opr.op[0]->type != typeStr) {
                        for (i=0;i<argn;i++) {
                            if ((temp->opr.op[i]->type == typeId) && (local_lookup(temp->opr.op[i]->id.name)->type == typeArray))
                            {   ep = local_lookup(temp->opr.op[i]->id.name);
                                for (j=0;j<*(ep->array.size)-1;j++){
                                    printf("\tpush\tfp[%d]\n",ep->array.base+j);
                                    printf("\tputi_\n");
                                }
                                    printf("\tpush\tfp[%d]\n",ep->array.base+j);
                                    printf("\tputi\n");//newline here
                            }
                            else{
                            ex(temp->opr.op[i],-1,-1,fp);
                            printf("\tputi\n");//with newline
                            (*fp)--;
                            }
                    }
                    }
                    else {//format string 
                        char* format = temp->opr.op[0]->str.value;
                        for (i=1;i<argn;i++) {
                            ex(temp->opr.op[i],-1,-1,fp);
                            printf("\tputi\t\"%s\"\n",format);//with newline
                            (*fp)--;
                        }
                    }
                    break;
                case PUTI_:
                    argn = p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++) {
                        ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                        printf("\tputi_\n");//without newline
                        (*fp)--;
                    } 
                    break;
                case PUTC:
                    argn = p->opr.op[0]->opr.nops;
                    if (argn == 1 || p->opr.op[0]->opr.op[0]->type != typeStr) {
                        for (i=0;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputc\n");//with newline
                            (*fp)--;
                        }
                    }
                    else {
                         char* format = p->opr.op[0]->opr.op[0]->str.value;
                         for (i=1;i<argn;i++) {
                            ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                            printf("\tputc\t\"%s\"\n",format);//with newline
                            (*fp)--;
                        }
                    }
                    break;
                case PUTC_:
                    argn = p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++) {
                        ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                        printf("\tputc_\n");//without newline
                        (*fp)--;
                    }
                    break;
                case PUTS:
                    temp = p->opr.op[0];
                    argn = temp->opr.nops;
                    if (argn ==1 || temp->opr.op[0]->type != typeStr) {
                        //without format string
                        for (i=0;i<argn;i++) {
                            if (temp->opr.op[i]->type == typeId && local_lookup(temp->opr.op[i]->id.name)->type == typeArray){
                                ep = local_lookup(temp->opr.op[i]->id.name);
                                for (j=0;j<*(ep->array.size)-1;j++){
                                    printf("\tpush\tfp[%d]\n",ep->array.base+j);
                                    printf("\tputc_\n");
                                }
                                    printf("\tpush\tfp[%d]\n",ep->array.base+j);
                                    printf("\tputc\n");//newline here
                            }
                            else {
                                ex(temp->opr.op[i],-1,-1,fp);
                                printf("\tputs\n");//with newline
                                (*fp)--;
                            }
                            
                        }
                    }
                    else {
                         char* format = temp->opr.op[0]->str.value;
                         for (i=1;i<argn;i++) {
                            ex(temp->opr.op[i],-1,-1,fp);
                            printf("\tputs\t\"%s\"\n",format);//with newline
                            (*fp)--;
                        }
                    }
                    break;
                case PUTS_:
                    argn = p->opr.op[0]->opr.nops;
                    for (i=0;i<argn;i++) {
                        ex(p->opr.op[0]->opr.op[i],-1,-1,fp);
                        printf("\tputs_\n");//without newline
                        (*fp)--;
                        }
                    
                    break;
                case '[': // array element
                    name = p->opr.op[0]->id.name;
                    if ((ep = local_lookup(name)) == NULL) {
                        printf("error: array %s undeclared.\n", name);
                    } else if (ep->type != typeArray && ep->type != typeArrayPointer) {
                        printf("error: %s is not an array\n", name);
                    } else {
                        if (ep->type == typeArray){
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
                        else{
                            //arraypointer
                            p = p->opr.op[1]; // the '|' node (arguments)
                            if (p->opr.nops != ep->ap.ndim) {
                                printf("error: array %s dimensions do not match!\n", name);
                                return 1;
                            }
                            size = ep->ap.size;
                            int pos = ep->ap.basepos;
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
                            printf("\tpush\tfp[%d]; add\n",pos);
                            printf("\tpop\tin; push\tfp[in]\n");
                        }
                    }
                    break;
                case '=':       
                    ex(p->opr.op[1],l1,l2,fp);
                    if (p->opr.op[0]->type == typeId){
                        name = p->opr.op[0]->id.name;
                        ep = local_lookup(name);
                        if (p->opr.nops == 2){
                            if (ep== NULL){
                                //redundant push
                                printf("\tpush\tsp[-1]\n");
                                (*fp)++;
                                //printf("//variable %s not defined, saved at fp[%d]\n",name,(*fp)-2);
                                insert_var(name,(*fp)-2);
                                printf("\tpop\tfp[%d]\n", local_lookup(name)->var.index);
                                (*fp)--; 
                            } 
                            else {
                                if (ep->type == typeVar){
                                    printf("\tpop\tfp[%d]\n", ep->var.index);
                                    (*fp)--; 
                                }
                                else{
                                    if(ep->type==typePointer){
                                        printf("\tpush\tfp[%d]\n",ep->pointer.pos);
                                        printf("\tpop\tin\n");
                                        printf("\tpop\tfp[in]\n");
                                        (*fp)--;
                                    }
                                }
                            }
                            
                        } else {
                            //global variable 
                            if (global_lookup(name) == NULL){
                                if (local ==1) printf("global variable %s not defined!\n",name);
                                else {
                                    //redundant push
                                    printf("\tpush\tsp[-1]\n");
                                    (*fp)++;
                                    //printf("//variable %s not defined, saved at fp[%d]\n",name,(*fp)-2);
                                    insert_var(name,(*fp)-2);
                                }
                            }
                            printf("\tpop\tsb[%d]\n", global_lookup(name)->var.index);
                            (*fp)--;

                        }
                    } else {
                        //array element or pointer
                        name = p->opr.op[0]->opr.op[0]->id.name;
                        if ((ep = local_lookup(name)) == NULL) {
                            printf("error: array %s undeclared.\n", name);
                        } else if (ep->type != typeArray && ep->type!=typeArrayPointer) {
                            printf("error: %s is not an array\n", name);
                        } else {
                            p = p->opr.op[0]->opr.op[1]; // the '|' node (arguments)
                            if (ep->type == typeArray){
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
                            else{//array pointer
                                    if (p->opr.nops != ep->ap.ndim) {
                                    printf("error: array %s dimensions do not match!\n", name);
                                    return 1;
                                    }
                                    size = ep->ap.size;
                                    int pos = ep->ap.basepos;
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
                                    printf("\tpush\tfp[%d]; add\n",pos);
                                    printf("\tpop\tin; pop\tfp[in]\n");
                                    (*fp) -=2;
                            }
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
