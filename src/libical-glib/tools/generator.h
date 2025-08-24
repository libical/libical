/*
 * SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <ctype.h>
#include <stdio.h>
#include <glib.h>
#include "xml-parser.h"

#define HEADER_TEMPLATE "header-template"
#define HEADER_HEADER_TEMPLATE "header-header-template"
#define HEADER_FORWARD_DECLARATIONS_TEMPLATE "header-forward-declarations-template"
#define INLINE_TRANSLATION "${translator} (${name})"
#define SOURCE_TEMPLATE "source-template"
#define COMMON_HEADER "libical-glib/i-cal-object"
#define HEADER_STRUCTURE_BOILERPLATE_TEMPLATE "header-structure-boilerplate-template"
#define SOURCE_STRUCTURE_BOILERPLATE_TEMPLATE "source-structure-boilerplate-template"
#define ENUM_HEADER "ICAL"
#define BUFFER_SIZE 4000
#define TAB_SIZE 8
#define RET_TAB_COUNT 2
#define METHOD_NAME_TAB_COUNT 4
#define COMMENT_LINE_LENGTH 100
#define PRIVATE_HEADER "libical-glib-private.h"
#define FORWARD_DECLARATIONS_HEADER "i-cal-forward-declarations.h"

/* Store the default value for bare type */
GHashTable *defaultValues;
/* The table for each type and its kind (std, enum) */
GHashTable *type2kind;
/* The table for each type and the file where this type is declared. */
GHashTable *type2structure;

gchar *get_upper_camel_from_lower_snake(const gchar *lowerSnake);
gchar *get_lower_snake_from_upper_camel(const gchar *upperCamel);
gchar *get_upper_snake_from_lower_snake(const gchar *lowerSnake);
gchar *get_lower_train_from_lower_snake(const gchar *lowerSnake);
gchar *get_upper_snake_from_upper_camel(const gchar *upperCamel);
gchar *get_lower_train_from_upper_camel(const gchar *upperCamel);

gchar *get_source_method_code(Method *method);
gchar *get_inline_parameter(Parameter *para);
gchar *get_source_method_comment(Method *method);
gchar *get_source_method_proto(Method *method);
gchar *get_source_method_body(Method *method, const gchar *nameSpace);
gchar *get_source_run_time_checkers(Method *method, const gchar *nameSpace);
gchar *get_translator_name_for_return(const gchar *upperCamel);
gchar *get_translator_for_parameter(Parameter *para);
gchar *get_translator_for_return(Ret *ret);

gchar *get_source_method_proto_new_full(Structure *structure);
gchar *get_source_method_proto_set_owner(Structure *structure);
gchar *get_source_method_proto_set_native(Structure *structure);
gchar *get_source_method_proto_set_is_global(Structure *structure);
gchar *get_source_method_proto_set_property(Structure *structure);
gchar *get_source_method_proto_get_property(Structure *structure);

GHashTable *get_hash_table_from_structure(Structure *structure);
void generate_header_method_protos(FILE *out, Structure *structure);
void generate_header_method_proto(FILE *out, Method *method, gboolean isPrivate);
void generate_header_method_new_full(FILE *out, Structure *structure);
void generate_header_method_get_type(FILE *out, Structure *structure);
void generate_header_enums(FILE *out, Structure *structure);
void generate_header_enum(FILE *out, Enumeration *enumeration);
void generate_header_includes(FILE *out, Structure *structure);
void generate_source_includes(FILE *out, Structure *structure);
void generate_header_forward_declaration(FILE *out, Structure *structure);
void generate_header_structure_boilerplate(FILE *out, Structure *structure, GHashTable *table);
void generate_header(FILE *out, Structure *structure, GHashTable *table);
void generate_source(FILE *out, Structure *structure, GHashTable *table);
void generate_source_structure_boilerplate(FILE *out, Structure *structure, GHashTable *table);
void generate_source_body_declaration(FILE *out, Structure *structure);
void generate_conditional(FILE *out, Structure *structure, gchar *statement, GHashTable *table);
void generate_code_from_template(FILE *in, FILE *out, Structure *structure, GHashTable *table);
void generate_header_and_source(Structure *structure, gchar *dir);
void generate_header_header_declaration(FILE *out, Structure *structure);
void generate_header_header_file(GList *structures);

#endif /* GENERATOR_H */
