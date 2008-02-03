/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EQ = 258,
     COLON = 259,
     DOT = 260,
     SEMICOLON = 261,
     SPACE = 262,
     HTAB = 263,
     LINESEP = 264,
     NEWLINE = 265,
     BEGIN_VCARD = 266,
     END_VCARD = 267,
     BEGIN_VCAL = 268,
     END_VCAL = 269,
     BEGIN_VEVENT = 270,
     END_VEVENT = 271,
     BEGIN_VTODO = 272,
     END_VTODO = 273,
     ID = 274,
     STRING = 275
   };
#endif
/* Tokens.  */
#define EQ 258
#define COLON 259
#define DOT 260
#define SEMICOLON 261
#define SPACE 262
#define HTAB 263
#define LINESEP 264
#define NEWLINE 265
#define BEGIN_VCARD 266
#define END_VCARD 267
#define BEGIN_VCAL 268
#define END_VCAL 269
#define BEGIN_VEVENT 270
#define END_VEVENT 271
#define BEGIN_VTODO 272
#define END_VTODO 273
#define ID 274
#define STRING 275




/* Copy the first part of user declarations.  */
#line 1 "vcc.y"


/***************************************************************************
(C) Copyright 1996 Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.

For purposes of this license notice, the term Licensors shall mean,
collectively, Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.
The term Licensor shall mean any of the Licensors.

Subject to acceptance of the following conditions, permission is hereby
granted by Licensors without the need for written agreement and without
license or royalty fees, to use, copy, modify and distribute this
software for any purpose.

The above copyright notice and the following four paragraphs must be
reproduced in all copies of this software and any software including
this software.

THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS AND NO LICENSOR SHALL HAVE
ANY OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS OR
MODIFICATIONS.

IN NO EVENT SHALL ANY LICENSOR BE LIABLE TO ANY PARTY FOR DIRECT,
INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOST PROFITS ARISING OUT
OF THE USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

EACH LICENSOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO ANY WARRANTY OF NONINFRINGEMENT OR THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.

The software is provided with RESTRICTED RIGHTS.  Use, duplication, or
disclosure by the government are subject to restrictions set forth in
DFARS 252.227-7013 or 48 CFR 52.227-19, as applicable.

***************************************************************************/

/*
 * src: vcc.c
 * doc: Parser for vCard and vCalendar. Note that this code is
 * generated by a yacc parser generator. Generally it should not
 * be edited by hand. The real source is vcc.y. The #line directives
 * can be commented out here to make it easier to trace through
 * in a debugger. However, if a bug is found it should
 * be fixed in vcc.y and this file regenerated.
 */


/* debugging utilities */
#if __DEBUG
#define DBG_(x) printf x
#else
#define DBG_(x)
#endif

#ifdef WIN32
#define snprintf   _snprintf
#define strcasecmp stricmp
#endif

/****  External Functions  ****/

/* assign local name to parser variables and functions so that
   we can use more than one yacc based parser.
*/

#define yyparse mime_parse
#define yylex mime_lex
#define yyerror mime_error
#define yychar mime_char
/* #define p_yyval p_mime_val */
#undef yyval
#define yyval mime_yyval
/* #define p_yylval p_mime_lval */
#undef yylval
#define yylval mime_yylval
#define yydebug mime_debug
#define yynerrs mime_nerrs
#define yyerrflag mime_errflag
#define yyss mime_ss
#define yyssp mime_ssp
#define yyvs mime_vs
#define yyvsp mime_vsp
#define yylhs mime_lhs
#define yylen mime_len
#define yydefred mime_defred
#define yydgoto mime_dgoto
#define yysindex mime_sindex
#define yyrindex mime_rindex
#define yygindex mime_gindex
#define yytable mime_table
#define yycheck mime_check
#define yyname mime_name
#define yyrule mime_rule
#define YYPREFIX "mime_"


#ifndef _NO_LINE_FOLDING
#define _SUPPORT_LINE_FOLDING 1
#endif

/* undef below if compile with MFC */
/* #define INCLUDEMFC 1 */

#if defined(WIN32) || defined(_WIN32)
#ifdef INCLUDEMFC
#include <afx.h>
#endif
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "vcc.h"

/****  Types, Constants  ****/

#define YYDEBUG		1	/* 1 to compile in some debugging code */
#define MAXTOKEN	256	/* maximum token (line) length */
#define YYSTACKSIZE 	50	/* ~unref ? */
#define MAXLEVEL	10	/* max # of nested objects parseable */
				/* (includes outermost) */


/****  Global Variables  ****/
int mime_lineNum, mime_numErrors; /* yyerror() can use these */
static VObject* vObjList;
static VObject *curProp;
static VObject *curObj;
static VObject* ObjStack[MAXLEVEL];
static int ObjStackTop;


/* A helpful utility for the rest of the app. */
#if __CPLUSPLUS__
extern "C" {
#endif

    extern void Parse_Debug(const char *s);
    static void yyerror(char *s);

#if __CPLUSPLUS__
    };
#endif

int yylex(void);
int yyparse(void);

