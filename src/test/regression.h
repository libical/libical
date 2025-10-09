/*======================================================================
 FILE: regression.h
 CREATOR: eric 03 April 1999

 SPDX-FileCopyrightText: 1999 Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/
#ifndef TEST_REGRESSION_H
#define TEST_REGRESSION_H

#include "icaltime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADD_TESTS_REQUIRING_INVESTIGATION 0
#define ADD_TESTS_BROKEN_BUILTIN_TZDATA 0

extern int VERBOSE;
extern int QUIET;

/* regression-component.c */
void create_new_component(void);
void create_new_component_with_va_args(void);
void create_simple_component(void);
void test_icalcomponent_get_span(void);

/* regression-classify.c */
void test_classify(void);

/* regression-recur.c */
void test_recur_file(void);

/* regression-cxx.c */
void test_cxx(void);

/* regression-storage.c */
void test_fileset_extended(void);
void test_dirset_extended(void);
void test_bdbset(void);

/* regression-utils.c */
const char *ical_timet_string(const icaltime_t t);
const char *ictt_as_string(struct icaltimetype t);
char *icaltime_as_ctime(struct icaltimetype t);

void _ok(const char *name, int result, const char *file, int linenum, const char *test);
void _is(const char *test_name, const char *str1, const char *str2, const char *file,
         int linenum);
void _int_is(const char *test_name, int i1, int i2, const char *file, int linenum);
#define ok(TEST, EX) (_ok(TEST, EX, __FILE__, __LINE__, #EX))
#define str_is(S1, S2, EX) (_is(S1, S2, EX, __FILE__, __LINE__))
#define int_is(I1, I2, EX) (_int_is(I1, I2, EX, __FILE__, __LINE__))
void test_header(const char *title, int test_set);
void test_start(int);
int test_end(void);
void test_run(const char *test_name, void (*test_fcn)(void), int do_test, int headeronly);

#ifdef __cplusplus
}

#endif

#endif
