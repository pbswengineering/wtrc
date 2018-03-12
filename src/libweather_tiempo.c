/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/**
 * @file libweather_tiempo.c
 * @brief Tiempo (ilmeteo.net) "driver" for libweather (implementation).
 *
 * Tiempo (ilmeteo.net) provides a nice API to get weather forecasts.
 * This "driver" provides a call to get Tiempo forecasts in libweather
 * format.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#include <stdio.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "config.h"
#include "libnet.h"
#include "libutils.h"
#include "libweather.h"
#include "libweather_cache.h"
#include "libweather_tiempo.h"

/// Template URL for @c *printf to get forecasts for an Italian location; the location ID and the Affiliate ID must be provided via.
#define TIEMPO_URL_TEMPLATE "http://api.ilmeteo.net/index.php?api_lang=it&localidad=%s&affiliate_id=%s&v=2&h=1"
/// Maximum length for a Tiempo API's URL.
#define TIEMPO_URL_MAX_LENGTH 256

/**
 * @brief Return the Tiempo's API endpoint for the forecasts of the specified location.
 *
 * Tiempo API require a formatted url (see @c TIEMPO_URL_TEMPLATE). This function
 * returns the URL for the specified location assuming that it's an Italian location
 * and by using a fixed Affiliate ID (for API accounting and throttling).
 *
 * @param[in] code Tiempo location code.
 * @return URL to get the XML weather forecasts for the specified location.
 */
gchar *wtr_tiempo_forecast_url(gchar *code) {
	gchar *url = (gchar *)g_malloc(sizeof(gchar) * TIEMPO_URL_MAX_LENGTH);
	g_snprintf(url, TIEMPO_URL_MAX_LENGTH, TIEMPO_URL_TEMPLATE, code, TIEMPO_AFFILATE_ID);
	return url;
}

/**
 * @brief Parses an hourly forecast from Tiempo's XML and returns a wtr_forecast_hour.
 *
 * Tiempo API provides weather forecasts in XML format. This function converts the
 * specified hourly forecast (tag @c hour) to a libweather's wtr_forecast_hour struct.
 *
 * @param[in] xmlHour The @c hour tag (libxml2).
 * @param[in] day The wtr_forecast_day of the @c day tag that contains the @c hour tag to be parsed.
 * @return The hourly forecasts as a wtr_forecast_hour.
 * @warning The function assumes that the xmlNode refers to an @hour tag. The caller must ensure that the tag is correct.
 */
wtr_forecast_hour *wtr_forecast_parse_hour(xmlNode *xmlHour, wtr_forecast_day *day) {
	wtr_forecast_hour *hour = (wtr_forecast_hour *)g_malloc(sizeof(wtr_forecast_hour));
	hour->tstamp = NULL;
	hour->wind_dir = NULL;
	char *value = (char *)xmlGetProp(xmlHour, (const xmlChar *)"value");
	GDateTime *only_time = parseDateTime(value, "%H:%M");
	gint hh = g_date_time_get_hour(only_time);
	gint mm = g_date_time_get_minute(only_time);
	hour->tstamp = g_date_time_new_local(g_date_time_get_year(day->date), g_date_time_get_month(day->date),
	                                     g_date_time_get_day_of_month(day->date), hh, mm, 0);
	g_date_time_unref(only_time);
	g_free(value);
	for (xmlNode *child = xmlHour->children; child; child = child->next) {
		if (g_strcmp0((const char *)child->name, "symbol") == 0) {
			hour->weather = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "temp") == 0) {
			hour->temp = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "wind") == 0) {
			hour->wind_dir = (char *)xmlGetProp(child, (const xmlChar *)"dir");
			hour->wind_speed = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "rain") == 0) {
			hour->rain = xmlGetPropDouble(child, "value");
		} else if (g_strcmp0((const char *)child->name, "humidity") == 0) {
			hour->humidity = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "pressure") == 0) {
			hour->pressure = xmlGetPropInt(child, "value");
		}
	}
	return hour;
}

/**
 * @brief Parses a daily forecast from Tiempo's XML and returns a wtr_forecast_day.
 *
 * Tiempo API provides weather forecasts in XML format. This function converts the
 * specified daily forecast (tag @c day) to a libweather's wtr_forecast_day struct.
 *
 * @param[in] xmlDay The @c day tag (libxml2).
 * @return The daily forecasts as a wtr_forecast_day.
 * @warning The function assumes that the xmlNode refers to an @hour tag. The caller must ensure that the tag is correct.
 */
