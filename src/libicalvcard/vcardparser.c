/* vcardparser.c : fast vCard parser
 *
 * Based mostly on work by Bron Gondwana <brong@fastmailteam.com>
 */

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "icalmemory.h"
#include "vcardcomponent.h"
#include "vcardparser.h"
#include "vcardparameter.h"
#include "vcardproperty.h"
#include "vcardvalue.h"

#define DEBUG 0

#if defined __GNUC__ &&  __GNUC__ > 6
    #define GCC_FALLTHROUGH __attribute__((fallthrough));
#else
    #define GCC_FALLTHROUGH /* fall through */
#endif

enum parse_error {
    PE_OK = 0,
    PE_BACKQUOTE_EOF,
    PE_BEGIN_PARAMS,
    PE_PROP_MULTIGROUP,
    PE_FINISHED_EARLY,
    PE_KEY_EOF,
    PE_KEY_EOL,
    PE_MISMATCHED_CARD,
    PE_NAME_EOF,
    PE_NAME_EOL,
    PE_NAME_INVALID,
    PE_PARAMVALUE_EOF,
    PE_PARAMVALUE_EOL,
    PE_QSTRING_EOF,
    PE_QSTRING_EOL,
    PE_QSTRING_EOV,
    PE_VALUE_INVALID,
    PE_ILLEGAL_CHAR,
    PE_NUMERR /* last */
};

struct buf {
    char *s;
    size_t len;
    size_t alloc;
};
#define BUF_INITIALIZER { NULL, 0, 0 }

struct vcardparser_state {
    struct buf buf;
    struct buf errbuf;
    const char *base;
    const char *itemstart;
    const char *p;
    const vcardproperty *version;

    /* current items */
    vcardcomponent *root;
    vcardcomponent *comp;
    vcardproperty *prop;
    vcardparameter *param;
    vcardvalue_kind value_kind;
};

struct vcardparser_errorpos {
    int startpos;
    int startline;
    int startchar;
    int errorpos;
    int errorline;
    int errorchar;
};

#define BUF_GROW 128

void buf_init(struct buf *buf, size_t size)
{
    buf->len = 0;
    buf->alloc = size;
    buf->s = icalmemory_new_buffer(buf->alloc);
}

static size_t buf_len(struct buf *buf)
{
    return buf->len;
}

static void buf_reset(struct buf *buf)
{
    buf->len = 0;
}

static void buf_free(struct buf *buf)
{
    if (buf->s) {
        icalmemory_free_buffer(buf->s);
        buf->s = NULL;
    }

    buf->len = buf->alloc = 0;
}

static void _buf_putc(struct buf *buf, char c)
{
    char *pos = buf->s + buf->len;

    icalmemory_append_char(&buf->s, &pos, &buf->alloc, c);
}

static void buf_putc(struct buf *buf, char c)
{
    _buf_putc(buf, c);

    buf->len++;
}

static const char *buf_cstring(struct buf *buf)
{
    _buf_putc(buf, '\0');

    return buf->s;
}

static void buf_trim(struct buf *buf)
{
    /* trim whitespace
     *
     * XXX we can do this within the existing buffer
     * because the new string length <= old string length
     */
    const char *s = buf_cstring(buf);

    for (buf->len = 0; *s; s++) {
        if (isspace(*s)) continue;
        buf->s[buf->len++] = *s;
    }
}

static void buf_vprintf(struct buf *buf, const char *fmt, va_list args)
{
    va_list ap;
    size_t size, n;

    /* Copy args in case we have to try again */
    va_copy(ap, args);

    size = buf->alloc - buf->len;
    n = vsnprintf(buf->s + buf->len, size, fmt, args);

    if (n >= size) {
        /* Grow the buffer and try again */
        size = n + BUF_GROW;
        buf->alloc += size;
        buf->s = icalmemory_resize_buffer(buf->s, buf->alloc);

        n = vsnprintf(buf->s + buf->len, size, fmt, ap);
    }
    va_end(ap);

    buf->len += n;
}

