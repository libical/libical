/* A Bison parser, made from icalyacc.y
   by GNU bison 1.34.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse ical_yyparse
#define yylex ical_yylex
#define yyerror ical_yyerror
#define yylval ical_yylval
#define yychar ical_yychar
#define yydebug ical_yydebug
#define yynerrs ical_yynerrs
# define	DIGITS	257
# define	INTNUMBER	258
# define	FLOATNUMBER	259
# define	STRING	260
# define	EOL	261
# define	EQUALS	262
# define	CHARACTER	263
# define	COLON	264
# define	COMMA	265
# define	SEMICOLON	266
# define	MINUS	267
# define	TIMESEPERATOR	268
# define	YTRUE	269
# define	YFALSE	270
# define	FREQ	271
# define	BYDAY	272
# define	BYHOUR	273
# define	BYMINUTE	274
# define	BYMONTH	275
# define	BYMONTHDAY	276
# define	BYSECOND	277
# define	BYSETPOS	278
# define	BYWEEKNO	279
# define	BYYEARDAY	280
# define	DAILY	281
# define	MINUTELY	282
# define	MONTHLY	283
# define	SECONDLY	284
# define	WEEKLY	285
# define	HOURLY	286
# define	YEARLY	287
# define	INTERVAL	288
# define	COUNT	289
# define	UNTIL	290
# define	WKST	291
# define	MO	292
# define	SA	293
# define	SU	294
# define	TU	295
# define	WE	296
# define	TH	297
# define	FR	298
# define	BIT8	299
# define	ACCEPTED	300
# define	ADD	301
# define	AUDIO	302
# define	BASE64	303
# define	BINARY	304
# define	BOOLEAN	305
# define	BUSY	306
# define	BUSYTENTATIVE	307
# define	BUSYUNAVAILABLE	308
# define	CALADDRESS	309
# define	CANCEL	310
# define	CANCELLED	311
# define	CHAIR	312
# define	CHILD	313
# define	COMPLETED	314
# define	CONFIDENTIAL	315
# define	CONFIRMED	316
# define	COUNTER	317
# define	DATE	318
# define	DATETIME	319
# define	DECLINECOUNTER	320
# define	DECLINED	321
# define	DELEGATED	322
# define	DISPLAY	323
# define	DRAFT	324
# define	DURATION	325
# define	EMAIL	326
# define	END	327
# define	FINAL	328
# define	FLOAT	329
# define	FREE	330
# define	GREGORIAN	331
# define	GROUP	332
# define	INDIVIDUAL	333
# define	INPROCESS	334
# define	INTEGER	335
# define	NEEDSACTION	336
# define	NONPARTICIPANT	337
# define	OPAQUE	338
# define	OPTPARTICIPANT	339
# define	PARENT	340
# define	PERIOD	341
# define	PRIVATE	342
# define	PROCEDURE	343
# define	PUBLIC	344
# define	PUBLISH	345
# define	RECUR	346
# define	REFRESH	347
# define	REPLY	348
# define	REQPARTICIPANT	349
# define	REQUEST	350
# define	RESOURCE	351
# define	ROOM	352
# define	SIBLING	353
# define	START	354
# define	TENTATIVE	355
# define	TEXT	356
# define	THISANDFUTURE	357
# define	THISANDPRIOR	358
# define	TIME	359
# define	TRANSPAENT	360
# define	UNKNOWN	361
# define	UTCOFFSET	362
# define	XNAME	363
# define	ALTREP	364
# define	CN	365
# define	CUTYPE	366
# define	DAYLIGHT	367
# define	DIR	368
# define	ENCODING	369
# define	EVENT	370
# define	FBTYPE	371
# define	FMTTYPE	372
# define	LANGUAGE	373
# define	MEMBER	374
# define	PARTSTAT	375
# define	RANGE	376
# define	RELATED	377
# define	RELTYPE	378
# define	ROLE	379
# define	RSVP	380
# define	SENTBY	381
# define	STANDARD	382
# define	URI	383
# define	TIME_CHAR	384
# define	UTC_CHAR	385

#line 3 "icalyacc.y"

/* -*- Mode: C -*-
  ======================================================================
  FILE: icalyacc.y
  CREATOR: eric 10 June 1999
  
  DESCRIPTION:
  
  $Id: icalyacc.c,v 1.2 2002-06-11 12:39:30 acampi Exp $
  $Locker:  $

  (C) COPYRIGHT 1999 Eric Busboom 
  http://www.softwarestudio.org

  The contents of this file are subject to the Mozilla Public License
  Version 1.0 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License at
  http://www.mozilla.org/MPL/
 
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
  the License for the specific language governing rights and
  limitations under the License.

  The original author is Eric Busboom
  The original code is icalitip.y



  =======================================================================*/

