/* -*- Mode: C -*- */
/*======================================================================
  FILE: icalcstp.h
  CREATOR: eric 20 April 1999
  
  $Id: icalcstp.h,v 1.2 2001-01-23 07:03:17 ebusboom Exp $


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


/********************** Server (Reciever) Interfaces *************************/

/* On the server side, the caller will recieve data from the incoming
   socket and pass it to icalcstps_next_input. The caller then takes
   the return from icalcstps_next_outpu and sends it out through the
   socket. This gives the caller a point of control. If the cstp code
   connected to the socket itself, it would be hard for the caller to
   do anything else after the cstp code was started.

   All of the server abd client command routines will generate
   response codes. On the server side, these responses will be turned
   into text and sent to the client. On the client side, the reponse
   is the one sent from the server.

   Since each command can return multiple responses, the responses are
   stored in the icalcstps object and are accesses by
   icalcstps_first_response() and icalcstps_next_response()

   How to use: 

   1) Construct a new icalcstps, bound to your code via stubs
   2) Repeat forever:
     2a) Get string from client & give to icalcstps_next_input()
     2b) Repeat until icalcstp_next_output returns 0:
       2b1) Call icalcstps_next_output. 
       2b2) Send string to client.
*/



typedef void icalcstps;

/* Er, they aren't really stubs, but pointers to the rountines that
   icalcstps_process_incoming will call when it recognizes a CSTP
   command in the data. BTW, the CONTINUE command is named 'cont'
   because 'continue' is a C keyword */
struct icalcstps_stubs {
  icalerrorenum (*abort)(icalcstps* cstp);
  icalerrorenum (*authenticate)(icalcstps* cstp, char* mechanism, 
                                    char* data);
  icalerrorenum (*calidexpand)(icalcstps* cstp, char* calid);
  icalerrorenum (*capability)(icalcstps* cstp);
  icalerrorenum (*cont)(icalcstps* cstp, unsigned int time);
  icalerrorenum (*identify)(icalcstps* cstp, char* id);
  icalerrorenum (*disconnect)(icalcstps* cstp);
  icalerrorenum (*sendata)(icalcstps* cstp, unsigned int time, 
                               icalcomponent *comp);
  icalerrorenum (*starttls)(icalcstps* cstp, char* command, 
                                char* data);
  icalerrorenum (*upnexpand)(icalcstps* cstp, char* upn);
  icalerrorenum (*unknown)(icalcstps* cstp, char* command, char* data);
};


icalcstps* icalcstps_new(struct icalcstps_stubs stubs);

void icalcstps_free(icalcstps* cstp);

int icalcstps_set_timeout(icalcstps* cstp, int sec);

/* Get the next string to send to the client */
char* icalcstps_next_output(icalcstps* cstp);

/* process the next string from the client */ 
int icalcstps_next_input(icalcstps* cstp);
   

/********************** Client (Sender) Interfaces **************************/

/* How to use: 

   1) Construct a new icalcstpc
   2) Issue a command by calling one of the command routines. 
   3) Repeat until both call icalcstpc_next_output and
   icalcstpc_next_input return 0:
     3a) Call icalcstpc_next_output. Send string to server. 
     3b) Get string from server, & give to icalcstp_next_input()
   4) Iterate with icalcstpc_first_response & icalcstp_next_response to 
   get the servers responses
   5) Repeat at #2
*/

typedef void* icalcstpc;

/* Response code sent by the server. */
typedef struct icalcstpc_response {	
	icalrequeststatus code;
	char *arg; /* These strings are owned by libical */
	char *debug_text;
	char *more_text;
	void* result;
} icalcstpc_response;

icalcstps* icalcstpc_new();

void* icalcstpc_free(icalcstpc* cstpc);

int icalcstpc_set_timeout(icalcstpc* cstp, int sec);


/* Get the next string to send to the server */
char* icalcstpc_next_output(icalcstpc* cstp);

/* process the next string from the server */ 
int icalcstpc_next_input(icalcstpc* cstp);

/* After icalcstpc_next_input returns a 0, there are responses
   ready. use these to get them */
icalcstpc_response icalcstpc_first_response(icalcstpc* cstp);
icalcstpc_response icalcstpc_next_response(icalcstpc* cstp);

/* Issue a command */
icalerrorenum icalcstpc_abort(icalcstpc* cstp);
icalerrorenum icalcstpc_authenticate(icalcstpc* cstp, char* mechanism, 
                                        char* init_data, char* f(char*) );
icalerrorenum icalcstpc_capability(icalcstpc* cstp);
icalerrorenum icalcstpc_calidexpand(icalcstpc* cstp,char* calid);
icalerrorenum icalcstpc_continue(icalcstpc* cstp, unsigned int time);
icalerrorenum icalcstpc_disconnect(icalcstpc* cstp);
icalerrorenum icalcstpc_identify(icalcstpc* cstp, char* id);
icalerrorenum icalcstpc_starttls(icalcstpc* cstp, char* command, 
                                    char* init_data, char* f(char*));
icalerrorenum icalcstpc_senddata(icalcstpc* cstp, unsigned int time,
				icalcomponent *comp);
icalerrorenum icalcstpc_upnexpand(icalcstpc* cstp,char* calid);
icalerrorenum icalcstpc_sendata(icalcstpc* cstp, unsigned int time,
                                   icalcomponent *comp);


#endif /* !ICALCSTP_H */