#define NOTESTART() state->itemstart = state->p
#define MAKE(X, Y) X = icalmemory_new_buffer(sizeof(struct Y))
#define PUTC(C) buf_putc(&state->buf, C)
#define INC(I) state->p += I
#define IS_CTRL(ch) \
    (ch > 0 && ch <= 0x1f && ch != '\r' && ch != '\n' && ch != '\t')
#define HANDLECTRL(state) \
{ \
    if (IS_CTRL(*state->p)) { \
        while (IS_CTRL(*state->p)) \
            state->p++; \
    } \
    if ((*state->p) == 0) \
        break; \
}

static int _parse_param_name(struct vcardparser_state *state)
{
    vcardparameter_kind kind;
    const char *name;

    while (*state->p) {

        /* Handle control characters and break for NUL char */
        HANDLECTRL(state);

        switch (*state->p) {
        case '=':
            name = buf_cstring(&state->buf);
            kind = vcardparameter_string_to_kind(name);
            state->param = vcardparameter_new(kind);
            if (kind == VCARD_X_PARAMETER) {
                vcardparameter_set_xname(state->param, name);
            } else if (kind == VCARD_IANA_PARAMETER) {
                vcardparameter_set_iana_name(state->param, name);
            }
            buf_reset(&state->buf);
            INC(1);
            return 0;

        case ';': /* vCard 2.1 parameter with just a value */
        case ':':
            state->param = vcardparameter_new(VCARD_TYPE_PARAMETER);
            /* no INC - we need to see this char up a layer */
            return 0;

        case '\r':
            INC(1);
            break; /* just skip */
        case '\n':
            if (state->p[1] != ' ' && state->p[1] != '\t')
                return PE_KEY_EOL;
            INC(2);
            break;

        /* XXX - check exact legal set? */
        default:
            PUTC(*state->p);
            INC(1);
            break;
        }
    }

    return PE_KEY_EOF;
}

/* just leaves it on the buffer */
static int _parse_param_quoted(struct vcardparser_state *state,
                               int structured, int multivalued)
{
    NOTESTART();

    while (*state->p) {

        /* Handle control characters and break for NUL char */
        HANDLECTRL(state);

        switch (*state->p) {
        case '"':
            INC(1);
            return 0;

        /* normal backslash quoting - NOTE, not strictly RFC compliant,
         * but I figure anyone who generates one PROBABLY meant to escape
         * the next character because it's so common, and LABEL definitely
         * allows \n, so we have to handle that anyway */
        case '\\':
            /* seen in the wild - \n split by line wrapping */
            if (state->p[1] == '\r') INC(1);
            if (state->p[1] == '\n') {
                if (state->p[2] != ' ' && state->p[2] != '\t')
                    return PE_QSTRING_EOL;
                INC(2);
            }
            if (!state->p[1])
                return PE_BACKQUOTE_EOF;
            if (state->p[1] == 'n' || state->p[1] == 'N')
                PUTC('\n');
            else
                PUTC(state->p[1]);
            INC(2);
            break;

        /* special value quoting for doublequote and endline (RFC 6868) */
        case '^':
            if (state->p[1] == '\r') INC(1);
            if (state->p[1] == '\n') {
                if (state->p[2] != ' ' && state->p[2] != '\t')
                    return PE_QSTRING_EOL;
                INC(2);
            }
            if (state->p[1] == '\'') {
                PUTC('"');
                INC(2);
            }
            else if (state->p[1] == 'n') { /* only lower case per the RFC */
                PUTC('\n');
                INC(2);
            }
            else if (state->p[1] == '^') {
                PUTC('^');
                INC(2);
            }
            else {
                PUTC('^');
                INC(1); /* treat next char normally */
            }
            break;

        case '\r':
            INC(1);
            break; /* just skip */
        case '\n':
            if (state->p[1] != ' ' && state->p[1] != '\t')
                return PE_QSTRING_EOL;
            INC(2);
            break;

        case ',':
            if (multivalued)
                return PE_QSTRING_EOV;
            /* or fall through, comma isn't special */
            GCC_FALLTHROUGH

        case ';':
            if (structured)
                return PE_QSTRING_EOV;
            /* or fall through, semi-colon isn't special */
            GCC_FALLTHROUGH

        default:
            PUTC(*state->p);
            INC(1);
            break;
        }
    }

    return PE_QSTRING_EOF;
}

