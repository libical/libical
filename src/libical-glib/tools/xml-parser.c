/*
 * SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#include "xml-parser.h"

Structure *structure_new(void)
{
    Structure *structure;

    structure = g_new0(Structure, 1);
    structure->nameSpace = NULL;
    structure->name = NULL;
    structure->native = NULL;
    structure->includes = NULL;
    structure->methods = NULL;
    structure->isBare = FALSE;
    structure->isPossibleGlobal = FALSE;
    structure->new_full_extraCode = NULL;
    structure->enumerations = NULL;
    structure->destroyFunc = NULL;
    structure->cloneFunc = NULL;
    structure->defaultNative = NULL;
    structure->dependencies = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    structure->declarations = NULL;
    structure->skips = NULL;
    return structure;
}

void structure_free(Structure *structure)
{
    GList *list;

    if (structure == NULL) {
        return;
    }

    for (list = g_list_first(structure->methods); list != NULL; list = list->next) {
        method_free((Method *)list->data);
    }
    for (list = g_list_first(structure->includes); list != NULL; list = list->next) {
        g_free(list->data);
    }
    for (list = g_list_first(structure->enumerations); list != NULL; list = g_list_next(list)) {
        Enumeration *enumeration = (Enumeration *)list->data;
        enumeration_free(enumeration);
        enumeration = NULL;
    }
    for (list = g_list_first(structure->declarations); list != NULL; list = g_list_next(list)) {
        declaration_free((Declaration *)list->data);
    }
    g_list_free(structure->methods);
    g_list_free(structure->includes);
    g_list_free(structure->enumerations);
    g_list_free(structure->declarations);
    g_free(structure->nameSpace);
    g_free(structure->name);
    g_free(structure->native);
    g_free(structure->destroyFunc);
    g_free(structure->cloneFunc);
    g_free(structure->defaultNative);
    g_free(structure->new_full_extraCode);
    g_hash_table_destroy(structure->dependencies);
    g_clear_pointer(&structure->skips, g_ptr_array_unref);
    g_free(structure);
}

Declaration *declaration_new(void)
{
    Declaration *declaration = g_new0(Declaration, 1);

    declaration->position = NULL;
    declaration->content = NULL;

    return declaration;
}

void declaration_free(Declaration *declaration)
{
    if (declaration == NULL) {
        return;
    }

    if (declaration->position != NULL) {
        g_free(declaration->position);
    }

    if (declaration->content != NULL) {
        g_free(declaration->content);
    }

    g_free(declaration);
}

Method *method_new(void)
{
    Method *method;

    method = g_new0(Method, 1);

    method->name = NULL;
    method->corresponds = NULL;
    method->kind = NULL;
    method->since = NULL;
    method->parameters = NULL;
    method->ret = NULL;
    method->comment = NULL;
    method->custom = NULL;
    method->annotations = NULL;
    return method;
}

void method_free(Method *method)
{
    GList *list;

    if (method == NULL) {
        return;
    }

    for (list = method->parameters; list != NULL; list = list->next) {
        parameter_free((Parameter *)list->data);
    }
    for (list = method->annotations; list != NULL; list = list->next) {
        g_free(list->data);
    }
    g_list_free(method->parameters);
    g_list_free(method->annotations);
    g_free(method->name);
    g_free(method->corresponds);
    g_free(method->kind);
    g_free(method->since);
    g_free(method->comment);
    g_free(method->custom);
    ret_free(method->ret);
    g_free(method);
}

Parameter *parameter_new(void)
{
    Parameter *parameter;

    parameter = g_new0(Parameter, 1);
    parameter->comment = NULL;
    parameter->name = NULL;
    parameter->type = NULL;
    parameter->annotations = NULL;
    parameter->autofill = NULL;
    parameter->translator = NULL;
    parameter->translatorArgus = NULL;
    parameter->native_op = NULL;
    parameter->owner_op = NULL;
    return parameter;
}

void parameter_free(Parameter *para)
{
    GList *list;

    if (para == NULL) {
        return;
    }

    for (list = para->annotations; list != NULL; list = list->next) {
        g_free(list->data);
    }

    for (list = g_list_first(para->translatorArgus); list != NULL; list = g_list_next(list)) {
        g_free(list->data);
    }
    g_list_free(para->annotations);
    g_list_free(para->translatorArgus);
    g_free(para->type);
    g_free(para->comment);
    g_free(para->name);
    g_free(para->autofill);
    g_free(para->translator);
    g_free(para->native_op);
    g_free(para->owner_op);
    g_free(para);
}

Ret *ret_new(void)
{
    Ret *ret;

    ret = g_new0(Ret, 1);
    ret->comment = NULL;
    ret->type = NULL;
    ret->annotations = NULL;
    ret->translator = NULL;
    ret->translatorArgus = NULL;
    ret->errorReturnValue = NULL;
    return ret;
}

void ret_free(Ret *ret)
{
    GList *list;

    if (ret == NULL) {
        return;
    }

    for (list = g_list_first(ret->annotations); list != NULL; list = g_list_next(list)) {
        g_free(list->data);
    }

    for (list = g_list_first(ret->translatorArgus); list != NULL; list = g_list_next(list)) {
        g_free(list->data);
    }

    g_list_free(ret->annotations);
    g_list_free(ret->translatorArgus);
    g_free(ret->type);
    g_free(ret->comment);
    g_free(ret->translator);
    g_free(ret->errorReturnValue);
    g_free(ret);
}

EnumerationItem *enumeration_item_new(const gchar *nativeName, const gchar *alias)
{
    EnumerationItem *item = g_new0(EnumerationItem, 1);

    item->nativeName = g_strdup(nativeName);
    item->alias = g_strdup(alias);

    return item;
}

void enumeration_item_free(EnumerationItem *item)
{
    if (item == NULL) {
        return;
    }

    g_free(item->nativeName);
    g_free(item->alias);
    g_free(item);
}

Enumeration *enumeration_new(void)
{
    Enumeration *enumeration = g_new0(Enumeration, 1);

    enumeration->items = NULL;
    enumeration->name = NULL;
    enumeration->nativeName = NULL;
    enumeration->defaultNative = NULL;
    enumeration->comment = NULL;

    return enumeration;
}

void enumeration_free(Enumeration *enumeration)
{
    if (enumeration == NULL) {
        return;
    }

    g_clear_pointer(&enumeration->items, g_ptr_array_unref);
    g_free(enumeration->name);
    g_free(enumeration->nativeName);
    g_free(enumeration->defaultNative);
    g_free(enumeration->comment);
    g_free(enumeration);
}

TemplateData *template_data_new(const char *name,
                                const char *requires_attrs, /* comma-separated list of required attributes */
                                const char *optional_attrs) /* comma-separated list of optional attributes */
{
    TemplateData *data;
    guint ii;

    if (name == NULL) {
        g_warning("Cannot create API template without name");
        return NULL;
    }
    if (requires_attrs == NULL) {
        g_warning("API template '%s' does not have set 'requires' attribute", name);
        return NULL;
    }

    data = g_new0(TemplateData, 1);
    data->name = g_strdup(name);
    data->requires_attrs = g_strsplit(requires_attrs, ",", 0);
    data->requires_variables = g_new0(gchar *, g_strv_length(data->requires_attrs) + 1);
    for (ii = 0; data->requires_attrs[ii] != NULL; ii++) {
        data->requires_variables[ii] = g_strconcat("${", data->requires_attrs[ii], "}", NULL);
    }
    if (optional_attrs != NULL && *optional_attrs != '\0') {
        data->optional_attrs = g_strsplit(optional_attrs, ",", 0);
        data->optional_variables = g_new0(gchar *, g_strv_length(data->optional_attrs) + 1);
        for (ii = 0; data->optional_attrs[ii] != NULL; ii++) {
            data->optional_variables[ii] = g_strconcat("${", data->optional_attrs[ii], "}", NULL);
        }
    }

    return data;
}

