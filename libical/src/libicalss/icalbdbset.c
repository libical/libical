/* -*- Mode: C -*-
  ======================================================================
  FILE: icalbdbset.c
 ======================================================================*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "icalbdbset.h"
#include "icalgauge.h"
#include <errno.h>
#include <sys/stat.h> /* for stat */
#include <unistd.h> /* for stat, getpid */
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> /* for fcntl */
#include <unistd.h> /* for fcntl */
#include "icalbdbsetimpl.h"

#define STRBUF_LEN 255

extern int errno;



/* these are just stub functions */
icalerrorenum icalbdbset_read_database(icalbdbset* bset, char *(*pfunc)(const DBT *dbt));
icalerrorenum icalbdbset_create_cluster(const char *path);
int icalbdbset_cget(DBC *dbcp, DBT *key, DBT *data, int access_method);

static int _compare_keys(DB *dbp, const DBT *a, const DBT *b);

    
/** Default options used when NULL is passed to icalset_new() **/
icalbdbset_options icalbdbset_options_default = {ICALBDB_EVENTS, DB_BTREE, 0644, 0, NULL, NULL};


static DB_ENV *ICAL_DB_ENV = 0;

/* Initialize the db environment */

int icalbdbset_init_dbenv(char *db_env_dir) {
  int ret;
  int flags;

  if (db_env_dir) {
    struct stat env_dir_sb;
    
    if (stat(db_env_dir, &env_dir_sb)) {
      fprintf(stderr, "The directory '%s' is missing, please create it.\n", db_env_dir);
      return EINVAL;
    }
  }
  
  ret = db_env_create(&ICAL_DB_ENV, 0);

  if (ret) {
    /* some kind of error... */
    return ret;
  }

  /* Do deadlock detection internally */
  if ((ret = ICAL_DB_ENV->set_lk_detect(ICAL_DB_ENV, DB_LOCK_DEFAULT)) != 0) {
    char * foo = db_strerror(ret);
    fprintf(stderr, "Could not initialize the database locking environment\n");
    return ret;
  }    

  flags = DB_INIT_LOCK | DB_INIT_TXN | DB_CREATE | DB_THREAD | \
    DB_RECOVER | DB_INIT_LOG | DB_INIT_MPOOL;
  ret = ICAL_DB_ENV->open(ICAL_DB_ENV, db_env_dir,  flags, S_IRUSR|S_IWUSR);
  
  if (ret) {
    char * foo = db_strerror(ret);
    ICAL_DB_ENV->err(ICAL_DB_ENV, ret, "dbenv->open");
    return ret;
  }
  return ret;
}

void icalbdbset_checkpoint(void) {
  int ret;
  char *err;

  switch (ret = ICAL_DB_ENV->txn_checkpoint(ICAL_DB_ENV, 0,0,0)) {
    
  case 0:
  case DB_INCOMPLETE:
    break;
  default:
    err = db_strerror(ret);
    ICAL_DB_ENV->err(ICAL_DB_ENV, ret, "checkpoint failed");
    abort();
  }
}

void icalbdbset_cleanup(void) {
  int ret;

  /* one last checkpoint.. */
  icalbdbset_checkpoint();

  if (ICAL_DB_ENV) 
     ret = ICAL_DB_ENV->close(ICAL_DB_ENV, 0);
}

DB_ENV *icalbdbset_get_env(void) {
  return ICAL_DB_ENV;
}


/** Initialize an icalbdbset.  Also attempts to populate from the
 *  database (primary if only dbp is given, secondary if sdbp is
 *  given) and creates an empty object if retrieval is unsuccessful.
 *  pfunc is used to unpack data from the database.  If not given, we
 *  assume data is a string.
 */

icalset* icalbdbset_init(icalset* set, const char* dsn, void* options_in)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalbdbset_options *options = options_in;
    int ret;
    DB *cal_db;
    char *subdb_name;

    if (options == NULL) 
      *options = icalbdbset_options_default;

    switch (options->subdb) {
    case ICALBDB_CALENDARS:
      subdb_name = "calendars";
      break;
    case ICALBDB_EVENTS:
      subdb_name = "events";
      break;
    case ICALBDB_TODOS:
      subdb_name = "todos";
      break;
    case ICALBDB_REMINDERS:
      subdb_name = "reminders";
      break;
    }
  
    cal_db = icalbdbset_bdb_open(set->dsn, 
				 subdb_name, 
				 options->dbtype, 
				 options->mode,
				 options->flag);
    if (cal_db == NULL)
      return NULL;

    bset->dbp = cal_db;
    bset->sdbp = NULL;
    bset->gauge = 0;
    bset->cluster = 0;
  
    if ((ret = icalbdbset_read_database(bset, options->pfunc)) != ICAL_NO_ERROR) {
      icalbdbset_free((icalset*)bset);
      return NULL;
    }

    return (icalset *)bset;
}


