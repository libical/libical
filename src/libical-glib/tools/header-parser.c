/*
 * SPDX-FileCopyrightText: 2025 Red Hat <www.redhat.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

/* This is a very naive C header parser, which extracts public symbols */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <string.h>

#include "header-parser.h"

static gchar *
skip_white_spaces(gchar *ptr,
                  gboolean *out_was_new_line)
{
    while (*ptr == '\t' || *ptr == '\r' || *ptr == '\n' || *ptr == ' ') {
        if (out_was_new_line) {
            *out_was_new_line = (*out_was_new_line) || *ptr == '\r' || *ptr == '\n';
        }
        ptr++;
    }

    return ptr;
}

static gchar *
skip_after(gchar *ptr,
           const gchar *after,
           gchar *out_last_char)
{
    while (*ptr && !strchr(after, *ptr)) {
        if (out_last_char) {
            *out_last_char = *ptr;
        }
        ptr++;
    }

    while (*ptr && strchr(after, *ptr)) {
        ptr++;
    }

    return ptr;
}

static gchar *
skip_ident(gchar *ptr)
{
    while (*ptr && ((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') || *ptr == '_')) {
        ptr++;
    }

    return ptr;
}

gboolean
parse_header_file(GHashTable *symbols, /* caller allocates, char * ~> itself */
                  const char *filename,
                  const char *export_token,
                  GError **error)
{
    gchar *content = NULL, *start, *ptr;
    gboolean is_new_line = TRUE, read_func_name = FALSE;
    guint export_token_len = export_token ? strlen(export_token) : 0;

    if (!g_file_get_contents(filename, &content, NULL, error)) {
        return FALSE;
    }

    if (export_token != NULL && !*export_token) {
        export_token = NULL;
    }

    start = content;
    ptr = content;

    while (*start) {
        ptr = skip_white_spaces(ptr, &is_new_line);
        start = ptr;

        if (*ptr == '/') {
            if (ptr[1] == '/') {
                ptr = skip_after(ptr, "\r\n", NULL);
                is_new_line = TRUE;
            } else if (ptr[1] == '*') {
                while (ptr[0] && ptr[1] && !(ptr[0] == '*' && ptr[1] == '/')) {
                    if (!is_new_line && (*ptr == '\n' || *ptr == '\r')) {
                        is_new_line = TRUE;
                    }
                    ptr++;
                }
                if (ptr[0] == '*' && ptr[1] == '/') {
                    ptr += 2;
                }
            }
        } else if (*ptr == '#') {
            gboolean done = FALSE;

            if (strncmp(ptr, "#define", 7) == 0) {
                /* covers "#define func(args) otherfunc(args) */
                ptr += 7;
                ptr = skip_white_spaces(ptr, NULL);
                start = ptr;
                ptr = skip_ident(ptr);
                if (*ptr == '(') {
                    g_hash_table_add(symbols, g_strndup(start, ptr - start));
                }
            }

            while (!done) {
                char last_char = 0;
                ptr = skip_after(ptr, "\r\n", &last_char);
                done = last_char != '\\';
            }

            is_new_line = TRUE;
        } else if (*ptr != 0) {
            ptr = skip_ident(ptr);
            if (ptr == start) {
                ptr++;
                is_new_line = FALSE;
            } else if (strncmp(start, "typedef", 7) == 0) {
                ptr = skip_white_spaces(ptr, NULL);
                start = ptr;
                ptr = skip_ident(ptr);
                /* catch: typedef enum enumname { ... } typename;*/
                if (strncmp(start, "enum", ptr - start) == 0) {
                    ptr = skip_after(ptr, "};", NULL);
                    if (ptr > start && ptr[-1] != ';') {
                        ptr = skip_white_spaces(ptr, NULL);
                        start = ptr;
                        ptr = skip_ident(ptr);
                        if (start != ptr) {
                            g_hash_table_add(symbols, g_strndup(start, ptr - start));
                            ptr = skip_after(ptr, ";", NULL);
                        }
                    }
                } else {
                    ptr = skip_after(ptr, ";", NULL);
                }
            } else if (is_new_line) {
                read_func_name = export_token == NULL || (ptr - start == export_token_len && strncmp(start, export_token, export_token_len) == 0);
                is_new_line = FALSE;
                if (!read_func_name && ptr - start == 4 && strncmp(start, "enum", ptr - start) == 0) {
                    ptr = skip_white_spaces(ptr, NULL);
                    start = ptr;
                    ptr = skip_ident(ptr);
                    if (start != ptr) {
                        gchar *ident = g_strndup(start, ptr - start);
                        g_hash_table_add(symbols, g_strconcat("enum ", ident, NULL));
                        g_free(ident);
                        ptr = skip_after(ptr, "}", NULL);
                    }
                }
            } else if (read_func_name) {
                const gchar *next = skip_white_spaces(ptr, &is_new_line);
                if (*next == '(') {
                    g_hash_table_add(symbols, g_strndup(start, ptr - start));
                    read_func_name = FALSE;
                    ptr = skip_after(ptr, ";", NULL);
                }
            }
        }
    }

    g_free(content);

    return TRUE;
}