void template_data_free(TemplateData *data)
{
    if (data == NULL) {
        return;
    }

    g_free(data->name);
    g_strfreev(data->requires_attrs);
    g_strfreev(data->requires_variables);
    g_strfreev(data->optional_attrs);
    g_strfreev(data->optional_variables);
    g_clear_pointer(&data->methods, g_ptr_array_unref);
    g_free(data);
}

static gchar *dup_attribute_value(xmlDocPtr doc, const xmlNode *list, int inLine)
{
    xmlChar *xml_value;
    gchar *glib_value;

    xml_value = xmlNodeListGetString(doc, list, inLine);
    if (!xml_value) {
        return NULL;
    }

    glib_value = g_strdup((const gchar *)xml_value);

    xmlFree(xml_value);

    return glib_value;
}

static gchar *dup_node_content(xmlNodePtr node)
{
    xmlChar *xml_value;
    gchar *glib_value;

    xml_value = xmlNodeGetContent(node);

    if (!xml_value) {
        return NULL;
    }

    glib_value = g_strdup((const gchar *)xml_value);

    xmlFree(xml_value);

    return glib_value;
}

GList *get_list_from_string(const gchar *str)
{
    gchar **ret;
    guint iter;
    guint len;
    GList *list = NULL;

    ret = g_strsplit(str, ",", 0);
    len = g_strv_length(ret);
    for (iter = 0; iter < len; iter++) {
        (void)g_strstrip(ret[iter]);
        list = g_list_append(list, ret[iter]);
    }

    g_free(ret);
    return list;
}

