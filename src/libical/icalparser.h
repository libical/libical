/*======================================================================
 FILE: icalparser.h
 CREATOR: eric 20 April 1999

 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

  The original code is icalparser.h
======================================================================*/

#ifndef ICALPARSER_H
#define ICALPARSER_H

#include "libical_ical_export.h"
#include "icalcomponent.h"

/**
 * @file  icalparser.h
 * @brief Line-oriented parsing.
 *
 * Create a new parser via icalparse_new_parser(), then add lines one at
 * a time with icalparse_add_line(). icalparser_add_line() will return
 * non-zero when it has finished with a component.
 */

/**
 * @struct icalparser_impl
 * @typedef icalparser
 * @private
 *
 * Implementation of the icalparser struct, which holds the
 * state for the current parsing operation.
 */
typedef struct icalparser_impl icalparser;

/**
 * @enum icalparser_state
 * @typedef icalparser_state
 * @brief Represents the current state of the parser
 */
typedef enum icalparser_state
{
    /** An error occured while parsing. */
    ICALPARSER_ERROR,

    /** Parsing was successful. */
    ICALPARSER_SUCCESS,

    /** Currently parsing the begin of a component */
    ICALPARSER_BEGIN_COMP,

    /** Currently parsing the end of the component */
    ICALPARSER_END_COMP,

    /** Parsing is currently in progress */
    ICALPARSER_IN_PROGRESS
} icalparser_state;

/**
 * @brief Creates a new icalparser. 
 * @return An icalparser object
 *
 * All icalparser objects created with this function need to be
 * freed using the icalparser_free() function.
 *
 * On error, it returns `NULL` and sets an icalerrno.
 */
LIBICAL_ICAL_EXPORT icalparser *icalparser_new(void);

/**
 * @brief Adds a single line to be parsed by the icalparser.
 * @param parser The current parser
 * @param str A single line to be parsed
 * @return When this was the last line of the component to be parsed,
 *  it returns the icalcomponent, otherwise it returns `NULL`.
 *
 * On error, it sets the icalparser_state.
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalparser_add_line(icalparser *parser, char *str);

/**
 * @brief Cleans out an icalparser and returns parsed component
 * @param parser The icalparser to clean
 * @return The parsed component
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalparser_clean(icalparser *parser);

/**
 * @brief Returns current state of the icalparser
 * @param parser The icalparser
 * @return The current state of the icalparser
 */
LIBICAL_ICAL_EXPORT icalparser_state icalparser_get_state(icalparser *parser);

/**
 * @brief Frees an icalparser object.
 * @param parser The icalparser to be freed.
 */
LIBICAL_ICAL_EXPORT void icalparser_free(icalparser *parser);

/**
 * @brief Message oriented parsing. 
 * @param parser The parser to use
 * @param line_gen_func A function that returns one content line per invocation
 * @return The parsed icalcomponent
 *
 * Reads an icalcomponent using the supplied line_gen_func, returning the parsed
 * component (or NULL on error).
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalparser_parse(icalparser *parser,
                                                    char *(*line_gen_func) (char *s,
                                                                            size_t size, void *d));

/**
 * @brief Sets the data that icalparser_parse will give to the line_gen_func
 * as the parameter 'd'.
 * @param parser The icalparser this applies to
 * @param data The pointer which will be passed to the line_gen_func as argument `d`
 *
 * If you use any of the icalparser_parser() or icalparser_get_line() functions,
 * the `line_gen_func` that they expect has a third `void* d` argument. This function
 * sets what will be passed to your line_gen_function as such argument.
 */
LIBICAL_ICAL_EXPORT void icalparser_set_gen_data(icalparser *parser, void *data);

/**
 * @brief Parse a string and return the parsed icalcomponent.
 * @param str The iCalendar to be parsed
 * @return An icalcomponent representing the iCalendar
 * 
 * On error, returns NULL and sets icalerrno
 */
LIBICAL_ICAL_EXPORT icalcomponent *icalparser_parse_string(const char *str);

/***********************************************************************
 * Parser support functions
 ***********************************************************************/

/** 
 * @brief Given a line generator function, return a single iCal content line.
 * @return Aa pointer to a single line of data or NULL if it reached
 *  end of file reading from the `line_gen_func`. Note that the pointer
 *  returned is owned by libical and must not be freed by the user.
 * @param parser The parser object to use
 * @param line_gen_func The function to use for reading data
 *
 * This function uses the supplied `line_gen_func` to read data in, 
 * until it has read a full line, and returns the full line.
 * To supply arbitrary data (as the parameter `d`) to your `line_gen_func`, 
 * call icalparser_set_gen_data().
 */
LIBICAL_ICAL_EXPORT char *icalparser_get_line(icalparser *parser,
                                              char *(*line_gen_func) (char *s,
                                                                      size_t size, void *d));

LIBICAL_ICAL_EXPORT char *icalparser_string_line_generator(char *out, size_t buf_size, void *d);

#endif /* !ICALPARSE_H */
