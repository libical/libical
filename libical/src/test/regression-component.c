#include "ical.h"
#include "regression.h"

#include <string.h>

icalcomponent* create_simple_component()
{

    icalcomponent* calendar;
    struct icalperiodtype rtime;

    rtime.start = icaltime_from_timet( time(0),0);
    rtime.end = icaltime_from_timet( time(0),0);

    rtime.end.hour++;



    /* Create calendar and add properties */
    calendar = icalcomponent_new(ICAL_VCALENDAR_COMPONENT);

    
    icalcomponent_add_property(
	calendar,
	icalproperty_new_version("2.0")
	);

    printf("%s\n",icalcomponent_as_ical_string(calendar));
 
    return calendar;
	   
}

/* Create a new component */
icalcomponent* create_new_component()
{

    icalcomponent* calendar;
    icalcomponent* timezone;
    icalcomponent* tzc;
    icalcomponent* event;
    struct icaltimetype atime = icaltime_from_timet( time(0),0);
    struct icaldatetimeperiodtype rtime;
    icalproperty* property;

    rtime.period.start = icaltime_from_timet( time(0),0);
    rtime.period.end = icaltime_from_timet( time(0),0);
    rtime.period.end.hour++;
    rtime.time = icaltime_null_time();



    /* Create calendar and add properties */
    calendar = icalcomponent_new(ICAL_VCALENDAR_COMPONENT);

    
    icalcomponent_add_property(
	calendar,
	icalproperty_new_version("2.0")
	);
    
    icalcomponent_add_property(
	calendar,
	icalproperty_new_prodid("-//RDU Software//NONSGML HandCal//EN")
	);
    
    /* Create a timezone object and add it to the calendar */

    timezone = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);

    icalcomponent_add_property(
	timezone,
	icalproperty_new_tzid("America/New_York")
	);

    /* Add a sub-component of the timezone */
    tzc = icalcomponent_new(ICAL_XDAYLIGHT_COMPONENT);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_dtstart(atime)
	);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_rdate(rtime)
	);
	    
    icalcomponent_add_property(
	tzc, 
	icalproperty_new_tzoffsetfrom(-4.0)
	);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_tzoffsetto(-5.0)
	);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_tzname("EST")
	);

    icalcomponent_add_component(timezone,tzc);

    icalcomponent_add_component(calendar,timezone);

    /* Add a second subcomponent */
    tzc = icalcomponent_new(ICAL_XSTANDARD_COMPONENT);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_dtstart(atime)
	);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_rdate(rtime)
	);
	    
    icalcomponent_add_property(
	tzc, 
	icalproperty_new_tzoffsetfrom(-4.0)
	);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_tzoffsetto(-5.0)
	);

    icalcomponent_add_property(
	tzc, 
	icalproperty_new_tzname("EST")
	);

    icalcomponent_add_component(timezone,tzc);

    /* Add an event */

    event = icalcomponent_new(ICAL_VEVENT_COMPONENT);

    icalcomponent_add_property(
	event,
	icalproperty_new_dtstamp(atime)
	);

    icalcomponent_add_property(
	event,
	icalproperty_new_uid("guid-1.host1.com")
	);

    /* add a property that has parameters */
    property = icalproperty_new_organizer("mrbig@host.com");
    
    icalproperty_add_parameter(
	property,
	icalparameter_new_role(ICAL_ROLE_CHAIR)
	);

    icalcomponent_add_property(event,property);

    /* add another property that has parameters */
    property = icalproperty_new_attendee("employee-A@host.com");
    
    icalproperty_add_parameter(
	property,
	icalparameter_new_role(ICAL_ROLE_REQPARTICIPANT)
	);

    icalproperty_add_parameter(
	property,
	icalparameter_new_rsvp(ICAL_RSVP_TRUE)
	);

    icalproperty_add_parameter(
	property,
	icalparameter_new_cutype(ICAL_CUTYPE_GROUP)
	);

    icalcomponent_add_property(event,property);


    /* more properties */

    icalcomponent_add_property(
	event,
	icalproperty_new_description("Project XYZ Review Meeting")
	);

    icalcomponent_add_property(
	event,
	icalproperty_new_categories("MEETING")
	);

    icalcomponent_add_property(
	event,
	icalproperty_new_class(ICAL_CLASS_PRIVATE)
	);
    
    icalcomponent_add_property(
	event,
	icalproperty_new_created(atime)
	);

    icalcomponent_add_property(
	event,
	icalproperty_new_summary("XYZ Project Review")
	);


    property = icalproperty_new_dtstart(atime);
    
    icalproperty_add_parameter(
	property,
	icalparameter_new_tzid("America/New_York")
	);

    icalcomponent_add_property(event,property);


    property = icalproperty_new_dtend(atime);
    
    icalproperty_add_parameter(
	property,
	icalparameter_new_tzid("America/New_York")
	);

    icalcomponent_add_property(event,property);

    icalcomponent_add_property(
	event,
	icalproperty_new_location("1CP Conference Room 4350")
	);

    icalcomponent_add_component(calendar,event);

    printf("%s\n",icalcomponent_as_ical_string(calendar));

    icalcomponent_free(calendar);

    return 0;
}

