/* -*- Mode: C -*-
  ======================================================================
  FILE: icalbdbset.c
  CREATOR: eric 23 December 1999
 ======================================================================*/


#if 0 // Many compile errors in this file

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
icalerrorenum icalbdbset_read_database(icalbdbset* cluster, char *(*pfunc)(const DBT *dbt));
icalerrorenum icalbdbset_create_cluster(const char *path);
int icalbdbset_cget(DBC *dbcp, DBT *key, DBT *data, int access_method);

static int _compare_keys(DB *dbp, const DBT *a, const DBT *b);

/* icalbdbset_new_impl -- allocates a new icalbdbset_impl object that
   contains an icalbdbset */

static DB_ENV *ICAL_DB_ENV = 0;

/* Initialize the db environment */

int icalbdbset_init(char *db_env_dir) {
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

static DB_ENV *icalbdbset_get_env(void) {
  return ICAL_DB_ENV;
}

icalbdbset* icalbdbset_new_impl()
{
    struct icalbdbset_impl* impl;
    int flags, ret;

    if ( ( impl = (struct icalbdbset_impl*)
	   malloc(sizeof(struct icalbdbset_impl))) == 0) {
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	errno = ENOMEM;
	return 0;
    }
  
    memset(impl,0,sizeof(struct icalbdbset_impl));
  
    strcpy(impl->id,ICALBDBSET_ID);

    return impl;
}

/* icalbdbset_database_open -- open a database and return a reference
   to it.  Used only for opening the primary index */

DB * icalbdbset_database_open(const char* database, 
			      const char* subdb, 
			      int type)
{
  return icalbdbset_database_open_int(database, 
				      subdb, 
				      type, DB_CREATE|DB_THREAD, 0664);
}

/* 
   icalbdbset_secondary_open -- open a secondary database, used for
   accessing secondary indices.  The callback function tells
   icalbdbset how to associate secondary key information with primary
   data.  See the BerkeleyDB reference guide for more information.
*/

DB * icalbdbset_secondary_open(DB *dbp,
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

  if (!sub_database) 
    goto err1;

  if (!ICAL_DB_ENV)
    icalbdbset_init(NULL);

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

  err1:
    return (0);
}


DB * icalbdbset_database_open_int(const char* path, 
				  const char *subdb, 
				  int type, 
				  int flags, 
				  mode_t mode) {

  DB *dbp, *sdbp;
  DBC *dbcp;
  int ret;

  /* Initialize the correct set of db subsystems (see capdb.c) */
  flags = flags | DB_CREATE | DB_THREAD;

  /* should just abort here instead of opening an env in the current dir..  */
  if (!ICAL_DB_ENV)
    icalbdbset_init(NULL);

  /* Create and initialize database object, open the database. */
  if ((ret = db_create(&dbp, ICAL_DB_ENV, 0)) != 0) {
    return (0);
  }

  /* set comparison function, if BTREE */
  if(mode == DB_BTREE)
    dbp->set_bt_compare(dbp, _compare_keys);

  if ((ret = dbp->open(dbp, path, subdb, type, flags, mode)) != 0) {
	   char foo[512];
	  strcpy(foo , db_strerror(ret));

    dbp->err(dbp, ret, "%s (sub-database: %s): open", path, subdb);
    abort();
    return 0;
  }

  return (dbp);

err1:
  return (0);
}

int icalbdbset_database_close(DB *dbp) {
  int ret;

  if (dbp && ((ret = dbp->close(dbp, 0)) != 0)) {
   	 return (1);
  }
  dbp = NULL;

  return (0);
}


/* icalbdbset_new -- create new instance of icalbdbset.  Also attempts
   to populate from the database (primary if only dbp is given,
   secondary if sdbp is given) and creates an empty object if
   retrieval is unsuccessful.  pfunc is used to unpack data from the
   database.  If not given, we assume data is a string.  
*/

icalbdbset* icalbdbset_new(DB *dbp, DB *sdbp, char *(*pfunc)(const DBT *dbt))
{
  struct icalbdbset_impl *impl;
  struct icaltimetype tt;
  int ret;
    
  memset(&tt,0,sizeof(struct icaltimetype));
    
  icalerror_clear_errno();
  icalerror_check_arg_rz( (dbp!=0), "dbp");

  impl = icalbdbset_new_impl(); 

  if (impl == 0){
    return 0;
  }

  impl->dbp = dbp;
  impl->sdbp = sdbp;
  impl->gauge = 0;
  impl->cluster = 0;

  if ((ret = icalbdbset_read_database(impl, pfunc)) != ICAL_NO_ERROR) {
    icalbdbset_free(impl);
    goto err1;
  }
    
  return (icalbdbset *)impl;

 err1:	
  return NULL;
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
icalerrorenum icalbdbset_read_database(icalbdbset* cluster, char *(*pfunc)(const DBT *dbt))
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

  struct icalbdbset_impl *impl = (struct icalbdbset_impl*)cluster;

  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  if (impl->sdbp) { dbp = impl->sdbp; }
  else { dbp = impl->dbp; }
     
  if(!dbp) { goto err1; }

  impl->cluster = icalcomponent_new(ICAL_XROOT_COMPONENT);

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
      
      icalcomponent_add_component(impl->cluster, cl);
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
void icalbdbset_free(icalbdbset* cluster)
{
    struct icalbdbset_impl *impl = (struct icalbdbset_impl*)cluster;
    int ret;

    icalerror_check_arg_rv((cluster!=0),"cluster");

    if (impl->cluster != 0){
	icalbdbset_commit(cluster);
	icalcomponent_free(impl->cluster);
	impl->cluster=0;
    }

    if(impl->gauge !=0){
	icalgauge_free(impl->gauge);
    }

    if(impl->path != 0){
	free((char *)impl->path);
	impl->path = 0;
    }

    if(impl->sindex != 0) {
      free((char *)impl->sindex);
      impl->sindex = 0;
    }

    free(impl);
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

  key->flags |= DB_DBT_MALLOC; // change these to DB_DBT_USERMEM
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

  key->flags |= DB_DBT_MALLOC; // change these to DB_DBT_USERMEM
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
  
  key->flags |= DB_DBT_MALLOC; // change these to DB_DBT_USERMEM
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

const char* icalbdbset_path(icalbdbset* cluster)
{
    struct icalbdbset_impl *impl = (struct icalbdbset_impl*)cluster;
    icalerror_check_arg_rz((cluster!=0),"cluster");

    return impl->path;
}

const char* icalbdbset_subdb(icalbdbset* cluster)
{
    struct icalbdbset_impl *impl = (struct icalbdbset_impl*)cluster;
    icalerror_check_arg_rz((cluster!=0),"cluster");

    return impl->subdb;
}

icalerrorenum icalbdbset_commit(icalbdbset *cluster) {
  DB *dbp;
  DBC *dbcp;
  DBT key, data;
  icalcomponent *c;
  char *str;
  int ret=0;
  struct icalbdbset_impl *impl = (struct icalbdbset_impl*)cluster;
  char keystore[256];
  char datastore[1024];
  char *more_mem = NULL;
  DB_TXN *tid;


  icalerror_check_arg_re((impl!=0),"cluster",ICAL_BADARG_ERROR);  

  dbp = impl->dbp;

  icalerror_check_arg_re((dbp!=0),"dbp is invalid",ICAL_BADARG_ERROR);

  if (impl->changed == 0)
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
    icalbdbset_init(NULL);

  if ((ret = ICAL_DB_ENV->txn_begin(ICAL_DB_ENV, NULL, &tid, 0)) != 0) {
	char *foo = db_strerror(ret);
	abort();
  }

  /* first delete everything in the database, because there could be removed components */
  if ((ret = dbp->cursor(dbp, tid, &dbcp, DB_DIRTY_READ)) != 0) {
    char *foo = db_strerror(ret);
    abort();
    
    dbp->err(dbp, ret, "primary index");
    /* leave impl->changed set to true */

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
//  if ((ret = dbcp->c_close(dbcp)) != 0) {
//     char *foo = db_strerror(ret);
//     abort();
//  }

  // this is weird..  does it really hold everything?

  for(c = icalcomponent_get_first_component(impl->cluster,ICAL_ANY_COMPONENT);
      c != 0;
      c = icalcomponent_get_next_component(impl->cluster,ICAL_ANY_COMPONENT)) {

    /* Note that we're always inserting into a primary index. */
    str = icalcomponent_as_ical_string(c);
    if (icalcomponent_isa(c) != ICAL_VAGENDA_COMPONENT)  {
      char *uidstr = (char *)icalcomponent_get_uid(c);

      key.data = strdup(uidstr);

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


  impl->changed = 0;    

  return ICAL_NO_ERROR;
    
} 

void icalbdbset_mark(icalbdbset* cluster){

    struct icalbdbset_impl *impl = (struct icalbdbset_impl*)cluster;

    icalerror_check_arg_rv((impl!=0),"cluster");

    impl->changed = 1;
}

icalcomponent* icalbdbset_get_component(icalbdbset* cluster){
    struct icalbdbset_impl *impl = (struct icalbdbset_impl*)cluster;

    icalerror_check_arg_re((impl!=0),"cluster",ICAL_BADARG_ERROR);

    return impl->cluster;
}


/* manipulate the components in the cluster */

icalerrorenum icalbdbset_add_component(icalbdbset *cluster,
					icalcomponent* child)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)cluster;

    icalerror_check_arg_re((cluster!=0),"cluster", ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child!=0),"child",ICAL_BADARG_ERROR);

    icalcomponent_add_component(impl->cluster,child);

    icalbdbset_mark(cluster);

    return ICAL_NO_ERROR;

}

icalerrorenum icalbdbset_remove_component(icalbdbset *cluster,
					   icalcomponent* child)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)cluster;

    icalerror_check_arg_re((cluster!=0),"cluster",ICAL_BADARG_ERROR);
    icalerror_check_arg_re((child!=0),"child",ICAL_BADARG_ERROR);

    icalcomponent_remove_component(impl->cluster,child);

    icalbdbset_mark(cluster);

    return ICAL_NO_ERROR;
}

int icalbdbset_count_components(icalbdbset *cluster,
				 icalcomponent_kind kind)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)cluster;

    if(cluster == 0){
	icalerror_set_errno(ICAL_BADARG_ERROR);
	return -1;
    }

    return icalcomponent_count_components(impl->cluster,kind);
}