/** open a database and return a reference to it.  Used only for
    opening the primary index.
    flag = set_flag() DUP | DUP_SORT
 */

icalset* icalbdbset_new(const char* database_filename, 
			icalbdbset_subdb_type subdb_type,
			int dbtype, int flag)
{
  icalbdbset_options options = icalbdbset_options_default;

  options.subdb = subdb_type;
  options.dbtype = dbtype;
  options.flag = flag;
  
  return icalset_new(ICAL_BDB_SET, database_filename, &options);
}

/**
 *  Open a secondary database, used for accessing secondary indices.
 *  The callback function tells icalbdbset how to associate secondary
 *  key information with primary data.  See the BerkeleyDB reference
 *  guide for more information.
 */

DB * icalbdbset_bdb_open_secondary(DB *dbp,
			       const char *database,
			       const char *sub_database, 
			       int (*callback) (DB *db, 
						const DBT *dbt1, 
						const DBT *dbt2, 
						DBT *dbt3),
			       int type)
{
  int ret;
  int flags;
  DB *sdbp;
  DB_TXN *tid;

  if (!sub_database) 
    goto err1;

  if (!ICAL_DB_ENV)
    icalbdbset_init_dbenv(NULL);

  if ((ret = ICAL_DB_ENV->txn_begin(ICAL_DB_ENV, NULL, &tid, 0)) != 0) {
	char *foo = db_strerror(ret);
	abort();
  }

  /* Open/create secondary */
  if((ret = db_create(&sdbp, ICAL_DB_ENV, 0)) != 0) {
	   char foo[512];
	  strcpy(foo , db_strerror(ret));
    dbp->err(dbp, ret, "secondary index: %s", sub_database);
    goto err1;
  }

  if ((ret = sdbp->set_flags(sdbp, DB_DUP | DB_DUPSORT)) != 0) {
    sdbp->err(sdbp, ret, "secondary index: %s", sub_database);


    goto err1;
  }

  /* hrm..  subdbs only use DB_CREATE?? */
  flags = DB_CREATE  | DB_THREAD;

  if ((ret = sdbp->open(sdbp, database, sub_database, type, (u_int32_t) flags, 0644)) != 0) {
	   char foo[512];
	  strcpy(foo, db_strerror(ret));
    dbp->err(dbp, ret, "secondary index: %s", sub_database);
    goto err1;
  }

  /* Associate the primary index with a secondary */
  if((ret = dbp->associate(dbp, sdbp, callback, 0)) != 0) {
    dbp->err(dbp, ret, "secondary index: %s", sub_database);
    goto err1;
  }

  return sdbp;

  if ((ret = tid->commit(tid, 0)) != 0) {
    char * foo = db_strerror(ret);
    abort();
  }

  err1:
    return (0);
}


DB* icalbdbset_bdb_open(const char* path, 
				  const char *subdb, 
				int dbtype, 
				mode_t mode,
				int flag)
{
  DB *dbp, *sdbp;
  DBC *dbcp;
  int ret;
  DB_TXN *tid;
  int flags;

  /* Initialize the correct set of db subsystems (see capdb.c) */
  flags =  DB_CREATE | DB_THREAD;

  /* should just abort here instead of opening an env in the current dir..  */
  if (!ICAL_DB_ENV)
    icalbdbset_init_dbenv(NULL);


  if ((ret = ICAL_DB_ENV->txn_begin(ICAL_DB_ENV, NULL, &tid, 0)) != 0) {
	char *foo = db_strerror(ret);
	abort();
  }

  /* Create and initialize database object, open the database. */
  if ((ret = db_create(&dbp, ICAL_DB_ENV, 0)) != 0) {
    return (0);
  }

  /* set comparison function, if BTREE */
  if (dbtype == DB_BTREE)
    dbp->set_bt_compare(dbp, _compare_keys);

  /* set DUP, DUPSORT */
  if (flag != 0)
    dbp->set_flags(dbp, flag);

  if ((ret = dbp->open(dbp, path, subdb, dbtype, flags, mode)) != 0) {
	   char foo[512];
	  strcpy(foo , db_strerror(ret));

    dbp->err(dbp, ret, "%s (sub-database: %s): open", path, subdb);
    abort();
    return 0;
  }

  if ((ret = tid->commit(tid, 0)) != 0) {
    char * foo = db_strerror(ret);
    abort();
  }

  return (dbp);

err1:
  return (0);
}

    

/* icalbdbset_parse_data -- parses using pfunc to unpack data. */
char *icalbdbset_parse_data(DBT *dbt, char *(*pfunc)(const DBT *dbt)) 
{
  char *ret;

  if(pfunc) {
    ret = (char *)pfunc(dbt);
  } else {
    ret = (char *) dbt->data;
  }

  return (ret);
}

