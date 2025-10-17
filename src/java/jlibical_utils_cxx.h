/*======================================================================
 FILE: jlibical_utils_cxx.h
 CREATOR: Srinivasa Boppana/George Norman

 SPDX-FileCopyrightText: 2002, Critical Path

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/

#ifndef TEST_CXX_H
#define TEST_CXX_H

#ifndef JNI_H
#include <jni.h>
#endif

namespace LibICal
{
class VComponent;
class ICalProperty;
class ICalValue;
class ICalParameter;
}

struct icaltimetype;
struct icaltriggertype;
struct icaldurationtype;
struct icalrecurrencetype;
struct icalperiodtype;

// get & set
void *getCObjectPtr(JNIEnv *env, jobject surrogate);
void setCObjectPtr(JNIEnv *env, jobject surrogate, void *subject);

// type-safe getters
LibICal::VComponent *getSubjectAsVComponent(JNIEnv *env, jobject surrogateComponent, int exceptionType);
LibICal::ICalProperty *getSubjectAsICalProperty(JNIEnv *env, jobject surrogateProperty, int exceptionType);
LibICal::ICalValue *getSubjectAsICalValue(JNIEnv *env, jobject surrogateValue, int exceptionType);
LibICal::ICalParameter *getSubjectAsICalParameter(JNIEnv *env, jobject surrogateParameter, int exceptionType);

bool copyObjToicaltimetype(JNIEnv *env, jobject src, icaltimetype *dest);
bool copyObjToicaltriggertype(JNIEnv *env, jobject src, icaltriggertype *dest);
bool copyObjToicaldurationtype(JNIEnv *env, jobject src, icaldurationtype *dest);
bool copyObjToicalrecurrencetype(JNIEnv *env, jobject src, icalrecurrencetype *dest);
bool copyObjToicalperiodtype(JNIEnv *env, jobject src, icalperiodtype *dest);

// exception handling
void throwException(JNIEnv *env, int cpErr);

// create objects
jobject createNewVComponentSurrogate(JNIEnv *env, LibICal::VComponent *subject);
jobject createNewICalPropertySurrogate(JNIEnv *env, LibICal::ICalProperty *subject);
jobject createNewICalValueSurrogate(JNIEnv *env, LibICal::ICalValue *subject);
jobject createNewICalParameterSurrogate(JNIEnv *env, LibICal::ICalParameter *subject);

jobject createNewICalTimeType(JNIEnv *env, icaltimetype *source);
jobject createNewICalTriggerType(JNIEnv *env, icaltriggertype *source);
jobject createNewICalDurationType(JNIEnv *env, icaldurationtype *source);
jobject createNewICalRecurrenceType(JNIEnv *env, icalrecurrencetype *source);
jobject createNewICalPeriodType(JNIEnv *env, icalperiodtype *source);

jobject doCreateNewSurrogate(JNIEnv *env, jclass surrogateClass, jlong subject);

#endif /*TEST_CXX_H*/