#include <stdlib.h>
#include <string.h> /* for strdup() */
#include "icalparser.h"
#include "icaltimezone.h"
#include "pvl.h"

#define YYPARSE_PARAM yy_globals
#define YYLEX_PARAM yy_globals
#define YY_EXTRA_TYPE ical_yyparams*

  /* ick...*/
#define yyextra ((ical_yyparams*)ical_yyget_extra(yy_globals))

void ical_yyerror(char* s);
void icalparser_clear_flex_input();
int ical_yy_lex(void);

void copy_list(short* array, size_t size);
void add_prop(icalproperty_kind);
void icalparser_fill_date(struct tm* t, char* dstr);
void icalparser_fill_time(struct tm* t, char* tstr);
void set_value_type(icalvalue_kind kind);
void set_parser_value_state();
static struct icaltimetype fill_datetime(struct icaltimetype *, char* d, char* t, int utc);
void ical_yy_error(char *s); /* Don't know why I need this.... */

/* Set the state of the lexer so it will interpret values ( iCAL
   VALUEs, that is, ) correctly. */


#line 65 "icalyacc.y"
#ifndef YYSTYPE
typedef union {
	float v_float;
	int   v_int;
	char* v_string;
} yystype;
# define YYSTYPE yystype
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		51
#define	YYFLAG		-32768
#define	YYNTBASE	141

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 385 ? yytranslate[x] : 158)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const short yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   137,     2,   138,     2,   140,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   136,     2,
       2,     2,   133,     2,     2,     2,     2,   134,     2,     2,
     139,     2,     2,   135,     2,     2,     2,   132,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     4,     6,     8,    10,    12,    14,    15,
      17,    18,    20,    25,    27,    30,    33,    36,    39,    42,
      45,    49,    52,    56,    59,    62,    63,    65,    67,    71,
      75,    79,    89,    96,    98,   100,   104
};
static const short yyrhs[] =
{
     142,     0,   145,     0,   154,     0,   155,     0,   157,     0,
       1,     0,     3,     0,     0,   131,     0,     0,   131,     0,
       3,   130,     3,   143,     0,   152,     0,   152,   148,     0,
       3,   132,     0,   130,   149,     0,   130,   150,     0,   130,
     151,     0,     3,   133,     0,     3,   133,   150,     0,     3,
     134,     0,     3,   134,   151,     0,     3,   135,     0,     3,
     136,     0,     0,   137,     0,   138,     0,   153,   139,   146,
       0,   153,   139,   148,     0,   153,   139,   147,     0,     3,
     130,     3,   143,   140,     3,   130,     3,   144,     0,     3,
     130,     3,   143,   140,   154,     0,   137,     0,   138,     0,
     156,     4,     4,     0,   156,     4,     4,     4,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   171,   173,   174,   175,   176,   177,   184,   192,   194,
     198,   200,   203,   218,   219,   222,   228,   231,   234,   239,
     243,   249,   253,   259,   265,   271,   275,   279,   285,   290,
     295,   305,   323,   349,   350,   353,   359
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "DIGITS", "INTNUMBER", "FLOATNUMBER", 
  "STRING", "EOL", "EQUALS", "CHARACTER", "COLON", "COMMA", "SEMICOLON", 
  "MINUS", "TIMESEPERATOR", "YTRUE", "YFALSE", "FREQ", "BYDAY", "BYHOUR", 
  "BYMINUTE", "BYMONTH", "BYMONTHDAY", "BYSECOND", "BYSETPOS", "BYWEEKNO", 
  "BYYEARDAY", "DAILY", "MINUTELY", "MONTHLY", "SECONDLY", "WEEKLY", 
  "HOURLY", "YEARLY", "INTERVAL", "COUNT", "UNTIL", "WKST", "MO", "SA", 
  "SU", "TU", "WE", "TH", "FR", "BIT8", "ACCEPTED", "ADD", "AUDIO", 
  "BASE64", "BINARY", "BOOLEAN", "BUSY", "BUSYTENTATIVE", 
  "BUSYUNAVAILABLE", "CALADDRESS", "CANCEL", "CANCELLED", "CHAIR", 
  "CHILD", "COMPLETED", "CONFIDENTIAL", "CONFIRMED", "COUNTER", "DATE", 
  "DATETIME", "DECLINECOUNTER", "DECLINED", "DELEGATED", "DISPLAY", 
  "DRAFT", "DURATION", "EMAIL", "END", "FINAL", "FLOAT", "FREE", 
  "GREGORIAN", "GROUP", "INDIVIDUAL", "INPROCESS", "INTEGER", 
  "NEEDSACTION", "NONPARTICIPANT", "OPAQUE", "OPTPARTICIPANT", "PARENT", 
  "PERIOD", "PRIVATE", "PROCEDURE", "PUBLIC", "PUBLISH", "RECUR", 
  "REFRESH", "REPLY", "REQPARTICIPANT", "REQUEST", "RESOURCE", "ROOM", 
  "SIBLING", "START", "TENTATIVE", "TEXT", "THISANDFUTURE", 
  "THISANDPRIOR", "TIME", "TRANSPAENT", "UNKNOWN", "UTCOFFSET", "XNAME", 
  "ALTREP", "CN", "CUTYPE", "DAYLIGHT", "DIR", "ENCODING", "EVENT", 
  "FBTYPE", "FMTTYPE", "LANGUAGE", "MEMBER", "PARTSTAT", "RANGE", 
  "RELATED", "RELTYPE", "ROLE", "RSVP", "SENTBY", "STANDARD", "URI", 
  "TIME_CHAR", "UTC_CHAR", "'W'", "'H'", "'M'", "'S'", "'D'", "'+'", 
  "'-'", "'P'", "'/'", "value", "date_value", "utc_char", "utc_char_b", 
  "datetime_value", "dur_date", "dur_week", "dur_time", "dur_hour", 
  "dur_minute", "dur_second", "dur_day", "dur_prefix", "duration_value", 
  "period_value", "plusminus", "utcoffset_value", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,   141,   141,   141,   141,   141,   141,   142,   143,   143,
     144,   144,   145,   146,   146,   147,   148,   148,   148,   149,
     149,   150,   150,   151,   152,   153,   153,   153,   154,   154,
     154,   155,   155,   156,   156,   157,   157
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       0,     1,     4,     1,     2,     2,     2,     2,     2,     2,
       3,     2,     3,     2,     2,     0,     1,     1,     3,     3,
       3,     9,     6,     1,     1,     3,     4
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     6,     7,    26,    27,     1,     2,     0,     3,     4,
       0,     5,     0,     0,     0,     8,     0,     0,    28,    30,
      29,    13,    35,     9,    12,    15,    24,     0,    16,    17,
      18,    14,    36,    25,    19,    21,    23,     0,    26,    27,
      32,     0,    20,     0,    22,     0,    10,    11,    31,     0,
       0,     0
};

