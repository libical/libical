/* -*- Mode: C -*-*/
/*======================================================================
  FILE: ical.i

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

  Contributions from:
  Graham Davison (g.m.davison@computer.org)

  ======================================================================*/  

%module LibicalWrap


%{
#include "ical.h"
#include "icalss.h"

#include <sys/types.h> /* for size_t */
#include <time.h>

%}


#include "fcntl.h" /* For Open flags */

typedef void icalcomponent;
typedef void icalproperty;

icalcomponent* icalparser_parse_string(char* str);


/* actually takes icalcomponent_kind */
icalcomponent* icalcomponent_new(int kind);
icalcomponent* icalcomponent_new_clone(icalcomponent* component);
icalcomponent* icalcomponent_new_from_string(char* str);

const char* icalcomponent_kind_to_string(int kind);
int icalcomponent_string_to_kind(const char* string);


char* icalcomponent_as_ical_string(icalcomponent* component);

void icalcomponent_free(icalcomponent* component);
int icalcomponent_count_errors(icalcomponent* component);
void icalcomponent_strip_errors(icalcomponent* component);
void icalcomponent_convert_errors(icalcomponent* component);

icalproperty* icalcomponent_get_current_property(icalcomponent* component);

icalproperty* icalcomponent_get_first_property(icalcomponent* component,
					      int kind);
icalproperty* icalcomponent_get_next_property(icalcomponent* component,
					      int kind);

icalcomponent* icalcomponent_get_current_component (icalcomponent* component);

icalcomponent* icalcomponent_get_first_component(icalcomponent* component,
					      int kind);
icalcomponent* icalcomponent_get_next_component(icalcomponent* component,
					      int kind);

void icalcomponent_add_property(icalcomponent* component,
				icalproperty* property);

void icalcomponent_remove_property(icalcomponent* component,
				   icalproperty* property);


void icalcomponent_add_component(icalcomponent* parent,
				icalcomponent* child);

void icalcomponent_remove_component(icalcomponent* parent,
				icalcomponent* child);

icalcomponent* icalcomponent_get_inner(icalcomponent* comp);

icalcomponent* icalcomponent_get_parent(icalcomponent* component);
int icalcomponent_isa(icalcomponent* component);

int icalrestriction_check(icalcomponent* comp);

/* actually takes icalproperty_kind */
icalproperty* icalproperty_new(int kind);

icalproperty* icalproperty_new_from_string(char* str);

char* icalproperty_as_ical_string(icalproperty *prop);

void icalproperty_set_parameter_from_string(icalproperty* prop,
                                          const char* name, const char* value);
const char* icalproperty_get_parameter_as_string(icalproperty* prop,
                                                 const char* name);
void icalproperty_remove_parameter_by_name(icalproperty* prop,
                                           const char *name);

void icalproperty_set_value_from_string(icalproperty* prop,const char* value, const char * kind);

const char* icalproperty_get_value_as_string(icalproperty* prop);
icalcomponent* icalproperty_get_parent(icalproperty* property);

const char* icalproperty_kind_to_string(int kind);
int icalproperty_string_to_kind(const char* string);
int icalproperty_string_to_enum(const char* str);
int icalproperty_enum_belongs_to_property(int kind, int e);
int icalproperty_kind_to_value_kind(int kind);

/* Deal with X properties */

void icalproperty_set_x_name(icalproperty* prop, const char* name);
const char* icalproperty_get_x_name(icalproperty* prop);

/* Return the name of the property -- the type name converted to a
   string, or the value of _get_x_name if the type is and X property */
const char* icalproperty_get_name (const icalproperty* prop);


int icalerror_supress(const char* error);
void icalerror_restore(const char* error, int es);
char* icalerror_perror();
void icalerror_clear_errno(void);


const char* icalvalue_kind_to_string(int kind);
int icalvalue_string_to_kind(const char* str);

char* icalparameter_as_ical_string(icalparameter* parameter);

const char* icalparameter_kind_to_string(int kind);
int icalparameter_string_to_kind(const char* string);

int* icallangbind_new_array(int size);
void icallangbind_free_array(int* array);
int icallangbind_access_array(int* array, int index);



/* int icalrecur_expand_recurrence(char* rule, int start, 
				int count, int* array);*/
int icalrecur_expand_recurrence(char* rule, int start, 
				int count, time_t* array);


/* Iterate through properties, components and parameters using strings for the kind */
icalproperty* icallangbind_get_first_property(icalcomponent *c,
                                              const char* prop);

icalproperty* icallangbind_get_next_property(icalcomponent *c,
                                              const char* prop);

icalcomponent* icallangbind_get_first_component(icalcomponent *c,
                                              const char* comp);

