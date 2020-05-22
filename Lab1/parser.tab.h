
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     ID = 259,
     RELOP = 260,
     TYPE = 261,
     FLOAT = 262,
     CHAR = 263,
     LP = 264,
     RP = 265,
     LB = 266,
     RB = 267,
     LC = 268,
     RC = 269,
     SEMI = 270,
     COMMA = 271,
     PLUS = 272,
     MINUS = 273,
     STAR = 274,
     DIV = 275,
     ASSIGNOP = 276,
     AND = 277,
     OR = 278,
     NOT = 279,
     IF = 280,
     ELSE = 281,
     WHILE = 282,
     RETURN = 283,
     FOR = 284,
     BREAK = 285,
     CONTINUE = 286,
     DOT = 287,
     SWITCH = 288,
     CASE = 289,
     COLON = 290,
     DEFAULT = 291,
     EXT_DEF_LIST = 292,
     EXT_VAR_DEF = 293,
     FUNC_DEF = 294,
     FUNC_DEC = 295,
     EXT_DEC_LIST = 296,
     PARAM_LIST = 297,
     PARAM_DEC = 298,
     VAR_DEF = 299,
     DEC_LIST = 300,
     DEF_LIST = 301,
     COMP_STM = 302,
     STM_LIST = 303,
     EXP_STMT = 304,
     IF_THEN = 305,
     IF_THEN_ELSE = 306,
     FUNC_CALL = 307,
     ARGS = 308,
     FUNCTION = 309,
     PARAM = 310,
     ARG = 311,
     CALL = 312,
     LABEL = 313,
     GOTO = 314,
     JLT = 315,
     JLE = 316,
     JGT = 317,
     JGE = 318,
     EQ = 319,
     NEQ = 320,
     ARRAY = 321,
     STRUCT = 322,
     INC = 323,
     DEC = 324,
     SELFPLUS = 325,
     SELFMINUS = 326,
     SELFSTAR = 327,
     SELFDIV = 328,
     STRUCT_DEF = 329,
     STRUCT_TYPE = 330,
     STRUCT_VISIT = 331,
     UMINUS = 332,
     LOWER_THEN_ELSE = 333
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1740 of yacc.c  */
#line 16 "parser.y"

	int    type_int;
	float  type_float;
    char   type_char;
	char   type_id[32];
	struct ASTNode *ptr;



/* Line 1740 of yacc.c  */
#line 140 "parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;

