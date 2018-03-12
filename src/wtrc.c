/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/**
 * @file wtrc.c
 * @brief Command line tool to get weather forecasts.
 *
 * This file contains the main function of wtrc; it parses command line
 * switches and runs the appropriate routines.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <time.h>

#include <curl/curl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <libxml/parser.h>

#include "libnet.h"
#include "libutils.h"
#include "libweather.h"
#include "libweather_tiempo.h"

/// Argument of the --search (-s) command line option, used to search for a location.
static gchar *opt_search = NULL;
/// Argument of the --location (-l) command line option, used to get weather forecasts (it can be the location code or the exact location
/// name).
static gchar *opt_location = NULL;
/// When false, only daily forecasts will be shown. When true, hourly forecasts will be shown as well.
static gboolean opt_hour = FALSE;

/// Command line switches configuration for g_option.
static GOptionEntry opt_entries[] = {{"search", 's', 0, G_OPTION_ARG_STRING, &opt_search, "Search a location whose name contains L", "L"},
                                     {"location", 'l', 0, G_OPTION_ARG_STRING, &opt_location,
                                      "Get weather forecasts for the location L (location code or name, if unique)", "L"},
                                     {"hour", 'h', 0, G_OPTION_ARG_NONE, &opt_hour, "Show hourly forecast", NULL},
                                     {NULL}};

/**
 * @brief Search a location by name.
 *
 * This function searches all locations that match, exactly or partially,
 * the given query, and shows the results on the screen.
 *
 * @param[in] query The search query.
 */
void search_location(char *query) {
	int locations = sizeof(WTR_LOCATIONS) / sizeof(wtr_location);
	GList *results = wtr_location_search(query, WTR_SEARCH_LOCATION_PARTIAL_NAME);
	int count = 0;
	for (GList *location = results; location != NULL; location = location->next) {
		wtr_location_print(*((wtr_location *)location->data));
		printf("\n");
		++count;
	}
	g_print("%d location%s found (%d locations available).\n\n", count, count != 1 ? "s" : "", locations);
	g_list_free(results);
}

/**
 * @brief Show the forecasts for the location on the screen.
 *
 * This function searches for a location by its code or exact name. If a
 * matching location is found, its weather forecasts will be shown on the
 * screen.
 *
 * @param[in] query Location code or exact name.
 */
void get_forecasts(char *query) {
	wtr_location *location = NULL;
	// If the query is not a number, assume that it's a location name
	wtr_location_search_type search_type;
	char *search_attribute;
	if (is_number(query)) {
		search_type = WTR_SEARCH_LOCATION_EXACT_CODE;
		search_attribute = "code";
	} else {
		search_type = WTR_SEARCH_LOCATION_EXACT_NAME;
		search_attribute = "name";
	}
	GList *results = wtr_location_search(query, search_type);
	GList *first = g_list_first(results);
	if (first == NULL) {
		g_print("Location with %s '%s' not found.\n", search_attribute, query);
		return;
	} else {
		location = (wtr_location *)first->data;
	}
	g_list_free(results);
	g_print("Weather forecasts for %s (%s)\n\n", location->name, location->province);
	wtr_forecast *forecast = wtr_tiempo_forecast_get(location->code);
	wtr_forecast_print(forecast, opt_hour);
	wtr_forecast_free(forecast);
}

/**
 * @brief Simple Tiempo weather forecast client.
 *
 * This command line client for Tiempo weather forecasts API allows to search
 * for a supported location (--search option) and to get weather forecasts
 * (--location option).
 *
 * @param[in] argc Command line arguments number (including the executable name).
 * @param[in] argv Command line arguments values (including the executable name).
 */
int main(int argc, char *argv[]) {
	int exit_status = EXIT_SUCCESS;
	GError *error = NULL;
	GOptionContext *context;
	context = g_option_context_new("- get weather forecasts");
	g_option_context_add_main_entries(context, opt_entries, NULL);
	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_printerr("Option parsing failed: %s\n", error->message);
		exit_status = EXIT_FAILURE;
		goto clean_and_exit;
	}
	if (opt_search == NULL && opt_location == NULL) {
		g_printerr("Incorrect usage, try --help.\n");
		exit_status = EXIT_FAILURE;
		goto clean_and_exit;
	}

	// Initialize the library and check potential ABI mismatches between
	// the version it was compiled for and the actual shared library used.
	LIBXML_TEST_VERSION
	xmlInitParser();
	CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (code == 0) {
		// test_libweather();
		if (opt_search != NULL) {
			search_location(opt_search);
		} else if (opt_location != NULL) {
			get_forecasts(opt_location);
		}
		curl_global_cleanup();
	} else {
		g_printerr("ERR: libcurl initialization failed\n");
	}
	// Free the global variables that may have been allocated by the parser.
	xmlCleanupParser();

clean_and_exit:
	g_option_context_free(context);
	return exit_status;
}
