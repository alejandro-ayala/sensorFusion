#pragma once
#include <string>

namespace business_logic
{
namespace Conectivity
{
enum class HTTP_POST_METHOD { Urlencoded, Json, Multipart };

struct CryptoConfig
{
	bool crtVerification;
	bool rootCA;
	bool checkHostName;
	std::string hostName;
};

//TODO review the right headers parameters
#define USER_AGENT "TBD"
const size_t RESPONSE_MAX_SIZE = 5000;

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
}
}
