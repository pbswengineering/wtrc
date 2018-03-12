/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/**
 * @file libweather_cache.c
 * @brief Filesystem-based cache for libweather "drivers" (implementation).
 *
 * This cache allows libweather "drivers" to save their forecasts on a
 * temporary filesystem area and to retrieve forecasts for the same
 * location and the same day without having to issue a network call twice.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#include <glib.h>
#include <glib/gprintf.h>

#include "libweather_cache.h"

/// Maximum length for a driver cache directory name.
#define MAX_WTR_CACHE_TEMP_DIR_LENGTH 1024

gchar *wtr_cache_dir() {
	// e.g. /tmp
	const gchar *tmp_dir = g_get_tmp_dir();
	gchar *cache_dir = (gchar *)g_malloc(sizeof(gchar) * MAX_WTR_CACHE_TEMP_DIR_LENGTH);
	// e.g. /tmp/libweather
	g_snprintf(cache_dir, MAX_WTR_CACHE_TEMP_DIR_LENGTH, "%s/libweather", tmp_dir);
	// If the directory doesn't exist, try to create it
	if (!g_file_test(cache_dir, G_FILE_TEST_IS_DIR)) {
		g_mkdir_with_parents(cache_dir, 0755);
	}
	return cache_dir;
}

gchar *wtr_cache_dir_today() {
	// e.g. /tmp/libweather
	gchar *cache_dir = wtr_cache_dir();
	GDateTime *today = g_date_time_new_now_local();
	gchar *today_str = g_date_time_format(today, "%Y%m%d");
	gchar *cache_dir_today = (gchar *)g_malloc(sizeof(gchar) * MAX_WTR_CACHE_TEMP_DIR_LENGTH);
	// e.g. /tmp/libweather/20180308
	g_snprintf(cache_dir_today, MAX_WTR_CACHE_TEMP_DIR_LENGTH, "%s/%s", cache_dir, today_str);
	// If the directory doesn't exist, try to create it
	if (!g_file_test(cache_dir_today, G_FILE_TEST_IS_DIR)) {
		g_mkdir_with_parents(cache_dir_today, 0755);
	}
	g_free(today_str);
	g_date_time_unref(today);
	g_free(cache_dir);
	return cache_dir_today;
}

gchar *wtr_cache_temp_file(gchar *driver, gchar *location_code) {
	// e.g. /tmp/libweather/20180308
	gchar *cache_dir_today = wtr_cache_dir_today();
	gchar *file = (gchar *)g_malloc(sizeof(gchar) * MAX_WTR_CACHE_TEMP_DIR_LENGTH);
	// e.g. /tmp/libweather/20180308/tiempo-1234546
	g_snprintf(file, MAX_WTR_CACHE_TEMP_DIR_LENGTH, "%s/%s-%s", cache_dir_today, driver, location_code);
	g_free(cache_dir_today);
	return file;
}

gchar *wtr_cache_get(gchar *driver, gchar *location_code) {
	gchar *file = wtr_cache_temp_file(driver, location_code);
	gchar *data = NULL;
	g_file_get_contents(file, &data, NULL, NULL);
	g_free(file);
	return data;
}

gchar *wtr_cache_set(gchar *driver, gchar *location_code, gchar *data) {
	gchar *file = wtr_cache_temp_file(driver, location_code);
	g_file_set_contents(file, data, -1, NULL);
	g_free(file);
	return NULL;
}
