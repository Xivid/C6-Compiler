typedef enum { typeCh,typeStr,typeCon, typeId, typeOpr } nodeEnum;

typedef struct {
    char value;
}charNodeType;

typedef struct{
    char* value;
}stringNodeType;

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

/* identifiers */
typedef struct {
    char* name;                      /* identifier name */
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        charNodeType ch;
        stringNodeType str;
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;


extern int sym[26];