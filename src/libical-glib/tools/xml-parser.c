/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2015 William Yu <williamyu@gnome.org>
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of version 2.1. of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "xml-parser.h"

Structure *
structure_new()
{
	Structure *structure;
	structure = g_new(Structure, 1);
	structure->nameSpace = NULL;
	structure->name = NULL;
	structure->native = NULL;
	structure->includes = NULL;
	structure->methods = NULL;
	structure->isBare = FALSE;
	structure->isPossibleGlobal = FALSE;
	structure->enumerations = NULL;
	structure->destroyFunc = NULL;
	structure->cloneFunc = NULL;
	structure->defaultNative = NULL;
	structure->dependencies = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	structure->declarations = NULL;
	return structure;
}

void
structure_free(Structure *structure)
{
	GList *list;
	Enumeration *enumeration;

	if (structure == NULL)
		return;

	for (list = g_list_first (structure->methods); list != NULL; list = list->next) {
		method_free((Method *)list->data);
	}
	for (list = g_list_first (structure->includes); list != NULL; list = list->next) {
		g_free(list->data);
	}
	for (list = g_list_first (structure->enumerations); list != NULL; list = g_list_next (list)) {
		enumeration = (Enumeration *)list->data;
		enumeration_free (enumeration);
		enumeration = NULL;
	}
	for (list = g_list_first (structure->declarations); list != NULL; list = g_list_next (list)) {
		declaration_free ((Declaration *)list->data);
	}
	g_list_free(structure->methods);
	g_list_free(structure->includes);
	g_list_free (structure->enumerations);
	g_list_free (structure->declarations);
	g_free(structure->nameSpace);
	g_free(structure->name);
	g_free(structure->native);
	g_free (structure->destroyFunc);
	g_free (structure->cloneFunc);
	g_free (structure->defaultNative);
	g_hash_table_destroy (structure->dependencies);

	structure = NULL;
}

Declaration *
declaration_new ()
{
	Declaration *declaration = g_new (Declaration, 1);
	declaration->position = NULL;
	declaration->content = NULL;

	return declaration;
}

void
declaration_free (Declaration *declaration)
{
	if (declaration == NULL)
		return;

	if (declaration->position != NULL)
		g_free (declaration->position);
	if (declaration->content != NULL)
		g_free (declaration->content);
	g_free (declaration);
}

