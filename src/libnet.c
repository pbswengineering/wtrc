/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/**
 * @file libnet.c
 * @brief Networking functions. (implementation).
 *
 * Libnet contains networking helper functions. For example, currently
 * there is a function to GET data via HTTP with a struct type that
 * contains the data and the HTTP results and a function to free the
 * heap used by the data.
 *
 * @author Paolo Bernardi
 * @date 6 Mar 2018
 */

#include <stdlib.h>
#include <string.h>

#include "libnet.h"

/**
 * @brief Initialize a net_http_rawdata.
 *
 * Allocates a buffer (only 1 byte for the NULL terminator) and
 * sets the other struct members to default 0 values.
 *
 * @warning a net_http_rawdata initialized with this function must be freed with net_http_rawdata_free()
 */
void net_http_rawdata_init(net_http_rawdata *data) {
	data->curl_code = 0;
	data->http_code = 0;
	data->len = 0;
	data->buffer = g_malloc(data->len + 1);
	if (data->buffer == NULL) {
		fprintf(stderr, "g_malloc() failed\n");
	} else {
		data->buffer[0] = '\0';
	}
}

/**
 * @brief Frees the heap used by a net_http_rawdata buffer.
 *
 * This function frees the net_http_rawdata buffer but not the
 * net_http_rawdata pointer itself, which must be freed by the
 * caller.
 *
 * @param[in] data The net_http_rawdata to free the buffer of.
 */
void net_http_rawdata_free(net_http_rawdata *data) {
	g_free(data->buffer);
}

/**
 * @brief Callback for cURL to write data into a net_http_rawdata.
 *
 * This function can be used with libcurl's @c CURLOPT_WRITEFUNCTION to
 * progressively write data into a net_http_rawdata_write.
 *
 * @param[in] ptr Chunk of new data received from the server.
 * @param[in] size Number of elements (chars) in the data buffer.
 * @param[in] nmemb Size of a single element (char) in the data buffer.
 * @param[in,out] data This data will be updated by appending the new chunk of data received from the server.
 * @return Total byte size of the updated data buffer.
 */
size_t net_http_rawdata_write(void *ptr, size_t size, size_t nmemb, net_http_rawdata *data) {
	size_t new_len = data->len + size * nmemb;
	data->buffer = g_realloc(data->buffer, new_len + 1);
	if (data->buffer == NULL) {
		fprintf(stderr, "g_realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(data->buffer + data->len, ptr, size * nmemb);
	data->buffer[new_len] = '\0';
	data->len = new_len;
	return size * nmemb;
}

/**
 * @brief Uses the @c curl_easy functions to perform an HTTP GET.
 *
 * This function assumes that libcurl has been correctly initialized.
 */
net_http_rawdata net_http_get(const gchar *url) {
	net_http_rawdata data;
	net_http_rawdata_init(&data);
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, net_http_rawdata_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	data.curl_code = curl_easy_perform(curl);
	if (data.curl_code == 0) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &data.http_code);
	}
	curl_easy_cleanup(curl);
	return data;
}
