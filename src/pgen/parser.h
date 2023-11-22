/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PGEN_PARSER_H_INCLUDED
# define YY_YY_SRC_PGEN_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    PARSER = 258,                  /* PARSER  */
    IS = 259,                      /* IS  */
    OBJECT = 260,                  /* OBJECT  */
    WITH = 261,                    /* WITH  */
    AS = 262,                      /* AS  */
    USE = 263,                     /* USE  */
    FATARROW = 264,                /* FATARROW  */
    INCLUDE = 265,                 /* INCLUDE  */
    OPAREN = 266,                  /* OPAREN  */
    CPAREN = 267,                  /* CPAREN  */
    SEMICOLON = 268,               /* SEMICOLON  */
    ARRAY = 269,                   /* ARRAY  */
    OF = 270,                      /* OF  */
    COMMA = 271,                   /* COMMA  */
    SELECT = 272,                  /* SELECT  */
    STRLIT = 273,                  /* STRLIT  */
    IDENT = 274                    /* IDENT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define PARSER 258
#define IS 259
#define OBJECT 260
#define WITH 261
#define AS 262
#define USE 263
#define FATARROW 264
#define INCLUDE 265
#define OPAREN 266
#define CPAREN 267
#define SEMICOLON 268
#define ARRAY 269
#define OF 270
#define COMMA 271
#define SELECT 272
#define STRLIT 273
#define IDENT 274

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 53 "../src/pgen/parser.y"

	struct strlit       strlit;
	struct ident        ident;
	struct objentry     objentry;
	struct objentry    *objentries;
	struct objparser    objparser;
	struct arrayparser  arrayparser;

#line 114 "src/pgen/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PGEN_PARSER_H_INCLUDED  */
