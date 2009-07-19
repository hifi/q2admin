//
// q2admin
//
// q2a_http.c
//
// copyright 2009 Toni Spets
//

#include "g_local.h"

#include <curl/curl.h>

#define HTTP_BUFFER_SIZE 1024*1024

CURLM *http_multi;
CURL *http_single;
char *http_buffer;
int http_buffer_pos;
int http_status;
void *http_callback;
int http_buffer_full;

size_t q2a_write_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//gi.dprintf("Q2A: q2a_write_func(%p, %d, %d, %p)\n", ptr, size, nmemb, stream);

	if(http_buffer_pos + size * nmemb >= HTTP_BUFFER_SIZE) {
		http_buffer_full = TRUE;
		gi.dprintf("Q2A: libCURL HTTP buffer is full, file not downloaded\n");
		return 0;
	}

	q2a_memcpy(http_buffer + http_buffer_pos, ptr, size * nmemb);
	http_buffer_pos += size * nmemb;

	return size * nmemb;
}

void q2a_http_init()
{
	http_multi = curl_multi_init();
	http_single = curl_easy_init();

	// reserve one megabyte of server memory for downloads, reserve last byte for NULL
	http_buffer = q2a_malloc(HTTP_BUFFER_SIZE + 1);
	if(!http_buffer) {
		gi.dprintf("Q2A: libCURL initialization failed: out of memory\n");
		return;
	}

	if(!http_multi || !http_single) {
		gi.dprintf("Q2A: libCURL initialization failed!\n");
		return;
	}

	gi.dprintf("libCURL %s initialized\n", curl_version());

	curl_easy_setopt(http_single, CURLOPT_WRITEFUNCTION, q2a_write_func);

	curl_easy_setopt(http_single, CURLOPT_NOPROGRESS, TRUE);
	curl_easy_setopt(http_single, CURLOPT_NOSIGNAL, TRUE);
	curl_easy_setopt(http_single, CURLOPT_USERAGENT, "Q2Admin/" Q2ADMINVERSION);

	// cache DNS entries for the whole session, prevents the server
	// from hanging mid-game because blocking DNS resolving
	curl_easy_setopt(http_single, CURLOPT_DNS_CACHE_TIMEOUT, -1);

	http_status = Q2A_HTTP_IDLE;
}

int q2a_http_status()
{
	if(!http_multi || !http_single)
		return Q2A_HTTP_NOT_READY;

	return http_status;
}

void q2a_http_shutdown()
{
	q2a_free(http_buffer);

	curl_easy_cleanup(http_single);
	curl_multi_cleanup(http_multi);

	http_status = Q2A_HTTP_NOT_READY;
}

/* start a file transfer, return true if it can do dat, false if a transfer is already going */
qboolean q2a_http_get(char *url, void *ready_cb, char *username, char *password)
{
	if(q2a_http_status() != Q2A_HTTP_IDLE)
		return FALSE;

	curl_easy_setopt(http_single, CURLOPT_URL, url);

	// support authentication
	if(username && password) {
		curl_easy_setopt(http_single, CURLOPT_USERNAME, username);
		curl_easy_setopt(http_single, CURLOPT_PASSWORD, username);
	}

	curl_multi_add_handle(http_multi, http_single);

	http_status = Q2A_HTTP_WORKING;
	http_buffer_pos = 0;
	http_callback = ready_cb;
	http_buffer_full = FALSE;

	return FALSE;
}

void q2a_http_tick()
{
	CURLMsg *msg;
	CURLMcode ret = CURLM_CALL_MULTI_PERFORM;
	int tmp = 0;

	if(q2a_http_status() != Q2A_HTTP_WORKING)
		return;

	while(ret == CURLM_CALL_MULTI_PERFORM)
		ret = curl_multi_perform(http_multi, &tmp);

	// check the status of our transfer
	tmp = 1;
	while(tmp > 0) {
		msg = curl_multi_info_read(http_multi, &tmp);

		if(msg) {
			if(msg->msg == CURLMSG_DONE) {
				curl_multi_remove_handle(http_multi, msg->easy_handle);
				http_status = Q2A_HTTP_IDLE;

				// null the last byte just in case
				*(http_buffer + http_buffer_pos + 1) = 0;

				// if the buffer got full, return 0 bytes downloaded
				if(http_buffer_full)
					http_buffer_pos = 0;

				((void (*)(char *, int)) http_callback)(http_buffer, http_buffer_pos);
			}
		}
	}
}
