/* -*- Mode: C -*- */
/*======================================================================
  FILE: icalcstp.h
  CREATOR: eric 20 April 1999
  
  $Id: icalcstp.h,v 1.3 2001-02-22 05:04:20 ebusboom Exp $


 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

  The original code is icalcstp.h

======================================================================*/


#ifndef ICALCSTP_H
#define ICALCSTP_H

#include "ical.h"


/* Connection state, from the state machine in RFC2445 */
enum cstps_state {
    NO_STATE,
    CONNECTED,
    AUTHENTICATED,
    IDENTIFIED,
    DISCONNECTED,
    RECEIVE
};

/* CSTP Commands that a client can issue to a server */
typedef enum icalcstp_command {
    ICAL_ABORT_COMMAND,
    ICAL_AUTHENTICATE_COMMAND,
    ICAL_CAPABILITY_COMMAND,
    ICAL_CONTINUE_COMMAND,
    ICAL_CALIDEXPAND_COMMAND,
    ICAL_IDENTIFY_COMMAND,
    ICAL_DISCONNECT_COMMAND,
    ICAL_SENDDATA_COMMAND,
    ICAL_STARTTLS_COMMAND,
    ICAL_UPNEXPAND_COMMAND,
    ICAL_COMPLETE_COMMAND,
    ICAL_UNKNOWN_COMMAND
} icalcstp_command;


char* icalcstp_command_to_string(icalcstp_command command);
icalcstp_command icalcstp_string_to_command(const char* str);

#endif /* !ICALCSTP_H */