/* This populates a cluster with the entire contents of a database */
icalerrorenum icalbdbset_read_database(icalbdbset* bset, char *(*pfunc)(const DBT *dbt))
{

  DB *dbp;
  DBC *dbcp;
  DBT key, data;
  char *str, *szpstr;
  int ret;
  char keystore[256];
  char datastore[1024];
  char *more_mem = NULL;
  DB_TXN *tid;

  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  if (bset->sdbp) { dbp = bset->sdbp; }
  else { dbp = bset->dbp; }
     
  if(!dbp) { goto err1; }

  bset->cluster = icalcomponent_new(ICAL_XROOT_COMPONENT);

  if ((ret = ICAL_DB_ENV->txn_begin(ICAL_DB_ENV, NULL, &tid, 0)) != 0) {
	char *foo = db_strerror(ret);
	abort();
  }

  /* acquire a cursor for the database */
  if ((ret = dbp->cursor(dbp, tid, &dbcp, 0)) != 0) {
    dbp->err(dbp, ret, "primary index");
    goto err1;
  }

  key.flags = DB_DBT_USERMEM;
  key.data = keystore;
  key.ulen = sizeof(keystore);

  data.flags= DB_DBT_USERMEM;
  data.data = datastore;
  data.ulen = sizeof(datastore);


  /* fetch the key/data pair */
  while (1) {
    ret = dbcp->c_get(dbcp, &key, &data, DB_NEXT);
    if (ret == DB_NOTFOUND) {
      break;
    } else if (ret == ENOMEM) {
      if (more_mem) free (more_mem);
      more_mem = malloc(data.ulen+1024);
      data.data = more_mem;
      data.ulen = data.ulen+1024;
    } else if (ret == DB_LOCK_DEADLOCK) {
      char *foo = db_strerror(ret);
      abort(); /* should retry in case of DB_LOCK_DEADLOCK */
    } else if (ret) {
      char *foo = db_strerror(ret);
      /* some other weird-ass error  */
      dbp->err(dbp, ret, "cursor");
      abort();
    } else {
      icalcomponent *cl;
      
      /* this prevents an array read bounds error */
      if((str = (char *)calloc(data.size + 1, sizeof(char)))==NULL)
	goto err2;
      memcpy(str, (char *)data.data, data.size);
      
      cl = icalparser_parse_string(str);
      
      icalcomponent_add_component(bset->cluster, cl);
      free(str);
    }
  }
  if(ret != DB_NOTFOUND) {
      goto err2;
  }


  if (more_mem) {
      free(more_mem);
      more_mem = NULL;
  }

  if ((ret = dbcp->c_close(dbcp)) != 0) {
        char * foo = db_strerror(ret);
        abort(); /* should retry in case of DB_LOCK_DEADLOCK */
  }

    if ((ret = tid->commit(tid, 0)) != 0) {
        char * foo = db_strerror(ret);
        abort();
    }

  return ICAL_NO_ERROR;

 err2:
  if (more_mem) free(more_mem);
  dbcp->c_close(dbcp);
  abort(); /* should retry in case of DB_LOCK_DEADLOCK */
  return ICAL_INTERNAL_ERROR;

 err1:
  dbp->err(dbp, ret, "cursor index");
  abort();
  return (ICAL_FILE_ERROR);
}


/* XXX add more to this */
void icalbdbset_free(icalset* set)
{
    icalbdbset *bset = (icalbdbset*)set;
    int ret;

    icalerror_check_arg_rv((bset!=0),"bset");

    if (bset->cluster != 0){
	icalbdbset_commit(set);
	icalcomponent_free(bset->cluster);
	bset->cluster=0;
    }

    if(bset->gauge !=0){
	icalgauge_free(bset->gauge);
    }

    if(bset->path != 0){
	free((char *)bset->path);
	bset->path = 0;
    }

    if(bset->sindex != 0) {
      free((char *)bset->sindex);
      bset->sindex = 0;
    }

    if (bset->dbp && 
	((ret = bset->dbp->close(bset->dbp, 0)) != 0)) {
    }
    bset->dbp = NULL;
}

/* return cursor is in rdbcp */
int icalbdbset_acquire_cursor(DB *dbp, DBC **rdbcp) {
  int ret=0;

  if((ret = dbp->cursor(dbp, NULL, rdbcp, 0)) != 0) {
    dbp->err(dbp, ret, "couldn't open cursor");
    goto err1;
  }

  return ICAL_NO_ERROR;

 err1:
  return ICAL_FILE_ERROR;

}

/* returns key/data in arguments */
int icalbdbset_get_first(DBC *dbcp, DBT *key, DBT *data) {
  return icalbdbset_cget(dbcp, key, data, DB_FIRST);
}

int icalbdbset_get_next(DBC *dbcp, DBT *key, DBT *data) {
  return icalbdbset_cget(dbcp, key, data, DB_NEXT);
}

