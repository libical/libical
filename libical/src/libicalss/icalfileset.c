/* -*- Mode: C -*-
  ======================================================================
  FILE: icalfileset.c
  CREATOR: eric 23 December 1999
  
  $Id: icalfileset.c,v 1.2 2001-01-05 01:56:57 ebusboom Exp $
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "icalfileset.h"
#include "icalgauge.h"
#include <errno.h>
#include <limits.h> /* For PATH_MAX */
#include <sys/stat.h> /* for stat */
#include <unistd.h> /* for stat, getpid */
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> /* for fcntl */
#include <unistd.h> /* for fcntl */

#include "icalfilesetimpl.h"

int icalfileset_lock(icalfileset *cluster);
int icalfileset_unlock(icalfileset *cluster);

icalerrorenum icalfileset_create_cluster(const char *path);

icalfileset* icalfileset_new_impl()
{
    struct icalfileset_impl* impl;

    if ( ( impl = (struct icalfileset_impl*)
	   malloc(sizeof(struct icalfileset_impl))) == 0) {
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	errno = ENOMEM;
	return 0;
    }

    strcpy(impl->id,ICALFILESET_ID);

    return impl;
}

char* read_from_file(char *s, size_t size, void *d)
{
    char *c = fgets(s,size, (FILE*)d);
    return c;
}

icalfileset* icalfileset_new(const char* path)
{
    return icalfileset_new_open(path, O_RDWR, 0664);
}

icalfileset* icalfileset_new_open(const char* path, int flags, mode_t mode)
{
    struct icalfileset_impl *impl = icalfileset_new_impl(); 
    struct stat sbuf;
    int createclusterfile = 0;
    icalparser *parser;
    struct icaltimetype tt;
    off_t cluster_file_size;

    memset(&tt,0,sizeof(struct icaltimetype));

    icalerror_clear_errno();
    icalerror_check_arg_rz( (path!=0), "path");

    if (impl == 0){
	return 0;
    }

    impl->changed  = 0;

    impl->cluster = 0;

    impl->stream = 0;
    impl->path = strdup(path); 
        
    /* Check if the path already exists and if it is a regular file*/
    if (stat(path,&sbuf) != 0){
	
	/* A file by the given name does not exist, or there was
           another error */
	cluster_file_size = 0;
	if (errno == ENOENT) {
	    /* It was because the file does not exist */
	    createclusterfile = 1;
	} else {
	    /* It was because of another error */
	    icalerror_set_errno(ICAL_FILE_ERROR);
	    return 0;
	}
    } else {
	/* A file by the given name exists, but is it a regular file */
	
	if (!S_ISREG(sbuf.st_mode)){ 
	    /* Nope, not a directory */
	    icalerror_set_errno(ICAL_FILE_ERROR);
	    return 0;
	} else {
	    /* Lets assume that it is a file of the right type */
          cluster_file_size = sbuf.st_size;
          createclusterfile = 0;
	}	
    }
    


    /* if cluster does not already exist, create it */
    
    if (createclusterfile == 1) {

	FILE* f;
	
	icalerror_clear_errno();
	
	f = fopen(path,"w");
	
	if (f == 0){
	    icalerror_set_errno(ICAL_FILE_ERROR);
	    icalfileset_free(impl);
	    return 0;
	}
	
	fclose(f);

    } else if(cluster_file_size > 0){

	FILE* f;
	errno = 0;
	f = fopen(impl->path,"r");
	
	if (f ==0 || errno != 0){
	    icalerror_set_errno(ICAL_FILE_ERROR); /* Redundant, actually */
	    icalfileset_free(impl);
	    return 0;
	}
	
	parser = icalparser_new();
	icalparser_set_gen_data(parser,f);
	impl->cluster = icalparser_parse(parser,read_from_file);
	icalparser_free(parser);

	if (impl->cluster == 0 || icalerrno != ICAL_NO_ERROR){
	    icalerror_set_errno(ICAL_PARSE_ERROR);
	    icalfileset_free(impl);
	    return 0;
	}

	if (icalcomponent_isa(impl->cluster) != ICAL_XROOT_COMPONENT){
	    /* The parser got a single component, so it did not put it in
	       an XROOT. */
	    icalcomponent *cl = impl->cluster;
	    impl->cluster = icalcomponent_new(ICAL_XROOT_COMPONENT);
	    icalcomponent_add_component(impl->cluster,cl);
	}

	fclose(f);
    }

    if(impl->cluster == 0){
	impl->cluster = icalcomponent_new(ICAL_XROOT_COMPONENT);
    }      
	
/*  icalfileset_lock(impl);*/
           
    return impl;
}
	
