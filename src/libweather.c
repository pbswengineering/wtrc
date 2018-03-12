/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/**
 * @file libweather.c
 * @brief Standardised data structures and functions for weather forecasts (implementation).
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "libnet.h"
#include "libutils.h"
#include "libweather.h"

/**
 * @brief Pretty-prints a location.
 *
 * The location is printed on the screen as a list of attributes.
 */
void wtr_location_print(wtr_location location) {
	printf("Location   : %s (%s)\n", location.name, location.province);
	printf("Coordinates: %f, %f\n", location.latitude, location.longitude);
	printf("Code       : %s\n", location.code);
}

/**
 * @brief Searches a location by partial name, exact name or exact code.
 *
 * This is a linear search through WTR_LOCATIONS.
 * This kind of search is quite fast even if the locations are more than 8000.
 */
GList *wtr_location_search(gchar *query, wtr_location_search_type search_type) {
	int count = sizeof(WTR_LOCATIONS) / sizeof(wtr_location);
	GList *list = NULL;
	gchar *nameUpper = g_utf8_strup(query, -1);
	for (int i = 0; i < count; ++i) {
		if ((search_type == WTR_SEARCH_LOCATION_PARTIAL_NAME && strstr(WTR_LOCATIONS[i].name, nameUpper) != NULL) ||
		    (search_type == WTR_SEARCH_LOCATION_EXACT_NAME && g_strcmp0(WTR_LOCATIONS[i].name, nameUpper) == 0) ||
		    (search_type == WTR_SEARCH_LOCATION_EXACT_CODE && g_strcmp0(WTR_LOCATIONS[i].code, nameUpper) == 0)) {
			list = g_list_append(list, (gpointer)&WTR_LOCATIONS[i]);
		}
	}
	g_free(nameUpper);
	return list;
}

/**
 * @brief Creates a new wtr_forecast on the heap.
 *
 * The wtr_forecast is created with an empty list of daily forecasts.
 *
 * @warning The wtr_forecast must be freed with wtr_forecast_free().
 */
wtr_forecast *wtr_forecast_init() {
	wtr_forecast *forecast = (wtr_forecast *)g_malloc(sizeof(wtr_forecast));
	forecast->days = NULL;
	return forecast;
}

/**
 * @brief frees a wtr_forecast pointer, as created by wtr_forecast_init().
 *
 * The wtr_forecast pointer must be freed with this function, which frees both
 * the heap-allocated values contained in the structure and the pointer itself.
 *
 * @warning This function must be called on wtr_forecast pointers instread of @c free.
 */
void wtr_forecast_free(wtr_forecast *forecast) {
	for (GList *day_ptr = forecast->days; day_ptr != NULL; day_ptr = day_ptr->next) {
		wtr_forecast_day *day = (wtr_forecast_day *)day_ptr->data;
		for (GList *hour_ptr = day->hours; hour_ptr != NULL; hour_ptr = hour_ptr->next) {
			wtr_forecast_hour *hour = (wtr_forecast_hour *)hour_ptr->data;
			g_date_time_unref(hour->tstamp);
			g_free(hour->wind_dir);
			g_free(hour);
		}
		g_date_time_unref(day->date);
		g_list_free(day->hours);
		g_free(day);
	}
	g_list_free(forecast->days);
	g_free(forecast);
}

/**
 * @brief Pretty-prints a wtr_forecast.
 *
 * This function prints on the screen a wtr_forecast, including daily and hourly
 * details.
 */
void wtr_forecast_print(wtr_forecast *forecast, gboolean details) {
	printf("Date   Min (°) Max (°) Humidity (%%) Wind(km/h) Weather\n");
	printf("----   ------- ------- ------------ ---------- -------\n");
	for (GList *day_ptr = forecast->days; day_ptr != NULL; day_ptr = day_ptr->next) {
		wtr_forecast_day *day = (wtr_forecast_day *)day_ptr->data;
		gchar *date_str = g_date_time_format(day->date, "%a %e");
		printf("%s %7d %7d %12d %10d %s\n", date_str, day->temp_min, day->temp_max, day->humidity, day->wind_speed,
		       wtr_weather_description(day->weather));
		g_free(date_str);
	}
	if (details) {
		for (GList *day_ptr = forecast->days; day_ptr != NULL; day_ptr = day_ptr->next) {
			wtr_forecast_day *day = (wtr_forecast_day *)day_ptr->data;
			gchar *date_str = g_date_time_format(day->date, "%A, %e %B");
			printf("\n\n%s\n\n", date_str);
			g_free(date_str);
			printf("Time  Temp (°) Weather\n");
			printf("----  -------- -------\n");
			for (GList *hour_ptr = day->hours; hour_ptr != NULL; hour_ptr = hour_ptr->next) {
				wtr_forecast_hour *hour = (wtr_forecast_hour *)hour_ptr->data;
				gchar *tstamp_str = g_date_time_format(hour->tstamp, "%H:%M");
				printf("%s %8d %s\n", tstamp_str, hour->temp, wtr_weather_description(hour->weather));
				g_free(tstamp_str);
			}
		}
	}
}

/**
 * @brief Returns an intelligible description for the weather condition.
 *
 * The returned string must not be deallocated since it's a pointer to a
 * constant string.
 */
const gchar *wtr_weather_description(gint weather) {
	switch (weather) {
		case WTR_CLEAR:
			return "Clear";
		case WTR_SCATTERED_CLOUDS:
			return "Scattered clouds";
		case WTR_CLOUDY:
			return "Cloudy";
		case WTR_OVERCAST:
			return "Overcast";
		case WTR_SCATTERED_CLOUDS_LIGHT_RAIN:
			return "Scattered clouds with light rain";
		case WTR_CLOUDY_LIGHT_RAIN:
			return "Cloudy with light rain";
		case WTR_OVERCAST_LIGHT_RAIN:
			return "Overcast with light rain";
		case WTR_SCATTERED_CLOUDS_MODERATE_RAIN:
			return "Scattered clouds with moderate rain";
		case WTR_CLOUDY_MODERATE_RAIN:
			return "Cloudy with moderate rain";
		case WTR_OVERCAST_MODERATE_RAIN:
			return "Overcast with moderate rain";
		case WTR_SCATTERED_CLOUDS_THUNDERSTORM:
			return "Scattered clouds with thunderstorms";
		case WTR_CLOUDY_THUNDERSTORM:
			return "Cloudy with thunderstorms";
		case WTR_OVERCAST_THUNDERSTORM:
			return "Overcast with thunderstorms";
		case WTR_SCATTERED_CLOUDS_THUNDERSTORM_HAIL:
			return "Scattered clouds with thunderstorms and hailstorms";
		case WTR_CLOUDY_THUNDERSTORM_HAIL:
			return "Cloudy with thunderstorms and hailstorms";
		case WTR_OVERCAST_THUNDERSTORM_HAIL:
			return "Overcast with thunderstorms and hailstorms";
		case WTR_SCATTERED_CLOUDS_SNOW:
			return "Scattered clouds with snow";
		case WTR_CLOUDY_SNOW:
			return "Cloudy with snow";
		case WTR_OVERCAST_SNOW:
			return "Overcast with snow";
		case WTR_SCATTERED_CLOUDS_SLEET:
			return "Scattered clouds with sleet";
		case WTR_CLOUDY_SLEET:
			return "Cloudy with sleet";
		case WTR_OVERCAST_SLEET:
			return "Overcast with sleet";
		default:
			return "Unknown";
	}
}