int icalbdbset_get_last(DBC *dbcp, DBT *key, DBT *data) {
  return icalbdbset_cget(dbcp, key, data, DB_LAST);
}

int icalbdbset_get_key(DBC *dbcp, DBT *key, DBT *data) {
  return icalbdbset_cget(dbcp, key, data, DB_SET);
}

int icalbdbset_delete(DB *dbp, DBT *key) {
  DB_TXN *tid;
  int ret;

  if ((ret = ICAL_DB_ENV->txn_begin(ICAL_DB_ENV, NULL, &tid, 0)) != 0) {
	char *foo = db_strerror(ret);
	abort();
  }

  ret = dbp->del(dbp, tid, key, 0);
  if (ret != DB_NOTFOUND && ret != 0) {
        char * foo = db_strerror(ret);
        abort();
  }

  if ((ret = tid->commit(tid, 0)) != 0) {
        char * foo = db_strerror(ret);
        abort();
  }
  return ret;
}

int icalbdbset_cget(DBC *dbcp, DBT *key, DBT *data, int access_method) {
  int ret=0;

  key->flags |= DB_DBT_MALLOC; /* change these to DB_DBT_USERMEM */
  data->flags |= DB_DBT_MALLOC;

  /* fetch the key/data pair */
  if((ret = dbcp->c_get(dbcp, key, data, access_method)) != 0) {
    goto err1;
  }

  return ICAL_NO_ERROR;

 err1:
  return ICAL_FILE_ERROR;
}


int icalbdbset_cput(DBC *dbcp, DBT *key, DBT *data, int access_method) {
  int ret=0;

  key->flags |= DB_DBT_MALLOC; /* change these to DB_DBT_USERMEM */
  data->flags |= DB_DBT_MALLOC;

  /* fetch the key/data pair */
  if((ret = dbcp->c_put(dbcp, key, data, 0)) != 0) {
    goto err1;
  }

  return ICAL_NO_ERROR;

 err1:
  return ICAL_FILE_ERROR;
}


int icalbdbset_put(DB *dbp, DBT *key, DBT *data, int access_method)
{
  int ret=0;
  DB_TXN *tid;

  if ((ret = ICAL_DB_ENV->txn_begin(ICAL_DB_ENV, NULL, &tid, 0)) != 0) {
	char *foo = db_strerror(ret);
	abort();
  }
  
  key->flags |= DB_DBT_MALLOC; /* change these to DB_DBT_USERMEM */
  data->flags |= DB_DBT_MALLOC;

  if((ret = dbp->put(dbp, tid, key, data, access_method))!=0) {
    char *strError = db_strerror(ret);
    icalerror_warn("icalbdbset_put faild: ");
    icalerror_warn(strError);
    tid->abort(tid);
    return ICAL_FILE_ERROR;
  }

  if ((ret = tid->commit(tid, 0)) != 0) {
        char * foo = db_strerror(ret);
        abort();
  }


  return ICAL_NO_ERROR;
}


/** Return the path of the database file **/

const char* icalbdbset_path(icalset* set)
{
    icalerror_check_arg_rz((set!=0),"set");

    return set->dsn;
}

const char* icalbdbset_subdb(icalset* set)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalerror_check_arg_rz((bset!=0),"bset");

    return bset->subdb;
}


/** Write changes out to the database file.
 */

