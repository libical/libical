/* -*- Mode: C -*-
    ======================================================================
    FILE: icalspanlist.c
    CREATOR: ebusboom 23 aug 2000
  
    $Id: icalspanlist.c,v 1.6 2002-06-11 18:53:00 acampi Exp $
    $Locker:  $
    
    (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of either: 
    
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html
    
    Or:
    
    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/


    ======================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ical.h"
#include "icalspanlist.h"
#include "pvl.h" 
#include <stdlib.h> /* for free and malloc */
#include <string.h>

struct icalspanlist_impl {
  pvl_list spans;		/**< list of icaltime_span data **/
  struct icaltimetype start;    /**< start time of span **/
  struct icaltimetype end;	/**< end time of span **/
};

/** @brief Internal comparison function for two spans
 *
 * Used to insert spans into the tree in sorted order.
 */

static int compare_span(void* a, void* b)
{
    struct icaltime_span *span_a = (struct icaltime_span *)a ;
    struct icaltime_span *span_b = (struct icaltime_span *)b ;
    
    if(span_a->start == span_b->start){
	return 0;
    } else if(span_a->start < span_b->start){
	return -1;
    }  else { /*if(span_a->start > span->b.start)*/
	return 1;
    }
}


/** @brief Make a free list from a set of VEVENT components.
 *
 *  @param set    A valid icalset containing VEVENTS
 *  @param start  The free list starts at this date/time
 *  @param end    The free list ends at this date/time
 *
 * Given a set of components,  a start time and an end time
 * return a spanlist that contains the free/busy times.
 */

icalspanlist* icalspanlist_new(icalset *set, 
			       struct icaltimetype start,
			       struct icaltimetype end)
{
    struct icaltime_span range;
    pvl_elem itr;
    icalcomponent *c,*inner;
    icalcomponent_kind kind, inner_kind;
    struct icalspanlist_impl *sl; 
    struct icaltime_span *freetime;

    if ( ( sl = (struct icalspanlist_impl*)
	   malloc(sizeof(struct icalspanlist_impl))) == 0) {
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return 0;
    }

    sl->spans =  pvl_newlist();
    sl->start =  start;
    sl->end   =  end;

    range.start = icaltime_as_timet(start);
    range.end = icaltime_as_timet(end);

    /*    printf("Range start: %s",ctime(&range.start));
	  printf("Range end  : %s",ctime(&range.end));*/


    /* Get a list of spans of busy time from the events in the set
       and order the spans based on the start time */

   for(c = icalset_get_first_component(set);
	c != 0;
	c = icalset_get_next_component(set)){

       struct icaltime_span span;

       kind  = icalcomponent_isa(c);
       inner = icalcomponent_get_inner(c);

       if(!inner){
	   continue;
       }

       inner_kind = icalcomponent_isa(inner);

       if( kind != ICAL_VEVENT_COMPONENT &&
	   inner_kind != ICAL_VEVENT_COMPONENT){
	   continue;
       }
       
       icalerror_clear_errno();

	span = icalcomponent_get_span(c);
	span.is_busy = 1;

	if(icalerrno != ICAL_NO_ERROR){
	    continue;
	}

	if ((range.start < span.end && icaltime_is_null_time(end)) ||
	    (range.start < span.end && range.end > span.start )){
	    
	    struct icaltime_span *s;

	    if ((s=(struct icaltime_span *)
		 malloc(sizeof(struct icaltime_span))) == 0){
		icalerror_set_errno(ICAL_NEWFAILED_ERROR);
		return 0;
	    }

	    memcpy(s,&span,sizeof(span));
		
	    pvl_insert_ordered(sl->spans,compare_span,(void*)s);

	}
    }
    
    /* Now Fill in the free time spans. loop through the spans. if the
       start of the range is not within the span, create a free entry
       that runs from the start of the range to the start of the
       span. */

     for( itr = pvl_head(sl->spans);
	 itr != 0;
	 itr = pvl_next(itr))
    {
	struct icaltime_span *s = (struct icaltime_span*)pvl_data(itr);

	if ((freetime=(struct icaltime_span *)
	     malloc(sizeof(struct icaltime_span))) == 0){
	    icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	    return 0;
	    }

	if(range.start < s->start){
	    freetime->start = range.start; 
	    freetime->end = s->start;
	    
	    freetime->is_busy = 0;


	    pvl_insert_ordered(sl->spans,compare_span,(void*)freetime);
	} else {
	    free(freetime);
	}
	
	range.start = s->end;
    }
     
     /* If the end of the range is null, then assume that everything
        after the last item in the calendar is open and add a span
        that indicates this */

     if( icaltime_is_null_time(end)){
	 struct icaltime_span* last_span;

	 last_span = (struct icaltime_span*)pvl_data(pvl_tail(sl->spans));

	 if (last_span != 0){

	     if ((freetime=(struct icaltime_span *)
		  malloc(sizeof(struct icaltime_span))) == 0){
		 icalerror_set_errno(ICAL_NEWFAILED_ERROR);
		 return 0;
	     }	
	
	     freetime->is_busy = 0;
	     freetime->start = last_span->end;
	     freetime->end = freetime->start;
	     pvl_insert_ordered(sl->spans,compare_span,(void*)freetime);
	 }
     }


     return sl;
}

