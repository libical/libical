from Libical import *

test_time()

test_period()

test_component()


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