static int _parse_param_value(struct vcardparser_state *state)
{
    int multivalued = vcardparameter_is_multivalued(state->param);
    int r;

    vcardstructuredtype *structured = NULL;
    vcardstrarray *field;

    if (vcardparameter_is_structured(state->param)) {
        structured = vcardstructured_new();
        field = vcardstrarray_new(2);
        structured->field[structured->num_fields++] = field;
        vcardparameter_set_jscomps(state->param, structured);
    }

    while (*state->p) {

        /* Handle control characters and break for NUL char */
        HANDLECTRL(state);

        switch (*state->p) {
        case '\\': /* normal backslash quoting */
            /* seen in the wild - \n split by line wrapping */
            if (state->p[1] == '\r') INC(1);
            if (state->p[1] == '\n') {
                if (state->p[2] != ' ' && state->p[2] != '\t')
                    return PE_PARAMVALUE_EOL;
                INC(2);
            }
            if (!state->p[1])
                return PE_BACKQUOTE_EOF;
            if (state->p[1] == 'n' || state->p[1] == 'N')
                PUTC('\n');
            else
                PUTC(state->p[1]);
            INC(2);
            break;

        case '^': /* special value quoting for doublequote (RFC 6868) */
            /* seen in the wild - \n split by line wrapping */
            if (state->p[1] == '\r') INC(1);
            if (state->p[1] == '\n') {
                if (state->p[2] != ' ' && state->p[2] != '\t')
                    return PE_PARAMVALUE_EOL;
                INC(2);
            }
            if (state->p[1] == '\'') {
                PUTC('"');
                INC(2);
            }
            else if (state->p[1] == 'n') {
                PUTC('\n');
                INC(2);
            }
            else if (state->p[1] == '^') {
                PUTC('^');
                INC(2);
            }
            else {
                PUTC('^');
                INC(1); /* treat next char normally */
            }
            break;

        case '"':
            INC(1);
            while ((r = _parse_param_quoted(state,
                                            structured != NULL,
                                            multivalued)) == PE_QSTRING_EOV) {
                if (structured) {
                    vcardstrarray_append(field, buf_cstring(&state->buf));

                    if (*state->p == ';') {
                        field = vcardstrarray_new(2);
                        structured->field[structured->num_fields++] = field;
                    }
                }
                else {
                    vcardparameter_add_value_from_string(state->param,
                                                         buf_cstring(&state->buf));
                }

                buf_reset(&state->buf);
                INC(1);
            }
            if (r) return r;
            break;

        case ':':
        case ';':
            /* done - end of parameter */
            if (structured) {
                vcardstrarray_append(field, buf_cstring(&state->buf));
            }
            else if (multivalued) {
                vcardparameter_add_value_from_string(state->param,
                                                     buf_cstring(&state->buf));
            }
            else {
                vcardparameter_set_value_from_string(state->param,
                                                     buf_cstring(&state->buf));

                /* if it is a VALUE parameter, set the value kind */
                if (vcardparameter_isa(state->param) == VCARD_VALUE_PARAMETER) {
                    vcardvalue_kind kind =
                        vcardvalue_string_to_kind(buf_cstring(&state->buf));
                    if (kind != VCARD_NO_VALUE) state->value_kind = kind;
                }
            }

            buf_reset(&state->buf);
            /* no INC - we need to see this char up a layer */
            return 0;

        case '\r':
            INC(1);
            break; /* just skip */

        case '\n':
            if (state->p[1] != ' ' && state->p[1] != '\t')
                return PE_PARAMVALUE_EOL;
            INC(2);
            break;

        case ',':
            if (multivalued) {
                vcardparameter_add_value_from_string(state->param,
                                                     buf_cstring(&state->buf));
                buf_reset(&state->buf);
                INC(1);
                break;
            }
            /* or fall through, comma isn't special */
            GCC_FALLTHROUGH

        default:
            PUTC(*state->p);
            INC(1);
            break;
        }
    }

    return PE_PARAMVALUE_EOF;
}

