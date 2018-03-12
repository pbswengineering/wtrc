/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __LIBWEATHER_CACHE_H__
#define __LIBWEATHER_CACHE_H__

#include <glib.h>

/**
 * @file libweather_cache.h
 * @brief Filesystem-based cache for libweather "drivers".
 *
 * This cache allows libweather "drivers" to save their forecasts on a
 * temporary filesystem area and to retrieve forecasts for the same
 * location and the same day without having to issue a network call twice.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

gchar *wtr_cache_get(gchar *driver, gchar *location_code);

gchar *wtr_cache_set(gchar *driver, gchar *location_code, gchar *data);

#endif  // __LIBWEATHER_CACHE_H__
