#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/x509_crt.h"

class CryptoMng
{
protected:



public:
	CryptoMng();
	virtual ~CryptoMng();

	struct altcp_tls_config* createTlsClientConfig(const uint8_t *ca, size_t ca_len);

	//void validateCertificate(mbedtls_x509_crt *srvcert,const uint8_t *cert, size_t cert_len);
//	void validatePrivKey(const uint8_t *privkey, size_t privkey_len,const uint8_t *privkey_pass,size_t privkey_pass_len);
};