icalcomponent* icallangbind_get_next_component(icalcomponent *c,
                                              const char* comp);

icalparameter* icallangbind_get_first_parameter(icalproperty *prop);

icalparameter* icallangbind_get_next_parameter(icalproperty *prop);


/* Return a string that can be evaluated in perl or python to
   generated a hash that holds the property's name, value and
   parameters. Sep is the hash seperation string, "=>" for perl and
   ":" for python */
const char* icallangbind_property_eval_string(icalproperty* prop, char* sep);

int icallangbind_string_to_open_flag(const char* str);

const char* icallangbind_quote_as_ical(const char* str);

/***********************************************************************
 Time routines 
***********************************************************************/


struct icaltimetype
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};	


/* Convert seconds past UNIX epoch to a timetype*/
struct icaltimetype icaltime_from_timet(int v, int is_date);

/** Convert seconds past UNIX epoch to a timetype, using timezones. */
struct icaltimetype icaltime_from_timet_with_zone(int tm,
        int is_date, icaltimezone *zone);

/* Return the time as seconds past the UNIX epoch */
/* Normally, this returns a time_t, but SWIG tries to turn that type
   into a pointer */
int icaltime_as_timet(struct icaltimetype tt);

/* Return a string represention of the time, in RFC2445 format. The
   string is owned by libical */
char* icaltime_as_ical_string(struct icaltimetype tt);

/* create a time from an ISO format string */
struct icaltimetype icaltime_from_string(const char* str);

/* Routines for handling timezones */
/* Return a null time, which indicates no time has been set. This time represent the beginning of the epoch */
struct icaltimetype icaltime_null_time(void);

/* Return true of the time is null. */
int icaltime_is_null_time(struct icaltimetype t);

/* Returns false if the time is clearly invalid, but is not null. This
   is usually the result of creating a new time type buy not clearing
   it, or setting one of the flags to an illegal value. */
int icaltime_is_valid_time(struct icaltimetype t);

/** @brief Return the timezone */
const icaltimezone *icaltime_get_timezone(const struct icaltimetype t);

/** @brief Return the tzid, or NULL for a floating time */
char *icaltime_get_tzid(const struct icaltimetype t);

/** @brief Set the timezone */
struct icaltimetype icaltime_set_timezone(struct icaltimetype *t,
        const icaltimezone *zone);

/* Returns true if time is of DATE type, false if DATE-TIME */
int icaltime_is_date(struct icaltimetype t);

/* Returns true if time is relative to UTC zone */
int icaltime_is_utc(struct icaltimetype t);

/* Reset all of the time components to be in their normal ranges. For
   instance, given a time with minutes=70, the minutes will be reduces
   to 10, and the hour incremented. This allows the caller to do
   arithmetic on times without worrying about overflow or
   underflow. */
struct icaltimetype icaltime_normalize(struct icaltimetype t);

/* Return the day of the year of the given time */
short icaltime_day_of_year(struct icaltimetype t);

/* Create a new time, given a day of year and a year. */
struct icaltimetype icaltime_from_day_of_year(short doy,  short year);

/* Return the day of the week of the given time. Sunday is 0 */
short icaltime_day_of_week(struct icaltimetype t);

/* Return the day of the year for the Sunday of the week that the
   given time is within. */
short icaltime_start_doy_of_week(struct icaltimetype t);

/* Return the week number for the week the given time is within */
short icaltime_week_number(struct icaltimetype t);

/* Return -1, 0, or 1 to indicate that a<b, a==b or a>b */
int icaltime_compare(struct icaltimetype a,struct icaltimetype b);

/* like icaltime_compare, but only use the date parts. */
int icaltime_compare_date_only(struct icaltimetype a, struct icaltimetype b);

/* Return the number of days in the given month */
short icaltime_days_in_month(short month,short year);

/** convert tt, of timezone tzid, into a utc time. Does nothing if the
   time is already UTC.  */
struct icaltimetype icaltime_convert_to_zone(struct icaltimetype tt,
           icaltimezone *zone);



/***********************************************************************
  Duration Routines 
***********************************************************************/


struct icaldurationtype
{
	int is_neg;
	unsigned int days;
	unsigned int weeks;
	unsigned int hours;
	unsigned int minutes;
	unsigned int seconds;
};

struct icaldurationtype icaldurationtype_from_int(int t);
struct icaldurationtype icaldurationtype_from_string(const char*);
int icaldurationtype_as_int(struct icaldurationtype duration);
char* icaldurationtype_as_ical_string(struct icaldurationtype d);
struct icaldurationtype icaldurationtype_null_duration();
int icaldurationtype_is_null_duration(struct icaldurationtype d);