/* Create a new component, using the va_args list */

icalcomponent* create_new_component_with_va_args()
{

    icalcomponent* calendar;
    struct icaltimetype atime = icaltime_from_timet( time(0),0);
    struct icaldatetimeperiodtype rtime;
    
    rtime.period.start = icaltime_from_timet( time(0),0);
    rtime.period.end = icaltime_from_timet( time(0),0);
    rtime.period.end.hour++;
    rtime.time = icaltime_null_time();

    calendar = 
	icalcomponent_vanew(
	    ICAL_VCALENDAR_COMPONENT,
	    icalproperty_new_version("2.0"),
	    icalproperty_new_prodid("-//RDU Software//NONSGML HandCal//EN"),
	    icalcomponent_vanew(
		ICAL_VTIMEZONE_COMPONENT,
		icalproperty_new_tzid("America/New_York"),
		icalcomponent_vanew(
		    ICAL_XDAYLIGHT_COMPONENT,
		    icalproperty_new_dtstart(atime),
		    icalproperty_new_rdate(rtime),
		    icalproperty_new_tzoffsetfrom(-4.0),
		    icalproperty_new_tzoffsetto(-5.0),
		    icalproperty_new_tzname("EST"),
		    0
		    ),
		icalcomponent_vanew(
		    ICAL_XSTANDARD_COMPONENT,
		    icalproperty_new_dtstart(atime),
		    icalproperty_new_rdate(rtime),
		    icalproperty_new_tzoffsetfrom(-5.0),
		    icalproperty_new_tzoffsetto(-4.0),
		    icalproperty_new_tzname("EST"),
		    0
		    ),
		0
		),
	    icalcomponent_vanew(
		ICAL_VEVENT_COMPONENT,
		icalproperty_new_dtstamp(atime),
		icalproperty_new_uid("guid-1.host1.com"),
		icalproperty_vanew_organizer(
		    "mrbig@host.com",
		    icalparameter_new_role(ICAL_ROLE_CHAIR),
		    0
		    ),
		icalproperty_vanew_attendee(
		    "employee-A@host.com",
		    icalparameter_new_role(ICAL_ROLE_REQPARTICIPANT),
		    icalparameter_new_rsvp(ICAL_RSVP_TRUE),
		    icalparameter_new_cutype(ICAL_CUTYPE_GROUP),
		    0
		    ),
		icalproperty_new_description("Project XYZ Review Meeting"),
		icalproperty_new_categories("MEETING"),
		icalproperty_new_class(ICAL_CLASS_PUBLIC),
		icalproperty_new_created(atime),
		icalproperty_new_summary("XYZ Project Review"),
		icalproperty_vanew_dtstart(
		    atime,
		    icalparameter_new_tzid("America/New_York"),
		    0
		    ),
		icalproperty_vanew_dtend(
		    atime,
		    icalparameter_new_tzid("America/New_York"),
		    0
		    ),
		icalproperty_new_location("1CP Conference Room 4350"),
		0
		),
	    0
	    );
	
    printf("%s\n",icalcomponent_as_ical_string(calendar));
    

    icalcomponent_free(calendar);

    return 0;
}

static void print_span(int c, struct icaltime_span span ){

    if (span.start == 0)
	printf("#%02d start: (empty)\n",c);
    else 
	printf("#%02d start: %s\n",c,ical_timet_string(span.start));

    if (span.end == 0)
	printf("    end  : (empty)\n");
    else 
	printf("    end  : %s\n",ical_timet_string(span.end));

}

