#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#include <stdlib.h>
#include <string.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20070509

#define YYEMPTY (-1)
#define yyclearin    (yychar = YYEMPTY)
#define yyerrok      (yyerrflag = 0)
#define YYRECOVERING (yyerrflag != 0)

extern int yyparse(void);

static int yygrowstack(void);
#define YYPREFIX "yy"
#line 2 "icalssyacc.y"
/* -*- Mode: C -*-
  ======================================================================
  FILE: icalssyacc.y
  CREATOR: eric 08 Aug 2000
  
  DESCRIPTION:
  
  $Id: icalssyacc.y,v 1.10 2008-01-14 00:35:26 dothebart Exp $
  $Locker:  $

(C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom

  ======================================================================*/
/*#define YYDEBUG 1*/
#include <stdlib.h>
#include <string.h> /* for strdup() */
#include <limits.h> /* for SHRT_MAX*/
#include <libical/ical.h>
#include "icalgauge.h"
#include "icalgaugeimpl.h"

extern struct icalgauge_impl *icalss_yy_gauge;

#define YYPARSE_PARAM yy_globals
#define YYLEX_PARAM yy_globals
#define YY_EXTRA_TYPE  icalgauge_impl*


void sserror(char *s); 

static void ssyacc_add_where(struct icalgauge_impl* impl, char* prop, 
			icalgaugecompare compare , char* value);
static void ssyacc_add_select(struct icalgauge_impl* impl, char* str1);
static void ssyacc_add_from(struct icalgauge_impl* impl, char* str1);
static void set_logic(struct icalgauge_impl* impl,icalgaugelogic l);

/* Don't know why I need this....  */
 
