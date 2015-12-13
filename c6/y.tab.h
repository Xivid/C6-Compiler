/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTEGER = 258,
    VARIABLE = 259,
    CHAR = 260,
    STRING = 261,
    FOR = 262,
    WHILE = 263,
    IF = 264,
    PRINT = 265,
    READ = 266,
    DO = 267,
    BREAK = 268,
    CONTINUE = 269,
    ARRAY = 270,
    RETURN = 271,
    GETI = 272,
    GETC = 273,
    GETS = 274,
    PUTI = 275,
    PUTC = 276,
    PUTS = 277,
    PUTI_ = 278,
    PUTC_ = 279,
    PUTS_ = 280,
    STRINGX = 281,
    IFX = 282,
    ELSE = 283,
    AND = 284,
    OR = 285,
    GE = 286,
    LE = 287,
    EQ = 288,
    NE = 289,
    UMINUS = 290
  };
#endif
/* Tokens.  */
#define INTEGER 258
#define VARIABLE 259
#define CHAR 260
#define STRING 261
#define FOR 262
#define WHILE 263
#define IF 264
#define PRINT 265
#define READ 266
#define DO 267
#define BREAK 268
#define CONTINUE 269
#define ARRAY 270
#define RETURN 271
#define GETI 272
#define GETC 273
#define GETS 274
#define PUTI 275
#define PUTC 276
#define PUTS 277
#define PUTI_ 278
#define PUTC_ 279
#define PUTS_ 280
#define STRINGX 281
#define IFX 282
#define ELSE 283
#define AND 284
#define OR 285
#define GE 286
#define LE 287
#define EQ 288
#define NE 289
#define UMINUS 290

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 28 "c5.y" /* yacc.c:1909  */

    int iValue;                 /* integer value */
    char* var;                /* symbol table variable name */
    nodeType *nPtr;             /* node pointer */
    char cValue; /* char value*/
    char* sValue; /*string value*/

#line 132 "y.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