Method *
method_new()
{
	Method *method;
	method = g_new(Method, 1);

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

void
method_free(Method *method)
{
	GList *list;

	if (method == NULL)
		return;

	for (list = method->parameters; list != NULL; list = list->next) {
		parameter_free((Parameter *) list->data);
	}
	for (list = method->annotations; list != NULL; list = list->next) {
		g_free(list->data);
	}
	g_list_free(method->parameters);
	g_list_free (method->annotations);
	g_free(method->name);
	g_free(method->corresponds);
	g_free(method->kind);
	g_free(method->since);
	g_free(method->comment);
	g_free (method->custom);
	ret_free(method->ret);

	method = NULL;
}

Parameter *
parameter_new()
{
	Parameter *parameter;
	parameter = g_new(Parameter, 1);
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

void
parameter_free(Parameter *para)
{
	GList *list;
	if (para == NULL)
		return;

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
	g_free (para->native_op);
	g_free (para->owner_op);

	para = NULL;
}

Ret *
ret_new()
{
	Ret *ret;

	ret = g_new(Ret, 1);
	ret->comment = NULL;
	ret->type = NULL;
	ret->annotations = NULL;
	ret->translator = NULL;
	ret->translatorArgus = NULL;
	ret->errorReturnValue = NULL;
	return ret;
}

void
ret_free(Ret *ret)
{
	GList *list;

	if (ret == NULL)
		return;

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
	g_free (ret->errorReturnValue);

	ret = NULL;
}

Enumeration *
enumeration_new ()
{
	Enumeration *enumeration = g_new (Enumeration, 1);
	enumeration->elements = NULL;
	enumeration->name = NULL;
	enumeration->defaultNative = NULL;
	enumeration->comment = NULL;

	return enumeration;
}

void
enumeration_free (Enumeration *enumeration)
{
	GList *iter;
	if (enumeration == NULL)
		return;

	for (iter = g_list_first (enumeration->elements); iter != NULL; iter = g_list_next (iter)) {
		g_free (iter->data);
	}
	g_list_free (enumeration->elements);
	g_free (enumeration->name);
	g_free (enumeration->defaultNative);
	g_free (enumeration->comment);
	enumeration = NULL;
}

GList *
get_list_from_string(const gchar *str)
{
	gchar **ret;
	int iter;
	int len;
	GList *list = NULL;

	ret = g_strsplit(str, ",", 0);
	len = g_strv_length(ret);
	for (iter = 0; iter < len; iter++) {
		g_strstrip(ret[iter]);
		list = g_list_append(list, ret[iter]);
	}

	g_free(ret);
	return list;
}

gboolean
parse_parameters(xmlNode *node, Method *method)
{
	xmlAttr *attr;
	Parameter *para;
	gchar *anno;
	gchar *argus;


	if (xmlStrcmp(node->name, (xmlChar *) "parameter") != 0)
		return FALSE;

	for (; xmlStrcmp(node->name, (xmlChar *) "parameter") == 0; node = node->next) {

		para = parameter_new();

		for (attr = node->properties; attr != NULL; attr = attr->next) {
			if (xmlStrcmp(attr->name, (xmlChar *) "type") == 0) {
				para->type = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			} else if (xmlStrcmp(attr->name, (xmlChar *) "name") == 0) {
				para->name = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			} else if (xmlStrcmp(attr->name, (xmlChar *) "comment") == 0) {
				para->comment = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			} else if (xmlStrcmp(attr->name, (xmlChar *) "annotation") == 0) {
				anno = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
				para->annotations = get_list_from_string((gchar *) anno);
				xmlFree(anno);
			} else if (xmlStrcmp(attr->name, (xmlChar *) "translator_argus") == 0) {
				argus = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
				para->translatorArgus = get_list_from_string((gchar *) argus);
				xmlFree(argus);
			} else if (xmlStrcmp(attr->name, (xmlChar *) "translator") == 0) {
				para->translator = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			} else if (xmlStrcmp(attr->name, (xmlChar *) "autofill") == 0) {
				para->autofill = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			} else if (xmlStrcmp (attr->name, (xmlChar *) "native_op") == 0) {
				para->native_op = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			} else if (xmlStrcmp (attr->name, (xmlChar *) "owner_op") == 0) {
				para->owner_op = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			} else {
				printf("The tag name of %s in parameter cannot be finished\n", attr->name);
			}
		}
		method->parameters = g_list_append(method->parameters, para);
	}

	return TRUE;
}

gboolean
parse_return(xmlNode *node, Method *method)
{
	xmlAttr *attr;
	gchar *anno;
	gchar *argus;

	if (xmlStrcmp(node->name, (xmlChar *) "returns") != 0) {
		return FALSE;
	}

	method->ret = ret_new();

	for (attr = node->properties; attr != NULL; attr = attr->next) {
		if (xmlStrcmp(attr->name, (xmlChar *)"type") == 0) {
			method->ret->type = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"comment") == 0) {
			method->ret->comment = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"annotation") == 0) {
			anno = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			method->ret->annotations = get_list_from_string((gchar *) anno);
			xmlFree(anno);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"translator") == 0) {
			method->ret->translator = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *) "translator_argus") == 0) {
			argus = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			method->ret->translatorArgus = get_list_from_string((gchar *) argus);
			xmlFree(argus);
		} else if (xmlStrcmp (attr->name, (xmlChar *) "error_return_value") == 0) {
			method->ret->errorReturnValue = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else {
			printf("The tag name of %s in return cannot be finished\n", attr->name);
		}
	}
	return TRUE;
}

