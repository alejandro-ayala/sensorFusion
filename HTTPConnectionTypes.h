#ifndef LWIP_HDR_APPS_HTTP_CLIENT_H
#define LWIP_HDR_APPS_HTTP_CLIENT_H


enum class HTTP_POST_METHOD { Urlencoded, Json, Multipart };

//TODO review the right headers parameters
#define USER_AGENT "TBD"
const size_t RESPONSE_MAX_SIZE = 500;

#ifdef TEST_BUILD
typedef struct mbedtls_ctr_drbg_context
{
	int dummyVal;
}mbedtls_ctr_drbg_context;

typedef struct mbedtls_x509_crt
{
	int dummyVal;
}mbedtls_x509_crt;

typedef struct mbedtls_entropy_context
{
	int dummyVal;
}mbedtls_entropy_context;

typedef struct mbedtls_ssl_context
{
	int dummyVal;
}mbedtls_ssl_context;

typedef struct mbedtls_ssl_config
{
	int dummyVal;
}mbedtls_ssl_config;

typedef struct ip_addr_t
{
	uint32_t dummyVal;
}ip_addr_t;
#endif
#endif /* LWIP_HDR_APPS_HTTP_CLIENT_H */