gboolean parse_parameters(xmlNode *node, Method *method)
{
    xmlAttr *attr;

    if (xmlStrcmp(node->name, (xmlChar *)"parameter") != 0) {
        return FALSE;
    }

    for (; xmlStrcmp(node->name, (xmlChar *)"parameter") == 0; node = node->next) {

        Parameter *para = parameter_new();

        for (attr = node->properties; attr != NULL; attr = attr->next) {
            if (xmlStrcmp(attr->name, (xmlChar *)"type") == 0) {
                para->type = dup_attribute_value(attr->doc, attr->children, 1);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"name") == 0) {
                para->name = dup_attribute_value(attr->doc, attr->children, 1);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"comment") == 0) {
                para->comment = dup_attribute_value(attr->doc, attr->children, 1);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"annotation") == 0) {
                xmlChar *anno;

                anno = xmlNodeListGetString(attr->doc, attr->children, 1);
                para->annotations = get_list_from_string((const gchar *)anno);
                xmlFree(anno);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"translator_argus") == 0) {
                xmlChar *argus;

                argus = xmlNodeListGetString(attr->doc, attr->children, 1);
                para->translatorArgus = get_list_from_string((const gchar *)argus);
                xmlFree(argus);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"translator") == 0) {
                para->translator = dup_attribute_value(attr->doc, attr->children, 1);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"autofill") == 0) {
                para->autofill = dup_attribute_value(attr->doc, attr->children, 1);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"native_op") == 0) {
                para->native_op = dup_attribute_value(attr->doc, attr->children, 1);
            } else if (xmlStrcmp(attr->name, (xmlChar *)"owner_op") == 0) {
                para->owner_op = dup_attribute_value(attr->doc, attr->children, 1);
            } else {
                fprintf(stderr,
                        "The tag name of %s in parameter cannot be finished\n",
                        (char *)attr->name);
            }
        }
        method->parameters = g_list_append(method->parameters, para);
    }

    return TRUE;
}

gboolean parse_return(xmlNode *node, Method *method)
{
    xmlAttr *attr;

    if (xmlStrcmp(node->name, (xmlChar *)"returns") != 0) {
        return FALSE;
    }

    method->ret = ret_new();

    for (attr = node->properties; attr != NULL; attr = attr->next) {
        if (xmlStrcmp(attr->name, (xmlChar *)"type") == 0) {
            method->ret->type = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"comment") == 0) {
            method->ret->comment = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"annotation") == 0) {
            xmlChar *anno;

            anno = xmlNodeListGetString(attr->doc, attr->children, 1);
            method->ret->annotations = get_list_from_string((const gchar *)anno);
            xmlFree(anno);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"translator") == 0) {
            method->ret->translator = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"translator_argus") == 0) {
            xmlChar *argus;

            argus = xmlNodeListGetString(attr->doc, attr->children, 1);
            method->ret->translatorArgus = get_list_from_string((const gchar *)argus);
            xmlFree(argus);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"error_return_value") == 0) {
            method->ret->errorReturnValue = dup_attribute_value(attr->doc, attr->children, 1);
        } else {
            fprintf(stderr,
                    "The tag name of '%s' in 'returns' cannot be finished\n",
                    (char *)attr->name);
        }
    }
    return TRUE;
}

