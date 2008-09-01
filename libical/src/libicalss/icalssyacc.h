/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRING = 258,
     SELECT = 259,
     FROM = 260,
     WHERE = 261,
     COMMA = 262,
     QUOTE = 263,
     EQUALS = 264,
     NOTEQUALS = 265,
     LESS = 266,
     GREATER = 267,
     LESSEQUALS = 268,
     GREATEREQUALS = 269,
     AND = 270,
     OR = 271,
     EOL = 272,
     END = 273,
     IS = 274,
     NOT = 275,
     SQLNULL = 276
   };
#endif
#define STRING 258
#define SELECT 259
#define FROM 260
#define WHERE 261
#define COMMA 262
#define QUOTE 263
#define EQUALS 264
#define NOTEQUALS 265
#define LESS 266
#define GREATER 267
#define LESSEQUALS 268
#define GREATEREQUALS 269
#define AND 270
#define OR 271
#define EOL 272
#define END 273
#define IS 274
#define NOT 275
#define SQLNULL 276




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 59 "icalssyacc.y"
typedef union YYSTYPE {
	char* v_string;
} YYSTYPE;
/* Line 1275 of yacc.c.  */
#line 83 "icalssyacc.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE sslval;



