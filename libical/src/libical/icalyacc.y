%pure_parser

%{
/* -*- Mode: C -*-
  ======================================================================
  FILE: icalyacc.y
  CREATOR: eric 10 June 1999
  
  DESCRIPTION:
  
  $Id: icalyacc.y,v 1.10 2002-06-07 12:51:17 acampi Exp $
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
struct icaltimetype fill_datetime(struct icaltimetype *, char* d, char* t, int utc);
void ical_yy_error(char *s); /* Don't know why I need this.... */

/* Set the state of the lexer so it will interpret values ( iCAL
   VALUEs, that is, ) correctly. */

%}

%union {
	float v_float;
	int   v_int;
	char* v_string;
}


  /* Renaming hack */

/*
#define    yymaxdepth ical_yy_maxdepth
#define    yyparse ical_yy_parse
#define    yyerror ical_yy_error
#define    yylval  ical_yy_lval
#define    yychar  ical_yy_char
#define    yydebug ical_yy_debug
#define    yypact  ical_yy_pact
#define    yyr1    ical_yy_r1
#define    yyr2    ical_yy_r2
#define    yydef   ical_yy_def
#define    yychk   ical_yy_chk
#define    yypgo   ical_yy_pgo
#define    yyact   ical_yy_act
#define    yyexca  ical_yy_exca
#define yyerrflag ical_yy_errflag
#define yynerrs    ical_yy_nerrs
#define    yyps    ical_yy_ps
#define    yypv    ical_yy_pv
#define    yys     ical_yy_s
#define    yy_yys  ical_yy_yys
#define    yystate ical_yy_state
#define    yytmp   ical_yy_tmp
#define    yyv     ical_yy_v
#define    yy_yyv  ical_yy_yyv
#define    yyval   ical_yy_val
#define    yylloc  ical_yy_lloc
#define yyreds     ical_yy_reds
#define yytoks     ical_yy_toks
#define yylhs      ical_yy_yylhs
#define yylen      ical_yy_yylen
#define yydefred ical_yy_yydefred
#define yydgoto    ical_yy_yydgoto
#define yydefred ical_yy_yydefred
#define yydgoto    ical_yy_yydgoto
#define yysindex ical_yy_yysindex
#define yyrindex ical_yy_yyrindex
#define yygindex ical_yy_yygindex
#define yytable     ical_yy_yytable
#define yycheck     ical_yy_yycheck
#define yyname   ical_yy_yyname
#define yyrule   ical_yy_yyrule
#define yy_scan_bytes ical_yy_scan_bytes
#define yy_scan_string ical_yy_scan_string
#define yy_scan_buffer ical_yy_scan_buffer
*/

/* These are redefined with the -P option to flex */
/*
#define yy_create_buffer  ical_yy_create_buffer 
#define yy_delete_buffer ical_yy_delete_buffer
#define yy_flex_debug ical_yy_flex_debug
#define yy_init_buffer ical_yy_init_buffer
#define yy_flush_buffer ical_yy_flush_buffer
#define yy_load_buffer_state ical_yy_load_buffer_state
#define yy_switch_to_buffer ical_yy_switch_to_buffer
#define yyin ical_yyin
#define yyleng ical_yyleng
#define yylex ical_yylex
#define yylineno ical_yylineno
#define yyout ical_yyout
#define yyrestart ical_yyrestart
#define yytext ical_yytext
#define yywrap ical_yywrap             
*/


%token <v_string> DIGITS
%token <v_int> INTNUMBER
%token <v_float> FLOATNUMBER
%token <v_string> STRING   
%token EOL EQUALS CHARACTER COLON COMMA SEMICOLON MINUS TIMESEPERATOR 

%token YTRUE YFALSE

%token FREQ BYDAY BYHOUR BYMINUTE BYMONTH BYMONTHDAY BYSECOND BYSETPOS BYWEEKNO
%token BYYEARDAY DAILY MINUTELY MONTHLY SECONDLY WEEKLY HOURLY YEARLY
%token INTERVAL COUNT UNTIL WKST MO SA SU TU WE TH FR 

%token BIT8 ACCEPTED ADD AUDIO BASE64 BINARY BOOLEAN BUSY BUSYTENTATIVE
%token BUSYUNAVAILABLE CALADDRESS CANCEL CANCELLED CHAIR CHILD COMPLETED
%token CONFIDENTIAL CONFIRMED COUNTER DATE DATETIME DECLINECOUNTER DECLINED
%token DELEGATED DISPLAY DRAFT DURATION EMAIL END FINAL FLOAT FREE GREGORIAN
%token GROUP INDIVIDUAL INPROCESS INTEGER NEEDSACTION NONPARTICIPANT
%token OPAQUE OPTPARTICIPANT PARENT PERIOD PRIVATE PROCEDURE PUBLIC PUBLISH
%token RECUR REFRESH REPLY REQPARTICIPANT REQUEST RESOURCE ROOM SIBLING
%token START TENTATIVE TEXT THISANDFUTURE THISANDPRIOR TIME TRANSPAENT
%token UNKNOWN UTCOFFSET XNAME