icalerrorenum icalbdbset_select(icalbdbset* set, icalgauge* gauge)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)set;

    icalerror_check_arg_re(gauge!=0,"gauge",ICAL_BADARG_ERROR);

    impl->gauge = gauge;

    return ICAL_NO_ERROR;
}

void icalbdbset_clear(icalbdbset* gauge)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)gauge;
    
    impl->gauge = 0;

}

icalcomponent* icalbdbset_fetch(icalbdbset* store, icalcomponent_kind kind, const char* uid)
{
    icalcompiter i;    
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)store;
    
    for(i = icalcomponent_begin_component(impl->cluster, kind);
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

int icalbdbset_has_uid(icalbdbset* store,const char* uid)
{
    assert(0); /* HACK, not implemented */
    return 0;
}

/******* support routines for icalbdbset_fetch_match *********/

struct icalbdbset_id{
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
icalcomponent* icalbdbset_fetch_match(icalbdbset* set, icalcomponent *comp)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)set;
    icalcompiter i;    

    struct icalbdbset_id comp_id, match_id;
    
    comp_id = icalbdbset_get_id(comp);

    for(i = icalcomponent_begin_component(impl->cluster,ICAL_ANY_COMPONENT);
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


icalerrorenum icalbdbset_modify(icalbdbset* store, icalcomponent *old,
				 icalcomponent *new)
{
    assert(0); /* HACK, not implemented */
    return ICAL_NO_ERROR;
}


/* Iterate through components */
icalcomponent* icalbdbset_get_current_component (icalbdbset* cluster)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)cluster;

    icalerror_check_arg_rz((cluster!=0),"cluster");

    return icalcomponent_get_current_component(impl->cluster);
}