wtr_forecast_day *wtr_forecast_parse_day(xmlNode *xmlDay) {
	wtr_forecast_day *day = (wtr_forecast_day *)g_malloc(sizeof(wtr_forecast_day));
	day->hours = NULL;
	char *value = (char *)xmlGetProp(xmlDay, (const xmlChar *)"value");
	day->date = parseDateTime(value, "%Y%m%d");
	// printf("Day: %s\n", value);
	g_free(value);
	for (xmlNode *child = xmlDay->children; child; child = child->next) {
		if (g_strcmp0((const char *)child->name, "symbol") == 0) {
			day->weather = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "tempmin") == 0) {
			day->temp_min = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "tempmax") == 0) {
			day->temp_max = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "wind") == 0) {
			day->wind_speed = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "rain") == 0) {
			day->rain = xmlGetPropDouble(child, "value");
		} else if (g_strcmp0((const char *)child->name, "humidity") == 0) {
			day->humidity = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "pressure") == 0) {
			day->pressure = xmlGetPropInt(child, "value");
		} else if (g_strcmp0((const char *)child->name, "hour") == 0) {
			wtr_forecast_hour *hour = wtr_forecast_parse_hour(child, day);
			day->hours = g_list_append(day->hours, hour);
		}
	}
	return day;
}

/**
 * @brief Parses a 5-day forecast from Tiempo's XML and returns a wtr_forecast.
 *
 * Tiempo API provides weather forecasts in XML format. This function converts the
 * specified 5-days forecast to a libweather's wtr_forecast struct.
 *
 * @param[in] report The @c report tag (libxml2), which is the root of Tiempo's XML forecasts.
 * @return The 5-days forecasts as a wtr_forecast.
 * @warning The caller of this function must free the wtr_forecast with wtr_forecast_free().
 */
wtr_forecast *wtr_forecast_parse(char *content, size_t length) {
	// The document being in memory, it have no base per RFC 2396,
	// and the "noname.xml" argument will serve as its base.
	xmlDocPtr doc = xmlReadMemory(content, length, "noname.xml", NULL, 0);
	if (doc == NULL) {
		fprintf(stderr, "Failed to parse document\n");
		return NULL;
	}
	xmlNode *report = xmlDocGetRootElement(doc);
	if (g_strcmp0((const char *)report->name, "report") != 0) {
		fprintf(stderr, "Tiempo XML parsing error: root element report not found.\n");
		return NULL;
	}
	xmlNode *location = report->children;
	if (g_strcmp0((const char *)location->name, "location") != 0) {
		fprintf(stderr, "Tiempo XML parsing error: location element inside report not found.\n");
		return NULL;
	}
	wtr_forecast *forecast = wtr_forecast_init();
	for (xmlNode *child = location->children; child; child = child->next) {
		// Inside location there are other elements, such as "interesting"
		if (g_strcmp0((const char *)child->name, "day") != 0) {
			continue;
		}
		forecast->days = g_list_append(forecast->days, wtr_forecast_parse_day(child));
	}
	xmlFreeDoc(doc);
	return forecast;
}

/**
 * @brief Gets Tiempo's 5-days forecasts via their HTTP API.
 *
 * Tiempo API provides weather forecasts in XML format via an HTTP API. This function
 * makes an HTTP GET call, gets the XML forecasts and returns them as a libweather's
 * wtr_forecast structure.
 *
 * @warning The caller of this function must free the wtr_forecast with wtr_forecast_free().
 */
wtr_forecast *wtr_tiempo_forecast_get(gchar *code) {
	wtr_forecast *forecast = NULL;
	gchar *cached_xml = wtr_cache_get(WTR_DRIVER_TIEMPO, code);
	if (cached_xml != NULL) {
		// Use the cached XML
		forecast = wtr_forecast_parse(cached_xml, strlen(cached_xml));
		g_free(cached_xml);
	} else {
		// Cache miss, must download the forecasts XML via the HTTP API
		gchar *url = wtr_tiempo_forecast_url(code);
		net_http_rawdata data = net_http_get(url);
		g_free(url);
		if (data.curl_code) {
			g_printerr("wtr_tiempo_forecast_get curl error %u: %s\n", data.curl_code, curl_easy_strerror(data.curl_code));
		} else if (data.http_code != 200) {
			g_printerr("wtr_tiempo_forecast_get HTTP status code %lu\n", data.http_code);
		} else {
			forecast = wtr_forecast_parse(data.buffer, data.len);
			// Don't cache incorrect XML data
			if (forecast != NULL) {
				wtr_cache_set(WTR_DRIVER_TIEMPO, code, data.buffer);
			}
		}
		net_http_rawdata_free(&data);
	}
	return forecast;
}
