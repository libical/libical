/*
 * SPDX-FileCopyrightText: 2015 William Yu <williamyu@gnome.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "header-parser.h"

#include "generator.h"

#include <errno.h>

static const gchar *templates_dir = NULL;

static FILE *open_file(const gchar *dir, const gchar *filename)
{
    gchar *path;
    FILE *tmpl = NULL;

    path = g_build_filename(dir, filename, NULL);
    if (path) {
        tmpl = fopen(path, "rb");
        if (!tmpl) {
            fprintf(stderr, "generator: Failed to open %s: %s\n", path, strerror(errno));
        }

        g_free(path);
    }
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
            if (para) {
                comment_len = 0;
                if (para->comment) {
                    comment_len = (guint)strlen(para->comment);
                }

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
    if (method->since != NULL && *method->since != '\0') {
        buffer = g_strconcat(res, "\n *\n * Since: ", method->since, "\n *", NULL);
        g_free(res);
        res = buffer;
    }

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
    memset(ret, 0, BUFFER_SIZE);

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
        if (isupper((int)upperCamel[i]) && i != 0) {
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
    len = (guint)strlen(lowerSnake);

    ret = g_strdup(lowerSnake);
    for (i = 0; i < len; i++) {
        if (lowerSnake[i] == '_') {
            ret[i] = '-';
        }
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
        if (ret[i] == '_') {
            ret[i] = '-';
        }
    }

    return ret;
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

    if (fwrite(str, sizeof(gchar), len, fp) != len) {
        g_error("Failed to write %d bytes to file: %s", (gint)len, g_strerror(errno));
    }
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

    if (structure->native != NULL) {
        /** Open or create the private header file if it does not exist.
         * Create the forward declaration.
         * Create the new_full method in it.
         */
        privateHeader = open_private_header();
        typeName = g_strconcat(structure->nameSpace, structure->name, NULL);
        privateHeaderComment = g_strconcat("\n/* Private methods for ", typeName, " */\n", NULL);
        g_free(typeName);
        write_str(privateHeader, privateHeaderComment);
        g_free(privateHeaderComment);
        generate_header_method_new_full(privateHeader, structure);
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

    while (!feof(in) && !ferror(in) && (c = fgetc(in)) != EOF) {
        if (c == '$') {
            if ((c = fgetc(in)) != '{' && c != '^') {
                printf("The following char is not {");
                g_free(buffer);
                return;
            }

            if (c == '^') {
                count = 1;
                last = '\0';
                while (!feof(in) && !ferror(in) && !((c = fgetc(in)) == '$' && last == '^' && count == 1)) {
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
                while (!feof(in) && !ferror(in) && (c = fgetc(in)) != '}') {
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
                } else if (g_strcmp0(buffer, "body_declaration") == 0) {
                    generate_source_body_declaration(out, structure);
                } else if (g_hash_table_contains(table, buffer)) {
                    val = g_hash_table_lookup(table, buffer);
                    write_str(out, val);
                    val = NULL;

                    if (g_strcmp0(buffer, "new_full_extraCode") == 0) {
                        write_str(out, "\n    ");
                    }
                } else if (g_strcmp0(buffer, "new_full_extraCode") == 0) {
                    /* For simplicity, after lookup in the 'table', to
                       not force declaration of it in every .xml file */
                } else if (g_strcmp0(buffer, "structure_boilerplate") == 0) {
                    if (structure->native != NULL) {
                        generate_header_structure_boilerplate(out, structure, table);
                    }
                } else if (g_hash_table_contains(table, buffer)) {
                    val = g_hash_table_lookup(table, buffer);
                    write_str(out, val);
                    val = NULL;
                } else if (g_strcmp0(buffer, "source_boilerplate") == 0) {
                    if (structure->native != NULL) {
                        generate_source_structure_boilerplate(out, structure, table);
                    }
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

            g_free(ownUpperCamel);
            kind = g_strdup(g_hash_table_lookup(type2kind, typeName));
            if (g_strcmp0(kind, "enum") != 0) {
                g_free(kind);
                g_free(upperCamel);
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
            g_free(ownUpperCamel);
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
    if (!in) {
        return;
    }

    out = fopen(FORWARD_DECLARATIONS_HEADER, "wb");
    if (!out) {
        fclose(in);
        fprintf(stderr, "Failed to open '%s' for writing\n", FORWARD_DECLARATIONS_HEADER);
        return;
    }

    typeNames = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    for (link = structures; link; link = g_list_next(link)) {
        Structure *structure = link->data;

        if (!structure) {
            continue;
        }

        for (g_hash_table_iter_init(&iter_table, structure->dependencies);
             g_hash_table_iter_next(&iter_table, &key, &value);) {
            typeName = (gchar *)key;
            if (g_hash_table_contains(type2structure, typeName)) {
                typeKind = g_hash_table_lookup(type2kind, typeName);
                if (g_strcmp0(typeKind, "std") == 0 &&
                    !g_hash_table_contains(typeNames, typeName)) {
                    (void)g_hash_table_insert(typeNames, g_strdup(typeName), (gchar *)"std");
                }
            }
        }
    }

    *buffer = '\0';

    while (!feof(in) && !ferror(in) && (c = fgetc(in)) != EOF) {
        if (c == '$') {
            if (!feof(in) && !ferror(in) && (c = fgetc(in)) != '{' && c != '^') {
                printf("The following char is not {");
                fclose(in);
                fclose(out);
                return;
            }

            while (!feof(in) && !ferror(in) && (c = fgetc(in)) != '}') {
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
    gchar *nameSpaceLowerSnake;
    gchar *nameLowerSnake;

    g_return_val_if_fail(structure != NULL, NULL);

    table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);
    (void)g_hash_table_insert(table, (char *)"commonHeader", g_strdup(COMMON_HEADER));
    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerSnake = get_lower_snake_from_upper_camel(upperCamel);
    upperSnake = get_upper_snake_from_lower_snake(lowerSnake);
    lowerTrain = get_lower_train_from_lower_snake(lowerSnake);
    nameSpaceLowerSnake = get_upper_snake_from_upper_camel(structure->nameSpace);
    nameLowerSnake = get_upper_snake_from_upper_camel(structure->name);

    (void)g_hash_table_insert(table, (char *)"upperCamel", upperCamel);
    (void)g_hash_table_insert(table, (char *)"lowerSnake", lowerSnake);
    (void)g_hash_table_insert(table, (char *)"upperSnake", upperSnake);
    (void)g_hash_table_insert(table, (char *)"lowerTrain", lowerTrain);
    (void)g_hash_table_insert(table, (char *)"namespaceLowerSnake", nameSpaceLowerSnake);
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
        if (structure->new_full_extraCode && *structure->new_full_extraCode) {
            (void)g_hash_table_insert(table, (gchar *)"new_full_extraCode",
                                      g_strdup(structure->new_full_extraCode));
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
                    if (expression[++iter] != '{') {
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
                               "please check the template\n",
                               var);
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
    g_free(trueType);

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
        if (g_strcmp0(para->translator, (gchar *)"NONE") != 0) {
            res = g_strdup(para->translator);
        }
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

                    if (g_strcmp0(trueType, enumeration->name) == 0) {
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
        if (g_strcmp0(ret->translator, (gchar *)"NONE") != 0) {
            res = g_strdup(ret->translator);
        }
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

                    if (g_strcmp0(trueType, enumeration->name) == 0) {
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

static gboolean parameter_is_out(const Parameter *para)
{
    GList *link;
    for (link = para->annotations; link; link = g_list_next(link)) {
        if (g_strcmp0(link->data, "out") == 0 ||
            g_strcmp0(link->data, "inout") == 0 ||
            g_str_has_prefix(link->data, "out ")) {
            break;
        }
    }

    return link != NULL;
}

static gboolean annotation_contains_nullable(GList *annotations) /* gchar * */
{
    GList *link;

    for (link = annotations; link; link = g_list_next(link)) {
        if (g_strcmp0(link->data, "nullable") == 0) {
            break;
        }
    }

    return link != NULL;
}

static gboolean annotation_contains_optional(GList *annotations) /* gchar * */
{
    GList *link;

    for (link = annotations; link; link = g_list_next(link)) {
        if (g_strcmp0(link->data, "optional") == 0) {
            break;
        }
    }

    return link != NULL;
}

gchar *get_inline_parameter(Parameter *para)
{
    gchar *buffer;
    gchar *ret;
    gchar *translator;
    gboolean is_nullable;

    is_nullable = !para->translator && !is_enum_type(para->type) &&
                  annotation_contains_nullable(para->annotations);

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
        if (is_nullable) {
            (void)g_stpcpy(buffer + strlen(buffer), "((");
            (void)g_stpcpy(buffer + strlen(buffer), para->name);
            (void)g_stpcpy(buffer + strlen(buffer), ")?(");
        }
        (void)g_stpcpy(buffer + strlen(buffer), translator);
        (void)g_stpcpy(buffer + strlen(buffer), " (");
        if (para->translator == NULL && !is_enum_type(para->type)) {
            (void)g_stpcpy(buffer + strlen(buffer), "I_CAL_OBJECT ((ICalObject *)");
        }
    }

    (void)g_stpcpy(buffer + strlen(buffer), para->name);

    if (translator != NULL) {
        if (para->translator == NULL && !is_enum_type(para->type)) {
            (void)g_stpcpy(buffer + strlen(buffer), ")");
        }
        (void)g_stpcpy(buffer + strlen(buffer), ")");
        if (is_nullable) {
            (void)g_stpcpy(buffer + strlen(buffer), "):NULL)");
        }

        g_free(translator);
    }

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

    g_return_val_if_fail(method != NULL, NULL);

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

    if (method->custom != NULL) {
        (void)g_stpcpy(buffer + strlen(buffer), method->custom);
    } else {
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

    g_return_val_if_fail(method != NULL, NULL);

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

gchar *get_translator_name_for_return(const gchar *upperCamel)
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

    for (i = 0; i < const_prefix_len && i < type_len && const_prefix[i] == type[i]; i++)
        ;

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

static void initialize_default_value_table(void)
{
    defaultValues = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    (void)g_hash_table_insert(defaultValues, g_strdup("gboolean"), g_strdup("FALSE"));
    (void)g_hash_table_insert(defaultValues, g_strdup("gdouble"), g_strdup("0"));
    (void)g_hash_table_insert(defaultValues, g_strdup("gint"), g_strdup("0"));
    (void)g_hash_table_insert(defaultValues, g_strdup("gsize"), g_strdup("0"));
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

    upperCamel = g_strconcat(structure->nameSpace, structure->name, NULL);
    lowerTrain = get_lower_train_from_upper_camel(upperCamel);
    g_free(upperCamel);

    headerName = g_strconcat(dir, lowerTrain, ".h", NULL);
    sourceName = g_strconcat(dir, lowerTrain, ".c", NULL);
    g_free(lowerTrain);

    header = fopen(headerName, "w");
    source = fopen(sourceName, "w");

    g_free(headerName);
    g_free(sourceName);

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
    guint ii;
    gchar *newName;
    gchar *comment;
    gchar *tmp;

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
        write_str(out, comment);
        g_free(comment);
        comment = NULL;
        tmp = NULL;
    }

    /* Generate the declaration */
    write_str(out, "typedef enum {");

    for (ii = 0; enumeration->items != NULL && ii < enumeration->items->len; ii++) {
        const EnumerationItem *item = g_ptr_array_index(enumeration->items, ii);
        int prefix_len = 0;
        const gchar *use_prefix = NULL;
        const gchar *use_name = item->alias ? item->alias : item->nativeName;

        if (ii > 0) {
            write_str(out, ",");
        }
        if (g_str_has_prefix(use_name, ENUM_HEADER_ICAL)) {
            prefix_len = strlen(ENUM_HEADER_ICAL);
            use_prefix = "I_CAL_";
        } else if (g_str_has_prefix(use_name, ENUM_HEADER_VCARD)) {
            prefix_len = strlen(ENUM_HEADER_VCARD);
            use_prefix = "I_CAL_VCARD_";
        } else {
            fprintf(stderr, "The enum name '%s' in '%s' cannot be processed, it has no known prefix\n", use_name, enumeration->name);
            continue;
        }
        if (use_name[prefix_len] == '_') {
            newName = g_strconcat(use_prefix, use_name + prefix_len + 1, NULL);
        } else {
            newName = g_strconcat(use_prefix, use_name + prefix_len, NULL);
        }

        write_str(out, "\n\t");
        write_str(out, newName);
        write_str(out, " = ");
        write_str(out, item->nativeName);

        g_free(newName);
    }

    write_str(out, "\n} ");
    write_str(out, enumeration->name);
    write_str(out, ";\n");
}

gchar *get_source_run_time_checkers(Method *method, const gchar *nameSpace)
{
    GList *iter;
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
    guint nameSpace_len;
    gboolean param_is_out;

    g_return_val_if_fail(method != NULL, NULL);
    g_return_val_if_fail(nameSpace != NULL && *nameSpace != '\0', NULL);

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';
    res = NULL;
    nameSpace_len = (guint)strlen(nameSpace);

    for (iter = g_list_first(method->parameters); iter != NULL; iter = g_list_next(iter)) {
        parameter = (Parameter *)iter->data;

        if (parameter && parameter->type && parameter->type[strlen(parameter->type) - 1] == '*') {
            trueType = get_true_type(parameter->type);
            if (!trueType) {
                fprintf(stderr, "Unknown type '%s' in method %s\n", parameter->type, method->name);
                continue;
            }
            for (i = 0;
                 i < nameSpace_len && trueType[i] && nameSpace[i] == trueType[i];
                 i++)
                ;

            if (i == nameSpace_len) {
                (void)g_stpcpy(buffer + strlen(buffer), "\t");
                if (annotation_contains_nullable(parameter->annotations)) {
                    (void)g_stpcpy(buffer + strlen(buffer), "if(");
                    (void)g_stpcpy(buffer + strlen(buffer), parameter->name);
                    (void)g_stpcpy(buffer + strlen(buffer), ")\n\t\t");
                }
                nameSpaceUpperSnake = get_upper_snake_from_upper_camel(nameSpace);
                nameUpperSnake = get_upper_snake_from_upper_camel(trueType + i);
                typeCheck =
                    g_strconcat(nameSpaceUpperSnake, "_IS_", nameUpperSnake, " ((", trueType, "*)",
                                parameter->name, ")", NULL);
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

            param_is_out = parameter_is_out(parameter);
            if (i != nameSpace_len && ((!param_is_out && !annotation_contains_nullable(parameter->annotations)) ||
                                       (param_is_out && !annotation_contains_optional(parameter->annotations)))) {
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

static void generate_checks_file(const gchar *filename, GList *structures /* Structure * */)
{
    FILE *file;
    GString *calls;
    GList *link, *link2;

    file = fopen(filename, "wt");
    if (!file) {
        g_warning("Failed to open '%s' for writing", filename);
        return;
    }

    calls = g_string_new("");

    fprintf(file, "#include \"libical-glib/libical-glib.h\"\n");

    for (link = structures; link; link = g_list_next(link)) {
        Structure *str = link->data;

        for (link2 = str->enumerations; link2; link2 = g_list_next(link2)) {
            Enumeration *enumeration = link2->data;
            guint ii;
            if (g_str_equal(enumeration->nativeName, "CUSTOM")) {
                continue;
            }
            fprintf(file, "static void test_%s(%s value)\n", enumeration->name, enumeration->nativeName);
            fprintf(file, "{\n"
                          "    switch(value){\n");
            for (ii = 0; enumeration->items != NULL && ii < enumeration->items->len; ii++) {
                const EnumerationItem *item = g_ptr_array_index(enumeration->items, ii);
                fprintf(file, "    case %s: break;\n", item->nativeName);
            }
            fprintf(file, "    }\n"
                          "}\n");
            g_string_append_printf(calls, "    test_%s((%s) %s);\n",
                                   enumeration->name, enumeration->nativeName,
                                   enumeration->defaultNative ? enumeration->defaultNative : "0");
        }
    }
    fprintf(file, "int main(void)\n"
                  "{\n"
                  "%s"
                  "    return 0;\n"
                  "}\n",
            calls->str);
    fclose(file);

    (void)g_string_free(calls, TRUE);
}

static GHashTable * /* gchar *name ~> TemplateData * */
parse_api_templates(void)
{
    GHashTable *api_templates;
    xmlDoc *doc;
    xmlNode *node, *child;
    gchar *filename;

    filename = g_build_filename(templates_dir, API_TEMPLATES_FILENAME, NULL);
    doc = xmlParseFile(filename);
    if (doc == NULL) {
        g_warning("Failed to read '%s'", filename);
        g_free(filename);
        return NULL;
    }

    node = xmlDocGetRootElement(doc);
    if (node == NULL) {
        g_warning("An api-templates file '%s' has not content", filename);
        g_free(filename);
        xmlFreeDoc(doc);
        return NULL;
    }

    if (xmlStrcmp(node->name, (xmlChar *)"templates") != 0) {
        g_warning("Unexpected api-templates file '%s' root node '%s', expected was 'templates'", filename, (const char *)node->name);
        g_free(filename);
        xmlFreeDoc(doc);
        return NULL;
    }

    api_templates = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)g_ptr_array_unref);

    for (child = xmlFirstElementChild(node); child != NULL; child = xmlNextElementSibling(child)) {
        if (xmlStrcmp(child->name, (const xmlChar *)"method-template") == 0) {
            GPtrArray *methods = g_ptr_array_new_with_free_func((GDestroyNotify)method_free);
            xmlNode *kid;
            for (kid = xmlFirstElementChild(child); kid != NULL; kid = xmlNextElementSibling(kid)) {
                if (xmlStrcmp(kid->name, (xmlChar *)"method") == 0) {
                    Method *method = method_new();
                    if (!parse_method(kid, method)) {
                        method_free(method);
                    } else {
                        g_ptr_array_add(methods, method);
                    }
                }
            }
            if (methods->len > 0) {
                xmlChar *name = xmlGetProp(child, (const xmlChar *)"name");
                xmlChar *requires = xmlGetProp(child, (const xmlChar *)"requires");
                xmlChar *optional = xmlGetProp(child, (const xmlChar *)"optional");
                TemplateData *data = template_data_new((const gchar *)name, (const gchar *)requires, (const gchar *)optional);
                if (data != NULL) {
                    data->methods = methods;
                    if (g_hash_table_contains(api_templates, data->name)) {
                        g_warning("Warning: API template file '%s' already contains template with name '%s'", filename, data->name);
                    }
                    g_hash_table_insert(api_templates, data->name, data);
                } else {
                    g_ptr_array_unref(methods);
                }
                g_clear_pointer(&name, xmlFree);
                g_clear_pointer(&requires, xmlFree);
                g_clear_pointer(&optional, xmlFree);
            } else {
                g_ptr_array_unref(methods);
            }
        } else {
            g_warning("File '%s' contains unexpected element '%s'", filename, (const char *)child->name);
        }
    }

    g_free(filename);
    xmlFreeDoc(doc);

    return api_templates;
}

static GList * /* Structure * */
parse_api_files(const gchar *apis_dir,
                GHashTable *type2kind,      /* nullable */
                GHashTable *type2structure) /* nullable */
{
    GDir *dir;
    GError *local_error = NULL;
    GHashTable *api_templates;
    GList *filenames = NULL, *iter_filenames;
    GList *structures = NULL;
    Structure *structure;
    const gchar *filename;
    gboolean success = TRUE;

    dir = g_dir_open(apis_dir, 0, &local_error);
    if (!dir) {
        g_warning("Failed to open API directory '%s': %s", apis_dir,
                  local_error ? local_error->message : "Unknown error");
        g_clear_error(&local_error);
        return NULL;
    }

    api_templates = parse_api_templates();

    /* Parse the all the XML files into the Structure */
    while (filename = g_dir_read_name(dir), filename) {
        filenames = g_list_prepend(filenames, g_strdup(filename));
    }
    filenames = g_list_sort(filenames, (GCompareFunc)g_strcmp0);
    for (iter_filenames = g_list_first(filenames); iter_filenames != NULL && success;
         iter_filenames = g_list_next(iter_filenames)) {
        xmlDoc *doc;
        xmlNode *node;
        gchar *path;
        guint len;

        filename = iter_filenames->data;
        len = strlen(filename);

        if (len <= 4 || g_ascii_strncasecmp(filename + len - 4, ".xml", 4) != 0) {
            continue;
        }

        path = g_build_filename(apis_dir, filename, NULL);
        doc = xmlParseFile(path);
        if (doc == NULL) {
            printf("The doc %s cannot be parsed.\n", path);
            g_free(path);
            success = FALSE;
            break;
        }

        g_free(path);

        node = xmlDocGetRootElement(doc);
        if (node == NULL) {
            printf("The root node cannot be retrieved from the doc\n");
            xmlFreeDoc(doc);
            success = FALSE;
            break;
        }

        structure = structure_new();
        if (!parse_structure(node, structure, api_templates)) {
            printf("The node cannot be parsed into a structure.\n");
            xmlFreeDoc(doc);
            success = FALSE;
            structure_free(structure);
            break;
        }

        if (structure->native != NULL && type2kind != NULL && type2structure != NULL) {
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
                    success = FALSE;
                    structure_free(structure);
                    break;
                }
            }
        }

        if (type2kind != NULL && type2structure != NULL) {
            GList *iter_list;

            for (iter_list = g_list_first(structure->enumerations); iter_list != NULL;
                 iter_list = g_list_next(iter_list)) {
                Enumeration *enumeration = (Enumeration *)iter_list->data;
                (void)g_hash_table_insert(type2kind, g_strdup(enumeration->name), (void *)"enum");
                (void)g_hash_table_insert(type2structure, g_strdup(enumeration->name), structure);

                if (enumeration->defaultNative != NULL) {
                    (void)g_hash_table_insert(defaultValues, g_strdup(enumeration->name),
                                              g_strdup(enumeration->defaultNative));
                } else {
                    printf("Please supply a default value for enum %s\n", enumeration->name);
                    xmlFreeDoc(doc);
                    success = FALSE;
                    structure_free(structure);
                    break;
                }
            }
        }
        structures = g_list_append(structures, structure);
        xmlFreeDoc(doc);
    }

    g_clear_pointer(&api_templates, g_hash_table_unref);
    g_list_free_full(filenames, g_free);
    g_dir_close(dir);

    if (!success) {
        g_list_free_full(structures, (GDestroyNotify)structure_free);
        structures = NULL;
    }

    return structures;
}

static gint generate_library(const gchar *apis_dir)
{
    gchar *buffer;
    GList *structures;
    GList *iter_list;
    gint res = 0;

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

    structures = parse_api_files(apis_dir, type2kind, type2structure);
    if (structures == NULL) {
        res = 1;
        goto out;
    }

    /* Generate the forward declarations header file */
    generate_forward_declarations_header_file(structures);

    /* Generate the common header for all the headers, which is libical-glib.h for here */
    generate_header_header_file(structures);

    /* Generate all the header and source files for each structure */
    for (iter_list = g_list_first(structures); iter_list != NULL;
         iter_list = g_list_next(iter_list)) {
        Structure *structure = (Structure *)iter_list->data;
        generate_header_and_source(structure, (char *)"");
    }

    generate_checks_file("ical-glib-build-check.c", structures);
out:
    g_hash_table_destroy(type2kind);
    g_hash_table_destroy(type2structure);
    g_hash_table_destroy(defaultValues);
    g_list_free_full(structures, (GDestroyNotify)structure_free);
    g_free(buffer);

    return res;
}

static void generate_declarations(FILE *out, Structure *structure, const gchar *position)
{
    GList *list_iter;
    Declaration *declaration;

    g_return_if_fail(out != NULL && structure != NULL);

    for (list_iter = g_list_first(structure->declarations); list_iter != NULL;
         list_iter = g_list_next(list_iter)) {
        declaration = (Declaration *)list_iter->data;

        if (g_strcmp0(declaration->position, position) == 0) {
            write_str(out, declaration->content);
            write_str(out, "\n");
        }
        declaration = NULL;
    }
}

void generate_header_header_declaration(FILE *out, Structure *structure)
{
    generate_declarations(out, structure, "header");
}

void generate_source_body_declaration(FILE *out, Structure *structure)
{
    generate_declarations(out, structure, "body");
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
    if (!in) {
        return;
    }

    out = fopen("libical-glib.h", "w");
    if (!out) {
        fprintf(stderr, "Failed to open libical-glib.h for writing\n");
        fclose(in);
        return;
    }

    buffer = g_new(gchar, BUFFER_SIZE);
    *buffer = '\0';

    while (!feof(in) && !ferror(in) && (c = fgetc(in)) != EOF) {
        if (c == '$') {
            if (!feof(in) && !ferror(in) && (c = fgetc(in)) != '{' && c != '^') {
                printf("The following char is not {");
                g_free(buffer);
                fclose(in);
                fclose(out);
                return;
            }

            while (!feof(in) && !ferror(in) && (c = fgetc(in)) != '}') {
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
                g_free(buffer);
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

static gboolean
remove_symbols_with_prefix_cb(gpointer key,
                              gpointer value,
                              gpointer user_data)
{
    const gchar *symbol = key;
    const gchar *prefix = user_data;

    _unused(value);

    return prefix != NULL && *prefix != '\0' && g_str_has_prefix(symbol, prefix);
}

static gint
sort_symbols_by_name_cb(gconstpointer aa,
                        gconstpointer bb)
{
    const char *symbol_a = *((const char **)aa);
    const char *symbol_b = *((const char **)bb);
    return g_strcmp0(symbol_a, symbol_b);
}

static gboolean
check_api_files(const gchar *apis_dir,
                GHashTable *symbols)
{
    GList *structures, *link;
    gboolean success = TRUE;

    structures = parse_api_files(apis_dir, NULL, NULL);

    for (link = structures; link != NULL; link = g_list_next(link)) {
        const Structure *structure = link->data;
        GList *iter;

        for (iter = structure->methods; iter != NULL; iter = g_list_next(iter)) {
            const Method *method = iter->data;
            /* do not check whether the 'corresponds' exists, it can be CUSTOM or sometimes repeated */
            if (method->corresponds != NULL) {
                g_hash_table_remove(symbols, method->corresponds);
                /* some methods have their counter parts without the"_r" suffix, cover them too */
                if (g_str_has_suffix(method->corresponds, "_r")) {
                    gchar *stripped = g_strndup(method->corresponds, strlen(method->corresponds) - 2);
                    g_hash_table_remove(symbols, stripped);
                    g_free(stripped);
                }
            }
        }

        for (iter = structure->enumerations; iter != NULL; iter = g_list_next(iter)) {
            const Enumeration *enumr = iter->data;
            if (enumr->nativeName != NULL) {
                g_hash_table_remove(symbols, enumr->nativeName);
            }
        }

        if (structure->skips != NULL) {
            guint ii;
            for (ii = 0; ii < structure->skips->len; ii++) {
                gchar *skip_symbol = g_ptr_array_index(structure->skips, ii);
                if (g_str_has_suffix(skip_symbol, "*")) {
                    gchar *prefix = g_strndup(skip_symbol, strlen(skip_symbol) - 1);
                    g_hash_table_foreach_remove(symbols, remove_symbols_with_prefix_cb, prefix);
                    g_free(prefix);
                } else {
                    g_hash_table_remove(symbols, skip_symbol);
                }
            }
        }
    }

    g_list_free_full(structures, (GDestroyNotify)structure_free);

    success = g_hash_table_size(symbols) == 0;
    if (!success) {
        GHashTableIter iter;
        GPtrArray *sorted;
        gpointer key = NULL;
        guint ii;

        fprintf(stderr,
                "Error: The following %u symbols are not covered by the libical-glib API files: ",
                g_hash_table_size(symbols));

        sorted = g_ptr_array_sized_new(g_hash_table_size(symbols));
        g_hash_table_iter_init(&iter, symbols);
        while (g_hash_table_iter_next(&iter, &key, NULL)) {
            const gchar *symbol = key;
            g_ptr_array_add(sorted, (gpointer)symbol);
        }
        g_ptr_array_sort(sorted, sort_symbols_by_name_cb);
        for (ii = 0; ii < sorted->len; ii++) {
            const gchar *symbol = g_ptr_array_index(sorted, ii);
            if (ii > 0) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "%s", symbol);
        }
        fprintf(stderr, "\n");
        fprintf(stderr,
                "Hint: Either add the definitions for them or declare them as <skip>symbol</skip> under <structure/>\n");
        g_ptr_array_unref(sorted);
    }

    return success;
}

int main(int argc, char *argv[])
{
    const gchar *apis_dir;
    gint res;

    if (argc < 3) {
        fprintf(stderr,
                "Requires two arguments, the first is path to templates, "
                "the second is a path to XML files with an API description\n");
        return 1;
    }

    /* The directory to search for templates */
    templates_dir = argv[1];
    /* The directory to search for XML files */
    apis_dir = argv[2];

    if (argc > 3) {
        GHashTable *symbols;
        int ii;

        if (strcmp(argv[3], "--check-api-files") != 0) {
            fprintf(stderr, "Unknown argument '%s', expects '--check-api-files'\n", argv[3]);
            return 2;
        }
        if (((argc - 4) & 1) != 0) {
            fprintf(stderr, "Expects pair of arguments, path to a header file and export token\n");
            return 3;
        }

        symbols = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

        for (ii = 4; ii < argc; ii += 2) {
            GError *error = NULL;
            if (!parse_header_file(symbols, argv[ii], argv[ii + 1], &error)) {
                fprintf(stderr, "Failed to parse header file '%s': %s\n",
                        argv[ii], error ? error->message : "Unknown error");
                g_hash_table_unref(symbols);
                g_clear_error(&error);
                return 4;
            }
        }

        if (check_api_files(apis_dir, symbols)) {
            ii = 0;
        } else {
            ii = 5;
        }
        g_hash_table_unref(symbols);
        return ii;
    }

    res = generate_library(apis_dir);
    close_private_header();

    return res;
}
