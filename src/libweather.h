/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __LIBWEATHER_H__
#define __LIBWEATHER_H__

/**
 * @file libweather.h
 * @brief Standardised data structures and functions for weather forecasts.
 *
 * Libweather provides data structures that represents weather forecasts
 * (daily and hourly) for a specified location and functions to handle
 * such structures. It also provides a struct to represent physical locations
 * and their code for weather forecasts services and functions to search
 * through them.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#include <stdint.h>

#include <glib.h>

/**
 * @brief Location search types.
 *
 * The wtr_location_search() functio can search locations in three ways:
 * by matching part of the location name, by matching exactly the location name
 * and by matching exactly the location code.
 */
typedef enum {
	/** The search query must match all of the location name or part of it. The search is not case sensitive. */
	WTR_SEARCH_LOCATION_PARTIAL_NAME,
	/** The search query must match exactly the location name. The search is not case sensitive. */
	WTR_SEARCH_LOCATION_EXACT_NAME,
	/** The search query must match exactly the location code. */
	WTR_SEARCH_LOCATION_EXACT_CODE,
} wtr_location_search_type;

/// Placeholder for an undefined weather condition, though it can actually be any number outside the @c WTR_* constant range.
#define WTR_UNDEFINED 0
/// Clear skies.
#define WTR_CLEAR 1
/// Scattered clouds.
#define WTR_SCATTERED_CLOUDS 2
/// Cloudy conditions mean at least 50 percent of the sky will be covered by clouds – there will be more clouds than sunshine.
#define WTR_CLOUDY 3
/// An overcast sky is usually a dull and gray-looking sky when clouds are expected to cover all of the surrounding area.
#define WTR_OVERCAST 4
/// Scattered clouds and light rain.
#define WTR_SCATTERED_CLOUDS_LIGHT_RAIN 5
/// Cloudy sky and light rain.
#define WTR_CLOUDY_LIGHT_RAIN 6
/// Overcast sky and light rain.
#define WTR_OVERCAST_LIGHT_RAIN 7
/// Scattered clouds and moderate rain.
#define WTR_SCATTERED_CLOUDS_MODERATE_RAIN 8
/// Cloudy sky and moderate rain.
#define WTR_CLOUDY_MODERATE_RAIN 9
/// Overcast sky and moderate rain.
#define WTR_OVERCAST_MODERATE_RAIN 10
/// Scattered clouds and thunderstorm.
#define WTR_SCATTERED_CLOUDS_THUNDERSTORM 11
/// Cloudy sky and thunderstorm.
#define WTR_CLOUDY_THUNDERSTORM 12
/// Overcast sky and thunderstorm.
#define WTR_OVERCAST_THUNDERSTORM 13
/// Scattered clouds with thunderstorms and hail.
#define WTR_SCATTERED_CLOUDS_THUNDERSTORM_HAIL 14
/// Cloudy sky with thunderstorms and hail.
#define WTR_CLOUDY_THUNDERSTORM_HAIL 15
/// Overcast sky with thunderstorms and hail.
#define WTR_OVERCAST_THUNDERSTORM_HAIL 16
/// Scattered clouds with snow.
#define WTR_SCATTERED_CLOUDS_SNOW 17
/// Cloudy sky with snow.
#define WTR_CLOUDY_SNOW 18
/// Overcast sky with snow.
#define WTR_OVERCAST_SNOW 19
/// Scattered clouds with sleet
#define WTR_SCATTERED_CLOUDS_SLEET 20
/// Cloudy sky wth sleet
#define WTR_CLOUDY_SLEET 21
/// Overcast sky with sleet
#define WTR_OVERCAST_SLEET 22

/**
 * Rappresenta una località italiana con tanto di nome, provincia (targa),
 * latitudine e longitudine (WGS84) e codice per le previsioni meteo Tiempo.
 */
/**
 * @brief Italian location with Tiempo's API code.
 *
 * Each location has a name (it's supposed to be upper case, included
 * the letter with accents), a province, WGS84 latitude and longitude
 * and Tiempo's API code.
 */
typedef struct {
	/// Name of the location (it's supposed to be upper case).
	gchar *name;
	/// Italian province of the location (2 letter code).
	gchar *province;
	/// WGS84 latitude.
	double latitude;
	/// WGS84 longitude.
	double longitude;
	/// Tiempo API's location code.
	gchar *code;
} wtr_location;