static void _parse_error(struct vcardparser_state *state,
                         enum vcardparameter_xlicerrortype type,
                         const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    buf_reset(&state->errbuf);
    buf_vprintf(&state->errbuf, fmt, ap);
    va_end(ap);

    if (state->prop) vcardproperty_free(state->prop);

    state->prop =
        vcardproperty_vanew_xlicerror(buf_cstring(&state->errbuf),
                                      vcardparameter_new_xlicerrortype(type),
                                      (void *) 0);
    buf_reset(&state->buf);
}

static int _parse_prop_params(struct vcardparser_state *state)
{
    vcardproperty_kind prop_kind = vcardproperty_isa(state->prop);
    const char *group = vcardproperty_get_group(state->prop);

    do {
        int r;

        INC(1); /* skip ';' */

        NOTESTART();

        /* get the name */
        r = _parse_param_name(state);
        if (r) {
            _parse_error(state,
                         VCARD_XLICERRORTYPE_PARAMETERNAMEPARSEERROR,
                         "%s '%s' in %s%s%s property. Removing entire property",
                         vcardparser_errstr(r), buf_cstring(&state->buf),
                         group ? group : "", group ? "." : "",
                         vcardproperty_kind_to_string(prop_kind));
            return r;
        }

        vcardproperty_add_parameter(state->prop, state->param);

        /* now get the value */
        r = _parse_param_value(state);
        if (r) {
            vcardparameter_kind param_kind = vcardparameter_isa(state->param);

            _parse_error(state,
                         VCARD_XLICERRORTYPE_PARAMETERVALUEPARSEERROR,
                         "%s for %s in %s%s%s property. Removing entire property",
                         vcardparser_errstr(r),
                         vcardparameter_kind_to_string(param_kind),
                         group ? group : "", group ? "." : "",
                         vcardproperty_kind_to_string(prop_kind));
            return r;
        }

    } while (*state->p == ';');  /* another parameter to parse */

    return 0;
}

static int _parse_prop_name(struct vcardparser_state *state)
{
    const char *name;
    char *group = NULL;
    vcardproperty_kind kind;
    int r = 0;

    NOTESTART();

    while (*state->p) {

        /* Handle control characters and break for NUL char */
        HANDLECTRL(state);

        switch (*state->p) {
        case ':':
        case ';':
            name = buf_cstring(&state->buf);
            kind = vcardproperty_string_to_kind(name);

            if (!(state->prop = vcardproperty_new(kind)))
                return PE_NAME_INVALID;

            if (kind == VCARD_X_PROPERTY)
                vcardproperty_set_x_name(state->prop, name);

            if (group)
                vcardproperty_set_group(state->prop, group);

            /* set default value kind */
            if (kind == VCARD_GEO_PROPERTY) {
                if (vcardproperty_get_version(state->version) == VCARD_VERSION_40) {
                    state->value_kind = VCARD_URI_VALUE;
                } else {
                    state->value_kind = VCARD_STRUCTURED_VALUE;
                }
            } else if (kind == VCARD_TZ_PROPERTY) {
                if (vcardproperty_get_version(state->version) == VCARD_VERSION_40) {
                    state->value_kind = VCARD_TEXT_VALUE;
                } else {
                    state->value_kind = VCARD_UTCOFFSET_VALUE;
                }
            } else {
                state->value_kind = vcardproperty_kind_to_value_kind(kind);
            }

            buf_reset(&state->buf);

            /* no INC - we need to see this char up a layer */
            return r;

        case '.':
            if (group) {
                char *tmp =
                    icalmemory_tmp_buffer(strlen(group) + buf_len(&state->buf) + 2);
                sprintf(tmp, "%s.%s", group, buf_cstring(&state->buf));
                group = tmp;
                r = PE_PROP_MULTIGROUP;
            }
            else {
                group = icalmemory_tmp_copy(buf_cstring(&state->buf));
            }
            buf_reset(&state->buf);
            INC(1);
            break;

        case '\r':
            INC(1);
            break; /* just skip */
        case '\n':
            if (state->p[1] == ' ' || state->p[1] == '\t') /* wrapped line */
                INC(2);
            else if (!state->buf.len) {
                /* no key yet?  blank intermediate lines are OK */
                INC(1);
            }
            else
                return PE_NAME_EOL;
            break;

        default:
            PUTC(*state->p);
            INC(1);
            break;
        }
    }

    return PE_NAME_EOF;
}

