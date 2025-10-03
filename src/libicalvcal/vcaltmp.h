/***************************************************************************
SPDX-FileCopyrightText: 1996 Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.
SPDX-License-Identifier: LicenseRef-APPLEMIT

The software is provided with RESTRICTED RIGHTS.  Use, duplication, or
disclosure by the government are subject to restrictions set forth in
DFARS 252.227-7013 or 48 CFR 52.227-19, as applicable.
***************************************************************************/

#ifndef VCALTMP_H
#define VCALTMP_H

#include "libical_vcal_export.h"
#include "vcc.h"

#if defined(__CPLUSPLUS__) || defined(__cplusplus)
extern "C"
{
#endif

    LIBICAL_VCAL_EXPORT VObject *vcsCreateVCal(const char *date_created,
                                               const char *location,
                                               const char *product_id,
                                               const char *time_zone,
                                               const char *version);

    LIBICAL_VCAL_EXPORT VObject *vcsAddEvent(VObject *vcal,
                                             const char *start_date_time,
                                             const char *end_date_time,
                                             const char *description,
                                             const char *summary,
                                             const char *categories,
                                             const char *classification,
                                             const char *status,
                                             const char *transparency,
                                             const char *uid,
                                             const char *url);

    LIBICAL_VCAL_EXPORT VObject *vcsAddTodo(VObject *vcal,
                                            const char *start_date_time,
                                            const char *due_date_time,
                                            const char *date_time_complete,
                                            const char *description,
                                            const char *summary,
                                            const char *priority,
                                            const char *classification,
                                            const char *status,
                                            const char *uid,
                                            const char *url);

    LIBICAL_VCAL_EXPORT VObject *vcsAddAAlarm(VObject *vevent,
                                              const char *run_time,
                                              const char *snooze_time,
                                              const char *repeat_count,
                                              const char *audio_content);

    LIBICAL_VCAL_EXPORT VObject *vcsAddMAlarm(VObject *vevent,
                                              const char *run_time,
                                              const char *snooze_time,
                                              const char *repeat_count,
                                              const char *email_address,
                                              const char *note);

    LIBICAL_VCAL_EXPORT VObject *vcsAddDAlarm(VObject *vevent,
                                              const char *run_time,
                                              const char *snooze_time,
                                              const char *repeat_count,
                                              const char *display_string);

    LIBICAL_VCAL_EXPORT VObject *vcsAddPAlarm(VObject *vevent,
                                              const char *run_time,
                                              const char *snooze_time,
                                              const char *repeat_count,
                                              const char *procedure_name);

#if defined(__CPLUSPLUS__) || defined(__cplusplus)
}

#endif

#endif /* VCALTMP_H */
