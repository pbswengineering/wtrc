/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#ifndef __LIBNET_H__
#define __LIBNET_H__

/**
 * @file libnet.h
 * @brief Networking functions.
 *
 * Libnet contains networking helper functions. For example, currently
 * there is a function to GET data via HTTP with a struct type that
 * contains the data and the HTTP results and a function to free the
 * heap used by the data.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#include <curl/curl.h>
#include <glib.h>

/**
 * @brief Result of an HTTP command.
 *
 * This struct contains the raw result of an HTTP command, its length,
 * the possible cURL error code and the HTTP status code.
 */
typedef struct {
	/// Raw result of the HTTP request. It can be used only if there's no cURL error and the HTTP code is 200.
	gchar *buffer;
	/// Length of the buffer. It can be used only if there's no cURL error and the HTTP code is 200.
	size_t len;
	/// cURL error code. A non-zero code means that there was an error.
	CURLcode curl_code;
	/// HTTP status code.
	unsigned long http_code;
} net_http_rawdata;

/**
 * @brief Simple HTTP GET client.
 *
 * This function sends an HTTP GET request to a web server and returns
 * the result of such call (raw char data, cURL status code and HTTP
 * response code)
 *
 * @param[in] url URL that will be passed to the HTTP GET call.
 * @return Raw char data returned by the server and cURL and HTTP response codes
 * @warning The caller has the responsibility to free the heap of the results by calling net_http_rawdata()
 */
net_http_rawdata net_http_get(const gchar *url);

/**
 * @brief Free the heap used by a net_http_rawdata variable.
 *
 * A net_http_rawdata variable allocates space on the heap for its @c buffer
 * member.
 *
 * @param[in] url URL that will be passed to the HTTP GET call.
 */
void net_http_rawdata_free(net_http_rawdata *data);

#endif  // __LIBNET_H__
