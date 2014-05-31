#ifdef WIN32

#ifndef roundup
#define roundup(x, y) ((((x)+((y)-1))/(y))*(y))
#endif

#include <stdlib.h>
#include <stdarg.h>

int
#if __STDC__
vsnprintf(char *str, size_t n, char const *fmt, va_list ap);
#else
vsnprintf(str, n, fmt, ap);
	char *str;
	size_t n;
	char *fmt;
	char *ap;
#endif


int
#if __STDC__
snprintf(char *str, size_t n, char const *fmt, ...);
#else
snprintf(str, n, fmt, va_alist);
	char *str;
	size_t n;
	char *fmt;
	va_dcl
#endif

#endif