/* TODO: older flex version produce whats here configured as OpenBSD */
/* autoconf should figure out, and which flex version we should be newer than? */
#ifndef __OpenBSD__
int sslex(void *YYPARSE_PARAM);
#else
int sslex();
#endif
#line 63 "icalssyacc.y"
typedef union {
	char* v_string;
} YYSTYPE;
#line 87 "icalssyacc.c"
#define STRING 257
#define SELECT 258
#define FROM 259
#define WHERE 260
#define COMMA 261
#define QUOTE 262
#define EQUALS 263
#define NOTEQUALS 264
#define LESS 265
#define GREATER 266
#define LESSEQUALS 267
#define GREATEREQUALS 268
#define AND 269
#define OR 270
#define EOL 271
#define END 272
#define IS 273
#define NOT 274
#define SQLNULL 275
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    0,    1,    1,    2,    2,    4,    4,    4,
    4,    4,    4,    4,    4,    4,    3,    3,    3,
};
short yylen[] = {                                         2,
    6,    4,    1,    1,    3,    1,    3,    0,    3,    3,
    4,    3,    3,    3,    3,    3,    1,    3,    3,
};
short yydefred[] = {                                      0,
    3,    0,    0,    4,    0,    0,    0,    6,    0,    5,
    0,    0,    0,    0,   17,    7,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    9,   12,   13,   14,   15,
   16,    0,   10,   18,   19,   11,
};
short yydgoto[] = {                                       3,
    5,    9,   14,   15,
};
short yysindex[] = {                                   -248,
    0, -236,    0,    0, -250, -235, -234,    0, -247,    0,
 -233, -232, -261, -254,    0,    0, -231, -230, -229, -228,
 -227, -226, -257, -233, -233,    0,    0,    0,    0,    0,
    0, -275,    0,    0,    0,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,   32,    0,
    1,    0,    0,   33,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    1,    1,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
    0,    0,    0,   -5,
};
#define YYTABLESIZE 271
short yytable[] = {                                      36,
    8,   17,   18,   19,   20,   21,   22,    1,    6,    2,
    7,   23,   11,   12,   24,   25,   32,   33,   34,   35,
    4,    8,   10,   13,   16,   26,   27,   28,   29,   30,
   31,    2,    1,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    8,
    8,
};
short yycheck[] = {                                     275,
    0,  263,  264,  265,  266,  267,  268,  256,  259,  258,
  261,  273,  260,  261,  269,  270,  274,  275,   24,   25,
  257,  257,  257,  257,  257,  257,  257,  257,  257,  257,
  257,    0,    0,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  269,
  270,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 275
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"STRING","SELECT","FROM","WHERE",
"COMMA","QUOTE","EQUALS","NOTEQUALS","LESS","GREATER","LESSEQUALS",
"GREATEREQUALS","AND","OR","EOL","END","IS","NOT","SQLNULL",
};
char *yyrule[] = {
"$accept : query_min",
"query_min : SELECT select_list FROM from_list WHERE where_list",
"query_min : SELECT select_list FROM from_list",
"query_min : error",
"select_list : STRING",
"select_list : select_list COMMA STRING",
"from_list : STRING",
"from_list : from_list COMMA STRING",
"where_clause :",
"where_clause : STRING EQUALS STRING",
"where_clause : STRING IS SQLNULL",
"where_clause : STRING IS NOT SQLNULL",
"where_clause : STRING NOTEQUALS STRING",
"where_clause : STRING LESS STRING",
"where_clause : STRING GREATER STRING",
"where_clause : STRING LESSEQUALS STRING",
"where_clause : STRING GREATEREQUALS STRING",
"where_list : where_clause",
"where_list : where_list AND where_clause",
"where_list : where_list OR where_clause",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      yydebug;
int      yynerrs;
int      yyerrflag;
int      yychar;
short   *yyssp;
YYSTYPE *yyvsp;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static short   *yyss;
static short   *yysslim;
static YYSTYPE *yyvs;
static int      yystacksize;
#line 113 "icalssyacc.y"

static void ssyacc_add_where(struct icalgauge_impl* impl, char* str1, 
	icalgaugecompare compare , char* value_str)
{

    struct icalgauge_where *where;
    char *compstr, *propstr, *c, *s,*l;
    
    if ( (where = malloc(sizeof(struct icalgauge_where))) ==0){
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return;
    }

    memset(where,0,sizeof(struct icalgauge_where));
    where->logic = ICALGAUGELOGIC_NONE;
    where->compare = ICALGAUGECOMPARE_NONE;
    where->comp = ICAL_NO_COMPONENT;
    where->prop = ICAL_NO_PROPERTY;

    /* remove enclosing quotes */
    s = value_str;
    if(*s == '\''){
	s++;
    }
    l = s+strlen(s)-1;
    if(*l == '\''){
	*l=0;
    }
	
    where->value = strdup(s);

    /* Is there a period in str1 ? If so, the string specified both a
       component and a property*/
    if( (c = strrchr(str1,'.')) != 0){
	compstr = str1;
	propstr = c+1;
	*c = '\0';
    } else {
	compstr = 0;
	propstr = str1;
    }


    /* Handle the case where a component was specified */
    if(compstr != 0){
	where->comp = icalenum_string_to_component_kind(compstr);
    } else {
	where->comp = ICAL_NO_COMPONENT;
    }

    where->prop = icalenum_string_to_property_kind(propstr);    

    where->compare = compare;

    if(where->value == 0){
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	free(where->value);
	return;
    }

    pvl_push(impl->where,where);
}

static void set_logic(struct icalgauge_impl* impl,icalgaugelogic l)
{
    pvl_elem e = pvl_tail(impl->where);
    struct icalgauge_where *where = pvl_data(e);

    where->logic = l;
   
}



static void ssyacc_add_select(struct icalgauge_impl* impl, char* str1)
{
    char *c, *compstr, *propstr;
    struct icalgauge_where *where;
    
    /* Uses only the prop and comp fields of the where structure */
    if ( (where = malloc(sizeof(struct icalgauge_where))) ==0){
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return;
    }

    memset(where,0,sizeof(struct icalgauge_where));
    where->logic = ICALGAUGELOGIC_NONE;
    where->compare = ICALGAUGECOMPARE_NONE;
    where->comp = ICAL_NO_COMPONENT;
    where->prop = ICAL_NO_PROPERTY;

    /* Is there a period in str1 ? If so, the string specified both a
       component and a property*/
    if( (c = strrchr(str1,'.')) != 0){
	compstr = str1;
	propstr = c+1;
	*c = '\0';
    } else {
	compstr = 0;
	propstr = str1;
    }


    /* Handle the case where a component was specified */
    if(compstr != 0){
	where->comp = icalenum_string_to_component_kind(compstr);
    } else {
	where->comp = ICAL_NO_COMPONENT;
    }


    /* If the property was '*', then accept all properties */
    if(strcmp("*",propstr) == 0) {
	where->prop = ICAL_ANY_PROPERTY; 	    
    } else {
	where->prop = icalenum_string_to_property_kind(propstr);    
    }
    

    if(where->prop == ICAL_NO_PROPERTY){
      free(where);
      icalerror_set_errno(ICAL_BADARG_ERROR);
      return;
    }

    pvl_push(impl->select,where);
}

static void ssyacc_add_from(struct icalgauge_impl* impl, char* str1)
{
    icalcomponent_kind ckind;

    ckind = icalenum_string_to_component_kind(str1);

    if(ckind == ICAL_NO_COMPONENT){
	assert(0);
    }

    pvl_push(impl->from,(void*)ckind);

}


void sserror(char *s){
  fprintf(stderr,"Parse error \'%s\'\n", s);
  icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
}
#line 422 "icalssyacc.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(void)
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = yyssp - yyss;
    newss = (yyss != 0)
          ? (short *)realloc(yyss, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    yyss  = newss;
    yyssp = newss + i;
    newvs = (yyvs != 0)
          ? (YYSTYPE *)realloc(yyvs, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    yyerror("syntax error");

#ifdef lint
    goto yyerrlab;
#endif

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yyvsp[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 3:
#line 76 "icalssyacc.y"
{ 
                 yyclearin;
		 YYABORT;
           }
break;
case 4:
#line 83 "icalssyacc.y"
{ssyacc_add_select(icalss_yy_gauge,yyvsp[0].v_string);}
break;
case 5:
#line 84 "icalssyacc.y"
{ssyacc_add_select(icalss_yy_gauge,yyvsp[0].v_string);}
break;
case 6:
#line 89 "icalssyacc.y"
{ssyacc_add_from(icalss_yy_gauge,yyvsp[0].v_string);}
break;
case 7:
#line 90 "icalssyacc.y"
{ssyacc_add_from(icalss_yy_gauge,yyvsp[0].v_string);}
break;
case 9:
#line 95 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-2].v_string,ICALGAUGECOMPARE_EQUAL,yyvsp[0].v_string); }
break;
case 10:
#line 96 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-2].v_string,ICALGAUGECOMPARE_ISNULL,""); }
break;
case 11:
#line 97 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-3].v_string,ICALGAUGECOMPARE_ISNOTNULL,""); }
break;
case 12:
#line 98 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-2].v_string,ICALGAUGECOMPARE_NOTEQUAL,yyvsp[0].v_string); }
break;
case 13:
#line 99 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-2].v_string,ICALGAUGECOMPARE_LESS,yyvsp[0].v_string); }
break;
case 14:
#line 100 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-2].v_string,ICALGAUGECOMPARE_GREATER,yyvsp[0].v_string); }
break;
case 15:
#line 101 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-2].v_string,ICALGAUGECOMPARE_LESSEQUAL,yyvsp[0].v_string); }
break;
case 16:
#line 102 "icalssyacc.y"
{ssyacc_add_where(icalss_yy_gauge,yyvsp[-2].v_string,ICALGAUGECOMPARE_GREATEREQUAL,yyvsp[0].v_string); }
break;
case 17:
#line 106 "icalssyacc.y"
{set_logic(icalss_yy_gauge,ICALGAUGELOGIC_NONE);}
break;
case 18:
#line 107 "icalssyacc.y"
{set_logic(icalss_yy_gauge,ICALGAUGELOGIC_AND);}
break;
case 19:
#line 108 "icalssyacc.y"
{set_logic(icalss_yy_gauge,ICALGAUGELOGIC_OR);}
break;
#line 670 "icalssyacc.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    return (1);

yyaccept:
    return (0);
}