enum LexMode {
	L_NORMAL,
	L_VCARD,
	L_VCAL,
	L_VEVENT,
	L_VTODO,
	L_VALUES,
	L_BASE64,
	L_QUOTED_PRINTABLE
	};

/****  Private Forward Declarations  ****/
static void lexClearToken(void);
static char* lexGet1Value(void);
static int lexGeta(void);
static int lexGetc(void);
static char lexGetc_(void);
static char* lexGetDataFromBase64(void);
static char* lexGetQuotedPrintable(void);
static char* lexGetWord(void);
static int lexLookahead(void);
static char* lexLookaheadWord(void);
static void lexPopMode(int top);
static void lexPushMode(enum LexMode mode);
static void lexSkipLookahead(void);
static void lexSkipLookaheadWord(void);
static void lexSkipWhite(void);
static char* lexStr(void);
static int lexWithinMode(enum LexMode mode);
static void enterAttr(const char *s1, const char *s2);
static void enterProps(const char *s);
static void enterValues(const char *value);
static void finiLex(void);
static void mime_error_(char *s);
static VObject* Parse_MIMEHelper(void);
static VObject* popVObject(void);
static int pushVObject(const char *prop);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 197 "vcc.y"
{
    char *str;
    VObject *vobj;
    }
/* Line 187 of yacc.c.  */
#line 333 "vcc.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 346 "vcc.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   56

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  21
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  47
/* YYNRULES -- Number of states.  */
#define YYNSTATES  62

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   275

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     6,    10,    12,    14,    16,    17,
      22,    23,    27,    30,    32,    33,    39,    41,    42,    46,
      48,    51,    53,    56,    58,    62,    64,    65,    70,    72,
      74,    75,    76,    81,    82,    86,    89,    91,    93,    95,
      97,    98,   103,   104,   108,   109,   114,   115
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      22,     0,    -1,    23,    -1,    -1,    25,    24,    23,    -1,
      25,    -1,    26,    -1,    41,    -1,    -1,    11,    27,    29,
      12,    -1,    -1,    11,    28,    12,    -1,    30,    29,    -1,
      30,    -1,    -1,    32,     4,    31,    38,     9,    -1,     1,
      -1,    -1,    37,    33,    34,    -1,    37,    -1,    35,    34,
      -1,    35,    -1,     6,    36,    -1,    37,    -1,    37,     3,
      37,    -1,    19,    -1,    -1,    40,     6,    39,    38,    -1,
      40,    -1,    20,    -1,    -1,    -1,    13,    42,    44,    14,
      -1,    -1,    13,    43,    14,    -1,    45,    44,    -1,    45,
      -1,    46,    -1,    49,    -1,    29,    -1,    -1,    15,    47,
      29,    16,    -1,    -1,    15,    48,    16,    -1,    -1,    17,
      50,    29,    18,    -1,    -1,    17,    51,    18,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   225,   225,   229,   228,   231,   235,   236,   241,   240,
     251,   250,   262,   263,   267,   266,   276,   280,   279,   284,
     290,   291,   294,   297,   301,   308,   311,   311,   312,   316,
     317,   322,   321,   327,   326,   332,   333,   337,   338,   339,
     344,   343,   355,   354,   368,   367,   379,   378
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EQ", "COLON", "DOT", "SEMICOLON",
  "SPACE", "HTAB", "LINESEP", "NEWLINE", "BEGIN_VCARD", "END_VCARD",
  "BEGIN_VCAL", "END_VCAL", "BEGIN_VEVENT", "END_VEVENT", "BEGIN_VTODO",
  "END_VTODO", "ID", "STRING", "$accept", "mime", "vobjects", "@1",
  "vobject", "vcard", "@2", "@3", "items", "item", "@4", "prop", "@5",
  "attr_params", "attr_param", "attr", "name", "values", "@6", "value",
  "vcal", "@7", "@8", "calitems", "calitem", "eventitem", "@9", "@10",
  "todoitem", "@11", "@12", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    21,    22,    24,    23,    23,    25,    25,    27,    26,
      28,    26,    29,    29,    31,    30,    30,    33,    32,    32,
      34,    34,    35,    36,    36,    37,    39,    38,    38,    40,
      40,    42,    41,    43,    41,    44,    44,    45,    45,    45,
      47,    46,    48,    46,    50,    49,    51,    49
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     3,     1,     1,     1,     0,     4,
       0,     3,     2,     1,     0,     5,     1,     0,     3,     1,
       2,     1,     2,     1,     3,     1,     0,     4,     1,     1,
       0,     0,     4,     0,     3,     2,     1,     1,     1,     1,
       0,     4,     0,     3,     0,     4,     0,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     8,    31,     0,     2,     3,     6,     7,     0,     0,
       0,     0,     1,     0,    16,    25,     0,     0,     0,    17,
      11,    40,    44,    39,     0,     0,    37,    38,    34,     4,
       9,    12,    14,     0,     0,     0,     0,     0,    32,    35,
      30,     0,    18,    21,     0,    43,     0,    47,    29,     0,
      28,    22,    23,    20,    41,    45,    15,    26,     0,    30,
      24,    27
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,    13,     5,     6,     8,     9,    23,    17,
      40,    18,    33,    42,    43,    51,    19,    49,    59,    50,
       7,    10,    11,    24,    25,    26,    34,    35,    27,    36,
      37
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -18
static const yytype_int8 yypact[] =
{
      -5,   -10,   -11,     5,   -18,    10,   -18,   -18,     3,    -1,
      12,    16,   -18,    -5,   -18,   -18,    20,     0,    29,    30,
     -18,    19,    18,   -18,    23,     6,   -18,   -18,   -18,   -18,
     -18,   -18,   -18,    32,     3,    24,     3,    21,   -18,   -18,
      22,    25,   -18,    32,    27,   -18,    28,   -18,   -18,    36,
      41,   -18,    45,   -18,   -18,   -18,   -18,   -18,    25,    22,
     -18,   -18
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -18,   -18,    37,   -18,   -18,   -18,   -18,   -18,    -8,   -18,
     -18,   -18,   -18,     8,   -18,   -18,   -17,    -7,   -18,   -18,
     -18,   -18,   -18,    31,   -18,   -18,   -18,   -18,   -18,   -18,
     -18
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -47
static const yytype_int8 yytable[] =
{
      16,    14,   -10,   -33,    14,    12,     1,    14,     2,    31,
      -5,    20,   -13,    14,   -13,   -13,   -13,   -13,   -13,    15,
     -36,    21,    15,    22,    52,    15,    44,    21,    46,    22,
      28,    15,    30,    32,   -19,   -42,   -46,    38,    41,    47,
      45,    60,    48,    54,    15,    56,    55,    57,    58,     0,
      29,    53,    61,     0,     0,     0,    39
};

static const yytype_int8 yycheck[] =
{
       8,     1,    12,    14,     1,     0,    11,     1,    13,    17,
       0,    12,    12,     1,    14,    15,    16,    17,    18,    19,
      14,    15,    19,    17,    41,    19,    34,    15,    36,    17,
      14,    19,    12,     4,     4,    16,    18,    14,     6,    18,
      16,    58,    20,    16,    19,     9,    18,     6,     3,    -1,
      13,    43,    59,    -1,    -1,    -1,    25
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    11,    13,    22,    23,    25,    26,    41,    27,    28,
      42,    43,     0,    24,     1,    19,    29,    30,    32,    37,
      12,    15,    17,    29,    44,    45,    46,    49,    14,    23,
      12,    29,     4,    33,    47,    48,    50,    51,    14,    44,
      31,     6,    34,    35,    29,    16,    29,    18,    20,    38,
      40,    36,    37,    34,    16,    18,     9,     6,     3,    39,
      37,    38
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 229 "vcc.y"
    { addList(&vObjList, (yyvsp[(1) - (1)].vobj)); curObj = 0; }
    break;

  case 5:
#line 232 "vcc.y"
    { addList(&vObjList, (yyvsp[(1) - (1)].vobj)); curObj = 0; }
    break;

  case 8:
#line 241 "vcc.y"
    {
	lexPushMode(L_VCARD);
	if (!pushVObject(VCCardProp)) YYERROR;
	}
    break;

  case 9:
#line 246 "vcc.y"
    {
	lexPopMode(0);
	(yyval.vobj) = popVObject();
	}
    break;

  case 10:
#line 251 "vcc.y"
    {
	lexPushMode(L_VCARD);
	if (!pushVObject(VCCardProp)) YYERROR;
	}
    break;

  case 11:
#line 256 "vcc.y"
    {
	lexPopMode(0);
	(yyval.vobj) = popVObject();
	}
    break;

  case 14:
#line 267 "vcc.y"
    {
	lexPushMode(L_VALUES);
	}
    break;

  case 15:
#line 271 "vcc.y"
    {
	if (lexWithinMode(L_BASE64) || lexWithinMode(L_QUOTED_PRINTABLE))
	   lexPopMode(0);
	lexPopMode(0);
	}
    break;

  case 17:
#line 280 "vcc.y"
    {
	enterProps((yyvsp[(1) - (1)].str));
	}
    break;

  case 19:
#line 285 "vcc.y"
    {
	enterProps((yyvsp[(1) - (1)].str));
	}
    break;

  case 23:
#line 298 "vcc.y"
    {
	enterAttr((yyvsp[(1) - (1)].str),0);
	}
    break;

  case 24:
#line 302 "vcc.y"
    {
	enterAttr((yyvsp[(1) - (3)].str),(yyvsp[(3) - (3)].str));

	}
    break;

  case 26:
#line 311 "vcc.y"
    { enterValues((yyvsp[(1) - (2)].str)); }
    break;

  case 28:
#line 313 "vcc.y"
    { enterValues((yyvsp[(1) - (1)].str)); }
    break;

  case 30:
#line 317 "vcc.y"
    { (yyval.str) = 0; }
    break;

  case 31:
#line 322 "vcc.y"
    { if (!pushVObject(VCCalProp)) YYERROR; }
    break;

  case 32:
#line 325 "vcc.y"
    { (yyval.vobj) = popVObject(); }
    break;

  case 33:
#line 327 "vcc.y"
    { if (!pushVObject(VCCalProp)) YYERROR; }
    break;

  case 34:
#line 329 "vcc.y"
    { (yyval.vobj) = popVObject(); }
    break;

  case 40:
#line 344 "vcc.y"
    {
	lexPushMode(L_VEVENT);
	if (!pushVObject(VCEventProp)) YYERROR;
	}
    break;

  case 41:
#line 350 "vcc.y"
    {
	lexPopMode(0);
	popVObject();
	}
    break;

  case 42:
#line 355 "vcc.y"
    {
	lexPushMode(L_VEVENT);
	if (!pushVObject(VCEventProp)) YYERROR;
	}
    break;

  case 43:
#line 360 "vcc.y"
    {
	lexPopMode(0);
	popVObject();
	}
    break;

  case 44:
#line 368 "vcc.y"
    {
	lexPushMode(L_VTODO);
	if (!pushVObject(VCTodoProp)) YYERROR;
	}
    break;

  case 45:
#line 374 "vcc.y"
    {
	lexPopMode(0);
	popVObject();
	}
    break;

  case 46:
#line 379 "vcc.y"
    {
	lexPushMode(L_VTODO);
	if (!pushVObject(VCTodoProp)) YYERROR;
	}
    break;

  case 47:
#line 384 "vcc.y"
    {
	lexPopMode(0);
	popVObject();
	}
    break;


/* Line 1267 of yacc.c.  */
#line 1782 "vcc.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 390 "vcc.y"

static int pushVObject(const char *prop)
    {
    VObject *newObj;
    if (ObjStackTop == MAXLEVEL)
	return FALSE;

    ObjStack[++ObjStackTop] = curObj;

    if (curObj) {
        newObj = addProp(curObj,prop);
        curObj = newObj;
	}
    else
	curObj = newVObject(prop);

    return TRUE;
    }


/* This pops the recently built vCard off the stack and returns it. */
static VObject* popVObject()
    {
    VObject *oldObj;
    if (ObjStackTop < 0) {
	yyerror("pop on empty Object Stack\n");
	return 0;
	}
    oldObj = curObj;
    curObj = ObjStack[ObjStackTop--];

    return oldObj;
    }


static void enterValues(const char *value)
    {
    if (fieldedProp && *fieldedProp) {
	if (value) {
	    addPropValue(curProp,*fieldedProp,value);
	    }
	/* else this field is empty, advance to next field */
	fieldedProp++;
	}
    else {
	if (value) {
	    char *p1, *p2;
	    wchar_t *p3;
	    int i;

	    /* If the property already has a string value, we append this one,
	       using ';' to separate the values. */
	    if (vObjectUStringZValue(curProp)) {
		p1 = fakeCString(vObjectUStringZValue(curProp));
		i = strlen(p1)+strlen(value)+2;
		p2 = malloc(i);
		snprintf(p2,i,"%s;%s",p1,value);
		deleteStr(p1);
		p3 = (wchar_t *) vObjectUStringZValue(curProp);
		free(p3);
		setVObjectUStringZValue_(curProp,fakeUnicode(p2,0));
		deleteStr(p2);
	    } else {
	    setVObjectUStringZValue_(curProp,fakeUnicode(value,0));
	    }
	}
    }
    deleteStr(value);
    }

static void enterProps(const char *s)
    {
    curProp = addGroup(curObj,s);
    deleteStr(s);
    }

static void enterAttr(const char *s1, const char *s2)
    {
    const char *p1, *p2 = NULL;
    p1 = lookupProp_(s1);
    if (s2) {
	VObject *a;
	p2 = lookupProp_(s2);
	a = addProp(curProp,p1);
	setVObjectStringZValue(a,p2);
	}
    else
	addProp(curProp,p1);
    if (stricmp(p1,VCBase64Prop) == 0 || (s2 && stricmp(p2,VCBase64Prop)==0))
	lexPushMode(L_BASE64);
    else if (stricmp(p1,VCQuotedPrintableProp) == 0
	    || (s2 && stricmp(p2,VCQuotedPrintableProp)==0))
	lexPushMode(L_QUOTED_PRINTABLE);
    deleteStr(s1); deleteStr(s2);
    }


#define MAX_LEX_LOOKAHEAD_0 32
#define MAX_LEX_LOOKAHEAD 64
#define MAX_LEX_MODE_STACK_SIZE 10
#define LEXMODE() (lexBuf.lexModeStack[lexBuf.lexModeStackTop])

struct LexBuf {
	/* input */
#ifdef INCLUDEMFC
    CFile *inputFile;
#else
    FILE *inputFile;
#endif
    char *inputString;
    unsigned long curPos;
    unsigned long inputLen;
	/* lookahead buffer */
	/*   -- lookahead buffer is short instead of char so that EOF
	 /      can be represented correctly.
	*/
    unsigned long len;
    short buf[MAX_LEX_LOOKAHEAD];
    unsigned long getPtr;
	/* context stack */
    unsigned long lexModeStackTop;
    enum LexMode lexModeStack[MAX_LEX_MODE_STACK_SIZE];
	/* token buffer */
    unsigned long maxToken;
    char *strs;
    unsigned long strsLen;
    } lexBuf;

static void lexPushMode(enum LexMode mode)
    {
    if (lexBuf.lexModeStackTop == (MAX_LEX_MODE_STACK_SIZE-1))
	yyerror("lexical context stack overflow");
    else {
	lexBuf.lexModeStack[++lexBuf.lexModeStackTop] = mode;
	}
    }

static void lexPopMode(int top)
    {
    /* special case of pop for ease of error recovery -- this
	version will never underflow */
    if (top)
	lexBuf.lexModeStackTop = 0;
    else
	if (lexBuf.lexModeStackTop > 0) lexBuf.lexModeStackTop--;
    }

static int lexWithinMode(enum LexMode mode) {
    unsigned long i;
    for (i=0;i<lexBuf.lexModeStackTop;i++)
	if (mode == lexBuf.lexModeStack[i]) return 1;
    return 0;
    }

static char lexGetc_()
    {
    /* get next char from input, no buffering. */
    if (lexBuf.curPos == lexBuf.inputLen)
	return EOF;
    else if (lexBuf.inputString)
	return *(lexBuf.inputString + lexBuf.curPos++);
    else {
#ifdef INCLUDEMFC
	char result;
	return lexBuf.inputFile->Read(&result, 1) == 1 ? result : EOF;
#else
	return fgetc(lexBuf.inputFile);
#endif
	}
    }

static int lexGeta()
    {
    ++lexBuf.len;
    return (lexBuf.buf[lexBuf.getPtr] = lexGetc_());
    }

static int lexGeta_(int i)
    {
    ++lexBuf.len;
    return (lexBuf.buf[(lexBuf.getPtr+i)%MAX_LEX_LOOKAHEAD] = lexGetc_());
    }

static void lexSkipLookahead() {
    if (lexBuf.len > 0 && lexBuf.buf[lexBuf.getPtr]!=EOF) {
	/* don't skip EOF. */
        lexBuf.getPtr = (lexBuf.getPtr + 1) % MAX_LEX_LOOKAHEAD;
	lexBuf.len--;
        }
    }

static int lexLookahead() {
    int c = (lexBuf.len)?
	lexBuf.buf[lexBuf.getPtr]:
	lexGeta();
    /* do the \r\n -> \n or \r -> \n translation here */
    if (c == '\r') {
	int a = (lexBuf.len>1)?
	    lexBuf.buf[(lexBuf.getPtr+1)%MAX_LEX_LOOKAHEAD]:
	    lexGeta_(1);
	if (a == '\n') {
	    lexSkipLookahead();
	    }
	lexBuf.buf[lexBuf.getPtr] = c = '\n';
	}
    else if (c == '\n') {
	int a = (lexBuf.len>1)?
	    lexBuf.buf[lexBuf.getPtr+1]:
	    lexGeta_(1);
	if (a == '\r') {
	    lexSkipLookahead();
	    }
	lexBuf.buf[lexBuf.getPtr] = '\n';
	}
    return c;
    }

static int lexGetc() {
    int c = lexLookahead();
    if (lexBuf.len > 0 && lexBuf.buf[lexBuf.getPtr]!=EOF) {
	/* EOF will remain in lookahead buffer */
        lexBuf.getPtr = (lexBuf.getPtr + 1) % MAX_LEX_LOOKAHEAD;
	lexBuf.len--;
        }
    return c;
    }

static void lexSkipLookaheadWord() {
    if (lexBuf.strsLen <= lexBuf.len) {
	lexBuf.len -= lexBuf.strsLen;
	lexBuf.getPtr = (lexBuf.getPtr + lexBuf.strsLen) % MAX_LEX_LOOKAHEAD;
	}
    }

static void lexClearToken()
    {
    lexBuf.strsLen = 0;
    }

static void lexAppendc(int c)
    {
    lexBuf.strs[lexBuf.strsLen] = c;
    /* append up to zero termination */
    if (c == 0) return;
    lexBuf.strsLen++;
    if (lexBuf.strsLen >= lexBuf.maxToken) {
	/* double the token string size */
	lexBuf.maxToken <<= 1;
	lexBuf.strs = (char*) realloc(lexBuf.strs,(size_t)lexBuf.maxToken);
	}
    }

static char* lexStr() {
    return dupStr(lexBuf.strs,(size_t)lexBuf.strsLen+1);
    }

static void lexSkipWhite() {
    int c = lexLookahead();
    while (c == ' ' || c == '\t') {
	lexSkipLookahead();
	c = lexLookahead();
	}
    }

static char* lexGetWord() {
    int c;
    lexSkipWhite();
    lexClearToken();
    c = lexLookahead();
    while (c != EOF && !strchr("\t\n ;:=",c)) {
	lexAppendc(c);
	lexSkipLookahead();
	c = lexLookahead();
	}
    lexAppendc(0);
    return lexStr();
    }

static void lexPushLookaheadc(int c) {
    int putptr;
    /* can't putback EOF, because it never leaves lookahead buffer */
    if (c == EOF) return;
    putptr = (int)lexBuf.getPtr - 1;
    if (putptr < 0) putptr += MAX_LEX_LOOKAHEAD;
    lexBuf.getPtr = putptr;
    lexBuf.buf[putptr] = c;
    lexBuf.len += 1;
    }

static char* lexLookaheadWord() {
    /* this function can lookahead word with max size of MAX_LEX_LOOKAHEAD_0
     /  and thing bigger than that will stop the lookahead and return 0;
     / leading white spaces are not recoverable.
     */
    int c;
    int len = 0;
    int curgetptr = 0;
    lexSkipWhite();
    lexClearToken();
    curgetptr = (int)lexBuf.getPtr;	/* remember! */
    while (len < (MAX_LEX_LOOKAHEAD_0)) {
	c = lexGetc();
	len++;
	if (c == EOF || strchr("\t\n ;:=", c)) {
	    lexAppendc(0);
	    /* restore lookahead buf. */
	    lexBuf.len += len;
	    lexBuf.getPtr = curgetptr;
	    return lexStr();
	    }
        else
	    lexAppendc(c);
	}
    lexBuf.len += len;	/* char that has been moved to lookahead buffer */
    lexBuf.getPtr = curgetptr;
    return 0;
    }

#ifdef _SUPPORT_LINE_FOLDING
static void handleMoreRFC822LineBreak(int c) {
    /* suport RFC 822 line break in cases like
     *	ADR: foo;
     *    morefoo;
     *    more foo;
     */
    if (c == ';') {
	int a;
	lexSkipLookahead();
	/* skip white spaces */
	a = lexLookahead();
	while (a == ' ' || a == '\t') {
	    lexSkipLookahead();
	    a = lexLookahead();
	    }
	if (a == '\n') {
	    lexSkipLookahead();
	    a = lexLookahead();
	    if (a == ' ' || a == '\t') {
		/* continuation, throw away all the \n and spaces read so
		 * far
		 */
		lexSkipWhite();
		lexPushLookaheadc(';');
		}
	    else {
		lexPushLookaheadc('\n');
		lexPushLookaheadc(';');
		}
	    }
	else {
	    lexPushLookaheadc(';');
	    }
	}
    }

static char* lexGet1Value() {
    int c;
    lexSkipWhite();
    c = lexLookahead();
    lexClearToken();
    while (c != EOF && c != ';') {
	if (c == '\n') {
	    int a;
	    lexSkipLookahead();
	    a  = lexLookahead();
	    if (a == ' ' || a == '\t') {
		lexAppendc(' ');
		lexSkipLookahead();
		}
	    else {
		lexPushLookaheadc('\n');
		break;
		}
	    }
	else {
	    lexAppendc(c);
	    lexSkipLookahead();
	    }
	c = lexLookahead();
	}
    lexAppendc(0);
    handleMoreRFC822LineBreak(c);
    return c==EOF?0:lexStr();
    }
#endif


static int match_begin_name(int end) {
    char *n = lexLookaheadWord();
    int token = ID;
    if (n) {
	if (!stricmp(n,"vcard")) token = end?END_VCARD:BEGIN_VCARD;
	else if (!stricmp(n,"vcalendar")) token = end?END_VCAL:BEGIN_VCAL;
	else if (!stricmp(n,"vevent")) token = end?END_VEVENT:BEGIN_VEVENT;
	else if (!stricmp(n,"vtodo")) token = end?END_VTODO:BEGIN_VTODO;
	deleteStr(n);
	return token;
	}
    return 0;
    }


#ifdef INCLUDEMFC
void initLex(const char *inputstring, unsigned long inputlen, CFile *inputfile)
#else
void initLex(const char *inputstring, unsigned long inputlen, FILE *inputfile)
#endif
    {
    /* initialize lex mode stack */
    lexBuf.lexModeStack[lexBuf.lexModeStackTop=0] = L_NORMAL;

    /* iniatialize lex buffer. */
    lexBuf.inputString = (char*) inputstring;
    lexBuf.inputLen = inputlen;
    lexBuf.curPos = 0;
    lexBuf.inputFile = inputfile;

    lexBuf.len = 0;
    lexBuf.getPtr = 0;

    lexBuf.maxToken = MAXTOKEN;
    lexBuf.strs = (char*)malloc(MAXTOKEN);
    lexBuf.strsLen = 0;

    }

static void finiLex() {
    free(lexBuf.strs);
    }


/* This parses and converts the base64 format for binary encoding into
 * a decoded buffer (allocated with new).  See RFC 1521.
 */
static char * lexGetDataFromBase64()
    {
    unsigned long bytesLen = 0, bytesMax = 0;
    int quadIx = 0, pad = 0;
    unsigned long trip = 0;
    unsigned char b;
    int c;
    unsigned char *bytes = NULL;
    unsigned char *oldBytes = NULL;

    DBG_(("db: lexGetDataFromBase64\n"));
    while (1) {
	c = lexGetc();
	if (c == '\n') {
	    ++mime_lineNum;
	    if (lexLookahead() == '\n') {
		/* a '\n' character by itself means end of data */
		break;
		}
	    else continue; /* ignore '\n' */
	    }
	else {
	    if ((c >= 'A') && (c <= 'Z'))
		b = (unsigned char)(c - 'A');
	    else if ((c >= 'a') && (c <= 'z'))
		b = (unsigned char)(c - 'a') + 26;
	    else if ((c >= '0') && (c <= '9'))
		b = (unsigned char)(c - '0') + 52;
	    else if (c == '+')
		b = 62;
	    else if (c == '/')
		b = 63;
	    else if (c == '=') {
		b = 0;
		pad++;
	    } else if ((c == ' ') || (c == '\t')) {
		continue;
	    } else { /* error condition */
		if (bytes) free(bytes);
		else if (oldBytes) free(oldBytes);
		/* error recovery: skip until 2 adjacent newlines. */
		DBG_(("db: invalid character 0x%x '%c'\n", c,c));
		if (c != EOF)  {
		    c = lexGetc();
		    while (c != EOF) {
			if (c == '\n' && lexLookahead() == '\n') {
			    ++mime_lineNum;
			    break;
			    }
			c = lexGetc();
			}
		    }
		return NULL;
		}
	    trip = (trip << 6) | b;
	    if (++quadIx == 4) {
		unsigned char outBytes[3];
		int numOut;
		int i;
		for (i = 0; i < 3; i++) {
		    outBytes[2-i] = (unsigned char)(trip & 0xFF);
		    trip >>= 8;
		    }
		numOut = 3 - pad;
		if (bytesLen + numOut > bytesMax) {
		    if (!bytes) {
			bytesMax = 1024;
			bytes = (unsigned char*)malloc((size_t)bytesMax);
			}
		    else {
			bytesMax <<= 2;
			oldBytes = bytes;
			bytes = (unsigned char*)realloc(bytes,(size_t)bytesMax);
			}
		    if (bytes == 0) {
			mime_error("out of memory while processing BASE64 data\n");
			}
		    }
		if (bytes) {
		    memcpy(bytes + bytesLen, outBytes, numOut);
		    bytesLen += numOut;
		    }
		trip = 0;
		quadIx = 0;
		}
	    }
	} /* while */
    DBG_(("db: bytesLen = %d\n",  bytesLen));
    /* kludge: all this won't be necessary if we have tree form
	representation */
    if (bytes) {
	setValueWithSize(curProp,bytes,(unsigned int)bytesLen);
	free(bytes);
	}
    else if (oldBytes) {
	setValueWithSize(curProp,oldBytes,(unsigned int)bytesLen);
	free(oldBytes);
	}
    return 0;
    }

static int match_begin_end_name(int end) {
    int token;
    lexSkipWhite();
    if (lexLookahead() != ':') return ID;
    lexSkipLookahead();
    lexSkipWhite();
    token = match_begin_name(end);
    if (token == ID) {
	lexPushLookaheadc(':');
	DBG_(("db: ID '%s'\n", yylval.str));
	return ID;
	}
    else if (token != 0) {
	lexSkipLookaheadWord();
	deleteStr(yylval.str);
	DBG_(("db: begin/end %d\n", token));
	return token;
	}
    return 0;
    }

static char* lexGetQuotedPrintable()
    {
    char cur;

    lexClearToken();
    do {
	cur = lexGetc();
	switch (cur) {
	    case '=': {
		int c = 0;
		int next[2];
		int i;
		for (i = 0; i < 2; i++) {
		    next[i] = lexGetc();
		    if (next[i] >= '0' && next[i] <= '9')
			c = c * 16 + next[i] - '0';
		    else if (next[i] >= 'A' && next[i] <= 'F')
			c = c * 16 + next[i] - 'A' + 10;
		    else
			break;
		    }
		if (i == 0) {
		    /* single '=' follow by LINESEP is continuation sign? */
		    if (next[0] == '\n') {
			++mime_lineNum;
			}
		    else {
			lexPushLookaheadc('=');
			goto EndString;
			}
		    }
		else if (i == 1) {
		    lexPushLookaheadc(next[1]);
		    lexPushLookaheadc(next[0]);
		    lexAppendc('=');
		} else {
		    lexAppendc(c);
		    }
		break;
		} /* '=' */
	    case '\n': {
		lexPushLookaheadc('\n');
		goto EndString;
		}
	    case (char)EOF:
		break;
	    default:
		lexAppendc(cur);
		break;
	    } /* switch */
	} while (cur != (char)EOF);

EndString:
    lexAppendc(0);
    return lexStr();
    } /* LexQuotedPrintable */

int yylex() {

    int lexmode = LEXMODE();
    if (lexmode == L_VALUES) {
	int c = lexGetc();
	if (c == ';') {
	    DBG_(("db: SEMICOLON\n"));
	    lexPushLookaheadc(c);
#ifdef _SUPPORT_LINE_FOLDING
	    handleMoreRFC822LineBreak(c);
#endif
	    lexSkipLookahead();
	    return SEMICOLON;
	    }
	else if (strchr("\n",c)) {
	    ++mime_lineNum;
	    /* consume all line separator(s) adjacent to each other */
	    c = lexLookahead();
	    while (strchr("\n",c)) {
		lexSkipLookahead();
		c = lexLookahead();
		++mime_lineNum;
		}
	    DBG_(("db: LINESEP\n"));
	    return LINESEP;
	    }
	else {
	    char *p = 0;
	    lexPushLookaheadc(c);
	    if (lexWithinMode(L_BASE64)) {
		/* get each char and convert to bin on the fly... */
		p = lexGetDataFromBase64();
		yylval.str = p;
		return STRING;
		}
	    else if (lexWithinMode(L_QUOTED_PRINTABLE)) {
		p = lexGetQuotedPrintable();
		}
	    else {
#ifdef _SUPPORT_LINE_FOLDING
		p = lexGet1Value();
#else
		p = lexGetStrUntil(";\n");
#endif
		}
	    if (p) {
		DBG_(("db: STRING: '%s'\n", p));
		yylval.str = p;
		return STRING;
		}
	    else return 0;
	    }
	}
    else {
	/* normal mode */
	while (1) {
	    int c = lexGetc();
	    switch(c) {
		case ':': {
		    /* consume all line separator(s) adjacent to each other */
		    /* ignoring linesep immediately after colon. */
/*		    c = lexLookahead();
		    while (strchr("\n",c)) {
			lexSkipLookahead();
			c = lexLookahead();
			++mime_lineNum;
			}*/
		    DBG_(("db: COLON\n"));
		    return COLON;
		    }
		case ';':
		    DBG_(("db: SEMICOLON\n"));
		    return SEMICOLON;
		case '=':
		    DBG_(("db: EQ\n"));
		    return EQ;
		/* ignore whitespace in this mode */
		case '\t':
		case ' ': continue;
		case '\n': {
		    ++mime_lineNum;
		    continue;
		    }
		case EOF: return 0;
		    break;
		default: {
		    lexPushLookaheadc(c);
		    if (isalpha(c)) {
			char *t = lexGetWord();
			yylval.str = t;
			if (!stricmp(t, "begin")) {
			    return match_begin_end_name(0);
			    }
			else if (!stricmp(t,"end")) {
			    return match_begin_end_name(1);
			    }
		        else {
			    DBG_(("db: ID '%s'\n", t));
			    return ID;
			    }
			}
		    else {
			/* unknow token */
			return 0;
			}
		    break;
		    }
		}
	    }
	}
    return 0;
    }


/***************************************************************************/
/***							Public Functions						****/
/***************************************************************************/

static VObject* Parse_MIMEHelper()
    {
    ObjStackTop = -1;
    mime_numErrors = 0;
    mime_lineNum = 1;
    vObjList = 0;
    curObj = 0;

    if (yyparse() != 0)
	return 0;

    finiLex();
    return vObjList;
    }

DLLEXPORT(VObject*) Parse_MIME(const char *input, unsigned long len)
    {
    initLex(input, len, 0);
    return Parse_MIMEHelper();
    }


#if INCLUDEMFC

DLLEXPORT(VObject*) Parse_MIME_FromFile(CFile *file)
    {
    unsigned long startPos;
    VObject *result;

    initLex(0,-1,file);
    startPos = file->GetPosition();
    if (!(result = Parse_MIMEHelper()))
	file->Seek(startPos, CFile::begin);
    return result;
    }

#else

VObject* Parse_MIME_FromFile(FILE *file)
    {
    VObject *result;
    long startPos;

    initLex(0,(unsigned long)-1,file);
    startPos = ftell(file);
    if (!(result = Parse_MIMEHelper())) {
	fseek(file,startPos,SEEK_SET);
	}
    return result;
    }

DLLEXPORT(VObject*) Parse_MIME_FromFileName(char *fname)
    {
    FILE *fp = fopen(fname,"r");
    if (fp) {
	VObject* o = Parse_MIME_FromFile(fp);
	fclose(fp);
	return o;
	}
    else {
	char msg[256];
	snprintf(msg, sizeof(msg), "can't open file '%s' for reading\n", fname);
	mime_error_(msg);
	return 0;
	}
    }

#endif


static MimeErrorHandler mimeErrorHandler;

DLLEXPORT(void) registerMimeErrorHandler(MimeErrorHandler me)
    {
    mimeErrorHandler = me;
    }

static void mime_error(char *s)
    {
    char msg[256];
    if (mimeErrorHandler) {
	snprintf(msg,sizeof(msg),"%s at line %d", s, mime_lineNum);
	mimeErrorHandler(msg);
	}
    }

static void mime_error_(char *s)
    {
    if (mimeErrorHandler) {
	mimeErrorHandler(s);
	}
    }