icalcomponent* icalbdbset_get_first_component(icalbdbset* cluster)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)cluster;
    icalcomponent *c=0;

    icalerror_check_arg_rz((cluster!=0),"cluster");

    do {
	if (c == 0){
	    c = icalcomponent_get_first_component(impl->cluster,
						  ICAL_ANY_COMPONENT);
	} else {
	    c = icalcomponent_get_next_component(impl->cluster,
						 ICAL_ANY_COMPONENT);
	}

	if(c != 0 && (impl->gauge == 0 ||
		      icalgauge_compare(impl->gauge,c) == 1)){
	    return c;
	}

    } while(c != 0);


    return 0;
}

icalcomponent* icalbdbset_get_next_component(icalbdbset* cluster)
{
    struct icalbdbset_impl* impl = (struct icalbdbset_impl*)cluster;
    icalcomponent *c;

    icalerror_check_arg_rz((cluster!=0),"cluster");
    
    do {
	c = icalcomponent_get_next_component(impl->cluster,
					     ICAL_ANY_COMPONENT);

	if(c != 0 && (impl->gauge == 0 ||
		      icalgauge_compare(impl->gauge,c) == 1)){
	    return c;
	}
	
    } while(c != 0);
    
    
    return 0;
}

static int _compare_keys(DB *dbp, const DBT *a, const DBT *b)
{
  int ai, bi;

/* 
 * Returns: 
 * < 0 if a < b 
 * = 0 if a = b 
 * > 0 if a > b 
 */ 
	
  memcpy(&ai, a->data, sizeof(int)); 
  memcpy(&bi, b->data, sizeof(int)); 
  return (ai - bi); 
}



#endif // #if 0