/** @brief Destructor.
 *  @param s A valid icalspanlist
 *
 *  Free memory associated with the spanlist
 */

void icalspanlist_free(icalspanlist* s)
{
    struct icaltime_span *span;

    if (s == NULL)
      return;

    while( (span=pvl_pop(s->spans)) != 0){
	free(span);
    }
    
    pvl_free(s->spans);
    
    s->spans = 0;

    free(s);
}

/** @brief (Debug) print out spanlist to stdout
 *  @param sl A valid icalspanlist
 */

void icalspanlist_dump(icalspanlist* sl){
     int i = 0;
     pvl_elem itr;

     for( itr = pvl_head(sl->spans);
	 itr != 0;
	 itr = pvl_next(itr))
    {
	struct icaltime_span *s = (struct icaltime_span*)pvl_data(itr);
	
	printf("#%02d %d start: %s",++i,s->is_busy,ctime(&s->start));
	printf("      end  : %s",ctime(&s->end));

    }
}

icalcomponent* icalspanlist_make_free_list(icalspanlist* sl);
icalcomponent* icalspanlist_make_busy_list(icalspanlist* sl);


/** @brief Find next free time span
 *
 *  Given a spanlist and a time, find the next period of time
 *  that is free
 */

struct icalperiodtype icalspanlist_next_free_time(icalspanlist* sl,
						struct icaltimetype t)
{
     pvl_elem itr;
     struct icalperiodtype period;
     struct icaltime_span *s;

     time_t rangett= icaltime_as_timet(t);

     period.start = icaltime_null_time();
     period.end = icaltime_null_time();

     /* Is the reference time before the first span? If so, assume
        that the reference time is free */
     itr = pvl_head(sl->spans);
     s = (struct icaltime_span*)pvl_data(itr);

     if (s == 0){
	 /* No elements in span */
	 return period;
     }

     if(rangett <s->start ){
	 /* End of period is start of first span if span is busy, end
            of the span if it is free */
	 period.start =  t;

	 if (s->is_busy == 0){
	     period.end =  icaltime_from_timet(s->start,0);
	 } else {
	     period.end =  icaltime_from_timet(s->end,0);
	 }

	 return period;
     }

     /* Otherwise, find the first free span that contains the
        reference time. */

     for( itr = pvl_head(sl->spans);
	 itr != 0;
	 itr = pvl_next(itr))
    {
	s = (struct icaltime_span*)pvl_data(itr);

	if(s->is_busy == 0 && s->start >= rangett && 
	    ( rangett < s->end || s->end == s->start)){

	    if (rangett < s->start){
		period.start = icaltime_from_timet(s->start,0);
	    } else {
		period.start = icaltime_from_timet(rangett,0);
	    }
	    
	    period.end = icaltime_from_timet(s->end,0);

	    return period;
	}
			
    }

     period.start = icaltime_null_time();
     period.end = icaltime_null_time();

     return period;
}

struct icalperiodtype icalspanlist_next_busy_time(icalspanlist* sl,
						struct icaltimetype t);


/*** @brief Return a VFREEBUSY component for the corresponding spanlist
 *
 *   @param sl         A valid icalspanlist, from icalspanlist_new()
 *   @param organizer  The organizer specified as MAILTO:user@domain
 *   @param attendee   The attendee specified as MAILTO:user@domain
 *
 * This function returns a VFREEBUSY component for the given spanlist.
 * The start time is mapped to DTSTART, the end time to DTEND.
 * Each busy span is represented as a separate FREEBUSY entry.
 * An attendee parameter is required, and organizer parameter is
 * optional.
 */

icalcomponent *icalspanlist_as_vfreebusy(icalspanlist* sl,
					 const char* organizer,
					 const char* attendee) {
  icalcomponent *comp;
  icalproperty *p;
  struct icaltimetype atime = icaltime_from_timet( time(0),0);
  pvl_elem itr;
  icaltimezone *utc_zone;
  icalparameter *param;

  if (!attendee) {
    icalerror_set_errno(ICAL_USAGE_ERROR);
    return 0;
  }

  utc_zone = icaltimezone_get_utc_timezone ();

  comp = icalcomponent_new_vfreebusy();
  
  icalcomponent_add_property(comp, icalproperty_new_dtstart(sl->start));
  icalcomponent_add_property(comp, icalproperty_new_dtend(sl->end));
  icalcomponent_add_property(comp, icalproperty_new_dtstamp(atime));

  if (organizer) {
    icalcomponent_add_property(comp, icalproperty_new_organizer(organizer));
  }
  icalcomponent_add_property(comp, icalproperty_new_attendee(attendee));

  /* now add the freebusy sections.. */

  for( itr = pvl_head(sl->spans);  itr != 0;  itr = pvl_next(itr)) {
    struct icalperiodtype period;
    struct icaltime_span *s = (struct icaltime_span*)pvl_data(itr);

    if (s->is_busy == 1) {

      period.start = icaltime_from_timet_with_zone (s->start, 0, utc_zone);
      period.end   = icaltime_from_timet_with_zone (s->end, 0, utc_zone);
      period.duration = icaldurationtype_null_duration();


      p = icalproperty_new_freebusy(period);
      param = icalparameter_new_fbtype(ICAL_FBTYPE_BUSY);
      icalproperty_add_parameter(p, param);

      icalcomponent_add_property(comp, p);
    }
    
  }

  return comp;
}
