%option yylineno

%{
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "calc3.h"
#include "y.tab.h"
void yyerror(char *);
%}

%%

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"for"           return FOR;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"geti"          return GETI;
"getc"          return GETC;
"gets"          return GETS;
"puti_"         return PUTI_;
"putc_"         return PUTC_;
"puts_"         return PUTS_;
"puti"          return PUTI;
"putc"          return PUTC;
"puts"          return PUTS;
"print"         return PRINT;
"read"          return READ;
"&&"            return AND;
"||"            return OR;
"do"            return DO;
"break"         return BREAK;
"continue"      return CONTINUE;
"array"         return ARRAY;
"return"        return RETURN;
[a-zA-Z][a-zA-Z0-9]*   { 
                char* s = strdup(yytext);
                int i;
                for (i=0;s[i];i++)
                {
                    if (s[i] >='0' && s[i]<='9') continue;
                    s[i]=tolower(s[i]);
                }
                /* coverting to lowercase */
                yylval.var =s ;
                return VARIABLE;
            }

0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }
\'.\'         {
                yylval.cValue = yytext[1];//any character
                return CHAR;
            }
\"[^"\n]*\"        {
                yylval.sValue = strdup(yytext+1);
                yylval.sValue[yyleng-2] = 0;   //without the quotes
                return STRING;
            }
[-()<>=+*/;{}.%,@_\[\]] {
                return *yytext;
             }



\/\/.*\n	;	/* skip comments */

[ \t\n\r]+    ;   /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}

void yyerror(char *s) {
    fprintf(stderr, "line %d: %s\n", yylineno, s);
}