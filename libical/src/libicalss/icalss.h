#ifdef __cplusplus
extern "C" {
#endif
/*
 $Id: icalss.h,v 1.24 2002-12-05 13:40:02 acampi Exp $
*/
/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalgauge.h
 CREATOR: eric 23 December 1999



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

#ifndef ICALGAUGE_H
#define ICALGAUGE_H

/** @file icalgauge.h
 *  @brief Routines implementing a filter for ical components
 */

typedef struct icalgauge_impl icalgauge;

icalgauge* icalgauge_new_from_sql(char* sql, int expand);

int icalgauge_get_expand(icalgauge* gauge);

void icalgauge_free(icalgauge* gauge);

char* icalgauge_as_sql(icalcomponent* gauge);

void icalgauge_dump(icalgauge* gauge);


/** @brief Return true if comp matches the gauge.
 *
 * The component must be in
 * cannonical form -- a VCALENDAR with one VEVENT, VTODO or VJOURNAL
 * sub component 
 */
int icalgauge_compare(icalgauge* g, icalcomponent* comp);

/** Clone the component, but only return the properties 
 *  specified in the gauge */
icalcomponent* icalgauge_new_clone(icalgauge* g, icalcomponent* comp);

#endif /* ICALGAUGE_H*/
/* -*- Mode: C -*- */
/**
 @file icalset.h
 @author eric 28 November 1999

 Icalset is the "base class" for representations of a collection of
 iCal components. Derived classes (actually delegatees) include:
 
    icalfileset   Store components in a single file
    icaldirset    Store components in multiple files in a directory
    icalbdbset    Store components in a Berkeley DB File
    icalheapset   Store components on the heap
    icalmysqlset  Store components in a mysql database. 
**/

/*

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

#ifndef ICALSET_H
#define ICALSET_H

#include <limits.h> /* For PATH_MAX */

#ifdef PATH_MAX
#define ICAL_PATH_MAX PATH_MAX
#else
#define ICAL_PATH_MAX 1024
#endif


typedef struct icalset_impl icalset;

typedef enum icalset_kind {
    ICAL_FILE_SET,
    ICAL_DIR_SET,
    ICAL_BDB_SET
} icalset_kind;

typedef struct icalsetiter
{
	icalcompiter iter;    /* icalcomponent_kind, pvl_elem iter */
	icalgauge* gauge;
        icalrecur_iterator* ritr; /*the last iterator*/
        icalcomponent* last_component; /*the pending recurring component to be processed  */
	const char* tzid; /* the calendar's timezone id */
} icalsetiter;

struct icalset_impl {
        icalset_kind kind;
	int size;
        char *dsn;
        icalset* (*init)(icalset* set, const char *dsn, void *options);
	void (*free)(icalset* set);
	const char* (*path)(icalset* set);
	void (*mark)(icalset* set);
	icalerrorenum (*commit)(icalset* set); 
	icalerrorenum (*add_component)(icalset* set, icalcomponent* comp);
	icalerrorenum (*remove_component)(icalset* set, icalcomponent* comp);
	int (*count_components)(icalset* set,
			     icalcomponent_kind kind);
	icalerrorenum (*select)(icalset* set, icalgauge* gauge);
	void (*clear)(icalset* set);
	icalcomponent* (*fetch)(icalset* set, const char* uid);
	icalcomponent* (*fetch_match)(icalset* set, icalcomponent *comp);
	int (*has_uid)(icalset* set, const char* uid);
	icalerrorenum (*modify)(icalset* set, icalcomponent *old,
				     icalcomponent *newc);
	icalcomponent* (*get_current_component)(icalset* set);	
	icalcomponent* (*get_first_component)(icalset* set);
	icalcomponent* (*get_next_component)(icalset* set);
	icalsetiter (*icalset_begin_component)(icalset* set,
					 icalcomponent_kind kind, icalgauge* gauge);
	icalcomponent* (*icalsetiter_to_next)(icalset* set, icalsetiter* i);
	icalcomponent* (*icalsetiter_to_prior)(icalset* set, icalsetiter* i);
};

/** @brief Register a new derived class */
int icalset_register_class(icalset *set);


/** @brief Generic icalset constructor
 *  
 *  @param kind     The type of icalset to create
 *  @param dsn      Data Source Name - usually a pathname or DB handle
 *  @param options  Any implementation specific options
 *
 *  @return         A valid icalset reference or NULL if error.
 * 
 *  This creates any of the icalset types available.
 */

icalset* icalset_new(icalset_kind kind, const char* dsn, void* options);

icalset* icalset_new_file(const char* path);
icalset* icalset_new_file_reader(const char* path);
icalset* icalset_new_file_writer(const char* path);

icalset* icalset_new_dir(const char* path);
icalset* icalset_new_file_reader(const char* path);
icalset* icalset_new_file_writer(const char* path);

void icalset_free(icalset* set);

const char* icalset_path(icalset* set);

/** Mark the cluster as changed, so it will be written to disk when it
    is freed. **/
void icalset_mark(icalset* set);

/** Write changes to disk immediately */
icalerrorenum icalset_commit(icalset* set); 

icalerrorenum icalset_add_component(icalset* set, icalcomponent* comp);
icalerrorenum icalset_remove_component(icalset* set, icalcomponent* comp);

int icalset_count_components(icalset* set,
			     icalcomponent_kind kind);

/** Restrict the component returned by icalset_first, _next to those
    that pass the gauge. */
icalerrorenum icalset_select(icalset* set, icalgauge* gauge);

/** Clears the gauge defined by icalset_select() */
void icalset_clear_select(icalset* set);

/** Get a component by uid */
icalcomponent* icalset_fetch(icalset* set, const char* uid);

int icalset_has_uid(icalset* set, const char* uid);
icalcomponent* icalset_fetch_match(icalset* set, icalcomponent *c);

/** Modify components according to the MODIFY method of CAP. Works on
   the currently selected components. */
icalerrorenum icalset_modify(icalset* set, icalcomponent *oldc,
			       icalcomponent *newc);

/** Iterate through the components. If a guage has been defined, these
   will skip over components that do not pass the gauge */

icalcomponent* icalset_get_current_component(icalset* set);
icalcomponent* icalset_get_first_component(icalset* set);
icalcomponent* icalset_get_next_component(icalset* set);

/** External Iterator with gauge - for thread safety */
extern icalsetiter icalsetiter_null;

icalsetiter icalset_begin_component(icalset* set,
				 icalcomponent_kind kind, icalgauge* gauge);

/** Default _next, _prior, _deref for subclasses that use single cluster */
icalcomponent* icalsetiter_next(icalsetiter* i);
icalcomponent* icalsetiter_prior(icalsetiter* i);
icalcomponent* icalsetiter_deref(icalsetiter* i);

/** for subclasses that use multiple clusters that require specialized cluster traversal */
icalcomponent* icalsetiter_to_next(icalset* set, icalsetiter* i);
icalcomponent* icalsetiter_to_prior(icalset* set, icalsetiter* i);

#endif /* !ICALSET_H */



/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalcluster.h
 CREATOR: eric 23 December 1999



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

#ifndef ICALCLUSTER_H
#define ICALCLUSTER_H


typedef struct icalcluster_impl icalcluster;

icalcluster* icalcluster_new(const char *key, icalcomponent *data);
icalcluster* icalcluster_new_clone(const icalcluster *cluster);

void icalcluster_free(icalcluster *cluster);

const char* icalcluster_key(icalcluster *cluster);
int icalcluster_is_changed(icalcluster *cluster);
void icalcluster_mark(icalcluster *cluster);
void icalcluster_commit(icalcluster *cluster);

icalcomponent* icalcluster_get_component(icalcluster* cluster);
int icalcluster_count_components(icalcluster *cluster, icalcomponent_kind kind);
icalerrorenum icalcluster_add_component(icalcluster* cluster,
					icalcomponent* child);
icalerrorenum icalcluster_remove_component(icalcluster* cluster,
					   icalcomponent* child);

icalcomponent* icalcluster_get_current_component(icalcluster* cluster);
icalcomponent* icalcluster_get_first_component(icalcluster* cluster);
icalcomponent* icalcluster_get_next_component(icalcluster* cluster);

#endif /* !ICALCLUSTER_H */



/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalfileset.h
 CREATOR: eric 23 December 1999



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

#ifndef ICALFILESET_H
#define ICALFILESET_H

#include <sys/types.h> /* For open() flags and mode */
#include <sys/stat.h> /* For open() flags and mode */
#include <fcntl.h> /* For open() flags and mode */

#ifdef WIN32
#define mode_t int
#endif

typedef struct icalfileset_impl icalfileset;

icalset* icalfileset_new(const char* path);
icalset* icalfileset_new_reader(const char* path);
icalset* icalfileset_new_writer(const char* path);

icalset* icalfileset_init(icalset *set, const char *dsn, void* options);

icalfileset* icalfileset_new_from_cluster(const char* path, icalcluster *cluster);

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
				 icalcomponent_kind kind);

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
icalsetiter icalfileset_begin_component(icalset* set, icalcomponent_kind kind, icalgauge* gauge);
icalcomponent * icalfilesetiter_to_next(icalset* set, icalsetiter *iter);
icalcomponent* icalfileset_form_a_matched_recurrence_component(icalsetiter* itr);