icalerrorenum icalbdbset_commit(icalset *set) {
  DB *dbp;
  DBC *dbcp;
  DBT key, data;
  icalcomponent *c;
  char *str;
  int ret=0;
  char keystore[256];
  char datastore[1024];
  char *more_mem = NULL;
  DB_TXN *tid;
  icalbdbset *bset = (icalbdbset*)set;
  int bad_uid_counter = 0;

  icalerror_check_arg_re((bset!=0),"bset",ICAL_BADARG_ERROR);  

  dbp = bset->dbp;
  icalerror_check_arg_re((dbp!=0),"dbp is invalid",ICAL_BADARG_ERROR);

  if (bset->changed == 0)
    return ICAL_NO_ERROR;

  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));

  key.flags = DB_DBT_USERMEM; 
  key.data = keystore;
  key.ulen = sizeof(keystore);

  data.flags = DB_DBT_USERMEM;
  data.data = datastore;
  data.ulen = sizeof(datastore);
  
  if (!ICAL_DB_ENV)
    icalbdbset_init_dbenv(NULL);

  if ((ret = ICAL_DB_ENV->txn_begin(ICAL_DB_ENV, NULL, &tid, 0)) != 0) {
	char *foo = db_strerror(ret);
	abort();
  }

  /* first delete everything in the database, because there could be removed components */
  if ((ret = dbp->cursor(dbp, tid, &dbcp, DB_DIRTY_READ)) != 0) {
    char *foo = db_strerror(ret);
    abort();
    
    dbp->err(dbp, ret, "primary index");
    /* leave bset->changed set to true */

    return ICAL_INTERNAL_ERROR;
  }

  /* fetch the key/data pair, then delete it */
  while (1) {
    ret = dbcp->c_get(dbcp, &key, &data, DB_NEXT);
    if (ret == DB_NOTFOUND) {
      break;
    } else if (ret == ENOMEM)  {
      if (more_mem) free(more_mem);
      more_mem = malloc(data.ulen+1024);
      data.data = more_mem;
      data.ulen = data.ulen+1024;
    } else if (ret == DB_LOCK_DEADLOCK) {
      char *foo = db_strerror(ret);
      abort(); /* should retry in case of DB_LOCK_DEADLOCK */
    } else if (ret) {
      char *foo = db_strerror(ret);
      /* some other weird-ass error  */
      dbp->err(dbp, ret, "cursor");
      abort();
    } else {
       if ((ret = dbcp->c_del(dbcp,0))!=0) {
         dbp->err(dbp, ret, "cursor");
         if (ret == DB_KEYEMPTY) {
	    /* never actually created, continue onward.. */
            break;
         } else if (ret == DB_LOCK_DEADLOCK) {
            char *foo = db_strerror(ret);
            abort(); /* should retry in case of DB_LOCK_DEADLOCK */
         } else {
            char *foo = db_strerror(ret);
            abort();
         }
       }
    }
  }
  if (more_mem) {
	  free(more_mem);
	  more_mem = NULL;
  }
/*  if ((ret = dbcp->c_close(dbcp)) != 0) {
//     char *foo = db_strerror(ret);
//     abort();
*/  }

  /* this is weird..  does it really hold everything? */

  for(c = icalcomponent_get_first_component(bset->cluster,ICAL_ANY_COMPONENT);
      c != 0;
      c = icalcomponent_get_next_component(bset->cluster,ICAL_ANY_COMPONENT)) {

    /* Note that we're always inserting into a primary index. */
    str = icalcomponent_as_ical_string(c);
    if (icalcomponent_isa(c) != ICAL_VAGENDA_COMPONENT)  {
      char *uidstr = (char *)icalcomponent_get_uid(c);
      if (!uidstr) {
	/* no uid string, we need to add one
	   TODO use a better uid generator function... */
	char uidstr[256];
	snprintf(uidstr, 256, "baduid%d-%d", getpid(), bad_uid_counter++);
	key.data = strdup(uidstr);
      } else {
      key.data = strdup(uidstr);
      }

    } else {
      char *relcalid = NULL;

      relcalid = (char*)icalcomponent_get_relcalid(c);

      if (relcalid == NULL) {
         relcalid = (char*)icalcomponent_get_relcalid(c);
         abort();
      } else {
         key.data = strdup(relcalid);
      }
    }
    key.size = strlen(key.data);

    data.data = strdup(str);
    data.size = strlen(str);

    if ((ret = dbcp->c_put(dbcp, &key, &data, DB_KEYLAST)) != 0) {
      char * foo = db_strerror(ret);
      abort(); /* should retry in case of DB_LOCK_DEADLOCK */
    }

    free((char *)key.data);
    free((char *)data.data);
  } 

  if ((ret = dbcp->c_close(dbcp)) != 0) {
    char *foo = db_strerror(ret);
    abort(); /* should retry in case of DB_LOCK_DEADLOCK */
  }

  if ((ret = tid->commit(tid, 0)) != 0) {
    char * foo = db_strerror(ret);
    abort();
  }


  bset->changed = 0;    

  return ICAL_NO_ERROR;
} 


void icalbdbset_mark(icalset* set)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalerror_check_arg_rv((bset!=0),"bset");

    bset->changed = 1;
}


icalcomponent* icalbdbset_get_component(icalset* set)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalerror_check_arg_rz((bset!=0),"bset");

    return bset->cluster;
}


/* manipulate the components in the cluster */

icalerrorenum icalbdbset_add_component(icalset *set,
					icalcomponent* child)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalerror_check_arg_re((bset!=0),"bset", ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child!=0),"child",ICAL_BADARG_ERROR);

    icalcomponent_add_component(bset->cluster,child);

    icalbdbset_mark(set);

    return ICAL_NO_ERROR;
}


icalerrorenum icalbdbset_remove_component(icalset *set,
					   icalcomponent* child)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalerror_check_arg_re((bset!=0),"bset", ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child!=0),"child",ICAL_BADARG_ERROR);

    icalcomponent_remove_component(bset->cluster,child);

    icalbdbset_mark(set);

    return ICAL_NO_ERROR;
}


int icalbdbset_count_components(icalset *set,
				 icalcomponent_kind kind)
{
    icalbdbset *bset = (icalbdbset*)set;

    if(set == 0){
	icalerror_set_errno(ICAL_BADARG_ERROR);
	return -1;
    }

    return icalcomponent_count_components(bset->cluster,kind);
}