static const short yydefgoto[] =
{
      49,     5,    24,    48,     6,    18,    19,    20,    28,    29,
      30,    21,     7,     8,     9,    10,    11
};

static const short yypact[] =
{
      -1,-32768,  -126,     7,     8,-32768,-32768,  -133,-32768,-32768,
       9,-32768,    11,    -2,    12,  -116,  -129,    14,-32768,-32768,
  -32768,  -112,    15,-32768,  -120,-32768,-32768,  -125,-32768,-32768,
  -32768,-32768,-32768,     2,    18,    19,-32768,  -107,-32768,-32768,
  -32768,  -110,-32768,  -109,-32768,    22,  -104,-32768,-32768,    28,
      29,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    10,-32768,    -4,
      -3,-32768,-32768,     0,-32768,-32768,-32768
};


#define	YYLAST		140


static const short yytable[] =
{
       1,    16,     2,    25,    12,    37,    13,    26,    34,    35,
      36,   -33,   -34,    14,    15,    23,    22,    27,    17,    32,
      33,    41,    43,    45,    35,    46,    36,    47,    50,    51,
      42,    31,    44,    40,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
       0,     0,     0,     0,     0,     0,     3,     4,   -25,    38,
      39
};

static const short yycheck[] =
{
       1,     3,     3,   132,   130,     3,   139,   136,   133,   134,
     135,     4,     4,     4,     3,   131,     4,     3,   130,     4,
     140,     3,     3,   130,   134,     3,   135,   131,     0,     0,
      34,    21,    35,    33,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   137,   138,   139,   137,
     138
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Relocate the TYPE STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Type, Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	yymemcpy ((char *) yyptr, (char *) (Stack),			\
		  yysize * (YYSIZE_T) sizeof (Type));			\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (Type) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#if ! defined (yyoverflow) && ! defined (yymemcpy)
# if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#  define yymemcpy __builtin_memcpy
# else				/* not GNU C or C++ */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
#  if defined (__STDC__) || defined (__cplusplus)
yymemcpy (char *yyto, const char *yyfrom, YYSIZE_T yycount)
#  else
yymemcpy (yyto, yyfrom, yycount)
     char *yyto;
     const char *yyfrom;
     YYSIZE_T yycount;
#  endif
{
  register const char *yyf = yyfrom;
  register char *yyt = yyto;
  register YYSIZE_T yyi = yycount;

  while (yyi-- != 0)
    *yyt++ = *yyf++;
}
# endif
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 319 "/usr/local/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (short, yyss);
	YYSTACK_RELOCATE (YYSTYPE, yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (YYLTYPE, yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 6:
#line 177 "icalyacc.y"
{ 
                  yyextra->value = 0;
                  yyclearin;
                  }
    break;
case 7:
#line 185 "icalyacc.y"
{
	    struct icaltimetype stm = icaltime_null_date();
	    stm = fill_datetime(&stm, yyvsp[0].v_string, 0, 0);
	    yyextra->value = icalvalue_new_date(stm);
	}
    break;
case 8:
#line 193 "icalyacc.y"
{yyextra->utc = 0;}
    break;
case 9:
#line 194 "icalyacc.y"
{yyextra->utc = 1;}
    break;
case 10:
#line 199 "icalyacc.y"
{yyextra->utc_b = 0;}
    break;
case 11:
#line 200 "icalyacc.y"
{yyextra->utc_b = 1;}
    break;
case 12:
#line 205 "icalyacc.y"
{
	    struct  icaltimetype stm = icaltime_null_time();
	    stm = fill_datetime(&stm, yyvsp[-3].v_string, yyvsp[-1].v_string, yyextra->utc);

	    yyextra->value = 
		icalvalue_new_datetime(stm);
	}
    break;
case 15:
#line 223 "icalyacc.y"
{
	    yyextra->duration.weeks = atoi(yyvsp[-1].v_string);
	}
    break;
case 16:
#line 229 "icalyacc.y"
{
	}
    break;
case 17:
#line 232 "icalyacc.y"
{
	}
    break;
case 18:
#line 235 "icalyacc.y"
{
	}
    break;
case 19:
#line 240 "icalyacc.y"
{
	    yyextra->duration.hours = atoi(yyvsp[-1].v_string);
	}
    break;
case 20:
#line 244 "icalyacc.y"
{
	    yyextra->duration.hours = atoi(yyvsp[-2].v_string);
	}
    break;
case 21:
#line 250 "icalyacc.y"
{
	    yyextra->duration.minutes = atoi(yyvsp[-1].v_string);
	}
    break;
case 22:
#line 254 "icalyacc.y"
{
	    yyextra->duration.minutes = atoi(yyvsp[-2].v_string);
	}
    break;
case 23:
#line 260 "icalyacc.y"
{
	    yyextra->duration.seconds = atoi(yyvsp[-1].v_string);
	}
    break;
case 24:
#line 266 "icalyacc.y"
{
	    yyextra->duration.days = atoi(yyvsp[-1].v_string);
	}
    break;
case 25:
#line 272 "icalyacc.y"
{
	    yyextra->duration.is_neg = 0;
	}
    break;
case 26:
#line 276 "icalyacc.y"
{
	    yyextra->duration.is_neg = 0;
	}
    break;
case 27:
#line 280 "icalyacc.y"
{ 
	  yyextra->duration.is_neg = 1;
	}
    break;
case 28:
#line 286 "icalyacc.y"
{ 
	    yyextra->value = icalvalue_new_duration(yyextra->duration); 
	    memset(&yyextra->duration,0, sizeof(struct icaldurationtype));
	}
    break;
case 29:
#line 291 "icalyacc.y"
{ 
	    yyextra->value = icalvalue_new_duration(yyextra->duration); 
	    memset(&yyextra->duration,0, sizeof(struct icaldurationtype));
	}
    break;
case 30:
#line 296 "icalyacc.y"
{ 
	    yyextra->value = icalvalue_new_duration(yyextra->duration); 
	    memset(&yyextra->duration,0, sizeof(struct icaldurationtype));
	}
    break;
case 31:
#line 306 "icalyacc.y"
{
            struct icalperiodtype p;

	    p.start = icaltime_null_time();
	    p.start = fill_datetime(&p.start, yyvsp[-8].v_string, yyvsp[-6].v_string, yyextra->utc);

	    p.end = icaltime_null_time();
	    p.end = fill_datetime(&p.end, yyvsp[-3].v_string,yyvsp[-1].v_string, yyextra->utc_b);
		
	    p.duration.days = -1;
	    p.duration.weeks = -1;
	    p.duration.hours = -1;
	    p.duration.minutes = -1;
	    p.duration.seconds = -1;

	    yyextra->value = icalvalue_new_period(p);
	}
    break;
case 32:
#line 324 "icalyacc.y"
{
            struct icalperiodtype p;
	    
	    p.start = icaltime_null_time();	  
	    p.start = fill_datetime(&p.start, yyvsp[-5].v_string,yyvsp[-3].v_string, yyextra->utc);

	    p.end = icaltime_null_time();	  

	    /* The duration_value rule setes the global 'duration'
               variable, but it also creates a new value in
               yyextra->value. So, free that, then copy
               'duration' into the icalperiodtype struct. */

	    p.duration = icalvalue_get_duration(yyextra->value);
	    icalvalue_free(yyextra->value);
	    yyextra->value = 0;

	    yyextra->value = icalvalue_new_period(p);

	}
    break;
case 33:
#line 349 "icalyacc.y"
{ yyextra->utcsign = 1; }
    break;
case 34:
#line 350 "icalyacc.y"
{ yyextra->utcsign = -1; }
    break;
case 35:
#line 355 "icalyacc.y"
{
	    yyextra->value = icalvalue_new_utcoffset( yyextra->utcsign * ((yyvsp[-1].v_int*3600) + (yyvsp[0].v_int*60)) );
  	}
    break;
case 36:
#line 360 "icalyacc.y"
{
	    yyextra->value = icalvalue_new_utcoffset(yyextra->utcsign * ((yyvsp[-2].v_int*3600) + (yyvsp[-1].v_int*60) +(yyvsp[0].v_int)));
  	}
    break;
}

#line 705 "/usr/local/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 365 "icalyacc.y"


static struct icaltimetype fill_datetime(struct icaltimetype *stm, char* datestr, char* timestr, int utc)
{
	    if (datestr != 0){
		sscanf(datestr,"%4d%2d%2d",&(stm->year), &(stm->month), 
		       &(stm->day));
	    }

	    if (timestr != 0){
		sscanf(timestr,"%2d%2d%2d", &(stm->hour), &(stm->minute), 
		       &(stm->second));
	    }

	    stm->is_utc = utc;
	    if (utc) {
		stm->zone = icaltimezone_get_utc_timezone();
	    }

	    return *stm;

}

void ical_yyerror(char* s)
{
    /*fprintf(stderr,"Parse error \'%s\'\n", s);*/
}

