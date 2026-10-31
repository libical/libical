/*
 * SPDX-FileCopyrightText: 2025 Red Hat <www.redhat.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifndef HEADER_PARSER_H
#define HEADER_PARSER_H

#include <glib.h>

G_BEGIN_DECLS

gboolean
parse_header_file(GHashTable *symbols, /* caller allocates, char * ~> itself */
                  const char *filename,
                  const char *export_token,
                  GError **error);

G_END_DECLS

#endif /* HEADER_PARSER_H */