struct icaltimetype  icaltime_add(struct icaltimetype t,
				  struct icaldurationtype  d);

struct icaldurationtype  icaltime_subtract(struct icaltimetype t1,
					   struct icaltimetype t2);


/***********************************************************************
  Period Routines 
***********************************************************************/


struct icalperiodtype 
{
	struct icaltimetype start;
	struct icaltimetype end;
	struct icaldurationtype duration;
};

struct icalperiodtype icalperiodtype_from_string (const char* str);

const char* icalperiodtype_as_ical_string(struct icalperiodtype p);
struct icalperiodtype icalperiodtype_null_period();
int icalperiodtype_is_null_period(struct icalperiodtype p);
int icalperiodtype_is_valid_period(struct icalperiodtype p);

/***********************************************************************
 * timezone handling routines
***********************************************************************/

/** Returns a single builtin timezone, given its Olson city name. */
icaltimezone* icaltimezone_get_builtin_timezone	(const char *location);

/** Returns the UTC timezone. */
icaltimezone* icaltimezone_get_utc_timezone	(void);

/***********************************************************************
  Storage Routines
***********************************************************************/

/** 
 * @brief options for opening an icalfileset.
 *
 * These options should be passed to the icalset_new() function
 */

struct icalfileset_options {
  int          flags;		/**< flags for open() O_RDONLY, etc  */
  mode_t       mode;		/**< file mode */
  icalcluster  *cluster;	/**< use this cluster to initialize data */
};

icalset* icalfileset_new(const char* path);
icalset* icalfileset_new_reader(const char* path);
icalset* icalfileset_new_writer(const char* path);

icalset* icalfileset_init(icalset *set, const char *dsn, void* options);

/* icalfileset* icalfileset_new_from_cluster(const char* path, icalcluster *cluster); */

icalcluster* icalfileset_produce_icalcluster(const char *path);

void icalfileset_free(icalset* cluster);

const char* icalfileset_path(icalset* cluster);

/* Mark the cluster as changed, so it will be written to disk when it
   is freed. Commit writes to disk immediately. */
void icalfileset_mark(icalset* set);
icalerrorenum icalfileset_commit(icalset* set); 

icalerrorenum icalfileset_add_component(icalset* set,
					icalcomponent* child);

icalerrorenum icalfileset_remove_component(icalset* set,
					   icalcomponent* child);

int icalfileset_count_components(icalset* set,
				 int kind);

/**
 * Restrict the component returned by icalfileset_first, _next to those
 * that pass the gauge. _clear removes the gauge 
 */
icalerrorenum icalfileset_select(icalset* set, icalgauge* gauge);

/** clear the gauge **/
void icalfileset_clear(icalset* set);

/** Get and search for a component by uid **/
icalcomponent* icalfileset_fetch(icalset* set, const char* uid);
int icalfileset_has_uid(icalset* set, const char* uid);
icalcomponent* icalfileset_fetch_match(icalset* set, icalcomponent *c);


/**
 *  Modify components according to the MODIFY method of CAP. Works on the
 *  currently selected components. 
 */
icalerrorenum icalfileset_modify(icalset* set, 
				 icalcomponent *oldcomp,
			       icalcomponent *newcomp);

/* Iterate through components. If a gauge has been defined, these
   will skip over components that do not pass the gauge */

icalcomponent* icalfileset_get_current_component (icalset* cluster);
icalcomponent* icalfileset_get_first_component(icalset* cluster);
icalcomponent* icalfileset_get_next_component(icalset* cluster);

/* External iterator for thread safety */
icalsetiter icalfileset_begin_component(icalset* set, int kind, icalgauge* gauge);
icalcomponent * icalfilesetiter_to_next(icalset* set, icalsetiter *iter);
icalcomponent* icalfileset_form_a_matched_recurrence_component(icalsetiter* itr);

/***********************************************************************
  Gauge Routines
***********************************************************************/

icalgauge* icalgauge_new_from_sql(char* sql, int expand);

int icalgauge_get_expand(icalgauge* gauge);

void icalgauge_free(icalgauge* gauge);

/* Pending Implementation */
/* char* icalgauge_as_sql(icalcomponent* gauge); */

void icalgauge_dump(icalgauge* gauge);


/** @brief Return true if comp matches the gauge.
 *
 * The component must be in
 * cannonical form -- a VCALENDAR with one VEVENT, VTODO or VJOURNAL
 * sub component 
 */
int icalgauge_compare(icalgauge* g, icalcomponent* comp);

/* Pending Implementation */
/** Clone the component, but only return the properties 
 *  specified in the gauge */
/* icalcomponent* icalgauge_new_clone(icalgauge* g, icalcomponent* comp); */