gboolean parse_comment(xmlNode *node, Method *method)
{
    if (xmlStrcmp(node->name, (xmlChar *)"comment") != 0) {
        return FALSE;
    }

    g_free(method->comment);
    method->comment = dup_node_content(node);
    return TRUE;
}

gboolean parse_custom(xmlNode *node, Method *method)
{
    if (xmlStrcmp(node->name, (xmlChar *)"custom") != 0) {
        return FALSE;
    }

    g_free(method->custom);
    method->custom = dup_node_content(node);
    return TRUE;
}

gboolean parse_method(xmlNode *node, Method *method)
{
    xmlNode *child;
    xmlAttr *attr;

    if (xmlStrcmp(node->name, (xmlChar *)"method") != 0) {
        return FALSE;
    }

    for (attr = node->properties; attr != NULL; attr = attr->next) {
        if (xmlStrcmp(attr->name, (xmlChar *)"name") == 0) {
            method->name = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"corresponds") == 0) {
            method->corresponds = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"kind") == 0) {
            method->kind = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"since") == 0) {
            method->since = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"annotation") == 0) {
            xmlChar *anno;

            anno = xmlNodeListGetString(attr->doc, attr->children, 1);
            method->annotations = get_list_from_string((const gchar *)anno);
            xmlFree(anno);
        } else {
            fprintf(stderr, "The attribute '%s' in method '%s' cannot be parsed",
                    (char *)attr->name, method->name);
            return TRUE;
        }
    }

    for (child = xmlFirstElementChild(node); child != NULL; child = xmlNextElementSibling(child)) {
        if (parse_parameters(child, method) != TRUE &&
            parse_return(child, method) != TRUE &&
            parse_comment(child, method) != TRUE && parse_custom(child, method) != TRUE) {
            fprintf(stderr,
                    "The node named '%s' in method '%s' cannot be parsed\n",
                    (char *)child->name, method->name);
            return FALSE;
        }
    }

    return TRUE;
}

gboolean parse_declaration(xmlNode *node, Declaration *declaration)
{
    xmlAttr *attr;

    g_return_val_if_fail(node != NULL && declaration != NULL, FALSE);
    if (xmlStrcmp(node->name, (xmlChar *)"declaration") != 0) {
        return FALSE;
    }

    for (attr = node->properties; attr != NULL; attr = attr->next) {
        if (xmlStrcmp(attr->name, (xmlChar *)"position") == 0) {
            declaration->position = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"content") == 0) {
            declaration->content = dup_attribute_value(attr->doc, attr->children, 1);
        } else {
            fprintf(stderr,
                    "The node named '%s' in declaration cannot be parsed\n",
                    (char *)attr->name);
        }
    }

    if (!declaration->content) {
        declaration->content = dup_node_content(node);
    }

    return TRUE;
}

