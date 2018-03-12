/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __LIBUTILS_H__
#define __LIBUTILS_H__

/**
 * @file libutils.h
 * @brief Generic utility functions.
 *
 * Libutils contains several unrelated utility functions. For example
 * there are type conversion functions, date parsing and xml attribute
 * reader functions.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

/**
 * @brief Possible outcomes of the str2int function.
 *
 * The str2int function can have several results: success or error, where
 * the error can be of overflow, underflow or just a string that cannot
 * be converted.
 */
typedef enum {
	/** The conversion was successful. */
	STR2INT_SUCCESS,
	/** The conversion would result in a number bigger than the biggest integer. */
	STR2INT_OVERFLOW,
	/** The conversion would result in a number smaller than the smallest integer. */
	STR2INT_UNDERFLOW,
	/** The string doesn't represent a correct number. */
	STR2INT_INCONVERTIBLE
} str2int_errno;

/**
 * @brief Converts a string to an integer, safely.
 *
 * This function converts a string to an integer. It is a wrapper
 * to strtol that checks for badly formatted strings and over/underflow.
 *
 * @param[out] out The converted integer will be stored here.
 * @param[in] s String to convert.
 * @param[in] base Integer base for the conversion (same range as strtol, 2 to 36)
 * @return STR2INT_SUCCESS if the conversion was done or a value that explain why the conversion was impossible.
 */
str2int_errno str2int(int *out, char *s, int base);

/**
 * @brief Possible outcomes of the str2double function.
 *
 * The str2double function can have several results: success or error, where
 * the error can be of overflow, underflow or just a string that cannot
 * be converted.
 */
typedef enum {
	/** The conversion was successful. */
	STR2DOUBLE_SUCCESS,
	/** The conversion would result in a number bigger than the biggest integer. */
	STR2DOUBLE_OVERFLOW,
	/** The conversion would result in a number smaller than the smallest integer. */
	STR2DOUBLE_UNDERFLOW,
	/** The string doesn't represent a correct number. */
	STR2DOUBLE_INCONVERTIBLE
} str2double_errno;

/**
 * @brief Converts a string to a double, safely.
 *
 * This function converts a string to a double. It is a wrapper
 * to strtod that checks for badly formatted strings and over/underflow.
 *
 * @param[out] out The converted double will be stored here.
 * @param[in] s String to convert.
 * @return STR2DOUBLE_SUCCESS if the conversion was done or a value that explains why the conversion was impossible.
 */
str2double_errno str2double(double *out, char *s);

/**
 * @brief Convert a property of an xmlNode to integer.
 *
 * This function converts the property of the node to an integer value.
 * It's an handy wrapper for libxml2's xmlGetProp. Internally it uses
 * str2int.
 *
 * @param[in] xmlNode XML node that contains the property.
 * @param[in] property Property whose value will ve converted to integer.
 * @return the property Value as integer or INT_MIN if the conversion was not possible.
 */
int xmlGetPropInt(xmlNode *node, char *property);

/**
 * @brief Convert a property of an xmlNode to double.
 *
 * This function converts the property of the node to a double value.
 * It's an handy wrapper for libxml2's xmlGetProp. Internally it uses
 * str2double.
 *
 * @param[in] xmlNode XML node that contains the property.
 * @param[in] property Property whose value will ve converted to double.
 * @return The property value as doule or DBL_MIN if the conversion was not possible.
 */
double xmlGetPropDouble(xmlNode *node, char *property);

/**
 * @brief Convert a string to a date according to a format (assumes that the strings represents a local time).
 *
 * This function converts a string to a GLib's GDateTime according to
 * the specified format. Internally the function uses strptime.
 * For example, to parse an ISO8601 timestamp:
 * @code{.c}
 * char *str = "2018-03-09T00:00:00.000";
 * GDateTime *tstamp = parseDateTime(str, "%FT%T");
 * // ...
 * g_date_time_unref(tstamp);
 * @endcode
 *
 * @param[in] str String to be converted.
 * @param[in] format Date and time format to be used for the conversion (the same as strptime).
 * @return Pointer to the GDateTime represented by the input string or NULL if the conversion was not possible.
 * @warning The GDateTime returned by this function must be deallocated by the caller.
 */
GDateTime *parseDateTime(char *str, char *format);

/**
 * @brief Check if the string is an integer number.
 *
 * This function checks if the given string represents an integer number
 * (this is true if all string characters are digits).
 *
 * @param[in] str NULL-terminated string to be checked.
 * @return True if the string is a number, false otherwise.
 */
gboolean is_number(char *str);

#endif  // __LIBUTILS_H__
