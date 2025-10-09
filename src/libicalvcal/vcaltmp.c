/*
This module provides some helper APIs for creating
a VCalendar object.

Note on APIs:
        1.      The APIs does not attempt to verify if the arguments
                passed are correct.
        2.      Where the argument to an API is not applicable, pass
                the value 0.
        3.      See the test program at the bottom of this file as an
                example of usage.
        4.      This code calls APIs in vobject.c.

*/

/***************************************************************************
SPDX-FileCopyrightText: 1996 Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.
SPDX-License-Identifier: LicenseRef-APPLEMIT

The software is provided with RESTRICTED RIGHTS.  Use, duplication, or
disclosure by the government are subject to restrictions set forth in
DFARS 252.227-7013 or 48 CFR 52.227-19, as applicable.

***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcaltmp.h"

VObject* vcsCreateVCal(
        const char *date_created,
        const char *location,
        const char *product_id,
        const char *time_zone,
        const char *version
        )
    {
    VObject *vcal = newVObject(VCCalProp);
#define Z(p,v) if (v) (void)addPropValue(vcal,p,v);
    Z(VCDCreatedProp, date_created);
    Z(VCLocationProp, location)
    Z(VCProdIdProp, product_id)
    Z(VCTimeZoneProp, time_zone)
    Z(VCVersionProp, version)
#undef Z
    return vcal;
    }

VObject* vcsAddEvent(
        VObject *vcal,
        const char *start_date_time,
        const char *end_date_time,
        const char *description,
        const char *summary,
        const char *categories,
        const char *classification,
        const char *status,
        const char *transparency,
        const char *uid,
        const char *url
        )
    {
    VObject *vevent = addProp(vcal,VCEventProp);
#define Z(p,v) if (v) (void)addPropValue(vevent,p,v);
    Z(VCDTstartProp,start_date_time);
    Z(VCDTendProp,end_date_time);
    if (description) {
        VObject *p = addPropValue(vevent,VCDescriptionProp,description);
        if (strchr(description,'\n'))
            (void)addProp(p,VCQuotedPrintableProp);
        }
    Z(VCSummaryProp,summary);
    Z(VCCategoriesProp,categories);
    Z(VCClassProp,classification);
    Z(VCStatusProp,status);
    Z(VCTranspProp,transparency);
    Z(VCUniqueStringProp,uid);
    Z(VCURLProp,url);
#undef Z
    return vevent;
    }

VObject* vcsAddTodo(
        VObject *vcal,
        const char *start_date_time,
        const char *due_date_time,
        const char *date_time_complete,
        const char *description,
        const char *summary,
        const char *priority,
        const char *classification,
        const char *status,
        const char *uid,
        const char *url
        )
    {
    VObject *vtodo = addProp(vcal,VCTodoProp);
#define Z(p,v) if (v) (void)addPropValue(vtodo,p,v);
    Z(VCDTstartProp,start_date_time);
    Z(VCDueProp,due_date_time);
    Z(VCCompletedProp,date_time_complete);
    if (description) {
        VObject *p = addPropValue(vtodo,VCDescriptionProp,description);
        if (strchr(description,'\n'))
            (void)addProp(p,VCQuotedPrintableProp);
        }
    Z(VCSummaryProp,summary);
    Z(VCPriorityProp,priority);
    Z(VCClassProp,classification);
    Z(VCStatusProp,status);
    Z(VCUniqueStringProp,uid);
    Z(VCURLProp,url);
#undef Z
    return vtodo;
    }

VObject* vcsAddAAlarm(
        VObject *vevent,
        const char *run_time,
        const char *snooze_time,
        const char *repeat_count,
        const char *audio_content
        )
    {
    VObject *aalarm= addProp(vevent,VCAAlarmProp);
#define Z(p,v) if (v) (void)addPropValue(aalarm,p,v);
    Z(VCRunTimeProp,run_time);
    Z(VCSnoozeTimeProp,snooze_time);
    Z(VCRepeatCountProp,repeat_count);
    Z(VCAudioContentProp,audio_content);
#undef Z
    return aalarm;
    }

VObject* vcsAddMAlarm(
        VObject *vevent,
        const char *run_time,
        const char *snooze_time,
        const char *repeat_count,
        const char *email_address,
        const char *note
        )
    {
    VObject *malarm= addProp(vevent,VCMAlarmProp);
#define Z(p,v) if (v) (void)addPropValue(malarm,p,v);
    Z(VCRunTimeProp,run_time);
    Z(VCSnoozeTimeProp,snooze_time);
    Z(VCRepeatCountProp,repeat_count);
    Z(VCEmailAddressProp,email_address);
    Z(VCNoteProp,note);
#undef Z
    return malarm;
    }

VObject* vcsAddDAlarm(
        VObject *vevent,
        const char *run_time,
        const char *snooze_time,
        const char *repeat_count,
        const char *display_string
        )
    {
    VObject *dalarm= addProp(vevent,VCDAlarmProp);
#define Z(p,v) if (v) (void)addPropValue(dalarm,p,v);
    Z(VCRunTimeProp,run_time);
    Z(VCSnoozeTimeProp,snooze_time);
    Z(VCRepeatCountProp,repeat_count);
    Z(VCDisplayStringProp,display_string);
#undef Z
    return dalarm;
    }

VObject* vcsAddPAlarm(
        VObject *vevent,
        const char *run_time,
        const char *snooze_time,
        const char *repeat_count,
        const char *procedure_name
        )
    {
    VObject *palarm= addProp(vevent,VCPAlarmProp);
#define Z(p,v) if (v) (void)addPropValue(palarm,p,v);
    Z(VCRunTimeProp,run_time);
    Z(VCSnoozeTimeProp,snooze_time);
    Z(VCRepeatCountProp,repeat_count);
    Z(VCProcedureNameProp,procedure_name);
#undef Z
    return palarm;
    }

#ifdef _TEST

/*
This testcase would generate a file call "frankcal.vcf" with
the following content:

BEGIN:VCALENDAR
DCREATED:19960523T100522
GEO:37.24,-17.87
PRODID:-//Frank Dawson/Hand Crafted In North Carolina//NONSGML Made By Hand//EN
VERSION:0.3
BEGIN:VEVENT
DTSTART:19960523T120000
DTEND:19960523T130000
DESCRIPTION;QUOTED-PRINTABLE:VERSIT PDI PR Teleconference/Interview =0A=
With Tom Streeter and Frank Dawson - Discuss VERSIT PDI project and vCard and vCalendar=0A=
activities with European Press representatives.
SUMMARY:VERSIT PDI PR Teleconference/Interview
CATEGORIES:PHONE CALL
STATUS:CONFIRMED
TRANSP:19960523T100522-4000F100582713-009251
UID:https://www.ibm.com/raleigh/fdawson/~c:\or2\orgfiles\versit.or2
DALARM:19960523T114500;5;3;Your Telecon Starts At Noon!!!;
MALARM:19960522T120000;;;fdawson@raleigh.ibm.com;Remember 05/23 Noon Telecon!!!;
PALARM:19960523T115500;;;c:\or2\organize.exe c:\or2\orgfiles\versit.or2;
X-LDC-OR2-OLE:c:\temp\agenda.doc
END:VEVENT

BEGIN:VTODO
DUE:19960614T0173000
DESCRIPTION:Review VCalendar helper API.
END:VTODO

END:VCALENDAR
*/

