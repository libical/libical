/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalbdbset.h
 CREATOR: dml 12 December 2001
 (C) COPYRIGHT 2001, Critical Path

 $Id: icalbdbset.h,v 1.1 2002-06-13 15:36:56 acampi Exp $
 $Locker:  $
======================================================================*/

#ifndef ICALBDBSET_H
#define ICALBDBSET_H

#include "ical.h"
#include "icalset.h"
#include "icalgauge.h"
#include <db.h>

extern int icalbdbset_safe_saves;

typedef struct icalbdbset_impl icalbdbset;

/* sets up the db environment, should be done in parent thread.. */
int  icalbdbset_init(char *dir);
void icalbdbset_cleanup(void);
void icalbdbset_checkpoint(void);

/* Creates a component handle.  flags allows caller to 
   specify if database is internally a BTREE or HASH */
DB * icalbdbset_database_open(const char* database, 
			      const char *subdb, 
			      int type);

DB * icalbdbset_secondary_open(DB *dbp,
			       const char *subdb,
			       const char *sindex, 
			       int (*callback) (DB *db, 
						const DBT *dbt1, 
						const DBT *dbt2, 
						DBT *dbt3),
			       int type);
			       

/* Like _new, but takes open() flags for opening the file */
DB * icalbdbset_database_open_int(const char* path, 
					 const char *subdb,
					 int type,
					 int flags, 
					 mode_t mode);

int icalbdbset_database_close(DB *dbp);
icalbdbset* icalbdbset_new(DB *dbp, DB *sdbp, char *(*pfunc)(const DBT *dbt));
char *icalbdbset_parse_data(DBT *dbt, char *(*pfunc)(const DBT *dbt)) ;

void icalbdbset_free(icalbdbset* cluster);

/* cursor operations */
int icalbdbset_acquire_cursor(DB *dbp, DBC **rdbcp);
int icalbdbset_cget(DBC *dbcp, DBT *key, DBT *data, int access_method);
int icalbdbset_cput(DBC *dbcp, DBT *key, DBT *data, int access_method);

int icalbdbset_get_first(DBC *dbcp, DBT *key, DBT *data);
int icalbdbset_get_next(DBC *dbcp, DBT *key, DBT *data);
int icalbdbset_get_last(DBC *dbcp, DBT *key, DBT *data);
int icalbdbset_get_key(DBC *dbcp, DBT *key, DBT *data);
int icalbdbset_delete(DB *dbp, DBT *key);
int icalbdbset_put(DB *dbp, DBT *key, DBT *data, int access_method);

const char* icalbdbset_path(icalbdbset* cluster);
const char* icalbdbset_subdb(icalbdbset* cluster);

/* Mark the cluster as changed, so it will be written to disk when it
   is freed. Commit writes to disk immediately. */
void icalbdbset_mark(icalbdbset* cluster);
icalerrorenum icalbdbset_commit(icalbdbset *cluster);

icalerrorenum icalbdbset_add_component(icalbdbset* cluster,
					icalcomponent* child);

icalerrorenum icalbdbset_remove_component(icalbdbset* cluster,
					   icalcomponent* child);

int icalbdbset_count_components(icalbdbset* cluster,
				 icalcomponent_kind kind);

/* Restrict the component returned by icalbdbset_first, _next to those
   that pass the gauge. _clear removes the gauge */
icalerrorenum icalbdbset_select(icalbdbset* store, icalgauge* gauge);
void icalbdbset_clear(icalbdbset* store);

/* Get and search for a component by uid */
icalcomponent* icalbdbset_fetch(icalbdbset* cluster, icalcomponent_kind kind, const char* uid);
int icalbdbset_has_uid(icalbdbset* cluster, const char* uid);
icalcomponent* icalbdbset_fetch_match(icalbdbset* set, icalcomponent *c);

/* Iterate through components. If a gauge has been defined, these
   will skip over components that do not pass the gauge */

icalcomponent* icalbdbset_get_current_component (icalbdbset* cluster);
icalcomponent* icalbdbset_get_first_component(icalbdbset* cluster);
icalcomponent* icalbdbset_get_next_component(icalbdbset* cluster);
/* Return a reference to the internal component. You probably should
   not be using this. */

icalcomponent* icalbdbset_get_component(icalbdbset* cluster);

#endif /* !ICALBDBSET_H */



