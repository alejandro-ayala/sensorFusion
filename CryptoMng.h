/*
 * CryptoMng.h
 *
 *  Created on: 25 jul. 2022
 *      Author: Alejandro
 */

#ifndef _CRYPTOMNG_H_
#define _CRYPTOMNG_H_
#include <string>
#include <vector>
#ifndef TEST_BUILD
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#else
#include "HTTPConnectionTypes.h"
#endif

namespace Tools
{
	class SPDSocket;
}

class CryptoMng
{
public:
	CryptoMng();
	~CryptoMng();

	void configureSSL();
	void sendData(const std::string& getReq);
	void readResponse(std::string& response);
	void tlsHandshake();
	void certificateVerification();
	void closeSSLContext();
	Tools::SPDSocket* getSocket();

private:
	std::string serverIP = "Hostname";
	Tools::SPDSocket* socket;

	bool crtVerification = true;
	bool rootCA = true;
	bool checkHostName = false;
	void configureEntropy();
    void importRootCA();
	static int crtfVerify(void *ctx, mbedtls_x509_crt *crt, int depth,uint32_t *flags);
	void reconnect(void);
	bool readRootCA(std::string& rootCA);
	const mbedtls_ssl_context& getSslContext();
	bool checkCertificateValidityPeriod(mbedtls_x509_crt *crt, uint32_t *flags);
	static const char *ROOT_CA;

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctrfDrbg;
	mbedtls_x509_crt caCrtf;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config sslConfig;
};

#endif /* _CRYPTOMNG_H_ */
