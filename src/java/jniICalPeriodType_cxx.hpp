/*======================================================================
 FILE: jniICalPeriodType_cxx.hpp
 CREATOR: structConverter

 SPDX-FileCopyrightText: 2002, Critical Path

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/
#ifndef _jni_ICalPeriodType_H
#define _jni_ICalPeriodType_H
#include <jni.h>

// I forgot how to do this using a typedef in c++!!!!
#define ICalPeriodType icalperiodtype

#ifdef __cplusplus
extern "C" {
#endif

#include "ical.h"

void jni_SetStart_in_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);
void jni_GetStart_from_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);
void jni_SetEnd_in_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);
void jni_GetEnd_from_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);
void jni_SetDuration_in_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);
void jni_GetDuration_from_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);

void jni_SetAll_in_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);
void jni_GetAll_from_ICalPeriodType(struct ICalPeriodType *__ICalPeriodType_, JNIEnv *env, jobject thisICalPeriodType);

JNIEXPORT void JNICALL Java_net_cp_jlibical_ICalPeriodType_init__J(JNIEnv *env, jobject thisICalPeriodType, jlong data);
JNIEXPORT void JNICALL Java_net_cp_jlibical_ICalPeriodType_initFIDs(JNIEnv *env, jclass clazz);

#ifdef __cplusplus
}
#endif

#endif