static int _parse_prop_value(struct vcardparser_state *state)
{
    vcardproperty_kind prop_kind = vcardproperty_isa(state->prop);
    int is_multivalued = (state->value_kind == VCARD_TEXTLIST_VALUE) ||
        vcardproperty_is_multivalued(prop_kind);
    int is_structured  = (state->value_kind == VCARD_STRUCTURED_VALUE);
    const char *text_sep = NULL;
    vcardstructuredtype structured;
    vcardstrarray *textlist;
    vcardvalue *value;

    if (is_multivalued || is_structured) {
        textlist = vcardstrarray_new(2);

        if (is_structured) {
            memset(&structured, 0, sizeof(vcardstructuredtype));
            structured.field[structured.num_fields++] = textlist;
            text_sep = ";,";
        } else if ((state->value_kind == VCARD_TEXTLIST_VALUE) &&
                   vcardproperty_is_structured(prop_kind)) {
            text_sep = ";";
        } else {
            text_sep = ",";
        }
    }

    NOTESTART();

    while (*state->p) {

        /* Handle control characters and break for NUL char */
        HANDLECTRL(state);

        switch (*state->p) {
        case '\\':
            /* seen in the wild - \n split by line wrapping */
            while (strchr("\r\n", state->p[1])) {
                if (state->p[1] == '\r') {
                    INC(1);
                }
                if (state->p[1] == '\n') {
                    if (state->p[2] != ' ' && state->p[2] != '\t')
                        return PE_BACKQUOTE_EOF;
                    INC(2);
                }
            }
            if (!state->p[1])
                return PE_BACKQUOTE_EOF;
            /* preserve escape sequences */
            PUTC('\\');
            PUTC(state->p[1]);
            INC(2);
            break;
        case '\r':
            INC(1);
            break; /* just skip */
        case '\n':
            if (state->p[1] == ' ' || state->p[1] == '\t') {/* wrapped line */
                INC(2);
                break;
            }
            /* otherwise it's the end of the value */
            INC(1);
            goto out;

        case ',':
        case ';':
            if (is_structured || (is_multivalued && strchr(text_sep, *state->p))) {
                const char *str = buf_cstring(&state->buf);
                char *dequot_str =
                    vcardvalue_strdup_and_dequote_text(&str, text_sep);
                vcardstrarray_append(textlist, dequot_str);
                buf_reset(&state->buf);
                free(dequot_str);

                if (*state->p == ';' && is_structured) {
                    textlist = vcardstrarray_new(2);
                    structured.field[structured.num_fields++] = textlist;
                }
                INC(1);
                break;
            }
            /* or fall through, comma/semi-colon isn't special */
            GCC_FALLTHROUGH

        default:
            PUTC(*state->p);
            INC(1);
            break;
        }
    }

out:
    /* reaching the end of the file isn't a failure here,
     * it's just another type of end-of-value */

    if (is_multivalued || is_structured) {
        const char *str = buf_cstring(&state->buf);
        char *dequot_str =
            vcardvalue_strdup_and_dequote_text(&str, text_sep);
        vcardstrarray_append(textlist, dequot_str);
        buf_reset(&state->buf);
        free(dequot_str);

        if (is_structured)
            value = vcardvalue_new_structured(&structured);
        else
            value = vcardvalue_new_textlist(textlist);
    }
    else {
        /* repair critical property values */
        if (prop_kind == VCARD_VERSION_PROPERTY) {
            buf_trim(&state->buf);
            state->version = state->prop;
        }

        value = vcardvalue_new_from_string(state->value_kind,
                                           buf_cstring(&state->buf));
    }

    if (!value) return PE_VALUE_INVALID;

    vcardproperty_set_value(state->prop, value);
    buf_reset(&state->buf);

    return 0;
}

