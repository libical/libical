from Libical import *

comp_str = """
BEGIN:VEVENT
ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP:MAILTO:employee-A@host.com
COMMENT: When in the course of writting comments and nonsense text\, it 
 becomes necessary to insert a newline
DTSTART:19972512T120000
DTSTART:19970101T120000Z
DTSTART:19970101
DURATION:P3DT4H25M
FREEBUSY:19970101T120000/19970101T120000
FREEBUSY:19970101T120000/PT3H
FREEBUSY:19970101T120000/PT3H
END:VEVENT"""




c = Component(comp_str);

props = c.properties()

#print props
for p in props:

    print p.name(), p.value()

    print p.asIcalString()

dtstart = c.properties('DTSTART')[0]

print dtstart

print dtstart.day(), dtstart.month(), dtstart.year(), dtstart.value()

attendee = c.properties('ATTENDEE')[0]

print attendee

test_time()

file = open('littlefile.txt')
attachProp = Attach(file)
file.close()
attachProp.fmttype('text/ascii')
print "\n" + attachProp.name()
print attachProp.value_type()
print attachProp.fmttype()
attachProp['fmttype']=None
print "Calling value()"
print attachProp.value()
print "Calling asIcalString()"
print attachProp.asIcalString()