/** Return a reference to the internal component. You probably should
   not be using this. */

icalcomponent* icalfileset_get_component(icalset* cluster);

/** 
 * @brief options for opening an icalfileset.
 *
 * These options should be passed to the icalset_new() function
 */

typedef struct icalfileset_options {
  int          flags;		/**< flags for open() O_RDONLY, etc  */
  mode_t       mode;		/**< file mode */
  int          safe_saves;	/**< to lock or not */
  icalcluster  *cluster;	/**< use this cluster to initialize data */
} icalfileset_options;

extern icalfileset_options icalfileset_options_default;

#endif /* !ICALFILESET_H */



/* -*- Mode: C -*- */
/*======================================================================
 FILE: icaldirset.h
 CREATOR: eric 28 November 1999



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

#ifndef ICALDIRSET_H
#define ICALDIRSET_H


/* icaldirset Routines for storing, fetching, and searching for ical
 * objects in a database */

typedef struct icaldirset_impl icaldirset;

icalset* icaldirset_new(const char* path);

icalset* icaldirset_new_reader(const char* path);
icalset* icaldirset_new_writer(const char* path);


icalset* icaldirset_init(icalset* set, const char *dsn, void *options);
void icaldirset_free(icalset* set);

const char* icaldirset_path(icalset* set);