static void _parse_eatline(struct vcardparser_state *state)
{
    while (*state->p) {

        /* Handle control characters and break for NUL char */
        HANDLECTRL(state);

        switch (*state->p) {
        case '\n':
            if (state->p[1] == ' ' || state->p[1] == '\t') {/* wrapped line */
                INC(2);
                break;
            }
            /* otherwise it's the end of the line */
            INC(1);
            return;

        default:
            INC(1);
            break;
        }
    }
}

static void _parse_prop(struct vcardparser_state *state)
{
    int r = _parse_prop_name(state);
    if (r) {
        if (r == PE_PROP_MULTIGROUP) {
            vcardproperty_kind prop_kind = vcardproperty_isa(state->prop);

            _parse_error(state,
                         VCARD_XLICERRORTYPE_PROPERTYPARSEERROR,
                         "%s '%s.%s'. Removing entire property",
                         vcardparser_errstr(r),
                         vcardproperty_get_group(state->prop),
                         vcardproperty_kind_to_string(prop_kind));
            _parse_eatline(state);
        }
        else if (r == PE_NAME_INVALID) {
            _parse_error(state,
                         VCARD_XLICERRORTYPE_PROPERTYPARSEERROR,
                         "%s '%s'. Removing entire property",
                         vcardparser_errstr(r), buf_cstring(&state->buf));
            _parse_eatline(state);
        }
        else {
            _parse_error(state,
                         VCARD_XLICERRORTYPE_PROPERTYPARSEERROR,
                         "%s '%s'. Ignoring property",
                         vcardparser_errstr(r), buf_cstring(&state->buf));
        }
        return;
    }

    if (*state->p == ';') {
        r = _parse_prop_params(state);
        if (r) {
            /* errors handled in _parse_prop_params() */
            return;
        }
    }

    INC(1); /* skip ':' */
    r = _parse_prop_value(state);
    if (r) {
        vcardproperty_kind prop_kind = vcardproperty_isa(state->prop);
        const char *group = vcardproperty_get_group(state->prop);

        if (r == PE_VALUE_INVALID) {
            _parse_error(state,
                         VCARD_XLICERRORTYPE_VALUEPARSEERROR,
                         "Error parsing '%s' as %s value in %s%s%s property."
                         " Removing entire property",
                         buf_cstring(&state->buf),
                         vcardvalue_kind_to_string(state->value_kind),
                         group ? group : "", group ? "." : "",
                         vcardproperty_kind_to_string(prop_kind));
        }
        else {
            _parse_error(state,
                         VCARD_XLICERRORTYPE_VALUEPARSEERROR,
                         "%s in %s%s%s property. Removing entire property",
                         vcardparser_errstr(r),
                         group ? group : "", group ? "." : "",
                         vcardproperty_kind_to_string(prop_kind));
        }
    }
}

