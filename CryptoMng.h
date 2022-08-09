/*
 * CryptoMng.h
 *
 *  Created on: 23 jul. 2022
 *      Author: Alejandro
 */

#ifndef _CRYPTOMNG_H_
#define _CRYPTOMNG_H_
#include <string>
#include <vector>

#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

class CryptoMng
{
public:
	CryptoMng(bool crtVerification, int socket, std::string serverIP);
	~CryptoMng();

	void configureSSL(int socket);
	void sendData(const std::string& getReq);
	void readResponse();
	void tlsHandshake();
	void certificateVerification();

private:
	std::string serverIP;
	int configureTCPSocket();
	bool crtVerification;
	bool rootCA = true;
	bool checkHostName = false;
	void configureEntropy();
    void importRootCA();
	static int readDataCallback(void *ctx, unsigned char *buf, size_t len);
	static int sendDataCallback(void *ctx, const unsigned char *buf, size_t len);
	static int crtfVerify(void *ctx, mbedtls_x509_crt *crt, int depth,uint32_t *flags);
	std::string& readRootCA();
	const mbedtls_ssl_context& getSslContext();
	int socketNumber;
	static const char *ROOT_CA;

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctrfDrbg;
	mbedtls_x509_crt caCrtf;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config sslConfig;
};

#endif /* _CRYPTOMNG_H_ */
