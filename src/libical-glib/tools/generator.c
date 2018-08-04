/*
 * Copyright (C) 2015 William Yu <williamyu@gnome.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of either:
 *
 *   The LGPL as published by the Free Software Foundation, version
 *   2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * Or:
 *
 *   The Mozilla Public License Version 2.0. You may obtain a copy of
 *   the License at http://www.mozilla.org/MPL/
 */

#include "generator.h"

#include <errno.h>

static const gchar *templates_dir = NULL;

static FILE *open_file(const gchar *dir, const gchar *filename)
{
    gchar *path;
    FILE *tmpl;

    path = g_build_filename(dir, filename, NULL);
    g_return_val_if_fail(path != NULL, NULL);

    tmpl = fopen(path, "rb");
    if (!tmpl)
        fprintf(stderr, "Failed to open '%s'\n", path);

    g_free(path);

    return tmpl;
}

gchar *get_source_method_comment(Method *method)
{
    GList *iter_list;
    GList *jter;
    Parameter *para;
    gchar *anno;
    gchar *buffer;
    gchar *res;
    gchar *comment_line;
    guint iter;
    guint len;
    gint count;
    gint cursor;
    gchar *full_flag;
    guint full_flag_len;
    gchar *full_comment;
    guint comment_len;

    g_return_val_if_fail(method != NULL, NULL);

    res = g_strdup("/**");
    buffer = g_strconcat(res, "\n * ", method->name, ":", NULL);
    g_free(res);
    res = buffer;

    for (iter_list = g_list_first(method->annotations); iter_list != NULL;
         iter_list = g_list_next(iter_list)) {
        anno = (gchar *)iter_list->data;
        buffer = g_strconcat(res, " (", anno, ")", NULL);
        g_free(res);
        res = buffer;
    }

    /* Processing the parameters */
    if (method->parameters != NULL) {
        full_flag = g_strdup("FULL:");
        full_flag_len = (guint)strlen(full_flag);

        for (iter_list = g_list_first(method->parameters); iter_list != NULL;
             iter_list = g_list_next(iter_list)) {
            para = (Parameter *)iter_list->data;
            comment_len = (guint)strlen(para->comment);

            /* Handling the special case in which the parameter's comment is fully specified */
            for (iter = 0; iter < full_flag_len && iter < comment_len; iter++) {
                if (full_flag[iter] != para->comment[iter]) {
                    break;
                }
            }

            if (iter == full_flag_len) {
                full_comment = g_new(gchar, comment_len - full_flag_len + 1);
                (void)g_stpcpy(full_comment, para->comment + full_flag_len);
                buffer = g_strconcat(res, "\n * ", full_comment, NULL);
                g_free(res);
                res = buffer;
                g_free(full_comment);
            } else {
                buffer = g_strconcat(res, "\n * @", NULL);
                g_free(res);
                res = buffer;

                para = (Parameter *)iter_list->data;
                buffer = g_strconcat(res, para->name, NULL);
                g_free(res);
                res = buffer;

                for (jter = g_list_first(para->annotations);
                     jter != NULL; jter = g_list_next(jter)) {
                    anno = (gchar *)jter->data;
                    if (jter == g_list_first(para->annotations)) {
                        buffer = g_strconcat(res, ": (", anno, ")", NULL);
                    } else {
                        buffer = g_strconcat(res, " (", anno, ")", NULL);
                    }
                    g_free(res);
                    res = buffer;
                }

                if (para->comment != NULL) {
                    buffer = g_strconcat(res, ": ", para->comment, NULL);
                    g_free(res);
                    res = buffer;
                }
            }
        }

        g_free(full_flag);
    }

    /* Processing general comment */
    if (method->comment != NULL) {
        comment_line = g_new(gchar, BUFFER_SIZE);
        *comment_line = '\0';
        len = (guint)strlen(method->comment);
        count = 0;
        (void)g_stpcpy(comment_line, "\n *\n * ");
        for (iter = 0; iter < len; iter++) {
            if (count >= COMMENT_LINE_LENGTH && method->comment[iter] == ' ') {
                (void)g_stpcpy(comment_line + strlen(comment_line), "\n *");
                count = -1;
            }

            cursor = (gint)strlen(comment_line);
            comment_line[cursor] = method->comment[iter];
            comment_line[cursor + 1] = '\0';

            count++;
        }

        buffer = g_strconcat(res, comment_line, NULL);
        g_free(res);
        g_free(comment_line);
        res = buffer;
    }

    /* Processing return */
    if (method->ret != NULL) {
        buffer = g_strconcat(res, "\n *\n * Returns", NULL);
        g_free(res);
        res = buffer;

        for (jter = g_list_first(method->ret->annotations);
             jter != NULL; jter = g_list_next(jter)) {
            anno = (gchar *)jter->data;
            if (jter == g_list_first(method->ret->annotations)) {
                buffer = g_strconcat(res, ": (", anno, ")", NULL);
            } else {
                buffer = g_strconcat(res, " (", anno, ")", NULL);
            }
            g_free(res);
            res = buffer;
        }

        if (method->ret->comment != NULL) {
            buffer = g_strconcat(res, ": ", method->ret->comment, NULL);
            g_free(res);
            res = buffer;
        }
    }

    /* Processing the since */
    buffer = g_strconcat(res, "\n *\n * Since: ", method->since, "\n *", NULL);
    g_free(res);
    res = buffer;

    buffer = g_strconcat(res, "\n **/", NULL);
    g_free(res);
    res = buffer;
    buffer = NULL;
    return res;
}