static int _parse_vcard(struct vcardparser_state *state,
                        vcardcomponent *comp, int only_one)
{
    vcardcomponent *sub;
    const char *cardstart = state->p;
    int r = 0;

    while (*state->p) {
        /* whitespace is very skippable before AND afterwards */
        if (*state->p == '\r' || *state->p == '\n' ||
            *state->p == ' ' || *state->p == '\t') {
            INC(1);
            continue;
        }

        _parse_prop(state);

        if (vcardproperty_isa(state->prop) == VCARD_BEGIN_PROPERTY) {
            const char *val =
                vcardvalue_get_text(vcardproperty_get_value(state->prop));
            vcardcomponent_kind kind = vcardcomponent_string_to_kind(val);

            if (kind == VCARD_NO_COMPONENT) {
                state->itemstart = cardstart;
                r = PE_MISMATCHED_CARD;
                break;
            }

            vcardproperty_free(state->prop);
            state->prop = NULL;

            sub = vcardcomponent_new(kind);
            vcardcomponent_add_component(comp, sub);
            r = _parse_vcard(state, sub, /*only_one*/0);
            if (r || only_one) break;
        }
        else if (!comp) {
            /* no comp means we're at the top level, haven't seen a BEGIN! */
            state->itemstart = cardstart;
            r = PE_MISMATCHED_CARD;
            break;
        }
        else if (vcardproperty_isa(state->prop) == VCARD_END_PROPERTY) {
            const char *val =
                vcardvalue_get_text(vcardproperty_get_value(state->prop));
            vcardcomponent_kind kind = vcardcomponent_string_to_kind(val);

            if (kind != vcardcomponent_isa(comp)) {
                /* special case mismatched card, the "start" was the start of
                 * the card */
                state->itemstart = cardstart;
                r = PE_MISMATCHED_CARD;
            }

            break;
        }
        else {
            vcardcomponent_add_property(comp, state->prop);
            state->prop = NULL;
        }
    }

    if (state->prop) {
        if (vcardproperty_isa(state->prop) != VCARD_END_PROPERTY) {
            r = PE_FINISHED_EARLY;
        }

        vcardproperty_free(state->prop);
        state->prop = NULL;
    }

    return r;
}

static int vcardparser_parse(struct vcardparser_state *state, int only_one)
{
    state->root = vcardcomponent_new(VCARD_XROOT_COMPONENT);

    state->p = state->base;

    buf_init(&state->buf, BUF_GROW);

    /* don't parse trailing non-whitespace */
    return _parse_vcard(state, state->root, only_one);
}

/* FREE MEMORY */

static void _free_state(struct vcardparser_state *state)
{
    buf_free(&state->buf);
    buf_free(&state->errbuf);

    if (state->root) vcardcomponent_free(state->root);

    memset(state, 0, sizeof(struct vcardparser_state));
}

static void vcardparser_free(struct vcardparser_state *state)
{
    _free_state(state);
}

/* PUBLIC API */

const char *vcardparser_errstr(int err)
{
    switch(err) {
    case PE_BACKQUOTE_EOF:
        return "EOF after backslash";
    case PE_BEGIN_PARAMS:
        return "Params on BEGIN field";
    case PE_PROP_MULTIGROUP:
        return "Multiple group levels in property name";
    case PE_FINISHED_EARLY:
        return "vCard not completed";
    case PE_KEY_EOF:
        return "End of data while parsing parameter key";
    case PE_KEY_EOL:
        return "End of line while parsing parameter key";
    case PE_MISMATCHED_CARD:
        return "Closed a different card name than opened";
    case PE_NAME_EOF:
        return "End of data while parsing property name";
    case PE_NAME_EOL:
        return "End of line while parsing property name";
    case PE_NAME_INVALID:
        return "Invalid property name";
    case PE_PARAMVALUE_EOF:
        return "End of data while parsing parameter value";
    case PE_PARAMVALUE_EOL:
        return "End of line while parsing parameter value";
    case PE_QSTRING_EOF:
        return "End of data while parsing quoted value";
    case PE_QSTRING_EOL:
        return "End of line while parsing quoted value";
    case PE_VALUE_INVALID:
        return "Invalid value for property";
    case PE_ILLEGAL_CHAR:
        return "Illegal character in vCard";
    }
    return "Unknown error";
}

vcardcomponent *vcardparser_parse_string(const char *str)
{
    struct vcardparser_state parser;
    vcardcomponent *vcard = NULL;
    int r;

    memset(&parser, 0, sizeof(struct vcardparser_state));

    parser.base = str;
    r = vcardparser_parse(&parser, 0);
    if (!r) {
        if (vcardcomponent_count_components(parser.root,
                                            VCARD_VCARD_COMPONENT) == 1) {
            vcard = vcardcomponent_get_first_component(parser.root,
                                                       VCARD_VCARD_COMPONENT);
            vcardcomponent_remove_component(parser.root, vcard);
        }
        else {
            vcard = parser.root;
            parser.root = NULL;
        }
    }

    vcardparser_free(&parser);

    return vcard;
}