/** Test icalcomponent_get_span()
 *
 */
void test_icalcomponent_get_span()
{
    time_t tm1 = 973378800; /*Sat Nov  4 23:00:00 UTC 2000,
			      Sat Nov  4 15:00:00 PST 2000 */
    time_t tm2 = 973382400; /*Sat Nov  5 00:00:00 UTC 2000 
			      Sat Nov  4 16:00:00 PST 2000 */
    struct icaldurationtype dur;
    struct icaltime_span span;
    icalcomponent *c;
    icaltimezone *azone, *bzone; 
    int	tnum = 0;

    /** test 0
     *	Direct assigning time_t means they will be interpreted as UTC
     */
    span.start = tm1;
    span.end = tm2;
    print_span(tnum++,span);

    /** test 1
     *	We specify times in a timezone, the returned span is in UTC
     */
    azone = icaltimezone_get_builtin_timezone("America/Los_Angeles");
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_vanew_dtstart(
		    icaltime_from_timet_with_zone(tm1,0,azone),
		    icalparameter_new_tzid("America/Los_Angeles"),0),
		icalproperty_vanew_dtend(
		    icaltime_from_timet_with_zone(tm2,0,azone),
		    icalparameter_new_tzid("America/Los_Angeles"),0),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /** test 2
     *	We specify times as floating, the returned span is in UTC
     *	with no conversion applied - so result should be as test 0
     */
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
	    icalproperty_vanew_dtstart(icaltime_from_timet(tm1,0),0),
	    icalproperty_vanew_dtend(icaltime_from_timet(tm2,0),0),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /** test 3
     *	We specify times in a timezone, the returned span is in UTC
     */
    azone = icaltimezone_get_builtin_timezone("America/New_York");
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_vanew_dtstart(
		    icaltime_from_timet_with_zone(tm1,0,azone),
		    icalparameter_new_tzid("America/New_York"),0),
		icalproperty_vanew_dtend(
		    icaltime_from_timet_with_zone(tm2,0,azone),
		    icalparameter_new_tzid("America/New_York"),0),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /** test 4
     *	We specify times in two different timezones, the returned span
     *	is in UTC
     */
    azone = icaltimezone_get_builtin_timezone("America/New_York");
    bzone = icaltimezone_get_builtin_timezone("America/Los_Angeles");
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_vanew_dtstart(
		    icaltime_from_timet_with_zone(tm1,0,azone),
		    icalparameter_new_tzid("America/New_York"),0),
		icalproperty_vanew_dtend(
		    icaltime_from_timet_with_zone(tm2,0,bzone),
		    icalparameter_new_tzid("America/Los_Angeles"),0),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /** test 5
     *	We specify start time in a timezone and a duration, the returned span
     *	is in UTC
     */
    azone = icaltimezone_get_builtin_timezone("America/Los_Angeles");
    memset(&dur,0,sizeof(dur));
    dur.minutes = 30;
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_vanew_dtstart(
		    icaltime_from_timet_with_zone(tm1,0,azone),
		    icalparameter_new_tzid("America/Los_Angeles"),0),
	    icalproperty_new_duration(dur),

	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    icalerror_errors_are_fatal = 0;
    /** test 6
     *	We specify only start time, should return a null span with no error
     */
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_new_dtstart(icaltime_from_timet(tm1,0)),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /** test 7
     *	We specify start and end date
     */
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_new_dtstart(icaltime_from_timet(tm1,1)),
		icalproperty_new_dtend(icaltime_from_timet(tm1,1)),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /** test 8
     *	We specify start and end date
     */
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_new_dtstart(icaltime_from_timet(tm1,1)),
		icalproperty_new_dtend(icaltime_from_timet(tm2,1)),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /** test 9
     *	We specify start date
     */
    c = icalcomponent_vanew(
	    ICAL_VEVENT_COMPONENT,
		icalproperty_new_dtstart(icaltime_from_timet(tm1,1)),
	    0
	    );

    span = icalcomponent_get_span(c);
    print_span(tnum++,span);

    icalcomponent_free(c);

    /* assert(icalerrno == ICAL_MALFORMEDDATA_ERROR); */
    icalerror_errors_are_fatal = 1;
}
