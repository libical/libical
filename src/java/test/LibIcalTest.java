package net.cp.jlibical;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class LibIcalTest {

    static {
        System.loadLibrary("ical_jni");
    }

    @Test
    public void testVAgenda() {
        ICalProperty calidProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_RELCALID_PROPERTY);
        ICalProperty ownerProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_OWNER_PROPERTY);

        calidProp.set_relcalid("1212");
        ownerProp.set_owner("Bill Smith");

        VAgenda vAgenda = new VAgenda();
        vAgenda.add_property(calidProp);
        vAgenda.add_property(ownerProp);

        String expected = "BEGIN:VAGENDA\r\nRELCALID:1212\r\nOWNER:Bill Smith\r\nEND:VAGENDA\r\n";
        assertEquals(expected, vAgenda.as_ical_string());
    }

    @Test
    public void testVEvent() {
        ICalProperty summProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_SUMMARY_PROPERTY);
        ICalProperty startProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_DTSTART_PROPERTY);
        ICalProperty endProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_DTEND_PROPERTY);
        ICalProperty locationProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_LOCATION_PROPERTY);
        ICalProperty descProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_DESCRIPTION_PROPERTY);

        ICalTimeType starttime = new ICalTimeType();
        starttime.setYear(2001);
        starttime.setMonth(12);
        starttime.setDay(21);
        starttime.setHour(18);
        starttime.setMinute(0);
        starttime.setSecond(0);
        starttime.setZone("UTC");

        ICalTimeType endtime = new ICalTimeType();
        endtime.setYear(2002);
        endtime.setMonth(1);
        endtime.setDay(1);
        endtime.setHour(8);
        endtime.setMinute(0);
        endtime.setSecond(0);
        endtime.setZone("UTC");

        summProp.set_summary("New Year's Eve Party, and more");
        startProp.set_dtstart(starttime);
        endProp.set_dtend(endtime);
        locationProp.set_location("Bothin, Marin County, CA, USA");
        descProp.set_description("Drive carefully; come to have fun!");

        VEvent vEvent = new VEvent();
        vEvent.add_property(summProp);
        vEvent.add_property(startProp);
        vEvent.add_property(endProp);
        vEvent.add_property(locationProp);
        vEvent.add_property(descProp);

        assertEquals("New Year's Eve Party, and more", vEvent.get_summary());
        assertEquals(2001, vEvent.get_dtstart().getYear());
        assertEquals(2002, vEvent.get_dtend().getYear());
        assertEquals("Bothin, Marin County, CA, USA", vEvent.get_location());
        assertEquals("Drive carefully; come to have fun!", vEvent.get_description());
    }

    @Test
    public void testVToDo() {
        ICalProperty statusProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_STATUS_PROPERTY);
        ICalProperty dueProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_DUE_PROPERTY);

        ICalTimeType duetime = new ICalTimeType();
        duetime.setYear(2002);
        duetime.setMonth(12);
        duetime.setDay(21);
        duetime.setHour(18);
        duetime.setMinute(0);
        duetime.setSecond(0);
        duetime.setZone("UTC");

        statusProp.set_status(ICalProperty.ICalPropertyStatus.ICAL_STATUS_COMPLETED);
        dueProp.set_due(duetime);

        VToDo vtodo = new VToDo();
        vtodo.add_property(statusProp);
        vtodo.add_property(dueProp);

        String result = vtodo.as_ical_string();
        assertTrue(result.contains("BEGIN:VTODO"));
        assertTrue(result.contains("DUE:20021221T180000Z"));
        assertTrue(result.contains("END:VTODO"));
        assertTrue(result.contains("STATUS:COMPLETED"));
    }

    @Test
    public void testVAlarm() {
        VAlarm valarm = new VAlarm();

        ICalDurationType duration = new ICalDurationType();
        duration.setDays(0);
        duration.setHours(0);
        duration.setMinutes(15);
        duration.setIs_neg(1);

        ICalTriggerType trigger = new ICalTriggerType();
        trigger.setDuration(duration);

        ICalProperty triggerProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_TRIGGER_PROPERTY);
        triggerProp.set_trigger(trigger);

        valarm.add_property(triggerProp);

        String expected = "BEGIN:VALARM\r\nTRIGGER:-PT15M\r\nEND:VALARM\r\n";
        assertEquals(expected, valarm.as_ical_string());
    }

    @Test
    public void testPropertyExdate() {
        ICalTimeType testTime = new ICalTimeType();
        testTime.setYear(2001);
        testTime.setMonth(12);
        testTime.setDay(21);
        testTime.setHour(18);
        testTime.setMinute(0);
        testTime.setSecond(0);
        testTime.setZone("UTC");

        ICalProperty exdateProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_EXDATE_PROPERTY);
        exdateProp.set_exdate(testTime);
        
        ICalTimeType returnedTime = exdateProp.get_exdate();
        assertEquals(2001, returnedTime.getYear());
        assertEquals(12, returnedTime.getMonth());
        assertEquals(21, returnedTime.getDay());
        assertEquals(18, returnedTime.getHour());
        assertEquals(0, returnedTime.getMinute());
        assertEquals(0, returnedTime.getSecond());
    }

    @Test
    public void testPropertyExrule() {
        ICalTimeType testTime = new ICalTimeType();
        testTime.setYear(2001);
        testTime.setMonth(12);
        testTime.setDay(21);
        testTime.setHour(18);
        testTime.setMinute(0);
        testTime.setSecond(0);
        testTime.setZone("UTC");

        ICalProperty exruleProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_EXRULE_PROPERTY);
        ICalRecurrenceType exrule = new ICalRecurrenceType();
        
        exrule.setUntil(testTime);
        exrule.setFreq(ICalRecurrenceType.ICalRecurrenceTypeFrequency.ICAL_MINUTELY_RECURRENCE);
        exrule.setWeek_start(ICalRecurrenceType.ICalRecurrenceTypeWeekday.ICAL_SUNDAY_WEEKDAY);
        
        short[] testArray = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61};
        exrule.setBy_second(testArray);
        exrule.setBy_minute(testArray);
        
        exruleProp.set_exrule(exrule);

        ICalRecurrenceType returnedExrule = exruleProp.get_exrule();

        assertEquals(ICalRecurrenceType.ICalRecurrenceTypeFrequency.ICAL_MINUTELY_RECURRENCE, returnedExrule.getFreq());
        assertEquals(ICalRecurrenceType.ICalRecurrenceTypeWeekday.ICAL_SUNDAY_WEEKDAY, returnedExrule.getWeek_start());
        
        assertEquals(31, returnedExrule.getBy_secondIndexed(30));
        assertEquals(29, returnedExrule.getBy_minuteIndexed(28));
    }

    @Test
    public void testPropertyRecurrenceId() {
        ICalTimeType testTime = new ICalTimeType();
        testTime.setYear(2001);
        testTime.setMonth(12);
        testTime.setDay(21);
        testTime.setHour(18);
        testTime.setMinute(0);
        testTime.setSecond(0);
        testTime.setZone("UTC");

        ICalProperty recurrenceIdProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_RECURRENCEID_PROPERTY);
        recurrenceIdProp.set_recurrenceid(testTime);
        
        ICalTimeType returnedTime = recurrenceIdProp.get_recurrenceid();
        assertEquals(2001, returnedTime.getYear());
        assertEquals(12, returnedTime.getMonth());
        assertEquals(21, returnedTime.getDay());
        assertEquals(18, returnedTime.getHour());
        assertEquals(0, returnedTime.getMinute());
        assertEquals(0, returnedTime.getSecond());
    }

    @Test
    public void testPropertyRrule() {
        ICalTimeType testTime = new ICalTimeType();
        testTime.setYear(2001);
        testTime.setMonth(12);
        testTime.setDay(21);
        testTime.setHour(18);
        testTime.setMinute(0);
        testTime.setSecond(0);
        testTime.setZone("UTC");

        ICalProperty rruleProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_RRULE_PROPERTY);
        ICalRecurrenceType rrule = new ICalRecurrenceType();
        
        rrule.setUntil(testTime);
        rrule.setFreq(ICalRecurrenceType.ICalRecurrenceTypeFrequency.ICAL_MINUTELY_RECURRENCE);
        rrule.setWeek_start(ICalRecurrenceType.ICalRecurrenceTypeWeekday.ICAL_SUNDAY_WEEKDAY);
        
        short[] testHourArray = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
        rrule.setBy_hour(testHourArray);
        
        rruleProp.set_rrule(rrule);

        ICalRecurrenceType returnedRrule = rruleProp.get_rrule();

        assertEquals(ICalRecurrenceType.ICalRecurrenceTypeFrequency.ICAL_MINUTELY_RECURRENCE, returnedRrule.getFreq());
        assertEquals(ICalRecurrenceType.ICalRecurrenceTypeWeekday.ICAL_SUNDAY_WEEKDAY, returnedRrule.getWeek_start());
        
        assertEquals(12, returnedRrule.getBy_hourIndexed(11));
    }

    @Test
    public void testPropertyFreebusy() {
        ICalTimeType startTime = new ICalTimeType();
        startTime.setYear(2001);
        startTime.setMonth(12);
        startTime.setDay(21);
        startTime.setHour(18);
        startTime.setMinute(0);
        startTime.setSecond(0);
        startTime.setZone("UTC");

        ICalTimeType endTime = new ICalTimeType();
        endTime.setYear(2002);
        endTime.setMonth(1);
        endTime.setDay(1);
        endTime.setHour(8);
        endTime.setMinute(0);
        endTime.setSecond(0);
        endTime.setZone("UTC");

        ICalProperty freebusyProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_FREEBUSY_PROPERTY);
        ICalPeriodType period = new ICalPeriodType();
        ICalDurationType duration = new ICalDurationType();
        
        duration.setDays(0);
        duration.setHours(10);
        duration.setMinutes(15);

        period.setStart(startTime);
        period.setEnd(endTime);
        period.setDuration(duration);

        freebusyProp.set_freebusy(period);

        ICalPeriodType returnedPeriod = freebusyProp.get_freebusy();
        ICalTimeType returnedStart = returnedPeriod.getStart();
        ICalTimeType returnedEnd = returnedPeriod.getEnd();
        ICalDurationType returnedDuration = returnedPeriod.getDuration();

        assertEquals(2001, returnedStart.getYear());
        assertEquals(12, returnedStart.getMonth());
        assertEquals(21, returnedStart.getDay());

        assertEquals(2002, returnedEnd.getYear());
        assertEquals(1, returnedEnd.getMonth());
        assertEquals(1, returnedEnd.getDay());

        assertEquals(0, returnedDuration.getDays());
        assertEquals(10, returnedDuration.getHours());
        assertEquals(15, returnedDuration.getMinutes());
    }

    @Test
    public void testPropertyDtstamp() {
        ICalTimeType testTime = new ICalTimeType();
        testTime.setYear(2001);
        testTime.setMonth(12);
        testTime.setDay(21);
        testTime.setHour(18);
        testTime.setMinute(0);
        testTime.setSecond(0);
        testTime.setZone("UTC");

        ICalProperty dtstampProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_DTSTAMP_PROPERTY);
        dtstampProp.set_dtstamp(testTime);
        
        ICalTimeType returnedTime = dtstampProp.get_dtstamp();
        assertEquals(2001, returnedTime.getYear());
        assertEquals(12, returnedTime.getMonth());
        assertEquals(21, returnedTime.getDay());
        assertEquals(18, returnedTime.getHour());
        assertEquals(0, returnedTime.getMinute());
        assertEquals(0, returnedTime.getSecond());
    }

    @Test
    public void testPropertyAttendee() {
        ICalProperty attendeeProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_ATTENDEE_PROPERTY);
        String expectedAttendee = "MAILTO:userid@domain";
        attendeeProp.set_attendee(expectedAttendee);
        
        assertEquals(expectedAttendee, attendeeProp.get_attendee());
    }

    @Test
    public void testPropertyComment() {
        ICalProperty commentProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_COMMENT_PROPERTY);
        String expectedComment = "Test comment";
        commentProp.set_comment(expectedComment);
        
        assertEquals(expectedComment, commentProp.get_comment());
    }

    @Test
    public void testPropertyOrganizer() {
        ICalProperty organizerProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_ORGANIZER_PROPERTY);
        String expectedOrganizer = "MAILTO:organizer@domain";
        organizerProp.set_organizer(expectedOrganizer);
        
        assertEquals(expectedOrganizer, organizerProp.get_organizer());
    }

    @Test
    public void testVComponentParsing() {
        String content = "BEGIN:VCALENDAR\nVERSION:2.1\nBEGIN:VEVENT\nUID:abcd12345\nDTSTART:20020307T180000Z\nDTEND:20020307T190000Z\nSUMMARY:Important Meeting\nEND:VEVENT\nEND:VCALENDAR";
        
        VComponent vc = new VComponent(content);
        assertNotNull(vc);
        
        VEvent vEvent = (VEvent) vc.get_first_component(VComponent.ICalComponentKind.ICAL_VEVENT_COMPONENT);
        assertNotNull(vEvent);
        
        assertEquals("Important Meeting", vEvent.get_summary());
        assertEquals("abcd12345", vEvent.get_uid());
    }

    @Test
    public void testVComponentRecurrenceId() {
        String content = "BEGIN:VCALENDAR\nVERSION:2.1\nBEGIN:VEVENT\nUID:abcd12345\nDTSTART:20020307T180000Z\nDTEND:20020307T190000Z\nSUMMARY:Important Meeting\nEND:VEVENT\nEND:VCALENDAR";
        
        VComponent vc = new VComponent(content);
        assertNotNull(vc);
        
        ICalTimeType testTime = new ICalTimeType();
        testTime.setYear(2002);
        testTime.setMonth(2);
        testTime.setDay(2);
        testTime.setHour(2);
        testTime.setMinute(2);
        testTime.setSecond(2);
        testTime.setZone("UTC");

        vc.set_recurrenceid(testTime);
        ICalTimeType returnedTime = vc.get_recurrenceid();
        
        assertEquals(2002, returnedTime.getYear());
        assertEquals(2, returnedTime.getMonth());
        assertEquals(2, returnedTime.getDay());
        assertEquals(2, returnedTime.getHour());
        assertEquals(2, returnedTime.getMinute());
        assertEquals(2, returnedTime.getSecond());
    }

    @Test
    public void testVComponentDtstamp() {
        String content = "BEGIN:VCALENDAR\nVERSION:2.1\nBEGIN:VEVENT\nUID:abcd12345\nDTSTART:20020307T180000Z\nDTEND:20020307T190000Z\nSUMMARY:Important Meeting\nEND:VEVENT\nEND:VCALENDAR";
        
        VComponent vc = new VComponent(content);
        assertNotNull(vc);
        
        ICalTimeType testTime = new ICalTimeType();
        testTime.setYear(2002);
        testTime.setMonth(2);
        testTime.setDay(2);
        testTime.setHour(2);
        testTime.setMinute(2);
        testTime.setSecond(2);
        testTime.setZone("UTC");

        vc.set_dtstamp(testTime);
        ICalTimeType returnedTime = vc.get_dtstamp();
        
        assertEquals(2002, returnedTime.getYear());
        assertEquals(2, returnedTime.getMonth());
        assertEquals(2, returnedTime.getDay());
        assertEquals(2, returnedTime.getHour());
        assertEquals(2, returnedTime.getMinute());
        assertEquals(2, returnedTime.getSecond());
    }

    @Test
    public void testVAlarmWithAttendee() {
        VAlarm valarm = new VAlarm();

        ICalDurationType duration = new ICalDurationType();
        duration.setDays(0);
        duration.setHours(0);
        duration.setMinutes(15);
        duration.setIs_neg(1);

        ICalTriggerType trigger = new ICalTriggerType();
        trigger.setDuration(duration);

        ICalProperty triggerProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_TRIGGER_PROPERTY);
        triggerProp.set_trigger(trigger);
        valarm.add_property(triggerProp);

        String userEmail = "userid@domain";
        ICalProperty attendeeProp = new ICalProperty(ICalProperty.ICalPropertyKind.ICAL_ATTENDEE_PROPERTY);
        attendeeProp.set_attendee("MAILTO:" + userEmail);
        valarm.add_property(attendeeProp);

        assertEquals("MAILTO:" + userEmail, attendeeProp.get_attendee());
        
        String alarmString = valarm.as_ical_string();
        assertTrue(alarmString.contains("TRIGGER:-PT15M"));
        assertTrue(alarmString.contains("ATTENDEE:MAILTO:userid@domain"));
    }

    @Test
    public void testICalParameterRole() {
        ICalParameter roleProp = new ICalParameter(ICalParameter.ICalParameterKind.ICAL_ROLE_PARAMETER);
        roleProp.set_role(ICalParameter.ICalParameterRole.ICAL_ROLE_REQPARTICIPANT);
        
        assertEquals(ICalParameter.ICalParameterRole.ICAL_ROLE_REQPARTICIPANT, roleProp.get_role());
    }

    @Test
    public void testICalParameterPartstat() {
        ICalParameter partstatProp = new ICalParameter(ICalParameter.ICalParameterKind.ICAL_PARTSTAT_PARAMETER);
        partstatProp.set_partstat(ICalParameter.ICalParameterPartStat.ICAL_PARTSTAT_DECLINED);
        
        assertEquals(ICalParameter.ICalParameterPartStat.ICAL_PARTSTAT_DECLINED, partstatProp.get_partstat());
    }
}