/* Mark the cluster as changed, so it will be written to disk when it
   is freed. Commit writes to disk immediately*/
void icaldirset_mark(icalset* set);
icalerrorenum icaldirset_commit(icalset* set);

icalerrorenum icaldirset_add_component(icalset* store, icalcomponent* comp);
icalerrorenum icaldirset_remove_component(icalset* store, icalcomponent* comp);

int icaldirset_count_components(icalset* store,
			       icalcomponent_kind kind);

/* Restrict the component returned by icaldirset_first, _next to those
   that pass the gauge. _clear removes the gauge. */
icalerrorenum icaldirset_select(icalset* store, icalgauge* gauge);
void icaldirset_clear(icalset* store);

/* Get a component by uid */
icalcomponent* icaldirset_fetch(icalset* store, const char* uid);
int icaldirset_has_uid(icalset* store, const char* uid);
icalcomponent* icaldirset_fetch_match(icalset* set, icalcomponent *c);

/* Modify components according to the MODIFY method of CAP. Works on
   the currently selected components. */
icalerrorenum icaldirset_modify(icalset* store, icalcomponent *oldc,
			       icalcomponent *newc);

/* Iterate through the components. If a gauge has been defined, these
   will skip over components that do not pass the gauge */

icalcomponent* icaldirset_get_current_component(icalset* store);
icalcomponent* icaldirset_get_first_component(icalset* store);
icalcomponent* icaldirset_get_next_component(icalset* store);

/* External iterator for thread safety */
icalsetiter icaldirset_begin_component(icalset* set, icalcomponent_kind kind, icalgauge* gauge);
icalcomponent* icaldirsetiter_to_next(icalset* set, icalsetiter* i);
icalcomponent* icaldirsetiter_to_prior(icalset* set, icalsetiter* i);

typedef struct icaldirset_options {
  int flags;			/**< flags corresponding to the open() system call O_RDWR, etc. */
} icaldirset_options;

#endif /* !ICALDIRSET_H */



/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalcalendar.h
 CREATOR: eric 23 December 1999



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

#ifndef ICALCALENDAR_H
#define ICALCALENDAR_H


/* icalcalendar
 * Routines for storing calendar data in a file system. The calendar 
 * has two icaldirsets, one for incoming components and one for booked
 * components. It also has interfaces to access the free/busy list
 * and a list of calendar properties */

typedef struct icalcalendar_impl icalcalendar;

