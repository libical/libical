/*
 * SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 */

#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <libxml/xmlreader.h>
#include <stdio.h>
#include <glib.h>

typedef struct Parameter {
    gchar *type;
    GList *annotations;
    gchar *comment;
    gchar *name;
    gchar *autofill;
    gchar *translator;
    GList *translatorArgus;
    gchar *native_op;
    gchar *owner_op;
} Parameter;

typedef struct Ret {
    gchar *type;
    GList *annotations;
    gchar *comment;
    gchar *translator;
    GList *translatorArgus;
    gchar *errorReturnValue;
} Ret;

typedef struct Method {
    gchar *name;
    gchar *corresponds;
    gchar *kind;
    gchar *since;
    GList *parameters;
    Ret *ret;
    gchar *comment;
    gchar *custom;
    GList *annotations;
} Method;

typedef struct Structure {
    gchar *nameSpace;
    gchar *name;
    gchar *native;
    GList *includes;
    GList *methods;
    gboolean isBare;
    gboolean isPossibleGlobal;
    gchar *new_full_extraCode;
    GList *enumerations;
    GHashTable *dependencies;
    gchar *destroyFunc;
    gchar *cloneFunc;
    gchar *defaultNative;
    GList *declarations;
    GPtrArray *skips; /* gchar * */
} Structure;

typedef struct Declaration {
    gchar *position;
    gchar *content;
} Declaration;

typedef struct EnumerationItem {
    gchar *nativeName;
    gchar *alias;
} EnumerationItem;

typedef struct Enumeration {
    gchar *name;
    gchar *nativeName;
    GPtrArray *items; /* EnumerationItem * */
    gchar *defaultNative;
    gchar *comment;
} Enumeration;

typedef struct TemplateData {
    gchar *name;
    gchar **requires_attrs;     /* NULL-terminated array of required attributes */
    gchar **requires_variables; /* attr names in the same order as in requires_attrs, only in a form "${name}" */
    gchar **optional_attrs;     /* NULL-terminated array of optional attributes */
    gchar **optional_variables; /* attr names in the same order as in optional_attrs, only in a form "${name}" */
    GPtrArray *methods;         /* Method * */
} TemplateData;

Structure *structure_new(void);
void structure_free(Structure *structure);
Method *method_new(void);
void method_free(Method *method);
Parameter *parameter_new(void);
void parameter_free(Parameter *para);
Ret *ret_new(void);
void ret_free(Ret *ret);
EnumerationItem *enumeration_item_new(const gchar *nativeName, const gchar *alias);
void enumeration_item_free(EnumerationItem *item);
Enumeration *enumeration_new(void);
void enumeration_free(Enumeration *enumeration);
Declaration *declaration_new(void);
void declaration_free(Declaration *declaration);
TemplateData *template_data_new(const char *name, const char *requires_attrs, const char *optional_attrs); /* comma-separated list of required/optional attributes */
void template_data_free(TemplateData *data);

GList *get_list_from_string(const gchar *str);
gchar *get_true_type(const gchar *type);
void populate_dependencies(Structure *structure);
gboolean parse_parameters(xmlNode *node, Method *method);
gboolean parse_return(xmlNode *node, Method *method);
gboolean parse_comment(xmlNode *node, Method *method);
gboolean parse_method(xmlNode *node, Method *method);
gboolean parse_structure(xmlNode *node, Structure *structure, GHashTable *api_templates); /* gchar *name ~> TemplateData * */
gboolean parse_enumeration(xmlNode *node, Enumeration *enumeration);
gboolean parse_custom(xmlNode *node, Method *method);
gboolean parse_declaration(xmlNode *node, Declaration *declaration);

#endif /* XML_PARSER_H */