gboolean
parse_comment(xmlNode *node, Method *method)
{
	if (xmlStrcmp(node->name, (xmlChar *) "comment") != 0) {
		return FALSE;
	}

	g_free(method->comment);
	method->comment = (gchar *)xmlNodeGetContent(node);
	return TRUE;
}

gboolean
parse_custom(xmlNode *node, Method *method)
{
	if (xmlStrcmp(node->name, (xmlChar *) "custom") != 0) {
		return FALSE;
	}

	g_free(method->custom);
	method->custom = (gchar *)xmlNodeGetContent(node);
	return TRUE;
}

gboolean
parse_method(xmlNode *node, Method *method)
{
	xmlNode *child;
	xmlAttr *attr;
	gchar *anno;

	if (xmlStrcmp(node->name, (xmlChar *) "method") != 0) {
		return FALSE;
	}

	for (attr = node->properties; attr != NULL; attr = attr->next) {
		if (xmlStrcmp(attr->name, (xmlChar *)"name") == 0) {
			method->name = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"corresponds") == 0) {
			method->corresponds = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"kind") == 0) {
			method->kind = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"since") == 0) {
			method->since = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"annotation") == 0) {
			anno = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			method->annotations = get_list_from_string(anno);
			xmlFree(anno);
		} else {
			printf("The attribute %s in method %s cannot be parsed", attr->name, node->name);
			return TRUE;
		}
	}

	for (child = xmlFirstElementChild(node); child != NULL; child = xmlNextElementSibling(child)) {
		if (parse_parameters(child, method) != TRUE &&
			parse_return(child, method) != TRUE &&
			parse_comment(child, method) != TRUE &&
			parse_custom(child, method) != TRUE) {
			printf("The node named %s in method cannot be parsed\n", child->name);
			return FALSE;
		}
	}

	return TRUE;
}

gboolean
parse_declaration (xmlNode *node, Declaration *declaration)
{
	xmlAttr *attr;

	g_return_val_if_fail (node != NULL && declaration != NULL, FALSE);
	if (xmlStrcmp (node->name, (xmlChar *)"declaration") != 0) {
		return FALSE;
	}

	for (attr = node->properties; attr != NULL; attr = attr->next) {
		if (xmlStrcmp (attr->name, (xmlChar *)"position") == 0) {
			declaration->position = (gchar *)xmlNodeListGetString (attr->doc, attr->children, 1);
		} else if (xmlStrcmp (attr->name, (xmlChar *)"content") == 0) {
			declaration->content = (gchar *)xmlNodeListGetString (attr->doc, attr->children, 1);
		} else {
			printf ("The node named %s in declaration cannot be parsed\n", attr->name);
		}
	}

	return TRUE;
}

gboolean
parse_enumeration (xmlNode *node, Enumeration *enumeration)
{
	xmlAttr *attr;
	xmlNode *child;
	gchar *elementName;

	g_return_val_if_fail (node != NULL && enumeration != NULL, FALSE);
	if (xmlStrcmp (node->name, (xmlChar *)"enum") != 0) {
		return FALSE;
	}

	for (attr = node->properties; attr != NULL; attr = attr->next) {
		if (xmlStrcmp (attr->name, (xmlChar *)"name") == 0) {
			enumeration->name = (gchar *)xmlNodeListGetString (attr->doc, attr->children, 1);
		} else if (xmlStrcmp (attr->name, (xmlChar *)"default_native") == 0) {
			enumeration->defaultNative = (gchar *)xmlNodeListGetString (attr->doc, attr->children, 1);
		} else if (xmlStrcmp (attr->name, (xmlChar *)"comment") == 0) {
			enumeration->comment = (gchar *)xmlNodeListGetString (attr->doc, attr->children, 1);
		} else {
			printf ("The node named %s in enum cannot be parsed\n", attr->name);
		}
	}

	for (child = xmlFirstElementChild (node); child != NULL; child = xmlNextElementSibling (child)) {
		if (xmlStrcmp (child->name, (xmlChar *)"element") != 0) {
			printf ("The child node named %s is not an element\n", child->name);
			continue;
		}
		elementName = (gchar *)xmlNodeListGetString(child->properties->doc, child->properties->children, 1);
		enumeration->elements = g_list_append (enumeration->elements, elementName);
		elementName = NULL;
	}
	return TRUE;
}