icalcalendar* icalcalendar_new(char* dir);

void icalcalendar_free(icalcalendar* calendar);

int icalcalendar_lock(icalcalendar* calendar);

int icalcalendar_unlock(icalcalendar* calendar);

int icalcalendar_islocked(icalcalendar* calendar);

int icalcalendar_ownlock(icalcalendar* calendar);

icalset* icalcalendar_get_booked(icalcalendar* calendar);

icalset* icalcalendar_get_incoming(icalcalendar* calendar);

icalset* icalcalendar_get_properties(icalcalendar* calendar);

icalset* icalcalendar_get_freebusy(icalcalendar* calendar);


#endif /* !ICALCALENDAR_H */



/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalclassify.h
 CREATOR: eric 21 Aug 2000



 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/


 =========================================================================*/

#ifndef ICALCLASSIFY_H
#define ICALCLASSIFY_H


icalproperty_xlicclass icalclassify(icalcomponent* c,icalcomponent* match, 
			      const char* user);

icalcomponent* icalclassify_find_overlaps(icalset* set, icalcomponent* comp);

char* icalclassify_class_to_string(icalproperty_xlicclass c);


#endif /* ICALCLASSIFY_H*/


				    


/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalspanlist.h
 CREATOR: eric 21 Aug 2000



 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/


 =========================================================================*/
#ifndef ICALSPANLIST_H
#define ICALSPANLIST_H


/** @file icalspanlist.h
 *  @brief Code that supports collections of free/busy spans of time
 */

typedef struct icalspanlist_impl icalspanlist;


/** @brief Constructor
 * Make a free list from a set of component. Start and end should be in UTC 
 */

icalspanlist* icalspanlist_new(icalset *set, 
				struct icaltimetype start,
				struct icaltimetype end);

/** @brief Destructor
 */
void icalspanlist_free(icalspanlist* spl);

/* Unimplemented functions */
icalcomponent* icalspanlist_make_free_list(icalspanlist* sl);
icalcomponent* icalspanlist_make_busy_list(icalspanlist* sl);

/** Get first next free time after time t. all times are in UTC. */
struct icalperiodtype icalspanlist_next_free_time(icalspanlist* sl,
						struct icaltimetype t);
/** Get first next busy time after time t. all times are in UTC. */
struct icalperiodtype icalspanlist_next_busy_time(icalspanlist* sl,
						struct icaltimetype t);

void icalspanlist_dump(icalspanlist* s);

/** @brief Return a valid VFREEBUSY component for this span */
icalcomponent *icalspanlist_as_vfreebusy(icalspanlist* s_in,
					 const char* organizer,
					 const char* attendee);

/** @brief Return an integer matrix of total events per delta_t timespan */
int *icalspanlist_as_freebusy_matrix(icalspanlist* span, int delta_t);

/** @brief Construct an icalspanlist from a VFREEBUSY component */
icalspanlist *icalspanlist_from_vfreebusy(icalcomponent* c);

#endif
				    


/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalmessage.h
 CREATOR: eric 07 Nov 2000



 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/


 =========================================================================*/


#ifndef ICALMESSAGE_H
#define ICALMESSAGE_H


icalcomponent* icalmessage_new_accept_reply(icalcomponent* c, 
					    const char* user,
					    const char* msg);

icalcomponent* icalmessage_new_decline_reply(icalcomponent* c,
					    const char* user,
					    const char* msg);

/* New is modified version of old */
icalcomponent* icalmessage_new_counterpropose_reply(icalcomponent* oldc,
						    icalcomponent* newc,
						    const char* user,
						    const char* msg);


icalcomponent* icalmessage_new_delegate_reply(icalcomponent* c,
					      const char* user,
					      const char* delegatee,
					      const char* msg);


icalcomponent* icalmessage_new_cancel_event(icalcomponent* c,
					    const char* user,
					    const char* msg);
icalcomponent* icalmessage_new_cancel_instance(icalcomponent* c,
					    const char* user,
					    const char* msg);
icalcomponent* icalmessage_new_cancel_all(icalcomponent* c,
					    const char* user,
					    const char* msg);


icalcomponent* icalmessage_new_error_reply(icalcomponent* c,
					   const char* user,
					   const char* msg,
					   const char* debug,
					   icalrequeststatus rs);


#endif /* ICALMESSAGE_H*/
#ifdef __cplusplus
};
#endif