/** Set the gauge **/

icalerrorenum icalbdbset_select(icalset* set, icalgauge* gauge)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalerror_check_arg_re((bset!=0),"bset", ICAL_BADARG_ERROR);
    icalerror_check_arg_re(gauge!=0,"gauge",ICAL_BADARG_ERROR);

    bset->gauge = gauge;

    return ICAL_NO_ERROR;
}

    
/** Clear the gauge **/

void icalbdbset_clear(icalset* set)
{
  icalbdbset *bset = (icalbdbset*)set;
  icalerror_check_arg_rv((bset!=0),"bset");
    
  bset->gauge = 0;
}


icalcomponent* icalbdbset_fetch(icalset* set, icalcomponent_kind kind, const char* uid)
{
    icalcompiter i;    
  icalbdbset *bset = (icalbdbset*)set;
  icalerror_check_arg_re((bset!=0),"bset", ICAL_BADARG_ERROR);
    
  for(i = icalcomponent_begin_component(bset->cluster, kind);
	icalcompiter_deref(&i)!= 0; icalcompiter_next(&i)){
	
	icalcomponent *this = icalcompiter_deref(&i);
	icalproperty  *p = NULL;
	const char    *this_uid = NULL;

	if (this != 0){
	    if (kind == ICAL_VAGENDA_COMPONENT) {
		p = icalcomponent_get_first_property(this,ICAL_RELCALID_PROPERTY);
		if (p != NULL) this_uid = icalproperty_get_relcalid(p);
	    } else {
		p = icalcomponent_get_first_property(this,ICAL_UID_PROPERTY);
		if (p != NULL) this_uid = icalproperty_get_uid(p);
	    }

	    if(this_uid==NULL){
		icalerror_warn("icalbdbset_fetch found a component with no UID");
		continue;
	    }

	    if (strcmp(uid,this_uid)==0){
		return this;
	    }
	}
    }

    return 0;
}


int icalbdbset_has_uid(icalset* store,const char* uid)
{
    assert(0); /* HACK, not implemented */
    return 0;
}


/******* support routines for icalbdbset_fetch_match *********/

struct icalbdbset_id {
    char* uid;
    char* recurrence_id;
    int sequence;
};

void icalbdbset_id_free(struct icalbdbset_id *id)
{
    if(id->recurrence_id != 0){
	free(id->recurrence_id);
    }

    if(id->uid != 0){
	free(id->uid);
    }

}

struct icalbdbset_id icalbdbset_get_id(icalcomponent* comp)
{

    icalcomponent *inner;
    struct icalbdbset_id id;
    icalproperty *p;

    inner = icalcomponent_get_first_real_component(comp);
    
    p = icalcomponent_get_first_property(inner, ICAL_UID_PROPERTY);

    assert(p!= 0);

    id.uid = strdup(icalproperty_get_uid(p));

    p = icalcomponent_get_first_property(inner, ICAL_SEQUENCE_PROPERTY);

    if(p == 0) {
	id.sequence = 0;
    } else { 
	id.sequence = icalproperty_get_sequence(p);
    }

    p = icalcomponent_get_first_property(inner, ICAL_RECURRENCEID_PROPERTY);

    if (p == 0){
	id.recurrence_id = 0;
    } else {
	icalvalue *v;
	v = icalproperty_get_value(p);
	id.recurrence_id = strdup(icalvalue_as_ical_string(v));

	assert(id.recurrence_id != 0);
    }

    return id;
}

/* Find the component that is related to the given
   component. Currently, it just matches based on UID and
   RECURRENCE-ID */

icalcomponent* icalbdbset_fetch_match(icalset* set, icalcomponent *comp)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalcompiter i;    
    struct icalbdbset_id comp_id, match_id;
    
    icalerror_check_arg_rz((bset!=0),"bset");
    comp_id = icalbdbset_get_id(comp);

    for(i = icalcomponent_begin_component(bset->cluster,ICAL_ANY_COMPONENT);
	icalcompiter_deref(&i)!= 0; icalcompiter_next(&i)){
	
	icalcomponent *match = icalcompiter_deref(&i);

	match_id = icalbdbset_get_id(match);

	if(strcmp(comp_id.uid, match_id.uid) == 0 &&
	   ( comp_id.recurrence_id ==0 || 
	     strcmp(comp_id.recurrence_id, match_id.recurrence_id) ==0 )){

	    /* HACK. What to do with SEQUENCE? */

	    icalbdbset_id_free(&match_id);
	    icalbdbset_id_free(&comp_id);
	    return match;
	    
	}
	
	icalbdbset_id_free(&match_id);
    }

    icalbdbset_id_free(&comp_id);
    return 0;

}