gboolean parse_enumeration(xmlNode *node, Enumeration *enumeration)
{
    xmlAttr *attr;
    xmlNode *child;

    g_return_val_if_fail(node != NULL && enumeration != NULL, FALSE);
    if (xmlStrcmp(node->name, (xmlChar *)"enum") != 0) {
        return FALSE;
    }

    for (attr = node->properties; attr != NULL; attr = attr->next) {
        if (xmlStrcmp(attr->name, (xmlChar *)"name") == 0) {
            enumeration->name = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"native_name") == 0) {
            enumeration->nativeName = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"default_native") == 0) {
            enumeration->defaultNative = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"comment") == 0) {
            enumeration->comment = dup_attribute_value(attr->doc, attr->children, 1);
        } else {
            fprintf(stderr,
                    "The node named '%s' in enum '%s' cannot be parsed\n",
                    (char *)attr->name, enumeration->name);
        }
    }

    for (child = xmlFirstElementChild(node); child != NULL; child = xmlNextElementSibling(child)) {
        xmlChar *name, *alias;
        if (xmlStrcmp(child->name, (xmlChar *)"element") != 0) {
            fprintf(stderr,
                    "The child node named '%s' is not an element in enumeration '%s'\n",
                    (char *)child->name, enumeration->name);
            continue;
        }
        name = xmlGetProp(child, (const xmlChar *)"name");
        if (name == NULL) {
            fprintf(stderr,
                    "The element node does not have a 'name' attribute in enumeration '%s'\n",
                    enumeration->name);
            continue;
        }
        alias = xmlGetProp(child, (const xmlChar *)"alias");
        if (enumeration->items == NULL) {
            enumeration->items = g_ptr_array_new_with_free_func((GDestroyNotify)enumeration_item_free);
        }
        g_ptr_array_add(enumeration->items, enumeration_item_new((const gchar *)name, (const gchar *)alias));
        g_clear_pointer(&name, xmlFree);
        g_clear_pointer(&alias, xmlFree);
    }
    return TRUE;
}

static char *
replace_variables_in_string(const char *str,
                            GHashTable *variables)
{
    GString *tmp;
    guint ii;

    if (str == NULL || !strstr(str, "${")) {
        return g_strdup(str);
    }

    tmp = g_string_new(str);
    for (ii = 0; ii < tmp->len; ii++) {
        if (tmp->str[ii] == '$' && tmp->str[ii + 1] == '{') {
            char *end = strchr(tmp->str + ii + 1, '}');
            if (end != NULL) {
                const gchar *value;
                char last = end[1];
                end[1] = '\0';
                value = g_hash_table_lookup(variables, tmp->str + ii);
                if (value) {
                    end[1] = last;
                    g_string_erase(tmp, ii, end - tmp->str - ii + 1);
                    g_string_insert(tmp, ii, value);
                    ii += strlen(value) - 1;
                } else {
                    g_warning("Cannot find variable '%s'", tmp->str + ii);
                    end[1] = last;
                }
            }
        }
    }

    return g_string_free(tmp, tmp->len == 0);
}

static GList *
replace_variables_in_list(/*const*/ GList *src,
                          GHashTable *variables)
{
    GList *des = NULL, *link;

    for (link = src; link != NULL; link = g_list_next(link)) {
        const char *str = link->data;
        char *text = replace_variables_in_string(str, variables);
        if (text != NULL && *text != '\0') {
            if (strchr(text, ',')) {
                char **strv = g_strsplit(text, ",", 0);
                if (strv != NULL) {
                    guint ii;
                    for (ii = 0; strv[ii]; ii++) {
                        const char *item = strv[ii];
                        if (*item != '\0') {
                            des = g_list_prepend(des, g_strstrip(g_strdup(item)));
                        }
                    }
                    g_strfreev(strv);
                } else {
                    des = g_list_prepend(des, text);
                }
            } else {
                des = g_list_prepend(des, text);
            }
        } else {
            g_free(text);
        }
    }

    return g_list_reverse(des);
}

