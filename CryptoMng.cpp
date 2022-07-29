
#include <string.h>
#include <stdint.h>
#include <string.h>

#include "CryptoMng.h"
#include "altcp_tls_mbedtls_mem.h"
#include "altcp_tls_mbedtls_structs.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
//#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/ssl_cache.h"
#include "lwip/opt.h"
//TODO ADAB: move to the correct configuration files.
#define LWIP_ALTCP_TLS_MBEDTLS 1
#if LWIP_ALTCP /* don't build if not configured for use in lwipopts.h */

#if LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS

#ifndef ALTCP_MBEDTLS_RNG_FN
/** ATTENTION: It is *really* important to *NOT* use this dummy RNG in production code!!!! */
static int
dummy_rng(void *ctx, unsigned char *buffer, size_t len)
{
  static size_t ctr;
  size_t i;
  LWIP_UNUSED_ARG(ctx);
  for (i = 0; i < len; i++) {
    buffer[i] = (unsigned char)++ctr;
  }
  return 0;
}
#define ALTCP_MBEDTLS_RNG_FN dummy_rng
#endif /* ALTCP_MBEDTLS_RNG_FN */

struct altcp_tls_config {
  mbedtls_ssl_config conf;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_x509_crt *cert;
  mbedtls_pk_context *pkey;
  mbedtls_x509_crt *ca;
#if defined(MBEDTLS_SSL_CACHE_C) && ALTCP_MBEDTLS_SESSION_CACHE_TIMEOUT_SECONDS
  /** Inter-connection cache for fast connection startup */
  struct mbedtls_ssl_cache_context cache;
#endif
};

#ifndef ALTCP_MBEDTLS_ENTROPY_PTR
#define ALTCP_MBEDTLS_ENTROPY_PTR   NULL
#endif

#ifndef ALTCP_MBEDTLS_ENTROPY_LEN
#define ALTCP_MBEDTLS_ENTROPY_LEN   0
#endif

CryptoMng::CryptoMng()
{


}

CryptoMng::~CryptoMng()
{

}

struct altcp_tls_config* CryptoMng::createTlsClientConfig(const uint8_t *ca, size_t caLen)
{
	size_t sz;
	int ret;
	altcp_tls_config *conf;
	mbedtls_x509_crt *mem;

	if(ca != NULL)
	{
		sz += sizeof(mbedtls_x509_crt);
	}
	else
	{
		//THROW error
	}
	conf = (struct altcp_tls_config *)altcp_mbedtls_alloc_config(sz);
	if (conf == NULL)
	{
		//THROW error
	}
	mem = (mbedtls_x509_crt *)(conf + 1);

	if (ca!=NULL)
	{
		conf->ca = mem;
		mem++;
	}

	mbedtls_ssl_config_init(&conf->conf);
	mbedtls_entropy_init(&conf->entropy);
	mbedtls_ctr_drbg_init(&conf->ctr_drbg);

	/* Seed the RNG */
	ret = mbedtls_ctr_drbg_seed(&conf->ctr_drbg, ALTCP_MBEDTLS_RNG_FN, &conf->entropy, ALTCP_MBEDTLS_ENTROPY_PTR, ALTCP_MBEDTLS_ENTROPY_LEN);
	if (ret != 0)
	{
		LWIP_DEBUGF(ALTCP_MBEDTLS_DEBUG, ("mbedtls_ctr_drbg_seed failed: %d\n", ret));
		altcp_mbedtls_free_config(conf);
		return NULL;
	}

