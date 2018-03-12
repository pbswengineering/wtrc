/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __LIB_WEATHER_TIEMPO_H__
#define __LIB_WEATHER_TIEMPO_H__

#include <glib.h>
#include <glib/gprintf.h>

/**
 * @file libweather_tiempo.h
 * @brief Tiempo (ilmeteo.net) "driver" for libweather.
 *
 * Tiempo (ilmeteo.net) provides a nice API to get weather forecasts.
 * This "driver" provides a call to get Tiempo forecasts in libweather
 * format.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

/// Name of the libweather "driver" for Tiempo (ilmeteo.net).
#define WTR_DRIVER_TIEMPO "tiempo"

/**
 * @brief Get the Tiempo weather forecast and returns them in libweather format.
 *
 * This function gets the Tiempo (ilmeteo.net) forecasts using their API and
 * returns them as a wtr_forecast structure, so that they can used by libweather's
 * functions. The forecasts, for the next 5 days including the current one, contain
 * daily summaries and hourly details (the first 2 days have hour-by-hour details
 * while the next 3 days details refer to a 3 hour interval).
 *
 * @param[in] code Tiempo location code.
 * @return Weather forecasts as wtr_forecast, with daily and hourly forecasts for the next 5 days.
 * @warning The caller has the responsibility to free the returned forecasts by calling wtr_forecast_free()
 */
wtr_forecast *wtr_tiempo_forecast_get(gchar *code);

#endif  // #define __LIB_WEATHER_TIEMPO_H__