void testVcalAPIs() {
    FILE *fp;
    VObject *vcal = vcsCreateVCal(
        "19960523T100522",
        "37.24,-17.87",
        "-//Frank Dawson/Hand Crafted In North Carolina//NONSGML Made By Hand//EN",
        0,
        "0.3"
        );

    VObject *vevent = vcsAddEvent(
        vcal,
        "19960523T120000",
        "19960523T130000",
        "VERSIT PDI PR Teleconference/Interview \nWith Tom Streeter and Frank Dawson - Discuss VERSIT PDI project and vCard and vCalendar\nactivities with European Press representatives.",
        "VERSIT PDI PR Teleconference/Interview",
        "PHONE CALL",
        0,
        "CONFIRMED",
        "19960523T100522-4000F100582713-009251",
        "https://www.ibm.com/raleigh/fdawson/~c:\\or2\\orgfiles\\versit.or2",
        0
        );

    vcsAddDAlarm(vevent, "19960523T114500", "5", "3",
            "Your Telecon Starts At Noon!!!");
    vcsAddMAlarm(vevent, "19960522T120000", 0, 0, "fdawson@raleigh.ibm.com",
            "Remember 05/23 Noon Telecon!!!");
    vcsAddPAlarm(vevent, "19960523T115500", 0 ,0,
            "c:\\or2\\organize.exe c:\\or2\\orgfiles\\versit.or2");

    addPropValue(vevent, "X-LDC-OR2-OLE", "c:\\temp\\agenda.doc");

    vcsAddTodo(
        vcal,
        0,
        "19960614T0173000",
        0,
        "Review VCalendar helper API.",
        0,
        0,
        0,
        0,
        0,
        0
        );

    /* now do something to the resulting VObject */
    /* pretty print on stdout for fun */
    printVObject(vcal);
    /* open the output text file */

#define OUTFILE "frankcal.vcf"

    fp = fopen(OUTFILE, "w");
    if (fp) {
        /* write it in text form */
        writeVObject(fp,vcal);
        fclose(fp);
        }
    else {
        printf("open output file '%s' failed\n", OUTFILE);
        }
    }

void main() {
    testVcalAPIs();
    }

#endif

/* end of source file vcaltmp.c */
