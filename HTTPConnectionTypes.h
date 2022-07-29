#ifndef LWIP_HDR_APPS_HTTP_CLIENT_H
#define LWIP_HDR_APPS_HTTP_CLIENT_H

#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/altcp.h"
#include "lwip/prot/iana.h"
#include "lwip/pbuf.h"

#if LWIP_TCP && LWIP_CALLBACK_API

#ifdef __cplusplus
extern "C" {
#endif

enum class HTTP_POST_METHOD { Urlencoded, Json, Multipart };

#define HTTPC_CLIENT_AGENT "lwIP/" LWIP_VERSION_STRING " (http://savannah.nongnu.org/projects/lwip)"
#define HTTPC_POLL_INTERVAL     1
#define HTTPC_POLL_TIMEOUT      30 /* 15 seconds */

#define HTTPC_CONTENT_LEN_INVALID 0xFFFFFFFF

/**
 * @ingroup httpc
 * HTTP client result codes
 */
typedef enum ehttpc_result {
  /** File successfully received */
  HTTPC_RESULT_OK            = 0,
  /** Unknown error */
  HTTPC_RESULT_ERR_UNKNOWN   = 1,
  /** Connection to server failed */
  HTTPC_RESULT_ERR_CONNECT   = 2,
  /** Failed to resolve server hostname */
  HTTPC_RESULT_ERR_HOSTNAME  = 3,
  /** Connection unexpectedly closed by remote server */
  HTTPC_RESULT_ERR_CLOSED    = 4,
  /** Connection timed out (server didn't respond in time) */
  HTTPC_RESULT_ERR_TIMEOUT   = 5,
  /** Server responded with an error code */
  HTTPC_RESULT_ERR_SVR_RESP  = 6,
  /** Local memory error */
  HTTPC_RESULT_ERR_MEM       = 7,
  /** Local abort */
  HTTPC_RESULT_LOCAL_ABORT   = 8,
  /** Content length mismatch */
  HTTPC_RESULT_ERR_CONTENT_LEN = 9
} httpc_result_t;

typedef struct _httpc_state httpc_state_t;

/**
 * @ingroup httpc
 * Prototype of a http client callback function
 *
 * @param arg argument specified when initiating the request
 * @param httpc_result result of the http transfer (see enum httpc_result_t)
 * @param rx_content_len number of bytes received (without headers)
 * @param srv_res this contains the http status code received (if any)
 * @param err an error returned by internal lwip functions, can help to specify
 *            the source of the error but must not necessarily be != ERR_OK
 */
typedef void (*httpc_result_fn)(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err);

/**
 * @ingroup httpc
 * Prototype of http client callback: called when the headers are received
 *
 * @param connection http client connection
 * @param arg argument specified when initiating the request
 * @param hdr header pbuf(s) (may contain data also)
 * @param hdr_len length of the heders in 'hdr'
 * @param content_len content length as received in the headers (-1 if not received)
 * @return if != ERR_OK is returned, the connection is aborted
 */
typedef err_t (*httpc_headers_done_fn)(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len);

typedef struct _httpc_connection {
  ip_addr_t proxy_addr;
  u16_t proxy_port;
  u8_t use_proxy;
  /* @todo: add username:pass? */

#if LWIP_ALTCP
  altcp_allocator_t *altcp_allocator;
#endif

  /* this callback is called when the transfer is finished (or aborted) */
  httpc_result_fn result_fn;
  /* this callback is called after receiving the http headers
     It can abort the connection by returning != ERR_OK */
  httpc_headers_done_fn headers_done_fn;
} httpc_connection_t;


typedef enum ehttpc_parse_state {
  HTTPC_PARSE_WAIT_FIRST_LINE = 0,
  HTTPC_PARSE_WAIT_HEADERS,
  HTTPC_PARSE_RX_DATA
} httpc_parse_state_t;

typedef struct _httpc_state
{
  struct altcp_pcb* pcb;
  ip_addr_t remote_addr;
  u16_t remote_port;
  int timeout_ticks;
  struct pbuf *request;
  struct pbuf *rx_hdrs;
  u16_t rx_http_version;
  u16_t rx_status;
  altcp_recv_fn recv_fn;
  const httpc_connection_t *conn_settings;
  void* callback_arg;
  u32_t rx_content_len;
  u32_t hdr_content_len;
  httpc_parse_state_t parse_state;
#if HTTPC_DEBUG_REQUEST
  char* server_name;
  char* uri;
#endif
} httpc_state_t;

#ifdef __cplusplus
}
#endif
#endif /* LWIP_TCP && LWIP_CALLBACK_API */
#endif /* LWIP_HDR_APPS_HTTP_CLIENT_H */
