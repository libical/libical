lib_LTLIBRARIES = libical_jni.la

INCLUDES = -I../libical -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(JAVA_PLATFORM)

libical_jni_la_LDFLAGS = -no-defined -version-info 0:0:0

libical_jni_la_LIBADD = 			\
	../libical/libical_cxx.la

libical_jni_la_SOURCES =			\
	jlibical_consts_cxx.h			\
	jlibical_utils_cxx.cpp			\
	jlibical_utils_cxx.h			\
	jniICalDurationType_cxx.cpp		\
	jniICalDurationType_cxx.h		\
	jniICalPeriodType_cxx.cpp		\
	jniICalPeriodType_cxx.h			\
	jniICalRecurrenceType_cxx.cpp		\
	jniICalRecurrenceType_cxx.h		\
	jniICalTimeType_cxx.cpp			\
	jniICalTimeType_cxx.h			\
	jniICalTriggerType_cxx.cpp		\
	jniICalTriggerType_cxx.h		\
	net_cp_jlibical_ICalParameter_cxx.cpp	\
	net_cp_jlibical_ICalParameter_cxx.h	\
	net_cp_jlibical_ICalProperty_cxx.cpp	\
	net_cp_jlibical_ICalProperty_cxx.h	\
	net_cp_jlibical_ICalValue_cxx.cpp	\
	net_cp_jlibical_ICalValue_cxx.h		\
	net_cp_jlibical_VComponent_cxx.cpp	\
	net_cp_jlibical_VComponent_cxx.h

java_JAVA=\
ICalDurationType.java    ICalTriggerType.java     VEvent.java\
ICalParameter.java       ICalValue.java           VFreeBusy.java\
ICalPeriodType.java      VAgenda.java             VQuery.java\
ICalProperty.java        VAlarm.java              VToDo.java\
ICalRecurrenceType.java  VCalendar.java           \
ICalTimeType.java        VComponent.java

data_DATA = libical.jar

libical.jar: classjava.stamp
	(cd $(top_builddir); $(JAR) cf src/java/libical.jar net)

jlibical_jniincludedir = $(includedir)

TESTS = testjni.sh

testjni.sh:
	echo "$(JAVA) -Djava.library.path=@prefix@/lib -classpath $(JAVA_HOME)/lib/classes.zip:@prefix@/lib/libical.jar net.cp.jlibical.testjni" > testjni.sh && chmod a+x testjni.sh