icalerrorenum icalbdbset_modify(icalset* set, icalcomponent *old,
				 icalcomponent *newc)
{
    assert(0); /* HACK, not implemented */
    return ICAL_NO_ERROR;
}


/** Iterate through components. */
icalcomponent* icalbdbset_get_current_component (icalset* set)
{
    icalbdbset *bset = (icalbdbset*)set;

    icalerror_check_arg_rz((bset!=0),"bset");

    return icalcomponent_get_current_component(bset->cluster);
}


icalcomponent* icalbdbset_get_first_component(icalset* set)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalcomponent *c=0;

    icalerror_check_arg_rz((bset!=0),"bset");

    do {
        if (c == 0)
	    c = icalcomponent_get_first_component(bset->cluster,
						  ICAL_ANY_COMPONENT);
        else
	    c = icalcomponent_get_next_component(bset->cluster,
						 ICAL_ANY_COMPONENT);

	     if(c != 0 && (bset->gauge == 0 ||
		      icalgauge_compare(bset->gauge,c) == 1)){
	    return c;
	}

      } while (c!=0);

    return 0;
}


icalsetiter icalbdbset_begin_component(icalset* set, icalcomponent_kind kind, icalgauge* gauge)
{
    icalsetiter itr = icalsetiter_null;
    icalcomponent* comp = NULL;
    icalcompiter citr;
    icalbdbset *bset = (icalbdbset*) set;
    struct icaltimetype start, next, end;
    icalproperty *dtstart, *rrule, *prop, *due;
    struct icalrecurrencetype recur;
    int g = 0;
    int i = 0;
    char *str;

    icalerror_check_arg_re((set!=0), "set", icalsetiter_null);

    itr.gauge = gauge;

    citr = icalcomponent_begin_component(bset->cluster, kind);
    comp = icalcompiter_deref(&citr);

    if (gauge == 0) {
        itr.iter = citr;
        return itr;
    }

    while (comp != 0) {

        /* check if it is a recurring component and with guage expand, if so
         * we need to add recurrence-id property to the given component */
        rrule = icalcomponent_get_first_property(comp, ICAL_RRULE_PROPERTY);
        g = icalgauge_get_expand(gauge);

        if (rrule != 0
            && g == 1) {

            recur = icalproperty_get_rrule(rrule);

            if (icalcomponent_isa(comp) == ICAL_VEVENT_COMPONENT) {
                dtstart = icalcomponent_get_first_property(comp, ICAL_DTSTART_PROPERTY);
                if (dtstart)
                    start = icalproperty_get_dtstart(dtstart);
            } else if (icalcomponent_isa(comp) == ICAL_VTODO_COMPONENT) {
                    due = icalcomponent_get_first_property(comp, ICAL_DUE_PROPERTY);
                    if (due)
                        start = icalproperty_get_due(due);
            }

            if (itr.last_component == NULL) {
                itr.ritr = icalrecur_iterator_new(recur, start);
                next = icalrecur_iterator_next(itr.ritr);
                itr.last_component = comp;
            }
            else {
                next = icalrecur_iterator_next(itr.ritr);
                if (icaltime_is_null_time(next)){
                    itr.last_component = NULL;
                    icalrecur_iterator_free(itr.ritr);
                    itr.ritr = NULL;
                    return icalsetiter_null;
                } else {
                    itr.last_component = comp;
                }
            }

            /* add recurrence-id to the component
             * if there is a recurrence-id already, remove it, then add the new one */
            if (prop = icalcomponent_get_first_property(comp, ICAL_RECURRENCEID_PROPERTY))
                icalcomponent_remove_property(comp, prop);
            icalcomponent_add_property(comp, icalproperty_new_recurrenceid(next));

        }

        if (gauge == 0 || icalgauge_compare(itr.gauge, comp) == 1) {
            /* matches and returns */
            itr.iter = citr;
            return itr;
        }
       
        /* if there is no previous component pending, then get the next component  */
        if (itr.last_component == NULL)
            comp =  icalcompiter_next(&citr);
    }

    return icalsetiter_null;
}