void icalfileset_free(icalfileset* cluster)
{
    struct icalfileset_impl *impl = (struct icalfileset_impl*)cluster;

    icalerror_check_arg_rv((cluster!=0),"cluster");

    if (impl->cluster != 0){
	icalfileset_commit(cluster);
	icalcomponent_free(impl->cluster);
	impl->cluster=0;
    }

    if(impl->path != 0){
	free(impl->path);
	impl->path = 0;
    }

    if(impl->stream != 0){
/*	icalfileset_unlock(impl);*/
	fclose(impl->stream);
	impl->stream = 0;
    }

    free(impl);
}

const char* icalfileset_path(icalfileset* cluster)
{
    struct icalfileset_impl *impl = (struct icalfileset_impl*)cluster;
    icalerror_check_arg_rz((cluster!=0),"cluster");

    return impl->path;
}


int icalfileset_lock(icalfileset *cluster)
{
    struct icalfileset_impl *impl = (struct icalfileset_impl*)cluster;
    struct flock lock;
    int fd; 

    icalerror_check_arg_rz((impl->stream!=0),"impl->stream");

    fd  = fileno(impl->stream);

    lock.l_type = F_WRLCK;     /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = 0;  /* byte offset relative to l_whence */
    lock.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = 0;       /* #bytes (0 means to EOF) */

    return (fcntl(fd, F_SETLKW, &lock)); 
}

int icalfileset_unlock(icalfileset *cluster)
{
    struct icalfileset_impl *impl = (struct icalfileset_impl*)cluster;
    int fd;
    struct flock lock;
    icalerror_check_arg_rz((impl->stream!=0),"impl->stream");

    fd  = fileno(impl->stream);

    lock.l_type = F_WRLCK;     /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = 0;  /* byte offset relative to l_whence */
    lock.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = 0;       /* #bytes (0 means to EOF) */

    return (fcntl(fd, F_UNLCK, &lock)); 

}

icalerrorenum icalfileset_commit(icalfileset* cluster)
{
    FILE *f;
    char tmp[PATH_MAX]; 
    char *str;
    icalcomponent *c;
    
    struct icalfileset_impl *impl = (struct icalfileset_impl*)cluster;

    icalerror_check_arg_re((impl!=0),"cluster",ICAL_BADARG_ERROR);

    if (impl->changed == 0 ){
	return ICAL_NO_ERROR;
    }
    
#ifdef ICAL_SAFESAVES
    snprintf(tmp,PATH_MAX,"%s-tmp",impl->path);
#else	
    strcpy(tmp,impl->path);
#endif
    
    if ( (f = fopen(tmp,"w")) < 0 ){
	icalerror_set_errno(ICAL_FILE_ERROR);
	return ICAL_FILE_ERROR;
    }
    
    for(c = icalcomponent_get_first_component(impl->cluster,ICAL_ANY_COMPONENT);
	c != 0;
	c = icalcomponent_get_next_component(impl->cluster,ICAL_ANY_COMPONENT)){

	str = icalcomponent_as_ical_string(c);
    
	if (  fwrite(str,sizeof(char),strlen(str),f) < strlen(str)){
	    fclose(f);
	    return ICAL_FILE_ERROR;
	}
    }
    
    fclose(f);
    impl->changed = 0;    
        
#ifdef ICAL_SAFESAVES
    rename(tmp,impl->path); /* HACK, should check for error here */
#endif
    
    return ICAL_NO_ERROR;
    
} 

void icalfileset_mark(icalfileset* cluster){

    struct icalfileset_impl *impl = (struct icalfileset_impl*)cluster;

    icalerror_check_arg_rv((impl!=0),"cluster");

    impl->changed = 1;

}

icalcomponent* icalfileset_get_component(icalfileset* cluster){
   struct icalfileset_impl *impl = (struct icalfileset_impl*)cluster;

   icalerror_check_arg_re((impl!=0),"cluster",ICAL_BADARG_ERROR);

   return impl->cluster;
}


/* manipulate the components in the cluster */

icalerrorenum icalfileset_add_component(icalfileset *cluster,
			       icalcomponent* child)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)cluster;

    icalerror_check_arg_rv((cluster!=0),"cluster");
    icalerror_check_arg_rv((child!=0),"child");

    icalcomponent_add_component(impl->cluster,child);

    icalfileset_mark(cluster);

    return ICAL_NO_ERROR;

}