static void
parse_method_from_template(xmlNode *node,
                           GHashTable *api_templates,
                           Structure *structure)
{
    TemplateData *data;
    xmlChar *name;

    if (xmlStrcmp(node->name, (const xmlChar *)"method-from-template") != 0) {
        return;
    }

    name = xmlGetProp(node, (const xmlChar *)"name");
    if (name == NULL) {
        g_warning("Missing 'name' attribute on 'method-from-template'");
        return;
    }

    data = g_hash_table_lookup(api_templates, (const char *)name);
    if (data != NULL) {
        xmlChar *since = xmlGetProp(node, (const xmlChar *)"since");
        GHashTable *variables; /* char * "${name}"~>value */
        guint ii;

        variables = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)xmlFree);
        for (ii = 0; data->requires_attrs[ii] != NULL; ii++) {
            xmlChar *value = xmlGetProp(node, (const xmlChar *)data->requires_attrs[ii]);
            if (value != NULL) {
                g_hash_table_insert(variables, data->requires_variables[ii], value);
            } else {
                g_warning("Required attribute '%s' for template '%s' not found in structure '%s'", data->requires_attrs[ii], (const char *)name, structure->name);
            }
        }
        for (ii = 0; data->optional_attrs != NULL && data->optional_attrs[ii] != NULL; ii++) {
            xmlChar *value = xmlGetProp(node, (const xmlChar *)data->optional_attrs[ii]);
            if (value == NULL) {
                value = xmlStrdup((const xmlChar *)"");
            }
            g_hash_table_insert(variables, data->optional_variables[ii], value);
        }

        for (ii = 0; data->methods != NULL && ii < data->methods->len; ii++) {
            const Method *tmp_method = g_ptr_array_index(data->methods, ii);
            Method *method = method_new();
            GList *link;

#define fill_str_member(_out_str, _in_str, _member) (_out_str)->_member = replace_variables_in_string((_in_str)->_member, variables)
#define copy_str_list(_out_str, _in_str, _member) (_out_str)->_member = replace_variables_in_list((_in_str)->_member, variables)

            fill_str_member(method, tmp_method, name);
            fill_str_member(method, tmp_method, corresponds);
            fill_str_member(method, tmp_method, kind);
            fill_str_member(method, tmp_method, since);
            fill_str_member(method, tmp_method, comment);
            fill_str_member(method, tmp_method, custom);
            copy_str_list(method, tmp_method, annotations);

            if (tmp_method->ret != NULL) {
                method->ret = ret_new();
                fill_str_member(method->ret, tmp_method->ret, type);
                copy_str_list(method->ret, tmp_method->ret, annotations);
                fill_str_member(method->ret, tmp_method->ret, comment);
                fill_str_member(method->ret, tmp_method->ret, translator);
                copy_str_list(method->ret, tmp_method->ret, translatorArgus);
                fill_str_member(method->ret, tmp_method->ret, errorReturnValue);
            }

            for (link = tmp_method->parameters; link != NULL; link = g_list_next(link)) {
                const Parameter *tmp_param = link->data;
                Parameter *param = parameter_new();

                fill_str_member(param, tmp_param, type);
                copy_str_list(param, tmp_param, annotations);
                fill_str_member(param, tmp_param, comment);
                fill_str_member(param, tmp_param, name);
                fill_str_member(param, tmp_param, autofill);
                fill_str_member(param, tmp_param, translator);
                copy_str_list(param, tmp_param, translatorArgus);
                fill_str_member(param, tmp_param, native_op);
                fill_str_member(param, tmp_param, owner_op);

                method->parameters = g_list_prepend(method->parameters, param);
            }

#undef fill_str_member
#undef copy_str_list

            method->parameters = g_list_reverse(method->parameters);
            if (method->since == NULL && since != NULL) {
                method->since = g_strdup((const char *)since);
            }
            if (method->since == NULL) {
                method->since = g_strdup("4.0"); /* get it from somewhere */
            }

            structure->methods = g_list_prepend(structure->methods, method);
        }

        g_hash_table_unref(variables);
        g_clear_pointer(&since, xmlFree);
    } else {
        g_warning("No method template named '%s' found", (const char *)name);
    }
    g_clear_pointer(&name, xmlFree);
}