icalcomponent* icalbdbset_form_a_matched_recurrence_component(icalsetiter* itr)
{
    icalcomponent* comp = NULL;
    struct icaltimetype start, next, end;
    icalproperty *dtstart, *rrule, *prop, *due;
    struct icalrecurrencetype recur;
    int g = 0;
    int i = 0;
    char *str;

    comp = itr->last_component;

    if (comp == NULL || itr->gauge == NULL) {
        return NULL;
    }

    rrule = icalcomponent_get_first_property(comp, ICAL_RRULE_PROPERTY);

    recur = icalproperty_get_rrule(rrule);

    if (icalcomponent_isa(comp) == ICAL_VEVENT_COMPONENT) {
        dtstart = icalcomponent_get_first_property(comp, ICAL_DTSTART_PROPERTY);
        if (dtstart)
            start = icalproperty_get_dtstart(dtstart);
    } else if (icalcomponent_isa(comp) == ICAL_VTODO_COMPONENT) {
        due = icalcomponent_get_first_property(comp, ICAL_DUE_PROPERTY);
        if (due)
            start = icalproperty_get_due(due);
    }

    if (itr->ritr == NULL) {
        itr->ritr = icalrecur_iterator_new(recur, start);
        next = icalrecur_iterator_next(itr->ritr);
        itr->last_component = comp;
    } else {
        next = icalrecur_iterator_next(itr->ritr);
        if (icaltime_is_null_time(next)){
            /* no more recurrence, returns */
            itr->last_component = NULL;
            icalrecur_iterator_free(itr->ritr);
            itr->ritr = NULL;
            return NULL;
        } else {
            itr->last_component = comp;
        }
    }

    /* add recurrence-id to the component */
     * if there is a recurrence-id already, remove it, then add the new one */
    if (prop = icalcomponent_get_first_property(comp, ICAL_RECURRENCEID_PROPERTY))
        icalcomponent_remove_property(comp, prop);
        icalcomponent_add_property(comp, icalproperty_new_recurrenceid(next));

     if (itr->gauge == 0 || icalgauge_compare(itr->gauge, comp) == 1) {
         /* matches and returns */
         return comp;
     } 
     /* not matched */
     return NULL;


}

icalcomponent* icalbdbsetiter_to_next(icalset *set, icalsetiter* i)
{

    icalcomponent* c = NULL;
    icalbdbset *bset = (icalbdbset*) set;
    struct icaltimetype start, next, end;
    icalproperty *dtstart, *rrule, *prop, *due;
    struct icalrecurrencetype recur;
    int g = 0;
    char *str;


    do {
        c = icalcompiter_next(&(i->iter));

        if (c == 0) continue;
        if (i->gauge == 0) return c;


        rrule = icalcomponent_get_first_property(c, ICAL_RRULE_PROPERTY);
        g = icalgauge_get_expand(i->gauge);

        /*a recurring component with expand query */
        if (rrule != 0
            && g == 1) {

            recur = icalproperty_get_rrule(rrule);

            if (icalcomponent_isa(c) == ICAL_VEVENT_COMPONENT) {
                dtstart = icalcomponent_get_first_property(c, ICAL_DTSTART_PROPERTY);
                if (dtstart)
                    start = icalproperty_get_dtstart(dtstart);
            } else if (icalcomponent_isa(c) == ICAL_VTODO_COMPONENT) {
                due = icalcomponent_get_first_property(c, ICAL_DUE_PROPERTY);
                if (due)
                    start = icalproperty_get_due(due);
            }

            if (i->ritr == NULL) {
                i->ritr = icalrecur_iterator_new(recur, start);
                next = icalrecur_iterator_next(i->ritr);
                i->last_component = c;
            } else {
                next = icalrecur_iterator_next(i->ritr);
                if (icaltime_is_null_time(next)) {
                    /* no more recurrence, returns */
                    i->last_component = NULL;
                    icalrecur_iterator_free(i->ritr);
                    i->ritr = NULL;
                    return NULL;
                } else {
                    i->last_component = c;
                }
            }

            /* add recurrence-id to the component *
             * if there is a recurrence-id already, remove it, then add the new one */
        if (prop = icalcomponent_get_first_property(c, ICAL_RECURRENCEID_PROPERTY))
            icalcomponent_remove_property(c, prop);
        icalcomponent_add_property(c, icalproperty_new_recurrenceid(next));

        }

        if(c != 0 && (i->gauge == 0 ||
                        icalgauge_compare(i->gauge, c) == 1)){
            return c;
        }
    } while (c != 0);

    return 0;

}

icalcomponent* icalbdbset_get_next_component(icalset* set)
{
    icalbdbset *bset = (icalbdbset*)set;
    icalcomponent *c=0;

    struct icaltimetype start, next;
    icalproperty *dtstart, *rrule, *prop, *due;
    struct icalrecurrencetype recur;
    int g = 0;

    icalerror_check_arg_rz((bset!=0),"bset");
    
    do {
            c = icalcomponent_get_next_component(bset->cluster,
					     ICAL_ANY_COMPONENT);
                if(c != 0 && (bset->gauge == 0 ||
                    icalgauge_compare(bset->gauge,c) == 1)){
                    return c;
                }

    } while(c != 0);
    
    return 0;
}


static int _compare_keys(DB *dbp, const DBT *a, const DBT *b)
{
/* 
 * Returns: 
 * < 0 if a < b 
 * = 0 if a = b 
 * > 0 if a > b 
 */ 
	
    char*  ac = (char*)a->data;
    char*  bc = (char*)b->data;
    return (strncmp(ac, bc, a->size));
}