%token ALTREP CN CUTYPE DAYLIGHT DIR ENCODING EVENT FBTYPE FMTTYPE LANGUAGE 
%token MEMBER PARTSTAT RANGE RELATED RELTYPE ROLE RSVP SENTBY STANDARD URI

%token TIME_CHAR UTC_CHAR


%%

value:
	date_value
	| datetime_value
	| duration_value
	| period_value
        | utcoffset_value
        | error { 
                  yyextra->value = 0;
                  yyclearin;
                  }
	;


date_value: DIGITS
        {
	    struct icaltimetype stm = icaltime_null_date();
	    stm = fill_datetime(&stm, $1, 0, 0);
	    yyextra->value = icalvalue_new_date(stm);
	}
	;

utc_char: 
	/*empty*/  {yyextra->utc = 0;}
	| UTC_CHAR {yyextra->utc = 1;}
	;

/* This is used in the period_value, where there may be two utc characters per rule. */
utc_char_b: 
	/*empty*/  {yyextra->utc_b = 0;}
	| UTC_CHAR {yyextra->utc_b = 1;}
	;

datetime_value: 
	DIGITS TIME_CHAR DIGITS utc_char
        {
	    struct  icaltimetype stm = icaltime_null_time();
	    stm = fill_datetime(&stm, $1, $3, yyextra->utc);

	    yyextra->value = 
		icalvalue_new_datetime(stm);
	}
	;


/* Duration */


dur_date: dur_day
	| dur_day dur_time
	;

dur_week: DIGITS 'W'
	{
	    yyextra->duration.weeks = atoi($1);
	}
	;

dur_time: TIME_CHAR dur_hour 
	{
	}
	| TIME_CHAR dur_minute
	{
	}
	| TIME_CHAR dur_second
	{
	}
	;

dur_hour: DIGITS 'H'
	{
	    yyextra->duration.hours = atoi($1);
	}
	| DIGITS 'H' dur_minute
	{
	    yyextra->duration.hours = atoi($1);
	}
	;

dur_minute: DIGITS 'M'
	{
	    yyextra->duration.minutes = atoi($1);
	}
	| DIGITS 'M' dur_second
	{
	    yyextra->duration.minutes = atoi($1);
	}
	;

dur_second: DIGITS 'S'
	{
	    yyextra->duration.seconds = atoi($1);
	}
	;

dur_day: DIGITS 'D'
	{
	    yyextra->duration.days = atoi($1);
	}
	;

dur_prefix: /* empty */
	{
	    yyextra->duration.is_neg = 0;
	} 
	| '+'
	{
	    yyextra->duration.is_neg = 0;
	}
	| '-'
	{ 
	  yyextra->duration.is_neg = 1;
	}
	;

duration_value: dur_prefix 'P' dur_date
	{ 
	    yyextra->value = icalvalue_new_duration(yyextra->duration); 
	    memset(&yyextra->duration,0, sizeof(struct icaldurationtype));
	}
	| dur_prefix 'P' dur_time
	{ 
	    yyextra->value = icalvalue_new_duration(yyextra->duration); 
	    memset(&yyextra->duration,0, sizeof(struct icaldurationtype));
	}
	| dur_prefix 'P' dur_week
	{ 
	    yyextra->value = icalvalue_new_duration(yyextra->duration); 
	    memset(&yyextra->duration,0, sizeof(struct icaldurationtype));
	}
	;


/* Period */

period_value:  DIGITS TIME_CHAR DIGITS utc_char '/'  DIGITS TIME_CHAR DIGITS utc_char_b
	{
            struct icalperiodtype p;

	    p.start = icaltime_null_time();
	    p.start = fill_datetime(&p.start, $1, $3, yyextra->utc);

	    p.end = icaltime_null_time();
	    p.end = fill_datetime(&p.end, $6,$8, yyextra->utc_b);
		
	    p.duration.days = -1;
	    p.duration.weeks = -1;
	    p.duration.hours = -1;
	    p.duration.minutes = -1;
	    p.duration.seconds = -1;

	    yyextra->value = icalvalue_new_period(p);
	}
	| DIGITS TIME_CHAR DIGITS utc_char '/'  duration_value
	{
            struct icalperiodtype p;
	    
	    p.start = icaltime_null_time();	  
	    p.start = fill_datetime(&p.start, $1,$3, yyextra->utc);

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
	;


/* UTC Offset */

plusminus: '+' { yyextra->utcsign = 1; }
	| '-' { yyextra->utcsign = -1; }
	;

utcoffset_value: 
	plusminus INTNUMBER INTNUMBER
	{
	    yyextra->value = icalvalue_new_utcoffset( yyextra->utcsign * (($2*3600) + ($3*60)) );
  	}

	| plusminus INTNUMBER INTNUMBER INTNUMBER
	{
	    yyextra->value = icalvalue_new_utcoffset(yyextra->utcsign * (($2*3600) + ($3*60) +($4)));
  	}
	;

%%

struct icaltimetype fill_datetime(struct icaltimetype *stm, char* datestr, char* timestr, int utc)
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