icalerrorenum icalfileset_remove_component(icalfileset *cluster,
				  icalcomponent* child)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)cluster;

    icalerror_check_arg_rv((cluster!=0),"cluster");
    icalerror_check_arg_rv((child!=0),"child");

    icalcomponent_remove_component(impl->cluster,child);

    icalfileset_mark(cluster);

    return ICAL_NO_ERROR;
}

int icalfileset_count_components(icalfileset *cluster,
				 icalcomponent_kind kind)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)cluster;

    if(cluster == 0){
	icalerror_set_errno(ICAL_BADARG_ERROR);
	return -1;
    }

    return icalcomponent_count_components(impl->cluster,kind);
}

icalerrorenum icalfileset_select(icalfileset* set, icalgauge* gauge)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)set;

    icalerror_check_arg_re(gauge!=0,"guage",ICAL_BADARG_ERROR);

    impl->gauge = gauge;

    return ICAL_NO_ERROR;
}

void icalfileset_clear(icalfileset* gauge)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)gauge;
    
    impl->gauge = 0;

}

icalcomponent* icalfileset_fetch(icalfileset* store,const char* uid)
{
    icalcompiter i;    
    struct icalfileset_impl* impl = (struct icalfileset_impl*)store;
    
    for(i = icalcomponent_begin_component(impl->cluster,ICAL_ANY_COMPONENT);
	icalcompiter_deref(&i)!= 0; icalcompiter_next(&i)){
	
	icalcomponent *this = icalcompiter_deref(&i);
	icalcomponent *inner = icalcomponent_get_first_real_component(this);
	icalcomponent *p;
	const char *this_uid;

	if(inner != 0){
	    p = icalcomponent_get_first_property(inner,ICAL_UID_PROPERTY);
	    this_uid = icalproperty_get_uid(p);

	    if(this_uid==0){
		icalerror_warn("icalfileset_fetch found a component with no UID");
		continue;
	    }

	    if (strcmp(uid,this_uid)==0){
		return this;
	    }
	}
    }

    return 0;
}

int icalfileset_has_uid(icalfileset* store,const char* uid)
{
    assert(0); /* HACK, not implemented */
    return 0;
}

/******* support routines for icalfileset_fetch_match *********/

struct icalfileset_id{
    char* uid;
    char* recurrence_id;
    int sequence;
};

void icalfileset_id_free(struct icalfileset_id *id)
{
    if(id->recurrence_id != 0){
	free(id->recurrence_id);
    }

    if(id->uid != 0){
	free(id->uid);
    }

}

struct icalfileset_id icalfileset_get_id(icalcomponent* comp)
{

    icalcomponent *inner;
    struct icalfileset_id id;
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
icalcomponent* icalfileset_fetch_match(icalfileset* set, icalcomponent *comp)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)set;
    icalcompiter i;    

    struct icalfileset_id comp_id, match_id;
    
    comp_id = icalfileset_get_id(comp);

    for(i = icalcomponent_begin_component(impl->cluster,ICAL_ANY_COMPONENT);
	icalcompiter_deref(&i)!= 0; icalcompiter_next(&i)){
	
	icalcomponent *match = icalcompiter_deref(&i);

	match_id = icalfileset_get_id(match);

	if(strcmp(comp_id.uid, match_id.uid) == 0 &&
	   ( comp_id.recurrence_id ==0 || 
	     strcmp(comp_id.recurrence_id, match_id.recurrence_id) ==0 )){

	    /* HACK. What to do with SEQUENCE? */

	    icalfileset_id_free(&match_id);
	    icalfileset_id_free(&comp_id);
	    return match;
	    
	}
	
	icalfileset_id_free(&match_id);
    }

    icalfileset_id_free(&comp_id);
    return 0;

}


icalerrorenum icalfileset_modify(icalfileset* store, icalcomponent *old,
			       icalcomponent *new)
{
    assert(0); /* HACK, not implemented */
    return ICAL_NO_ERROR;
}


/* Iterate through components */
icalcomponent* icalfileset_get_current_component (icalfileset* cluster)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)cluster;

    icalerror_check_arg_rz((cluster!=0),"cluster");

    return icalcomponent_get_current_component(impl->cluster);
}


icalcomponent* icalfileset_get_first_component(icalfileset* cluster)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)cluster;
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
	    
}

icalcomponent* icalfileset_get_next_component(icalfileset* cluster)
{
    struct icalfileset_impl* impl = (struct icalfileset_impl*)cluster;
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