/**
 * @brief Hourly forecast.
 *
 * This hourly forecast can actually span a 3 hour period for
 * dates further than the 2 next days.
 */
typedef struct {
	/// Forecast beginning date and time.
	GDateTime *tstamp;
	/// Weather code (see the constants @c WTR_*).
	gint weather;
	/// Temperature, in Celsius degrees.
	gint temp;
	/// Wind speed, in km/h.
	gint wind_speed;
	/// Wind direction: N, E, S, O or combinations of 2 cardinal points.
	gchar *wind_dir;
	/// Rain level, in mm.
	gdouble rain;
	/// Humidity percentage.
	gint humidity;
	/// Pressure, in mb.
	gint pressure;
} wtr_forecast_hour;

/**
 * @brief Daily forecast.
 *
 * A daily forecast contains a daily summary and a list of hourly details.
 * Hourly details can actually be sampled ad 3 hours interval or something
 * like that, especially for days further than the next 2 days.
 */
typedef struct {
	/// Forecast date.
	GDateTime *date;
	/// Weather code (see the constants @c WTR_*).
	gint weather;
	/// Minimum emperature, in Celsius degrees.
	gint temp_min;
	/// Maximum emperature, in Celsius degrees.
	gint temp_max;
	/// Wind speed, in km/h.
	gint wind_speed;
	/// Rain level, in mm.
	gdouble rain;
	/// Humidity percentage.
	gint humidity;
	/// Pressure, in mb.
	gint pressure;
	/// Hourly forecasts for the day.
	GList *hours;
} wtr_forecast_day;

/**
 * @brief Weather forecasts for a location.
 *
 * The weather forecasts for a location contain a list of daily forecasts,
 * which in turn contain a list of hourly forecasts for each day.
 */
typedef struct { GList *days; } wtr_forecast;

/**
 * The location database has been placed in a separate header due to its size.
 */
#include "libweather_locations.h"

/**
 * @brief Pretty-prints a location on the screen.
 *
 * This function prints a location on the screen with a pretty format.
 */
void wtr_location_print(wtr_location location);

/**
 * @brief Search a location by name.
 *
 * This function searches an Italian location that can be used to get
 * weather forecasts based on its name. The search can return zero or
 * more results.
 *
 * @param[in] query Search query (the meaning of it depends on @p search_type)
 * @param[in] search_type Search criterion.
 * @return A list of locations (zero or more) that match that name.
 * @warning The returned GList must be freed with g_list_free() but the individual GList elements must not be freed.
 */
GList *wtr_location_search(gchar *name, wtr_location_search_type search_type);

/**
 * @brief Initializes a wtr_forecast struct.
 *
 * This function must be called before to get a pointer to a propertly initialized
 * wtr_forecast struct.
 *
 * @return a new wtr_forecast structure.
 * @warn The caller must free the wtr_forecast struct with wtr_forecast_free().
 */
wtr_forecast *wtr_forecast_init();

/**
 * Stampa le previsioni meteo a schermo.
 */
/**
 * @brief Pretty-prints the forecasts on the screen.
 *
 * This function prints the wtr_forecast on the screen in a pretty format.
 *
 * @param[in] forecast Weather forecast to print.
 * @param[in] details If true print the hourly details.
 */
void wtr_forecast_print(wtr_forecast *forecast, gboolean details);

/**
 * @brief Frees a wtr_forecast pointer.
 *
 * This function must be used to free a wtr_forecast pointer (as returned by
 * wtr_forecast_init(), for example) instead of @c free or @c g_free.
 *
 * @param[in,out] wtr_forecast The strcut to free. It must be a pointer to an heap-allocated variable.
 * @warn This fucntion frees the wtr_forecast pointer itself, after freeing its content.
 */
void wtr_forecast_free(wtr_forecast *forecast);

/**
 * @brief Return a description of the weather condition.
 *
 * This function returns a intelligible description for the specified
 * weather condition code.
 *
 * @param[in] weather Weather condition code (see the @c WTR_* constants).
 * @return An intelligible description for the weather conditions.
 */
const gchar *wtr_weather_description(gint weather);

#endif  // __LIBWEATHER_H__