gboolean parse_structure(xmlNode *node, Structure *structure, GHashTable *api_templates)
{
    xmlAttr *attr;
    xmlNode *child;
    Method *method;
    Enumeration *enumeration;
    Declaration *declaration;

    if (xmlStrcmp(node->name, (xmlChar *)"structure") != 0) {
        return FALSE;
    }

    for (attr = node->properties; attr != NULL; attr = attr->next) {
        if (xmlStrcmp(attr->name, (xmlChar *)"namespace") == 0) {
            structure->nameSpace = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"name") == 0) {
            structure->name = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"native") == 0) {
            structure->native = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"includes") == 0) {
            xmlChar *includes;

            includes = xmlNodeListGetString(attr->doc, attr->children, 1);
            structure->includes = get_list_from_string((const gchar *)includes);
            xmlFree(includes);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"is_possible_global") == 0) {
            xmlChar *strIsPossibleGlobal;

            strIsPossibleGlobal = xmlNodeListGetString(attr->doc, attr->children, 1);
            if (g_strcmp0((const gchar *)strIsPossibleGlobal, "true") == 0) {
                structure->isPossibleGlobal = TRUE;
            }
            xmlFree(strIsPossibleGlobal);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"destroy_func") == 0) {
            structure->destroyFunc = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"clone_func") == 0) {
            structure->cloneFunc = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"new_full_extra_code") == 0) {
            structure->new_full_extraCode = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"default_native") == 0) {
            structure->defaultNative = dup_attribute_value(attr->doc, attr->children, 1);
        } else if (xmlStrcmp(attr->name, (xmlChar *)"is_bare") == 0) {
            xmlChar *strIsBare;

            strIsBare = xmlNodeListGetString(attr->doc, attr->children, 1);
            if (g_strcmp0((const gchar *)strIsBare, "true") == 0) {
                structure->isBare = TRUE;
            }
            xmlFree(strIsBare);
        } else {
            fprintf(stderr,
                    "The attribute of %s in structure '%s' cannot be parsed\n",
                    (char *)attr->name, structure->name);
        }
    }

    for (child = xmlFirstElementChild(node); child != NULL; child = xmlNextElementSibling(child)) {
        if (g_strcmp0((gchar *)child->name, "method") == 0) {
            method = method_new();
            if (!parse_method(child, method)) {
                method_free(method);
            } else {
                structure->methods = g_list_prepend(structure->methods, method);
            }
            method = NULL;
        } else if (g_strcmp0((gchar *)child->name, "method-from-template") == 0) {
            parse_method_from_template(child, api_templates, structure);
        }
        if (g_strcmp0((gchar *)child->name, "declaration") == 0) {
            declaration = declaration_new();
            if (!parse_declaration(child, declaration)) {
                declaration_free(declaration);
            } else {
                structure->declarations = g_list_append(structure->declarations, declaration);
            }
            declaration = NULL;
        } else if (g_strcmp0((gchar *)child->name, "enum") == 0) {
            enumeration = enumeration_new();
            if (!parse_enumeration(child, enumeration)) {
                enumeration_free(enumeration);
            } else {
                structure->enumerations = g_list_append(structure->enumerations, enumeration);
            }
            enumeration = NULL;
        } else if (g_strcmp0((const gchar *)child->name, "skip") == 0) {
            gchar *symbol = dup_node_content(child);
            if (symbol != NULL) {
                if (structure->skips == NULL) {
                    structure->skips = g_ptr_array_new_with_free_func(g_free);
                }
                g_ptr_array_add(structure->skips, symbol);
            }
        }
    }

    populate_dependencies(structure);
    structure->methods = g_list_reverse(structure->methods);

    return TRUE;
}

void populate_dependencies(Structure *structure)
{
    GList *iter_method;
    GList *iter_para;
    Method *method;
    Parameter *para;
    gchar *trueType;

    iter_method = NULL;
    iter_para = NULL;
    method = NULL;
    para = NULL;
    trueType = NULL;

    for (iter_method = g_list_first(structure->methods); iter_method != NULL;
         iter_method = g_list_next(iter_method)) {
        method = (Method *)iter_method->data;
        /*Process the parameters */
        for (iter_para = g_list_first(method->parameters); iter_para != NULL;
             iter_para = g_list_next(iter_para)) {
            para = (Parameter *)iter_para->data;
            trueType = get_true_type(para->type);
            (void)g_hash_table_insert(structure->dependencies, trueType, NULL);
            trueType = NULL;
        }

        if (method->ret != NULL) {
            trueType = get_true_type(method->ret->type);
            (void)g_hash_table_insert(structure->dependencies, trueType, NULL);
            trueType = NULL;
        }
    }
}