gboolean
parse_structure(xmlNode *node, Structure *structure)
{
	xmlAttr *attr;
	xmlNode *child;
	Method *method;
	Enumeration *enumeration;
	gchar *strIsPossibleGlobal;
	gchar *strIsBare;
	gchar *includes;
	Declaration *declaration;

	if (xmlStrcmp(node->name, (xmlChar *) "structure") != 0) {
		return FALSE;
	}

	for (attr = node->properties; attr != NULL; attr = attr->next) {
		if (xmlStrcmp(attr->name, (xmlChar *) "namespace") == 0) {
			structure->nameSpace = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *) "name") == 0) {
			structure->name = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *) "native") == 0) {
			structure->native = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *) "includes") == 0) {
			includes = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			structure->includes = get_list_from_string((gchar *) includes);
			xmlFree(includes);
		} else if (xmlStrcmp(attr->name, (xmlChar *) "is_possible_global") == 0) {
			strIsPossibleGlobal = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			if (g_strcmp0(strIsPossibleGlobal, "true") == 0) {
				structure->isPossibleGlobal = TRUE;
			}
			g_free(strIsPossibleGlobal);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"destroy_func") == 0) {
			structure->destroyFunc = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"clone_func") == 0) {
			structure->cloneFunc = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"default_native") == 0) {
			structure->defaultNative = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
		} else if (xmlStrcmp(attr->name, (xmlChar *)"is_bare") == 0) {
			strIsBare = (gchar *)xmlNodeListGetString(attr->doc, attr->children, 1);
			if (g_strcmp0(strIsBare, "true") == 0) {
				structure->isBare = TRUE;
			}
			g_free(strIsBare);
		} else {
			printf("The attribute of %s in structure cannot be parsed\n", attr->name);
		}
	}

	for (child = xmlFirstElementChild(node); child != NULL; child = xmlNextElementSibling(child)) {
		if (g_strcmp0 ((gchar *)child->name, "method") == 0) {
			method = method_new();
			if (!parse_method(child, method))
				method_free(method);
			structure->methods = g_list_append(structure->methods, method);
			method = NULL;
		} if (g_strcmp0 ((gchar *)child->name, "declaration") == 0) {
			declaration = declaration_new ();
			if (!parse_declaration(child, declaration))
				declaration_free (declaration);
			structure->declarations = g_list_append (structure->declarations, declaration);
			declaration = NULL;
		} else if (g_strcmp0 ((gchar *)child->name, "enum") == 0) {
			enumeration = enumeration_new ();
			if (!parse_enumeration (child, enumeration))
				enumeration_free (enumeration);
			structure->enumerations = g_list_append (structure->enumerations, enumeration);
			enumeration = NULL;
		}
	}

	populate_dependencies (structure);

	return TRUE;
}

void
populate_dependencies (Structure *structure)
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

	for (iter_method = g_list_first (structure->methods); iter_method != NULL; iter_method = g_list_next (iter_method)) {
		method = (Method *)iter_method->data;
		/*Process the parameters*/
		for (iter_para = g_list_first (method->parameters); iter_para != NULL; iter_para = g_list_next (iter_para)) {
			para = (Parameter *)iter_para->data;
			trueType = get_true_type (para->type);
			g_hash_table_insert (structure->dependencies, trueType, NULL);
			trueType = NULL;
		}

		if (method->ret != NULL) {
			trueType = get_true_type (method->ret->type);
			g_hash_table_insert (structure->dependencies, trueType, NULL);
			trueType = NULL;
		}
	}
}
