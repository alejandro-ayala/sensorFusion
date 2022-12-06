#pragma once
#include <string>
#include <vector>
#ifndef TEST_BUILD
extern "C" {
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
}
#endif
#include "HTTPConnectionTypes.h"
#include "TestInclude.h"
#include "SocketController.h"
namespace Conectivity
{
class CryptoMng
{
private:
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctrfDrbg;
	mbedtls_x509_crt caCrtf;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config sslConfig;
	struct CryptoConfig cryptoCfg;


	SocketController* socket;

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

	TVIRTUAL void reconnect(void);
	TVIRTUAL void configureSSL();
	TVIRTUAL void sendData(const std::string& getReq);
	TVIRTUAL void readResponse(std::string& response);
	TVIRTUAL void handshakeSSL();
	TVIRTUAL void certificateVerification();
	TVIRTUAL void closeSSLContext();
	SocketController* getSocket();
};
}