gchar *get_upper_camel_from_lower_snake(const gchar *lowerSnake)
{
    gchar *buffer;
    gchar *ret;
    guint len;
    guint i;

    g_return_val_if_fail(lowerSnake != NULL && *lowerSnake != '\0', NULL);

    buffer = g_new(gchar, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
    buffer[0] = toupper(lowerSnake[0]);
    buffer[1] = '\0';
    for (i = 1; i < (guint)strlen(lowerSnake); i++) {
        if (lowerSnake[i] == '_') {
            len = (guint)strlen(buffer);
            buffer[len] = toupper(lowerSnake[++i]);
            buffer[len + 1] = '\0';
        } else {
            len = (guint)strlen(buffer);
            buffer[len] = lowerSnake[i];
            buffer[len + 1] = '\0';
        }
    }

    ret = g_new(gchar, strlen(buffer) + 1);
    for (i = 0; i < (guint)strlen(buffer); i++) {
        ret[i] = buffer[i];
    }
    ret[i] = '\0';
    g_free(buffer);
    return ret;
}

gchar *get_upper_snake_from_lower_snake(const gchar *lowerSnake)
{
    gchar *ret;
    gchar *dest;
    guint i;

    g_return_val_if_fail(lowerSnake != NULL && *lowerSnake != '\0', NULL);

    ret = g_new(gchar, BUFFER_SIZE);
    ret[0] = '\0';

    for (i = 0; i < (guint)strlen(lowerSnake); i++) {
        if (lowerSnake[i] == '_') {
            ret[i] = '_';
            ret[i + 1] = '\0';
        } else {
            ret[i] = toupper(lowerSnake[i]);
            ret[i + 1] = '\0';
        }
    }

    dest = g_new(gchar, strlen(ret) + 1);
    for (i = 0; i < (guint)strlen(ret); i++) {
        dest[i] = ret[i];
    }
    dest[i] = '\0';
    g_free(ret);
    return dest;
}

gchar *get_upper_snake_from_upper_camel(const gchar *upperCamel)
{
    gchar *lowerSnake;
    gchar *upperSnake;

    g_return_val_if_fail(upperCamel != NULL, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    upperSnake = get_upper_snake_from_lower_snake(lowerSnake);
    g_free(lowerSnake);
    return upperSnake;
}

gchar *get_lower_snake_from_upper_camel(const gchar *upperCamel)
{
    gchar *buffer;
    gchar *dest;
    guint i;
    guint len;

    g_return_val_if_fail(upperCamel != NULL && *upperCamel != '\0', NULL);

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    for (i = 0; i < (guint)strlen(upperCamel); i++) {
        if (isupper(upperCamel[i]) && i != 0) {
            len = (guint)strlen(buffer);
            buffer[len] = '_';
            buffer[len + 1] = tolower(upperCamel[i]);
            buffer[len + 2] = '\0';
        } else {
            len = (guint)strlen(buffer);
            buffer[len] = tolower(upperCamel[i]);
            buffer[len + 1] = '\0';
        }
    }

    dest = g_strdup(buffer);
    g_free(buffer);
    return dest;
}

gchar *get_lower_train_from_lower_snake(const gchar *lowerSnake)
{
    guint i;
    gchar *ret;
    guint len;

    g_return_val_if_fail(lowerSnake != NULL && *lowerSnake != '\0', NULL);
    len =  (guint)strlen(lowerSnake);

    ret = g_strdup(lowerSnake);
    for (i = 0; i < len; i++) {
        if (lowerSnake[i] == '_')
            ret[i] = '-';
    }

    return ret;
}

gchar *get_lower_train_from_upper_camel(const gchar *upperCamel)
{
    gchar *ret;
    guint i, len;

    g_return_val_if_fail(upperCamel != NULL && *upperCamel != '\0', NULL);

    ret = get_lower_snake_from_upper_camel(upperCamel);
    len = (guint)strlen(ret);
    for (i = 0; i < len; i++) {
        if (ret[i] == '_')
            ret[i] = '-';
    }

    return ret;
}

void generate_header_method_get_type(FILE *out, Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Method *get_type;

    g_return_if_fail(out != NULL && structure != NULL);
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    g_free(upperCamel);

    get_type = method_new();
    get_type->ret = ret_new();
    get_type->ret->type = g_strdup("GType");
    get_type->name = g_strconcat(lowerSnake, "_get_type", NULL);
    g_free(lowerSnake);
    generate_header_method_proto(out, get_type, FALSE);
    method_free(get_type);
}

void generate_header_method_new_full(FILE *out, Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Parameter *para;
    Method *new_full;

    g_return_if_fail(out != NULL && structure != NULL);

    new_full = method_new();
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    new_full->ret = ret_new();
    new_full->ret->type = g_strconcat(upperCamel, " *", NULL);
    g_free(upperCamel);

    new_full->name = g_strconcat(lowerSnake, "_new_full", NULL);
    g_free(lowerSnake);

    para = parameter_new();
    if (structure->isBare) {
        para->type = g_strdup(structure->native);
    } else {
        para->type = g_strconcat(structure->native, " *", NULL);
    }
    para->name = g_strdup("native");
    new_full->parameters = g_list_append(new_full->parameters, para);
    para = NULL;

    if (structure->isBare == FALSE) {
        para = parameter_new();
        para->type = g_strdup("GObject *");
        para->name = g_strdup("owner");
        new_full->parameters = g_list_append(new_full->parameters, para);
        para = NULL;
    }

    if (structure->isPossibleGlobal == TRUE) {
        para = parameter_new();
        para->type = g_strdup("gboolean");
        para->name = g_strdup("is_global_memory");
        new_full->parameters = g_list_append(new_full->parameters, para);
        para = NULL;
    }

    generate_header_method_proto(out, new_full, TRUE);
    method_free(new_full);
}

gchar *get_source_method_proto_new_full(Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Parameter *para;
    Method *new_full;
    gchar *res;

    g_return_val_if_fail(structure != NULL, NULL);

    new_full = method_new();
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);

    new_full->ret = ret_new();
    new_full->ret->type = g_strconcat(upperCamel, " *", NULL);
    g_free(upperCamel);

    new_full->name = g_strconcat(lowerSnake, "_new_full", NULL);
    g_free(lowerSnake);

    para = parameter_new();
    if (structure->isBare) {
        para->type = g_strdup(structure->native);
    } else {
        para->type = g_strconcat(structure->native, " *", NULL);
    }
    para->name = g_strdup("native");
    new_full->parameters = g_list_append(new_full->parameters, para);
    para = NULL;

    if (structure->isBare == FALSE) {
        para = parameter_new();
        para->type = g_strdup("GObject *");
        para->name = g_strdup("owner");
        new_full->parameters = g_list_append(new_full->parameters, para);
        para = NULL;
    }

    if (structure->isPossibleGlobal == TRUE) {
        para = parameter_new();
        para->type = g_strdup("gboolean");
        para->name = g_strdup("is_global_memory");
        new_full->parameters = g_list_append(new_full->parameters, para);
        para = NULL;
    }

    res = get_source_method_proto(new_full);
    method_free(new_full);
    return res;
}

gchar *get_source_method_proto_set_owner(Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Parameter *para;
    Method *set_owner;
    gchar *res;

    g_return_val_if_fail(structure != NULL, NULL);
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    set_owner = method_new();

    set_owner->name = g_strconcat(lowerSnake, "_set_owner", NULL);
    g_free(lowerSnake);

    para = parameter_new();
    para->type = g_strconcat("const ", upperCamel, " *", NULL);
    g_free(upperCamel);
    para->name = g_strdup("object");
    set_owner->parameters = g_list_append(set_owner->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("GObject *");
    para->name = g_strdup("owner");
    set_owner->parameters = g_list_append(set_owner->parameters, para);
    para = NULL;

    res = get_source_method_proto(set_owner);
    method_free(set_owner);
    return res;
}

gchar *get_source_method_proto_set_native(Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Parameter *para;
    Method *set_native;
    gchar *res;

    g_return_val_if_fail(structure != NULL, NULL);
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    set_native = method_new();
    set_native->name = g_strconcat(lowerSnake, "_set_native", NULL);
    g_free(lowerSnake);

    para = parameter_new();
    para->type = g_strconcat(upperCamel, " *", NULL);
    g_free(upperCamel);
    para->name = g_strdup("object");
    set_native->parameters = g_list_append(set_native->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("gpointer");
    para->name = g_strdup("native");
    set_native->parameters = g_list_append(set_native->parameters, para);
    para = NULL;

    res = get_source_method_proto(set_native);
    method_free(set_native);
    return res;
}

gchar *get_source_method_proto_set_is_global(Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Parameter *para;
    Method *set_is_global;
    gchar *res;

    g_return_val_if_fail(structure != NULL, NULL);
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    set_is_global = method_new();
    set_is_global->name = g_strconcat(lowerSnake, "_set_is_global", NULL);
    g_free(lowerSnake);

    para = parameter_new();
    para->type = g_strconcat(upperCamel, " *", NULL);
    g_free(upperCamel);
    para->name = g_strdup("object");
    set_is_global->parameters = g_list_append(set_is_global->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("gboolean");
    para->name = g_strdup("is_global_memory");
    set_is_global->parameters = g_list_append(set_is_global->parameters, para);
    para = NULL;

    res = get_source_method_proto(set_is_global);
    method_free(set_is_global);
    return res;
}

gchar *get_source_method_proto_set_property(Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Parameter *para;
    Method *set_property;
    gchar *res;

    g_return_val_if_fail(structure != NULL, NULL);
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    g_free(upperCamel);
    set_property = method_new();
    set_property->name = g_strconcat(lowerSnake, "_set_property", NULL);
    g_free(lowerSnake);

    para = parameter_new();
    para->type = g_strdup("GObject *");
    para->name = g_strdup("object");
    set_property->parameters = g_list_append(set_property->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("guint");
    para->name = g_strdup("property_id");
    set_property->parameters = g_list_append(set_property->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("const GValue *");
    para->name = g_strdup("value");
    set_property->parameters = g_list_append(set_property->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("GParamSpec *");
    para->name = g_strdup("pspec");
    set_property->parameters = g_list_append(set_property->parameters, para);
    para = NULL;

    res = get_source_method_proto(set_property);
    method_free(set_property);
    return res;
}

gchar *get_source_method_proto_get_property(Structure *structure)
{
    gchar *upperCamel;
    gchar *lowerSnake;
    Parameter *para;
    Method *get_property;
    gchar *res;

    g_return_val_if_fail(structure != NULL, NULL);
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    g_free(upperCamel);
    get_property = method_new();
    get_property->name = g_strconcat(lowerSnake, "_get_property", NULL);
    g_free(lowerSnake);

    para = parameter_new();
    para->type = g_strdup("GObject *");
    para->name = g_strdup("object");
    get_property->parameters = g_list_append(get_property->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("guint");
    para->name = g_strdup("property_id");
    get_property->parameters = g_list_append(get_property->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("GValue *");
    para->name = g_strdup("value");
    get_property->parameters = g_list_append(get_property->parameters, para);
    para = NULL;

    para = parameter_new();
    para->type = g_strdup("GParamSpec *");
    para->name = g_strdup("pspec");
    get_property->parameters = g_list_append(get_property->parameters, para);
    para = NULL;

    res = get_source_method_proto(get_property);
    method_free(get_property);
    return res;
}

static void write_str(FILE *fp, const gchar *str)
{
    size_t len;

    g_return_if_fail(fp != NULL);
    g_return_if_fail(str != NULL);

    len = strlen(str);

    if (fwrite(str, sizeof(gchar), len, fp) != len)
        g_error("Failed to write %d bytes to file: %s", (gint)len, g_strerror(errno));
}

static FILE *open_private_header(void)
{
    static gboolean first_private_header_write = TRUE;
    const gchar *mode = "ab";
    FILE *fp;
    const gchar *guard = "#ifndef LIBICAL_GLIB_PRIVATE_H\n#define LIBICAL_GLIB_PRIVATE_H\n";

    if (first_private_header_write) {
        first_private_header_write = FALSE;
        mode = "wb";
    }

    fp = fopen(PRIVATE_HEADER, mode);
    if ((g_strcmp0(mode, "wb") == 0) && fp != NULL) {
        write_str(fp, guard);
        write_str(fp, "\n#include \"libical-glib.h\"\n");
    }
    return fp;
}

static void close_private_header(void)
{
    FILE *fp;
    const gchar *endGuard = "#endif\n";

    fp = fopen(PRIVATE_HEADER, "ab");
    if (fp != NULL) {
        write_str(fp, endGuard);
        (void)fclose(fp);
    }
}

void generate_header_method_protos(FILE *out, Structure *structure)
{
    GList *iter;
    Method *method;
    gchar *typeName;
    FILE *privateHeader;
    gchar *privateHeaderComment;

    privateHeader = NULL;
    typeName = g_strconcat(structure->nameSpace, structure->name, NULL);
    privateHeaderComment = g_strconcat("\n/* Private methods for ", typeName, " */\n", NULL);

    if (structure->native != NULL) {
        /** Open or create the private header file if it does not exist.
         * Create the forward declaration.
         * Create the new_full method in it.
         */
        privateHeader = open_private_header();
        write_str(privateHeader, privateHeaderComment);
        generate_header_method_new_full(privateHeader, structure);

        generate_header_method_get_type(out, structure);
    }

    for (iter = g_list_first(structure->methods); iter != NULL; iter = g_list_next(iter)) {
        method = (Method *)iter->data;

        if (g_strcmp0(method->kind, "private") == 0) {
            /* This checks whether there was method declared in private header already.
               If not, the forward declaration is needed. */
            if (privateHeader == NULL) {
                privateHeader = open_private_header();
            }
            generate_header_method_proto(privateHeader, method, TRUE);
        } else {
            generate_header_method_proto(out, method, FALSE);
        }
    }

    g_free(typeName);

    if (privateHeader != NULL) {
        fclose(privateHeader);
        privateHeader = NULL;
    }
}

void generate_header_method_proto(FILE *out, Method *method, gboolean isPrivate)
{
    GList *iter_list;
    Parameter *para;
    gint count;
    gchar *buffer;
    gint iter;
    gint len;

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    if (isPrivate) {
        write_str(out, "\n");
    } else {
        write_str(out, "\nLIBICAL_ICAL_EXPORT\n");
    }

    /* Generate the method return */
    if (method->ret == NULL) {
        write_str(out, "void");
        for (iter = 0; iter < RET_TAB_COUNT; iter++) {
            write_str(out, "\t");
        }
    } else {
        count = (gint)strlen(method->ret->type) / TAB_SIZE;
        *buffer = '\0';
        if (count >= RET_TAB_COUNT) {
            buffer[0] = '\n';
            buffer[1] = '\0';
            count = 0;
        }
        for (iter = count; iter < RET_TAB_COUNT; iter++) {
            len = (gint)strlen(buffer);
            buffer[len] = '\t';
            buffer[len + 1] = '\0';
        }

        write_str(out, method->ret->type);
        if (method->ret->type[strlen(method->ret->type) - 1] != '*') {
            write_str(out, " ");
        }
        write_str(out, buffer);
    }

    /* Generate the method name */
    count = (gint)(strlen(method->name) / TAB_SIZE);
    *buffer = '\0';
    if (count >= METHOD_NAME_TAB_COUNT) {
        buffer[0] = '\n';
        buffer[1] = '\0';
        count = 0;
        for (iter = count; iter < RET_TAB_COUNT + METHOD_NAME_TAB_COUNT; iter++) {
            len = (gint)strlen(buffer);
            buffer[len] = '\t';
            buffer[len + 1] = '\0';
        }
    } else {
        for (iter = count; iter < METHOD_NAME_TAB_COUNT; iter++) {
            len = (gint)strlen(buffer);
            buffer[len] = '\t';
            buffer[len + 1] = '\0';
        }
    }

    write_str(out, method->name);
    write_str(out, buffer);

    /* Generate all the parameters */
    for (iter = 0; iter < RET_TAB_COUNT + METHOD_NAME_TAB_COUNT; iter++) {
        buffer[iter] = '\t';
    }
    buffer[iter] = '\0';

    if (method->parameters == NULL) {
        write_str(out, "(void);");
    } else {
        for (iter_list = g_list_first(method->parameters); iter_list != NULL;
             iter_list = g_list_next(iter_list)) {
            para = (Parameter *)iter_list->data;
            if (iter_list == g_list_first(method->parameters)) {
                write_str(out, "(");
            } else {
                write_str(out, ",\n");
                write_str(out, buffer);
                write_str(out, " ");
            }
            write_str(out, para->type);
            if (para->type[strlen(para->type) - 1] != '*') {
                write_str(out, " ");
            }
            write_str(out, para->name);
        }
        write_str(out, ");");
    }
    write_str(out, "\n");
    g_free(buffer);
}

void generate_code_from_template(FILE *in, FILE *out, Structure *structure, GHashTable *table)
{
    gint c;
    gchar *buffer;
    gint count;
    gchar last;
    gint len;
    GList *iter;
    gchar *method;
    gchar *val;

    g_return_if_fail(in != NULL && out != NULL && structure != NULL && table != NULL);

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    while ((c = fgetc(in)) != EOF) {
        if (c == '$') {
            if ((c = fgetc(in)) != '{' && c != '^') {
                printf("The following char is not {");
                g_free(buffer);
                return;
            }

            if (c == '^') {
                count = 1;
                last = '\0';
                while (!((c = fgetc(in)) == '$' && last == '^' && count == 1)) {
                    if (c == '^' && last == '$') {
                        ++count;
                        last = '\0';
                    } else if (c == '$' && last == '^') {
                        --count;
                        last = '\0';
                    } else {
                        last = c;
                    }
                    len = (gint)strlen(buffer);
                    buffer[len] = c;
                    buffer[len + 1] = '\0';
                }
                if (strlen(buffer) > 0) {
                    buffer[strlen(buffer) - 1] = '\0';
                }
                generate_conditional(out, structure, buffer, table);
            } else {
                while ((c = fgetc(in)) != '}') {
                    len = (gint)strlen(buffer);
                    buffer[len] = c;
                    buffer[len + 1] = '\0';
                }

                if (g_strcmp0(buffer, "source") == 0) {
                    for (iter = g_list_first(structure->methods); iter != NULL;
                         iter = g_list_next(iter)) {
                        method =
                            get_source_method_body((Method *)iter->data, structure->nameSpace);
                        write_str(out, method);
                        if (iter != g_list_last(structure->methods)) {
                            write_str(out, "\n\n");
                        }
                        g_free(method);
                    }
                } else if (g_strcmp0(buffer, "enums") == 0) {
                    generate_header_enums(out, structure);
                } else if (g_strcmp0(buffer, "protos") == 0) {
                    generate_header_method_protos(out, structure);
                } else if (g_strcmp0(buffer, "headerIncludes") == 0) {
                    generate_header_includes(out, structure);
                } else if (g_strcmp0(buffer, "sourceIncludes") == 0) {
                    generate_source_includes(out, structure);
                } else if (g_strcmp0(buffer, "header_declaration") == 0) {
                    generate_header_header_declaration(out, structure);
                } else if (g_hash_table_contains(table, buffer)) {
                    val = g_hash_table_lookup(table, buffer);
                    write_str(out, val);
                    val = NULL;
                } else if (g_strcmp0(buffer, "structure_boilerplate") == 0) {
                    if (structure->native != NULL)
                        generate_header_structure_boilerplate(out, structure, table);
                } else if (g_hash_table_contains(table, buffer)) {
                    val = g_hash_table_lookup(table, buffer);
                    write_str(out, val);
                    val = NULL;
                } else if (g_strcmp0(buffer, "source_boilerplate") == 0) {
                    if (structure->native != NULL)
                        generate_source_structure_boilerplate(out, structure, table);
                } else {
                    printf("The string %s is not recognized, please check the template\n", buffer);
                    fflush(NULL);
                    g_free(buffer);
                    return;
                }
            }
            buffer[0] = '\0';
        } else {
            fputc(c, out);
        }
    }

    g_free(buffer);
}

void generate_header(FILE *out, Structure *structure, GHashTable *table)
{
    FILE *in;

    g_return_if_fail(out != NULL && structure != NULL && table != NULL);

    in = open_file(templates_dir, HEADER_TEMPLATE);
    generate_code_from_template(in, out, structure, table);
    fclose(in);
}

void generate_header_structure_boilerplate(FILE *out, Structure *structure, GHashTable *table)
{
    FILE *in;

    g_return_if_fail(out != NULL && structure != NULL && table != NULL);

    in = open_file(templates_dir, HEADER_STRUCTURE_BOILERPLATE_TEMPLATE);
    generate_code_from_template(in, out, structure, table);
    fclose(in);
}

void generate_header_includes(FILE *out, Structure *structure)
{
    gchar *typeName;
    Structure *parentStructure;
    gchar *lowerTrain;
    gchar *upperCamel;
    gchar *ownUpperCamel;
    gchar *includeName;
    GHashTable *includeNames;
    GHashTableIter iter_table;
    gpointer key;
    gpointer value;
    gchar *kind;
    GList *iter;

    g_return_if_fail(out != NULL && structure != NULL);

    for (iter = g_list_first(structure->includes); iter != NULL; iter = g_list_next(iter)) {
        includeName = (gchar *)iter->data;
        write_str(out, "#include <");
        write_str(out, includeName);
        write_str(out, ">\n");
    }

    write_str(out, "#include <");
    write_str(out, COMMON_HEADER);
    write_str(out, ".h>\n");

    includeNames = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    g_free(upperCamel);

    for (g_hash_table_iter_init(&iter_table, structure->dependencies);
         g_hash_table_iter_next(&iter_table, &key, &value);) {
        typeName = (gchar *)key;
        if (g_hash_table_contains(type2structure, typeName)) {
            parentStructure = g_hash_table_lookup(type2structure, typeName);
            upperCamel = g_strconcat(parentStructure->nameSpace, parentStructure->name, NULL);
            ownUpperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
            if (g_strcmp0(upperCamel, ownUpperCamel) == 0) {
                g_free(upperCamel);
                g_free(ownUpperCamel);
                continue;
            }

            kind = g_strdup(g_hash_table_lookup(type2kind, typeName));
            if (g_strcmp0(kind, "enum") != 0) {
                g_free(kind);
                g_free(upperCamel);
                g_free(ownUpperCamel);
                continue;
            }
            g_free(kind);
            lowerTrain = get_lower_train_from_upper_camel(upperCamel);
            g_free(upperCamel);
            (void)g_hash_table_insert(includeNames, lowerTrain, NULL);
            lowerTrain = NULL;
        }
    }

    for (g_hash_table_iter_init(&iter_table, includeNames);
         g_hash_table_iter_next(&iter_table, &key, &value);) {
        includeName = (gchar *)key;
        write_str(out, "#include <libical-glib/");
        write_str(out, includeName);
        write_str(out, ".h>\n");
    }
    g_hash_table_destroy(includeNames);
}

void generate_source_includes(FILE *out, Structure *structure)
{
    gchar *typeName;
    Structure *parentStructure;
    gchar *lowerTrain;
    gchar *upperCamel;
    gchar *ownUpperCamel;
    gchar *includeName;
    GHashTable *includeNames;
    GHashTableIter iter_table;
    gpointer key;
    gpointer value;

    g_return_if_fail(out != NULL && structure != NULL);

    includeNames = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerTrain = get_lower_train_from_upper_camel(upperCamel);
    g_free(upperCamel);
    write_str(out, "#include \"");
    write_str(out, lowerTrain);
    write_str(out, ".h\"\n");
    g_free(lowerTrain);

    write_str(out, "#include \"");
    write_str(out, PRIVATE_HEADER);
    write_str(out, "\"\n");

    for (g_hash_table_iter_init(&iter_table, structure->dependencies);
         g_hash_table_iter_next(&iter_table, &key, &value);) {
        typeName = (gchar *)key;
        if (g_hash_table_contains(type2structure, typeName)) {
            parentStructure = g_hash_table_lookup(type2structure, typeName);
            upperCamel = g_strconcat(parentStructure->nameSpace, parentStructure->name, NULL);
            ownUpperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
            if (g_strcmp0(upperCamel, ownUpperCamel) == 0) {
                g_free(upperCamel);
                g_free(ownUpperCamel);
                continue;
            }
            lowerTrain = get_lower_train_from_upper_camel(upperCamel);
            g_free(upperCamel);
            (void)g_hash_table_insert(includeNames, lowerTrain, NULL);
            lowerTrain = NULL;
        }
    }

    for (g_hash_table_iter_init(&iter_table, includeNames);
         g_hash_table_iter_next(&iter_table, &key, &value);) {
        includeName = (gchar *)key;
        write_str(out, "#include \"");
        write_str(out, includeName);
        write_str(out, ".h\"\n");
    }

    write_str(out, "\n");
    g_hash_table_destroy(includeNames);
}

void generate_forward_declarations_header_file(GList *structures)
{
    FILE *in, *out;
    gint c, len;
    gchar buffer[BUFFER_SIZE];
    gchar *typeName;
    gchar *typeKind;
    GList *link;
    GList *typeNamesList = NULL;
    GHashTable *typeNames;
    GHashTableIter iter_table;
    gpointer key;
    gpointer value;

    g_return_if_fail(structures != NULL);

    in = open_file(templates_dir, HEADER_FORWARD_DECLARATIONS_TEMPLATE);
    if (!in)
        return;

    out = fopen(FORWARD_DECLARATIONS_HEADER, "wb");
    if (!out) {
        fclose(in);
        fprintf(stderr, "Failed to open '%s' for writing\n", FORWARD_DECLARATIONS_HEADER);
        return;
    }

    typeNames = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    for (link = structures; link; link = g_list_next (link)) {
        Structure *structure = link->data;

        if (!structure)
            continue;

        for (g_hash_table_iter_init(&iter_table, structure->dependencies);
             g_hash_table_iter_next(&iter_table, &key, &value);) {
            typeName = (gchar *)key;
            if (g_hash_table_contains(type2structure, typeName)) {
                typeKind = g_hash_table_lookup(type2kind, typeName);
                if (g_strcmp0(typeKind, "std") == 0 &&
                    !g_hash_table_contains(typeNames, typeName)) {
                    (void)g_hash_table_insert(typeNames, g_strdup (typeName), (gchar *) "std");
                }
            }
        }
    }

    *buffer = '\0';

    while ((c = fgetc(in)) != EOF) {
        if (c == '$') {
            if ((c = fgetc(in)) != '{' && c != '^') {
                printf("The following char is not {");
                fclose(in);
                fclose(out);
                return;
            }

            while ((c = fgetc(in)) != '}') {
                len = (gint)strlen(buffer);
                buffer[len] = c;
                buffer[len + 1] = '\0';
            }

            if (g_strcmp0(buffer, "forward_declarations") == 0) {
                g_hash_table_iter_init(&iter_table, typeNames);
                while (g_hash_table_iter_next(&iter_table, &key, &value)) {
                    typeNamesList = g_list_prepend(typeNamesList, g_strdup(key));
                }
                typeNamesList = g_list_sort(typeNamesList, (GCompareFunc)g_strcmp0);
                for (link = g_list_first(typeNamesList); link != NULL; link = g_list_next(link)) {
                    typeName = link->data;
                    write_str(out, "typedef struct _");
                    write_str(out, typeName);
                    write_str(out, " ");
                    write_str(out, typeName);
                    write_str(out, ";\n");
                }
                g_list_free_full(typeNamesList, g_free);
            } else if (g_strcmp0(buffer, "upperSnake") == 0) {
                write_str(out, "I_CAL_FORWARD_DECLARATIONS");
            } else {
                fprintf(stderr, "The string '%s' is not recognized, please check the %s\n",
                       buffer, FORWARD_DECLARATIONS_HEADER);
                fflush(stderr);
                break;
            }
            buffer[0] = '\0';
        } else {
            fputc(c, out);
        }
    }

    g_hash_table_destroy(typeNames);
    fclose(out);
    fclose(in);
}

void generate_source(FILE *out, Structure *structure, GHashTable *table)
{
    FILE *in;

    g_return_if_fail(out != NULL && structure != NULL && table != NULL);

    in = open_file(templates_dir, SOURCE_TEMPLATE);
    generate_code_from_template(in, out, structure, table);
    fclose(in);
}

void generate_source_structure_boilerplate(FILE *out, Structure *structure, GHashTable *table)
{
    FILE *in;

    g_return_if_fail(out != NULL && structure != NULL && table != NULL);

    in = open_file(templates_dir, SOURCE_STRUCTURE_BOILERPLATE_TEMPLATE);
    generate_code_from_template(in, out, structure, table);
    fclose(in);
}

GHashTable *get_hash_table_from_structure(Structure *structure)
{
    GHashTable *table;

    gchar *upperCamel;
    gchar *lowerSnake;
    gchar *upperSnake;
    gchar *lowerTrain;
    gchar *namespaceLowerSnake;
    gchar *nameLowerSnake;

    g_return_val_if_fail(structure != NULL, NULL);

    table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);
    (void)g_hash_table_insert(table, (char *)"commonHeader", g_strdup(COMMON_HEADER));
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    upperSnake = get_upper_snake_from_lower_snake(lowerSnake);
    lowerTrain = get_lower_train_from_lower_snake(lowerSnake);
    namespaceLowerSnake = get_upper_snake_from_upper_camel(structure->nameSpace);
    nameLowerSnake = get_upper_snake_from_upper_camel(structure->name);

    (void)g_hash_table_insert(table, (char *)"upperCamel", upperCamel);
    (void)g_hash_table_insert(table, (char *)"lowerSnake", lowerSnake);
    (void)g_hash_table_insert(table, (char *)"upperSnake", upperSnake);
    (void)g_hash_table_insert(table, (char *)"lowerTrain", lowerTrain);
    (void)g_hash_table_insert(table, (char *)"namespaceLowerSnake", namespaceLowerSnake);
    (void)g_hash_table_insert(table, (char *)"nameLowerSnake", nameLowerSnake);

    if (structure->native != NULL) {
        (void)g_hash_table_insert(table, (char *)"set_owner",
                                  get_source_method_proto_set_owner(structure));
        (void)g_hash_table_insert(table, (char *)"set_native",
                                  get_source_method_proto_set_native(structure));
        (void)g_hash_table_insert(table, (char *)"set_is_global",
                                  get_source_method_proto_set_is_global(structure));
        (void)g_hash_table_insert(table, (char *)"set_property",
                                  get_source_method_proto_set_property(structure));
        (void)g_hash_table_insert(table, (char *)"get_property",
                                  get_source_method_proto_get_property(structure));
        (void)g_hash_table_insert(table, (char *)"new_full",
                                  get_source_method_proto_new_full(structure));
        if (structure->destroyFunc != NULL) {
            (void)g_hash_table_insert(table, (char *)"destroyFunc",
                                      g_strdup(structure->destroyFunc));
        } else {
            (void)g_hash_table_insert(table, (char *)"destroyFunc", g_strdup((gchar *)"g_free"));
        }
        if (structure->cloneFunc != NULL) {
            (void)g_hash_table_insert(table, (gchar *)"cloneFunc", g_strdup(structure->cloneFunc));
        }
        if (structure->isBare) {
            (void)g_hash_table_insert(table, (gchar *)"defaultNative",
                                      g_strdup(structure->defaultNative));
        }
    }

    (void)g_hash_table_insert(table, (char *)"native", g_strdup(structure->native));

    return table;
}

void generate_conditional(FILE *out, Structure *structure, gchar *statement, GHashTable *table)
{
    guint iter;
    gboolean isNegate;
    gboolean isTrue;
    gchar *condition;
    gchar *expression;
    gint count;
    gint len;
    gchar c;
    gchar *var;
    gchar *val;
    guint statement_len;
    guint expression_len;

    g_return_if_fail(out != NULL && structure != NULL && statement != NULL && *statement != '\0');

    iter = 0;
    isNegate = FALSE;
    isTrue = FALSE;
    statement_len = (guint)strlen(statement);

    if (statement[0] == '!') {
        isNegate = TRUE;
        iter = 1;
    }
    g_return_if_fail(iter + 1 < statement_len && statement[iter++] == '$' &&
                     statement[iter++] == '{');

    condition = g_new(gchar, BUFFER_SIZE);
    *condition = '\0';
    expression = g_new(gchar, BUFFER_SIZE);
    *expression = '\0';
    var = g_new(gchar, BUFFER_SIZE);
    *var = '\0';

    while (iter < statement_len && statement[iter] != '}') {
        len = (gint)strlen(condition);
        condition[len] = statement[iter++];
        condition[len + 1] = '\0';
    }

    if (g_strcmp0(condition, "isBare") == 0) {
        isTrue = structure->isBare;
    } else if (g_strcmp0(condition, "isPossibleGlobal") == 0) {
        isTrue = structure->isPossibleGlobal;
    } else {
        printf("The condition variable of %s is not recognized\n", condition);
    }
    g_free(condition);

    (void)g_stpcpy(expression, statement + iter + 1);
    expression_len = (guint)strlen(expression);

    if ((isNegate && !isTrue) || (!isNegate && isTrue)) {
        for (iter = 0; iter < (guint)strlen(expression); iter++) {
            if (iter < expression_len - 1 &&
                expression[iter] == '$' &&
                expression[iter + 1] == '^') {
                gchar *buffer;

                iter += 2;
                count = 1;
                buffer = g_new(gchar, BUFFER_SIZE);
                *buffer = '\0';
                while (iter < expression_len) {
                    if (iter < expression_len - 1 && expression[iter] == '^' &&
                        expression[iter + 1] == '$') {
                        --count;
                    }

                    if (iter < expression_len - 1 && expression[iter] == '$' &&
                        expression[iter + 1] == '^') {
                        ++count;
                    }

                    if (count == 0) {
                        iter += 1;
                        generate_conditional(out, structure, buffer, table);
                        break;
                    }
                    len = (gint)strlen(buffer);
                    buffer[len] = expression[iter];
                    buffer[len + 1] = '\0';
                    ++iter;
                }
                g_free(buffer);
            } else {
                c = expression[iter];
                if (c == '$') {
                    if ((c = expression[++iter]) != '{') {
                        printf("The following char is not {");
                        g_free(expression);
                        g_free(var);
                        return;
                    }

                    while ((c = expression[++iter]) != '}') {
                        len = (gint)strlen(var);
                        var[len] = c;
                        var[len + 1] = '\0';
                    }

                    if (g_hash_table_contains(table, var)) {
                        val = g_hash_table_lookup(table, var);
                        write_str(out, val);
                        val = NULL;
                    } else {
                        printf("The string %s is not recognized in conditional, "
                               "please check the template\n", var);
                        g_free(expression);
                        g_free(var);
                        return;
                    }
                    var[0] = '\0';
                } else {
                    fputc(c, out);
                }
            }
        }
    }
    g_free(expression);
    g_free(var);
}

gchar *get_source_method_code(Method *method)
{
    gchar *buffer;
    gchar *ret;
    GList *iter;
    gchar *para;

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    (void)g_stpcpy(buffer + strlen(buffer), method->corresponds);
    (void)g_stpcpy(buffer + strlen(buffer), " ");

    if (method->parameters == NULL) {
        (void)g_stpcpy(buffer + strlen(buffer), "()");
    } else {
        for (iter = g_list_first(method->parameters); iter != NULL; iter = g_list_next(iter)) {
            if (iter == g_list_first(method->parameters)) {
                (void)g_stpcpy(buffer + strlen(buffer), "(");
            } else {
                (void)g_stpcpy(buffer + strlen(buffer), ", ");
            }
            para = get_inline_parameter((Parameter *)iter->data);
            (void)g_stpcpy(buffer + strlen(buffer), para);
            g_free(para);
        }
        (void)g_stpcpy(buffer + strlen(buffer), ")");
    }

    ret = g_malloc(strlen(buffer) + 1);
    (void)g_stpcpy(ret, buffer);
    g_free(buffer);

    return ret;
}

static gboolean is_enum_type(const gchar *type)
{
    gchar *trueType;
    const gchar *structureKind;
    gboolean res = FALSE;

    g_return_val_if_fail(type != NULL, FALSE);

    trueType = get_true_type(type);
    if (trueType && g_hash_table_contains(type2kind, trueType)) {
        structureKind = g_hash_table_lookup(type2kind, trueType);
        res = g_strcmp0(structureKind, "enum") == 0;
    }
    g_free (trueType);

    return res;
}

gchar *get_translator_for_parameter(Parameter *para)
{
    gchar *trueType;
    gchar *res;
    gchar *structureKind;
    gboolean is_bare;
    Structure *structure;

    g_return_val_if_fail(para != NULL, NULL);

    res = NULL;
    is_bare = FALSE;

    if (para->translator != NULL) {
        if (g_strcmp0(para->translator, (gchar *)"NONE") != 0)
            res = g_strdup(para->translator);
    } else {
        trueType = get_true_type(para->type);
        if (g_hash_table_contains(type2kind, trueType)) {
            structureKind = g_strdup(g_hash_table_lookup(type2kind, trueType));
            structure = g_hash_table_lookup(type2structure, trueType);
            if (structure == NULL) {
                printf("ERROR: There is no corresponding structure for type %s\n", trueType);
            } else {
                is_bare = structure->isBare;
            }
            if (!structure) {
                /* printed an error above, do nothing in this case */
            } else if (g_strcmp0(structureKind, "enum") == 0) {
                GList *link;

                for (link = structure->enumerations; link; link = g_list_next(link)) {
                    Enumeration *enumeration = link->data;

                    if (g_strcmp0 (trueType, enumeration->name) == 0) {
                        if (!enumeration->nativeName) {
                            g_printerr("Missing 'native_name' for enum %s\n", enumeration->name);
                            break;
                        }

                        res = g_strconcat("(", enumeration->nativeName, ")", NULL);
                        break;
                    }
                }
            } else {
                /* If the kind of parameter is specified */
                if (para->native_op != NULL) {
                    if (g_strcmp0(para->native_op, "OBJECT") == 0) {
                        res =
                            g_strconcat("* (", structure->native, " *)i_cal_object_get_native",
                                        NULL);
                    } else if (g_strcmp0(para->native_op, "POINTER") == 0) {
                        res =
                            g_strconcat("(", structure->native, " *)i_cal_object_get_native", NULL);
                    } else if (g_strcmp0(para->native_op, "STEAL") == 0) {
                        res =
                            g_strconcat("(", structure->native, " *)i_cal_object_steal_native",
                                        NULL);
                    } else {
                        printf("The parameter kind \"%s\" is illegal!", para->native_op);
                        fflush(NULL);
                    }
                } else {
                    if (is_bare) {
                        res =
                            g_strconcat("* (", structure->native, " *)i_cal_object_get_native",
                                        NULL);
                    } else {
                        res =
                            g_strconcat("(", structure->native, " *)i_cal_object_get_native", NULL);
                    }
                }
            }
            g_free(structureKind);
        }
        g_free(trueType);
    }

    return res;
}

gchar *get_translator_for_return(Ret *ret)
{
    gchar *trueType;
    gchar *lowerSnake;
    gchar *res;
    gchar *kind;

    g_return_val_if_fail(ret != NULL, NULL);

    res = NULL;

    if (ret->translator != NULL) {
        if (g_strcmp0(ret->translator, (gchar *)"NONE") != 0)
            res = g_strdup(ret->translator);
    } else {
        trueType = get_true_type(ret->type);
        if (g_hash_table_contains(type2kind, trueType)) {
            Structure *structure;

            kind = g_strdup(g_hash_table_lookup(type2kind, trueType));
            structure = g_hash_table_lookup(type2structure, trueType);
            if (!structure) {
                printf("ERROR: There is no corresponding structure for type %s\n", trueType);
            }

            if (structure && g_strcmp0(kind, "enum") == 0) {
                GList *link;

                for (link = structure->enumerations; link; link = g_list_next(link)) {
                    Enumeration *enumeration = link->data;

                    if (g_strcmp0 (trueType, enumeration->name) == 0) {
                        if (!enumeration->nativeName) {
                            g_printerr("Missing 'native_name' for enum %s\n", enumeration->name);
                            break;
                        }

                        res = g_strconcat("(", enumeration->name, ")", NULL);
                        break;
                    }
                }
            } else {
                lowerSnake = get_lower_snake_from_upper_camel(trueType);
                res = g_strconcat(lowerSnake, "_new_full", NULL);
                g_free(lowerSnake);
            }
            g_free(kind);
        }
        g_free(trueType);
    }

    return res;
}

gchar *get_inline_parameter(Parameter *para)
{
    gchar *buffer;
    gchar *ret;
    gchar *translator;

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';
    translator = get_translator_for_parameter(para);

    if (para->native_op != NULL && translator == NULL) {
        g_error("No translator is found for parameter %s "
                "with type %s "
                "but native_op %s is supplied\n",
                para->name, para->type, para->native_op);
    }

    if (translator != NULL) {
        (void)g_stpcpy(buffer + strlen(buffer), translator);
        (void)g_stpcpy(buffer + strlen(buffer), " (");
        if (para->translator == NULL && !is_enum_type(para->type))
            (void)g_stpcpy(buffer + strlen(buffer), "I_CAL_OBJECT (");
    }

    (void)g_stpcpy(buffer + strlen(buffer), para->name);

    if (translator != NULL) {
        if (para->translator == NULL && !is_enum_type(para->type)) {
            (void)g_stpcpy(buffer + strlen(buffer), ")");
        }
        (void)g_stpcpy(buffer + strlen(buffer), ")");
    }

    /*
       if (translator != NULL) {
       if (para->translatorArgus != NULL) {
       for (iter = g_list_first (para->translatorArgus); iter != NULL; iter = g_list_next (iter)) {
       (void)g_stpcpy (buffer + strlen (buffer), ", ");
       (void)g_stpcpy (buffer + strlen (buffer), (gchar *)iter->data);
       }
       }

       else if (para->translator == NULL) {
       trueType = get_true_type (para->type);
       if (g_hash_table_contains (type2structure, trueType)) {
       structure = g_hash_table_lookup (type2structure, trueType);
       if (structure->isBare == FALSE) {
       (void)g_stpcpy (buffer + strlen (buffer), ", NULL");
       }
       }
       g_free (trueType);
       }
       (void)g_stpcpy (buffer + strlen (buffer), ")");
       g_free (translator);
       }
     */
    ret = g_new(gchar, strlen(buffer) + 1);
    (void)g_stpcpy(ret, buffer);
    g_free(buffer);
    buffer = NULL;

    return ret;
}

gchar *get_source_method_body(Method *method, const gchar *nameSpace)
{
    gchar *buffer;
    gchar *ret;
    gchar *body;
    gchar *proto;
    gchar *comment;
    GList *iter;
    gchar *checkers;
    gchar *translator;
    gchar *trueType;
    Structure *structure;
    Parameter *parameter;

    buffer = g_new(gchar, BUFFER_SIZE);
    buffer[0] = '\0';
    translator = NULL;

    comment = get_source_method_comment(method);
    (void)g_stpcpy(buffer + strlen(buffer), comment);
    g_free(comment);

    (void)g_stpcpy(buffer + strlen(buffer), "\n");

    proto = get_source_method_proto(method);
    (void)g_stpcpy(buffer + strlen(buffer), proto);
    g_free(proto);

    (void)g_stpcpy(buffer + strlen(buffer), "\n{\n");

    if (g_strcmp0(method->corresponds, (gchar *)"CUSTOM") != 0) {
        checkers = get_source_run_time_checkers(method, nameSpace);
        if (checkers != NULL) {
            (void)g_stpcpy(buffer + strlen(buffer), checkers);
            g_free(checkers);
        }

        /* op on the owner */
        /* TODO: Change the translatorArgus in Parameter to parent */
        for (iter = g_list_first(method->parameters); iter != NULL; iter = g_list_next(iter)) {
            parameter = (Parameter *)iter->data;
            if (parameter->owner_op != NULL) {
                if (g_strcmp0(parameter->owner_op, "REMOVE") == 0) {
                    (void)g_stpcpy(buffer + strlen(buffer),
                                   "\ti_cal_object_remove_owner (I_CAL_OBJECT (");
                    (void)g_stpcpy(buffer + strlen(buffer), parameter->name);
                    (void)g_stpcpy(buffer + strlen(buffer), "));\n");
                } else {
                    (void)g_stpcpy(buffer + strlen(buffer),
                                   "\ti_cal_object_set_owner ((ICalObject *)");
                    (void)g_stpcpy(buffer + strlen(buffer), parameter->name);
                    (void)g_stpcpy(buffer + strlen(buffer), ", (GObject *)");
                    (void)g_stpcpy(buffer + strlen(buffer), (gchar *)parameter->owner_op);
                    (void)g_stpcpy(buffer + strlen(buffer), ");\n");
                }
            } else if (parameter->translatorArgus != NULL) {
                (void)g_stpcpy(buffer + strlen(buffer), "\ti_cal_object_set_owner ((ICalObject *)");
                (void)g_stpcpy(buffer + strlen(buffer), parameter->name);
                (void)g_stpcpy(buffer + strlen(buffer), ", (GObject *)");
                (void)g_stpcpy(buffer + strlen(buffer), (gchar *)parameter->translatorArgus->data);
                (void)g_stpcpy(buffer + strlen(buffer), ");\n");
            }
        }

        (void)g_stpcpy(buffer + strlen(buffer), "\t");
        if (method->ret != NULL) {
            (void)g_stpcpy(buffer + strlen(buffer), "return ");
            translator = get_translator_for_return(method->ret);
            if (translator != NULL) {
                (void)g_stpcpy(buffer + strlen(buffer), translator);
                (void)g_stpcpy(buffer + strlen(buffer), " (");
            }
        }

        body = get_source_method_code(method);
        (void)g_stpcpy(buffer + strlen(buffer), body);
        g_free(body);

        if (method->ret != NULL && translator != NULL) {
            if (method->ret->translatorArgus != NULL) {
                if (g_strcmp0((gchar *)method->ret->translatorArgus->data, "NONE") != 0) {
                    for (iter = g_list_first(method->ret->translatorArgus); iter != NULL;
                         iter = g_list_next(iter)) {
                        (void)g_stpcpy(buffer + strlen(buffer), ", ");
                        (void)g_stpcpy(buffer + strlen(buffer), (gchar *)iter->data);
                    }
                }
            } else {
                trueType = get_true_type(method->ret->type);
                if (g_hash_table_contains(type2structure, trueType)) {
                    structure = g_hash_table_lookup(type2structure, trueType);
                    if (!structure->isBare && !is_enum_type(method->ret->type)) {
                        (void)g_stpcpy(buffer + strlen(buffer), ", NULL");
                    }
                }
                g_free(trueType);
                /*
                  if (g_strcmp0 (g_hash_table_lookup (allTypes, method->ret->type), "true") == 0) {
                      (void)g_stpcpy (buffer + strlen (buffer), ", ");
                      (void)g_stpcpy (buffer + strlen (buffer), "FALSE");
                  }
                **/
            }
            (void)g_stpcpy(buffer + strlen(buffer), ")");
            g_free(translator);
        }
        (void)g_stpcpy(buffer + strlen(buffer), ";");
    } else if (method->custom != NULL) {
        (void)g_stpcpy(buffer + strlen(buffer), method->custom);
    } else {
        printf("WARNING: No function body for the method: %s\n", method->name);
    }
    (void)g_stpcpy(buffer + strlen(buffer), "\n}");

    ret = g_new(gchar, strlen(buffer) + 1);
    (void)g_stpcpy(ret, buffer);
    g_free(buffer);

    return ret;
}

gchar *get_source_method_proto(Method *method)
{
    gchar *buffer;
    GList *iter_list;
    Parameter *para;
    gchar *ret;
    gint paddingLength;
    gchar *padding;
    gint iter;

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    /* Generate the method return */
    if (method->ret == NULL) {
        (void)g_stpcpy(buffer + strlen(buffer), "void");
    } else {
        (void)g_stpcpy(buffer + strlen(buffer), method->ret->type);
    }
    (void)g_stpcpy(buffer + strlen(buffer), "\n");

    /* Generate the method name */
    (void)g_stpcpy(buffer + strlen(buffer), method->name);

    paddingLength = (gint)strlen(method->name) + 2;
    padding = g_new(gchar, (gsize)(paddingLength + 1));
    for (iter = 0; iter < paddingLength; iter++) {
        padding[iter] = ' ';
    }
    padding[iter] = '\0';

    /* Generate all the parameters */
    if (method->parameters == NULL) {
        (void)g_stpcpy(buffer + strlen(buffer), " (void)");
    } else {
        for (iter_list = g_list_first(method->parameters); iter_list != NULL;
             iter_list = g_list_next(iter_list)) {
            para = (Parameter *)iter_list->data;
            if (iter_list == g_list_first(method->parameters)) {
                (void)g_stpcpy(buffer + strlen(buffer), " (");
            } else {
                (void)g_stpcpy(buffer + strlen(buffer), ", \n");
                (void)g_stpcpy(buffer + strlen(buffer), padding);
            }
            (void)g_stpcpy(buffer + strlen(buffer), para->type);
            if (para->type[strlen(para->type) - 1] != '*') {
                (void)g_stpcpy(buffer + strlen(buffer), " ");
            }
            (void)g_stpcpy(buffer + strlen(buffer), para->name);
        }
        (void)g_stpcpy(buffer + strlen(buffer), ")");
    }

    ret = g_malloc(strlen(buffer) + 1);
    (void)g_stpcpy(ret, buffer);
    g_free(buffer);
    g_free(padding);

    return ret;
}

gchar *get_translator_name_for_return(gchar *upperCamel)
{
    gchar *lowerSnake;
    gchar *res;

    g_return_val_if_fail(upperCamel != NULL, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    res = g_strconcat(lowerSnake, "_new_full", NULL);
    g_free(lowerSnake);
    return res;
}

gchar *get_true_type(const gchar *type)
{
    guint i;
    guint start;
    guint end;
    gchar *res;
    const gchar *const_prefix = "const";
    const guint const_prefix_len = (guint)strlen(const_prefix);
    guint type_len;

    g_return_val_if_fail(type != NULL && *type != '\0', NULL);

    type_len = (guint)strlen(type);
    end = type_len - 1;

    for (i = 0; i < const_prefix_len && i < type_len && const_prefix[i] == type[i]; i++);

    if (i == const_prefix_len) {
        start = i + 1;
    } else {
        start = 0;
    }

    if (type[type_len - 1] == '*') {
        end = type_len - 3;
    }

    res = g_new(gchar, end - start + 2);
    for (i = start; i <= end; i++) {
        res[i - start] = type[i];
    }
    res[end - start + 1] = '\0';
    return res;
}

static void initialize_default_value_table()
{
    defaultValues = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    (void)g_hash_table_insert(defaultValues, g_strdup("gboolean"), g_strdup("FALSE"));
    (void)g_hash_table_insert(defaultValues, g_strdup("gdouble"), g_strdup("0"));
    (void)g_hash_table_insert(defaultValues, g_strdup("gint"), g_strdup("0"));
    (void)g_hash_table_insert(defaultValues, g_strdup("gpointer"), g_strdup("NULL"));
    (void)g_hash_table_insert(defaultValues, g_strdup("time_t"), g_strdup("0"));
}

void generate_header_and_source(Structure *structure, gchar *dir)
{
    gchar *headerName;
    gchar *sourceName;
    gchar *upperCamel;
    gchar *lowerTrain;
    FILE *header;
    FILE *source;
    GHashTable *table;

    g_return_if_fail(structure != NULL);

    headerName = g_new(gchar, BUFFER_SIZE);
    *headerName = '\0';
    sourceName = g_new(gchar, BUFFER_SIZE);
    *sourceName = '\0';

    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerTrain = get_lower_train_from_upper_camel(upperCamel);
    g_free(upperCamel);

    (void)g_stpcpy(headerName + strlen(headerName), dir);
    (void)g_stpcpy(headerName + strlen(headerName), lowerTrain);
    (void)g_stpcpy(headerName + strlen(headerName), ".h");
    (void)g_stpcpy(sourceName + strlen(sourceName), dir);
    (void)g_stpcpy(sourceName + strlen(sourceName), lowerTrain);
    (void)g_stpcpy(sourceName + strlen(sourceName), ".c");

    header = fopen(headerName, "w");
    source = fopen(sourceName, "w");

    table = get_hash_table_from_structure(structure);
    generate_header(header, structure, table);
    generate_source(source, structure, table);

    if (header) {
        fclose(header);
    }
    if (source) {
        fclose(source);
    }
    g_hash_table_destroy(table);
}

void generate_header_enums(FILE *out, Structure *structure)
{
    GList *iter;
    Enumeration *enumeration;

    g_return_if_fail(out != NULL && structure != NULL);

    for (iter = g_list_first(structure->enumerations); iter != NULL; iter = g_list_next(iter)) {
        enumeration = (Enumeration *)iter->data;

        generate_header_enum(out, enumeration);
        write_str(out, "\n");
    }
}

void generate_header_enum(FILE *out, Enumeration *enumeration)
{
    GList *iter;
    gchar *nativeName;
    guint i;
    gchar *newName;
    gchar *comment;
    gchar *tmp;
    const guint enum_header_len = (guint)strlen(ENUM_HEADER);

    g_return_if_fail(out != NULL && enumeration != NULL);

    if (enumeration->defaultNative != NULL) {
        (void)g_hash_table_insert(defaultValues, g_strdup(enumeration->name),
                                  g_strdup(enumeration->defaultNative));
    } else {
        g_error("Please supply a default value for enum type %s by default_native\n",
                enumeration->name);
    }

    /* Generate the comment block */
    if (enumeration->comment != NULL) {
        //krazy:cond=style
        comment = g_strdup("/**");
        tmp = g_strconcat(comment, "\n * ", enumeration->name, ":", NULL);
        g_free(comment);
        comment = tmp;

        tmp = g_strconcat(comment, "\n * ", enumeration->comment, NULL);
        g_free(comment);
        comment = tmp;

        tmp = g_strconcat(comment, "\n */\n", NULL);
        g_free(comment);
        comment = tmp;
        //krazy:endcond=style
        write_str(out, comment);
        g_free(comment);
        comment = NULL;
        tmp = NULL;
    }

    /* Generate the declaration */
    write_str(out, "typedef enum {");

    for (iter = g_list_first(enumeration->elements); iter != NULL; iter = g_list_next(iter)) {
        nativeName = (gchar *)iter->data;
        if (iter != g_list_first(enumeration->elements)) {
            write_str(out, ",");
        }
        if (enum_header_len >= (guint)strlen(nativeName)) {
            printf("The enum name %s is longer than the enum header %s\n", nativeName, ENUM_HEADER);
            continue;
        }
        for (i = 0; i < enum_header_len; i++) {
            if (ENUM_HEADER[i] != nativeName[i]) {
                break;
            }
        }
        if (i != enum_header_len) {
            printf("The enum name %s cannot be processed\n", nativeName);
            continue;
        }
        if (nativeName[i] == '_') {
            newName = g_strconcat("I_CAL", nativeName + i, NULL);
        } else {
            newName = g_strconcat("I_CAL_", nativeName + i, NULL);
        }

        write_str(out, "\n\t");
        write_str(out, newName);
        write_str(out, " = ");
        write_str(out, nativeName);

        g_free(newName);
    }

    write_str(out, "\n} ");
    write_str(out, enumeration->name);
    write_str(out, ";\n");
}

gchar *get_source_run_time_checkers(Method *method, const gchar *namespace)
{
    GList *iter;
    GList *jter;
    Parameter *parameter;
    guint i;
    gchar *buffer;
    gchar *nameSpaceUpperSnake;
    gchar *nameUpperSnake;
    gchar *typeCheck;
    gchar *trueType;
    gchar *res;
    gchar *defaultValue;
    gchar *retTrueType;
    guint namespace_len;

    g_return_val_if_fail(method != NULL, NULL);
    g_return_val_if_fail(namespace != NULL && *namespace != '\0', NULL);

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';
    res = NULL;
    namespace_len = (guint)strlen(namespace);

    for (iter = g_list_first(method->parameters); iter != NULL; iter = g_list_next(iter)) {
        parameter = (Parameter *)iter->data;

        if (parameter && parameter->type && parameter->type[strlen(parameter->type) - 1] == '*') {
            trueType = get_true_type(parameter->type);
            for (i = 0;
                 i < namespace_len && trueType[i] && namespace[i] == trueType[i];
                 i++);

            if (i == namespace_len) {
                (void)g_stpcpy(buffer + strlen(buffer), "\t");
                nameSpaceUpperSnake = get_upper_snake_from_upper_camel(namespace);
                nameUpperSnake = get_upper_snake_from_upper_camel(trueType + i);
                typeCheck =
                    g_strconcat(nameSpaceUpperSnake, "_IS_", nameUpperSnake, " (", parameter->name,
                                ")", NULL);
                defaultValue = NULL;
                if (method->ret != NULL) {
                    retTrueType = get_true_type(method->ret->type);
                    (void)g_stpcpy(buffer + strlen(buffer), "g_return_val_if_fail (");
                    (void)g_stpcpy(buffer + strlen(buffer), typeCheck);
                    (void)g_stpcpy(buffer + strlen(buffer), ", ");
                    if (method->ret->errorReturnValue != NULL) {
                        defaultValue = g_strdup(method->ret->errorReturnValue);
                    } else if (method->ret->type[strlen(method->ret->type) - 1] == '*') {
                        defaultValue = g_strdup("NULL");
                    } else if (g_hash_table_contains(defaultValues, retTrueType)) {
                        defaultValue = g_strdup(g_hash_table_lookup(defaultValues, retTrueType));
                    } else {
                        printf("No default value provided for the return type %s in method %s\n",
                               method->ret->type, method->name);
                        defaultValue = g_strdup("NULL");
                    }
                    g_free(retTrueType);
                    (void)g_stpcpy(buffer + strlen(buffer), defaultValue);
                    g_free(defaultValue);
                    (void)g_stpcpy(buffer + strlen(buffer), ");");
                } else {
                    (void)g_stpcpy(buffer + strlen(buffer), "g_return_if_fail (");
                    (void)g_stpcpy(buffer + strlen(buffer), typeCheck);
                    (void)g_stpcpy(buffer + strlen(buffer), ");");
                }
                g_free(nameSpaceUpperSnake);
                g_free(nameUpperSnake);
                g_free(typeCheck);
                (void)g_stpcpy(buffer + strlen(buffer), "\n");
            }

            for (jter = g_list_first(parameter->annotations); jter != NULL;
                 jter = g_list_next(jter)) {
                if (g_strcmp0((gchar *)jter->data, "allow-none") == 0) {
                    break;
                }
            }

            if (jter == NULL) {
                (void)g_stpcpy(buffer + strlen(buffer), "\t");
                if (method->ret != NULL) {
                    (void)g_stpcpy(buffer + strlen(buffer), "g_return_val_if_fail (");
                    (void)g_stpcpy(buffer + strlen(buffer), parameter->name);
                    (void)g_stpcpy(buffer + strlen(buffer), " != NULL, ");
                    defaultValue = NULL;
                    if (method->ret->errorReturnValue != NULL) {
                        defaultValue = g_strdup(method->ret->errorReturnValue);
                    } else if (method->ret->type[strlen(method->ret->type) - 1] == '*') {
                        defaultValue = g_strdup("NULL");
                    } else if (g_hash_table_contains(defaultValues,
                                   (retTrueType = get_true_type(method->ret->type)))) {
                        defaultValue =
                            g_strdup(g_hash_table_lookup(defaultValues, retTrueType));
                        g_free(retTrueType);
                    } else {
                        printf("No default value provided for the return type %s in method %s\n",
                               method->ret->type, method->name);
                        defaultValue = g_strdup("NULL");
                    }
                    (void)g_stpcpy(buffer + strlen(buffer), defaultValue);
                    g_free(defaultValue);
                    (void)g_stpcpy(buffer + strlen(buffer), ");");
                } else {
                    (void)g_stpcpy(buffer + strlen(buffer), "g_return_if_fail (");
                    (void)g_stpcpy(buffer + strlen(buffer), parameter->name);
                    (void)g_stpcpy(buffer + strlen(buffer), " != NULL");
                    (void)g_stpcpy(buffer + strlen(buffer), ");");
                }
                (void)g_stpcpy(buffer + strlen(buffer), "\n");
            }
            g_free(trueType);
        }
    }

    if (strlen(buffer) > 0) {
        res = g_strconcat(buffer, "\n", NULL);
    }
    g_free(buffer);
    return res;
}

static gint generate_library(const gchar *apis_dir)
{
    xmlDoc *doc;
    xmlNode *node;
    Structure *structure;
    gchar *path;
    const gchar *filename;
    Enumeration *enumeration;
    gchar *buffer;
    GList *structures;
    GList *iter_list;
    GList *filenames = NULL;
    GList *iter_filenames;
    GDir *dir;
    GError *local_error = NULL;
    gint res = 0;
    gint len;

    g_return_val_if_fail(apis_dir != NULL, 1);
    g_return_val_if_fail(g_file_test(apis_dir, G_FILE_TEST_IS_DIR), 1);

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    /* Cache the type and its kind, like ICalComponnet--->std or ICalPropertyKind--->enum */
    type2kind = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    /* Cache the type and the structure where it is defined,
       like ICalComponent--->Structure_storing_ICalComponent */
    type2structure = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    initialize_default_value_table();
    structures = NULL;

    dir = g_dir_open(apis_dir, 0, &local_error);
    if (!dir) {
        g_warning("Failed to open API directory '%s': %s", apis_dir,
                  local_error ? local_error->message : "Unknown error");
        g_clear_error(&local_error);
        g_free(buffer);
        return 1;
    }

    /* Parse the all the XML files into the Structure */
    while (filename = g_dir_read_name(dir), filename) {
        filenames = g_list_prepend(filenames, g_strdup(filename));
    }
    filenames = g_list_sort(filenames, (GCompareFunc)g_strcmp0);
    for (iter_filenames = g_list_first(filenames); iter_filenames != NULL;
        iter_filenames = g_list_next(iter_filenames)) {
        filename = iter_filenames->data;
        len = (gint)strlen(filename);

        if (len <= 4 || g_ascii_strncasecmp(filename + len - 4, ".xml", 4) != 0)
            continue;

        path = g_build_filename(apis_dir, filename, NULL);
        doc = xmlParseFile(path);
        if (doc == NULL) {
            printf("The doc %s cannot be parsed.\n", path);
            g_free(path);
            res = 1;
            goto out;
        }

        g_free(path);

        node = xmlDocGetRootElement(doc);
        if (node == NULL) {
            printf("The root node cannot be retrieved from the doc\n");
            xmlFreeDoc(doc);
            res = 1;
            goto out;
        }

        structure = structure_new();
        if (!parse_structure(node, structure)) {
            printf("The node cannot be parsed into a structure.\n");
            xmlFreeDoc(doc);
            res = 1;
            goto out;
        }

        if (structure->native != NULL) {
            (void)g_hash_table_insert(type2kind,
                                      g_strconcat(structure->nameSpace, structure->name, NULL),
                                      (void *)"std");
            (void)g_hash_table_insert(type2structure,
                                      g_strconcat(structure->nameSpace, structure->name, NULL),
                                      structure);
            if (structure->isBare) {
                if (structure->defaultNative != NULL) {
                    (void)g_hash_table_insert(defaultValues,
                                              g_strconcat(structure->nameSpace, structure->name,
                                                          NULL),
                                              g_strdup(structure->defaultNative));
                } else {
                    printf("Please supply a default value for the bare structure %s\n",
                           structure->name);
                    xmlFreeDoc(doc);
                    res = 1;
                    goto out;
                }
            }
        }

        for (iter_list = g_list_first(structure->enumerations); iter_list != NULL;
             iter_list = g_list_next(iter_list)) {
            enumeration = (Enumeration *)iter_list->data;
            (void)g_hash_table_insert(type2kind, g_strdup(enumeration->name), (void *)"enum");
            (void)g_hash_table_insert(type2structure, g_strdup(enumeration->name), structure);

            if (enumeration->defaultNative != NULL) {
                (void)g_hash_table_insert(defaultValues, g_strdup(enumeration->name),
                                          g_strdup(enumeration->defaultNative));
            } else {
                printf("Please supply a default value for enum %s\n", enumeration->name);
                xmlFreeDoc(doc);
                res = 1;
                goto out;
            }
        }
        structures = g_list_append(structures, structure);
        xmlFreeDoc(doc);
    }

    /* Generate the forward declarations header file */
    generate_forward_declarations_header_file(structures);

    /* Generate the common header for all the headers, which is libical-glib.h for here */
    generate_header_header_file(structures);

    /* Generate all the header and source files for each structure */
    for (iter_list = g_list_first(structures); iter_list != NULL;
         iter_list = g_list_next(iter_list)) {
        structure = (Structure *)iter_list->data;
        generate_header_and_source(structure, (char *)"");
    }

 out:
    g_dir_close(dir);
    g_hash_table_destroy(type2kind);
    g_hash_table_destroy(type2structure);
    g_hash_table_destroy(defaultValues);
    g_list_free_full(structures, (GDestroyNotify)structure_free);
    g_list_free_full(filenames, g_free);
    g_free(buffer);

    return res;
}

void generate_header_header_declaration(FILE *out, Structure *structure)
{
    GList *list_iter;
    Declaration *declaration;

    g_return_if_fail(out != NULL && structure != NULL);

    for (list_iter = g_list_first(structure->declarations); list_iter != NULL;
         list_iter = g_list_next(list_iter)) {
        declaration = (Declaration *)list_iter->data;

        if (g_strcmp0(declaration->position, "header") == 0) {
            write_str(out, declaration->content);
            write_str(out, "\n");
        }
        declaration = NULL;
    }
}

void generate_header_header_file(GList *structures)
{
    FILE *out;
    FILE *in;
    gint c;
    gchar *buffer;
    GList *iter;
    gint len;
    gchar *header;
    gchar *upperCamel;
    gchar *lowerTrain;
    Structure *structure;

    g_return_if_fail(structures != NULL);

    in = open_file(templates_dir, HEADER_HEADER_TEMPLATE);
    if (!in)
        return;

    out = fopen("libical-glib.h", "w");
    if (!out) {
        fprintf(stderr, "Failed to open libical-glib.h for writing\n");
        fclose(in);
        return;
    }

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    while ((c = fgetc(in)) != EOF) {
        if (c == '$') {
            if ((c = fgetc(in)) != '{' && c != '^') {
                printf("The following char is not {");
                g_free (buffer);
                fclose(in);
                fclose(out);
                return;
            }

            while ((c = fgetc(in)) != '}') {
                len = (gint)strlen(buffer);
                buffer[len] = c;
                buffer[len + 1] = '\0';
            }

            if (g_strcmp0(buffer, "allHeaders") == 0) {
                for (iter = g_list_first(structures); iter != NULL; iter = g_list_next(iter)) {
                    structure = (Structure *)iter->data;
                    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
                    lowerTrain = get_lower_train_from_upper_camel(upperCamel);
                    header = g_strconcat("#include <libical-glib/", lowerTrain, ".h>\n", NULL);
                    write_str(out, header);
                    g_free(header);
                    g_free(upperCamel);
                    g_free(lowerTrain);
                    structure = NULL;
                }
            } else {
                printf("The string %s is not recognized, please check the header-header-template\n",
                       buffer);
                fflush(NULL);
                g_free (buffer);
                fclose(in);
                fclose(out);
                return;
            }
            buffer[0] = '\0';
        } else {
            fputc(c, out);
        }
    }

    fclose(in);
    fclose(out);
    g_free(buffer);
}

int main(int argc, char *argv[])
{
    const gchar *apis_dir;
    gint res;

    if (argc != 3) {
        fprintf(stderr,
                "Requires two arguments, the first is path to templates, "
                "the second is a path to XML files with an API description\n");
        return 1;
    }

    /* The directory to search for templates */
    templates_dir = argv[1];
    /* The directory to search for XML files */
    apis_dir = argv[2];

    res = generate_library(apis_dir);
    close_private_header();

    return res;
}