	  ret = mbedtls_ssl_config_defaults(&conf->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	  if (ret != 0)
	  {
		LWIP_DEBUGF(ALTCP_MBEDTLS_DEBUG, ("mbedtls_ssl_config_defaults failed: %d\n", ret));
		altcp_mbedtls_free_config(conf);
		return NULL;
	  }

	  mbedtls_ssl_conf_authmode(&conf->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
	  mbedtls_ssl_conf_rng(&conf->conf, mbedtls_ctr_drbg_random, &conf->ctr_drbg);

	if (ca)
	{
		mbedtls_x509_crt_init(conf->ca);
		ret = mbedtls_x509_crt_parse(conf->ca, ca, caLen);
		if (ret != 0)
		{
			LWIP_DEBUGF(ALTCP_MBEDTLS_DEBUG, ("mbedtls_x509_crt_parse ca failed: %d 0x%x", ret, -1*ret));
			altcp_mbedtls_free_config(conf);
			return NULL;
		}
		mbedtls_ssl_conf_ca_chain(&conf->conf, conf->ca, NULL);
	}
	return conf;
}



//void CryptoMng::validateCertificate(mbedtls_x509_crt *srvcert,const uint8_t *cert, size_t cert_len)
//{
//	uint8_t cert[] = {2D,0x2D,0x2D,0x2D,0x2D,0x42,0x45,0x47,0x49,0x4E,0x20,0x43,0x45,0x52,0x54,0x49,0x46,0x49,0x43,0x41,0x54,0x45,0x2D,0x2D,0x2D,0x2D,0x2D,30,0xEF,0xBF,0xBD,0x01,0xEF,0xBF,0xBD,0x30,0xEF,0xBF,0xBD,0x01,0x3A,0xEF,0xBF,0xBD,0x03,0x02,0x01,0x02,0x02,0x01,0x4D,0x30,0x06,0x08,0x2A,0xEF,0xBF,0xBD,0x48,0xEF,0xBF,0xBD,0x3D,0x04,0x03,0x02,0x30,0x4B,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x4E,0x4C,0x31,0x11,0x30,0x0F,0x06,0x03,0x55,0x04,0x13,0x08,0x50,0x6F,0x6C,0x61,0x72,0x53,0x53,0x4C,0x31,0x29,0x30,0x27,0x06,0x03,0x55,0x04,0x03,0x13,0x20,0x50,0x6F,0x6C,0x61,0x72,0x53,0x53,0x4C,0x20,0x54,0x65,0x73,0x74,0x20,0x49,0x6E,0x74,0x65,0x72,0x6D,0x65,0x64,0x69,0x61,0x74,0x65,0x20,0x45,0x43,0x20,0x43,0x41,0x30,0x1E,0x17,0x31,0x35,0x30,0x39,0x30,0x31,0x31,0x34,0x30,0x38,0x34,0x33,0x5A,0x17,0x32,0x35,0x30,0x38,0x32,0x39,0x31,0x34,0x30,0x38,0x34,0x33,0x5A,0x30,0x4A,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x4B,0x31,0x11,0x30,0x0F,0x06,0x03,0x55,0x04,0x13,0x08,0x6D,0x62,0x65,0x64,0x20,0x54,0x4C,0x53,0x31,0x28,0x30,0x26,0x06,0x03,0x55,0x04,0x03,0x13,0x1F,0x6D,0x62,0x65,0x64,0x20,0x54,0x4C,0x53,0x20,0x54,0x65,0x73,0x74,0x20,0x69,0x6E,0x74,0x65,0x72,0x6D,0x65,0x64,0x69,0x61,0x74,0x65,0x20,0x43,0x41,0x20,0x33,0x30,0x59,0x30,0x13,0x06,0x07,0x2A,0xEF,0xBF,0xBD,0x48,0xEF,0xBF,0xBD,0x3D,0x02,0x01,0x06,0x08,0x2A,0xEF,0xBF,0xBD,0x48,0xEF,0xBF,0xBD,0x3D,0x03,0x01,0x07,0x03,0x42,0x00,0x04,0xEF,0xBF,0xBD,0x7D,0xEF,0xBF,0xBD,0x58,0x72,0xEF,0xBF,0xBD,0x3C,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x3F,0x55,0x35,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x6F,0xEF,0xBF,0xBD,0x5E,0xEF,0xBF,0xBD,0x51,0x15,0x32,0x5A,0x57,0x04,0x6B,0x23,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x30,0x56,0xEF,0xBF,0xBD,0x35,0x5C,0x29,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x61,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x35,0xEF,0xBF,0xBD,0xD8,0x9F,0xEE,0xB6,0x81,0xEF,0xBF,0xBD,0x4B,0x55,0xEF,0xBF,0xBD,0x03,0xEF,0xBF,0xBD,0x75,0x3A,0xDB,0xB1,0xEF,0xBF,0xBD,0x10,0x30,0x0E,0x30,0x0C,0x06,0x03,0x55,0x1D,0x13,0x04,0x05,0x30,0x03,0x01,0x01,0xEF,0xBF,0xBD,0x30,0x06,0x08,0x2A,0xEF,0xBF,0xBD,0x48,0xEF,0xBF,0xBD,0x3D,0x04,0x03,0x02,0x03,0x68,0x00,0x30,0x65,0x02,0x30,0x09,0x47,0x16,0xEF,0xBF,0xBD,0x45,0xEF,0xBF,0xBD,0x37,0x34,0x19,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x59,0x78,0x3B,0x3D,0x3D,0x71,0xEF,0xBF,0xBD,0x11,0x0B,0x1D,0x15,0xEF,0xBF,0xBD,0x22,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x75,0x54,0x2D,0xD3,0xA9,0x75,0xEF,0xBF,0xBD,0x11,0x60,0xDD,0x9B,0xEF,0xBF,0xBD,0x75,0x07,0x6D,0x42,0x1E,0xEF,0xBF,0xBD,0x2B,0xEF,0xBF,0xBD,0x02,0x31,0x00,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x68,0xD5,0xAF,0x2A,0xEF,0xBF,0xBD,0x32,0xEF,0xBF,0xBD,0x7C,0xEF,0xBF,0xBD,0x7A,0xDF,0xAB,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x6B,0x41,0x3E,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x78,0x50,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x11,0x05,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x18,0x20,0x74,0x07,0x6B,0xEF,0xBF,0xBD,0xEF,0xBF,0xBD,0x3E,0x7D,0xD0,0x9B,0xEF,0xBF,0xBD,0x59,0xEF,0xBF,0xBD,2D,0x2D,0x2D,0x2D,0x2D,0x45,0x4E,0x44,0x20,0x43,0x45,0x52,0x54,0x49,0x46,0x49,0x43,0x41,0x54,0x45,0x2D,0x2D,0x2D,0x2D,0x2D};
//
//}

//void CryptoMng::validatePrivKey(const uint8_t *privkey, size_t privkey_len,const uint8_t *privkey_pass,size_t privkey_pass_len)
//{
//
//
//}

#endif /* LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS */
#endif /* LWIP_ALTCP */
