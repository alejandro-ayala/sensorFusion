#pragma once
#include <string>
#include <vector>
#ifndef TEST_BUILD
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#endif
#include "HTTPConnectionTypes.h"

#include "SocketHandler.h"

class CryptoMng
{
private:
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctrfDrbg;
	mbedtls_x509_crt caCrtf;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config sslConfig;
	struct CryptoConfig cryptoCfg;


	SocketHandler* socket;

	void configureEntropy();
    void importRootCA();
	static const char *ROOT_CA;
	static int crtfVerify(void *ctx, mbedtls_x509_crt *crt, int depth,uint32_t *flags);
	bool readRootCA(std::string& rootCA);
	const mbedtls_ssl_context& getSSLContext();
	bool checkCertificateValidityPeriod(mbedtls_x509_crt *crt, uint32_t *flags);

public:
	CryptoMng(CryptoConfig cryptoCfg);
	~CryptoMng();

	void reconnect(void);
	void configureSSL();
	void sendData(const std::string& getReq);
	void readResponse(std::string& response);
	void handshakeSSL();
	void certificateVerification();
	void closeSSLContext();
	SocketHandler* getSocket();
};

