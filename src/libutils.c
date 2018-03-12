/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/**
 * @file libutils.c
 * @brief Generic utility functions (implementation).
 *
 * Libutils contains several unrelated utility functions. For example
 * there are type conversion functions, date parsing and xml attribute
 * reader functions.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>

#include "libutils.h"

/**
 * @brief strptime extern declaration (sometimes it's not imported from time.h).
 *
 * I'm still unsure how to enable strptime in MSYS2 (in Linux I define
 * __USE_XOPEN before importing time.h), so I just declare the function here.
 */
extern char *strptime(__const char *__restrict __s, __const char *__restrict __fmt, struct tm *__tp);

/**
 * @brief Safe implementation of strtol.
 *
 * This strtol wrapper checks for strings that cannot be converted
 * and coerces the result from long to int making sure that it stays
 * between INT_MAX and INT_MIN.
 *
 * @see https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c/7021750#7021750
 */
str2int_errno str2int(int *out, char *s, int base) {
	char *end;
	if (s[0] == '\0' || isspace((unsigned char)s[0])) {
		return STR2INT_INCONVERTIBLE;
	}
	errno = 0;
	long l = strtol(s, &end, base);
	// Both checks are needed because INT_MAX == LONG_MAX is possible.
	if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX)) {
		return STR2INT_OVERFLOW;
	}
	if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN)) {
		return STR2INT_UNDERFLOW;
	}
	if (*end != '\0') {
		return STR2INT_INCONVERTIBLE;
	}
	*out = l;
	return STR2INT_SUCCESS;
}

/**
 * @brief Safe implementation of strtod.
 *
 * This strtod wrapper checks for strings that cannot be converted
 * and avoids over/underflow errors.
 */
str2double_errno str2double(double *out, char *s) {
	char *end;
	if (s[0] == '\0' || isspace((unsigned char)s[0])) {
		return STR2DOUBLE_INCONVERTIBLE;
	}
	errno = 0;
	double d = strtod(s, &end);
	if (errno == ERANGE && d == DBL_MAX) {
		return STR2DOUBLE_OVERFLOW;
	}
	if (errno == ERANGE && d == DBL_MIN) {
		return STR2DOUBLE_UNDERFLOW;
	}
	if (*end != '\0') {
		return STR2DOUBLE_INCONVERTIBLE;
	}
	*out = d;
	return STR2DOUBLE_SUCCESS;
}

/**
 * @brief xmlGetProp wrapper that converts the property to int.
 *
 * xmlGetProp returns a raw xmlChar *. This function uses str2int
 * to convert the result. If the result cannot be converted it
 * just returns INT_MIN. While this can be a nuance (in effect
 * I can't distinguish between INT_MIN values and wrong strings)
 * I still prefer to just return an int instead of using an output
 * parameter or returning a struct.
 */
int xmlGetPropInt(xmlNode *node, char *property) {
	char *str = (char *)xmlGetProp(node, (const xmlChar *)property);
	int i;
	if (str2int(&i, str, 10) != STR2INT_SUCCESS) {
		i = INT_MIN;
	}
	g_free(str);
	return i;
}

/**
 * @brief xmlGetProp wrapper that converts the property to double.
 *
 * xmlGetProp returns a raw xmlChar *. This function uses str2double
 * to convert the result. If the result cannot be converted it
 * just returns DBL_MIN. While this can be a nuance (in effect
 * I can't distinguish between DBL_MIN values and wrong strings)
 * I still prefer to just return a double instead of using an output
 * parameter or returning a struct.
 */
double xmlGetPropDouble(xmlNode *node, char *property) {
	char *str = (char *)xmlGetProp(node, (const xmlChar *)property);
	double d;
	if (str2double(&d, str) != STR2DOUBLE_SUCCESS) {
		d = DBL_MIN;
	}
	g_free(str);
	return d;
}

/**
 * @brief Parse a string into a GDateTime according to the specified format (assumes that the string represents local time).
 *
 * Looks like GLib doesn't have a function that parses a string into a
 * GDateTime according to a specific format, there's only a slightly
 * obscure function g_date_set_parse which doesn't even take a format
 * and handles only a GDate. For this reason I've implemented the parsing
 * here using strptime -> mktime -> g_date_time_new_from_unix_local.
 */
GDateTime *parseDateTime(char *str, char *format) {
	// I initialize the struct to not make Valgrind complain.
	// Seems the most harmless and clean thing in the world, right?
	// Well yes, in this case it is but sometimes it isn't...
	// https://www.schneier.com/blog/archives/2008/05/random_number_b.html
	// https://www.xkcd.com/424/
	struct tm time_struct = {0};
	strptime(str, format, &time_struct);
	time_t time_value = mktime(&time_struct);
	return g_date_time_new_from_unix_local(time_value);
}

/**
 * @brief Check if the NULL-terminated string is an integer.
 *
 * Go through the string with pointer arithmetic and stop at
 * the first non-digit character, if any.
 */
gboolean is_number(char *str) {
	for (size_t i = 0, size = strlen(str); i < size; ++i) {
		if (!g_ascii_isdigit(str[i])) {
			return FALSE;
		}
	}
	return TRUE;
}
