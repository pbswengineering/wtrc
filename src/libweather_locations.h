/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/**
 * @file libweather_locations.h
 * @brief List of Italian locations supported by libweather.
 *
 * This file contains a huge constant array with the list of all
 * Italian locations whose weather forecasts can be downloaded by
 * libweather.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#ifndef __LIBWEATHER_LOCATIONS_H__
#define __LIBWEATHER_LOCATIONS_H__

/**
 * @brief List of Italian locations with their Tiempo weather forecast code.
 *
 * List of Italian locations whose weather forecasts can be downloaded by libweather.
 * Every location has been associated a WGS84 coordinate.
 */
static const wtr_location WTR_LOCATIONS[] = {
    {.name = "ACQUASPARTA", .province = "TR", .latitude = 42.6911449, .longitude = 12.5464788, .code = "28756"},
    {.name = "MONTECASTRILLI", .province = "TR", .latitude = 42.652434, .longitude = 12.488567, .code = "30429"},
    {.name = "ORVIETO", .province = "TR", .latitude = 42.7186152, .longitude = 12.1087907, .code = "30625"},
    {.name = "TERNI", .province = "TR", .latitude = 42.5641417, .longitude = 12.6405466, .code = "31553"},
    {.name = "PERUGIA", .province = "PG", .latitude = 43.1119613, .longitude = 12.3890104, .code = "30721"}};

#endif  // __LIBWEATHER_LOCATIONS_H__
