
#include <stdint.h>
#include <string.h>
#ifndef TEST_BUILD
#include "lwip/err.h"
#include "../../../external/mbedtls/include/mbedtls/x509.h"
#endif
#include <business_logic/Conectivity/CryptoMng.h>
#include "ProjectExceptions.h"
#include <business_logic/Conectivity/SocketController.h>

#ifndef TEST_BUILD
namespace
{

using namespace business_logic::Conectivity;
const char *CryptoMng::ROOT_CA = "-----BEGIN CERTIFICATE-----\nMIIDEzCCAfsCFCoP4eoHE17jVrmoc+NxSybT9FL+MA0GCSqGSIb3DQEBCwUAMEYxCzAJBgNVBAYTAlNQMQ8wDQYDVQQIDAZNYWRyaWQxDzANBgNVBAcMBk1hZHJpZDEVMBMGA1UECgwMQW1wYWNpbW9uIENBMB4XDTIyMDgwMjExMDYyMVoXDTMyMDczMDExMDYyMVowRjELMAkGA1UEBhMCU1AxDzANBgNVBAgMBk1hZHJpZDEPMA0GA1UEBwwGTWFkcmlkMRUwEwYDVQQKDAxBbXBhY2ltb24gQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCVN9RU5AWqfHeiCef+1ZbFbKGchHzHSEW57ctTP2lxqHjjHVqEjY57DZO3JoHe8ex8Jd3yC7zlBAUa/ezIkivitAoSXHWWVsrMavMMd9VJzjQ6sVym5PwrnTRt4AyN1y97pctNXKPXRD01CaTAmloniEGs4grft96Q/qbyY9FdOszc3odlHhw00cv0Li/e/wBa137NN8lqSQeK13yyZs2eO/lKJUQ1JAgVdU/aGtQfCX82I6iHB07rts8J7vpd2lm/O4jaWtbBhDPefSdGSJoaZFQkENP6r3D5kmu/S4KWmDe1YYTN/9lJMlnBygwejjKgONurPmgewd9OW+q84qa1AgMBAAEwDQYJKoZIhvcNAQELBQADggEBAEfMX28wkyM+wgWDsjR7YftXt27xVcAkBtGpphPkMhBIE81TYWLtnO7Xj8VNj6jcbb1K/ABcdL5AtO48bhO4/Eh4ACf9oC1WBDI3yYIkVmY8RCNrEsTacmKBUlHq88Q3V50mXOsr9hnJUEzogBoBUZSlrNcUPnuRp05gcFobY0BoGNhDkEobglfC47rMjM3pgF8PWHTMgAQBip2okzD2bW3Ay1Oc2pUFZQpfQD0MEaFExk6a0lyPAaL7GX0CvUyh96e0e7DMn1W/VWPv0vErAJQE7d6vT64xSZNCWnyUxLd/bE+ulJkDTIX6BLp58a8tuaeWT8Nk3Wy2BH4iQqoXnSo=-----END CERTIFICATE-----\n";
/*fail CA*/
//const char *CryptoMng::ROOT_CA = "-----BEGIN CERTIFICATE-----\nMIIDSzCCAjOgAwIBAgIUOJXtmYcCBW/o8V+HXI8AccG8qwgwDQYJKoZIhvcNAQELBQAwFjEUMBIGA1UEAwwLRWFzeS1SU0EgQ0EwHhcNMjIwODAyMDczNzQ5WhcNMzIwNzMwMDczNzQ5WjAWMRQwEgYDVQQDDAtFYXN5LVJTQSBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPP/2ysUZIf6HpHH3F41SkqvsAQHVR+5f4nyXHYzYr25rlx79zBKNz4aTuxCX6YxdYULse0aYPAg49wBcFZ2QHoOEhRjmAk8OT8/xNNpGABrfreyi4f0OrsmaMGOeiWUYtEjb6VArlTl4Y+FiFiyS0l1UzO2f+1f/Wd8MH/iRUUJ0TehbP2f3Pw6URSlgWbc/jEqiW77Oud1uyaBdNduc3OsV78QbyuHa39HP2pB06mY7IUN930AHgIIeKm5TVujIDx/8eija/p0aeBhx5k+qfAmPcD/mY9rF+jiCUsjnuyOSeMKGpe8/Xt6lD0Ta6lrHhtikZTLrBi0tEs0CQuZO9cCAwEAAaOBkDCBjTAdBgNVHQ4EFgQUIwnFlbnnDHvcy5GKeyA/QPQxgxMwUQYDVR0jBEowSIAUIwnFlbnnDHvcy5GKeyA/QPQxgxOhGqQYMBYxFDASBgNVBAMMC0Vhc3ktUlNBIENBghQ4le2ZhwIFb+jxX4dcjwBxwbyrCDAMBgNVHRMEBTADAQH/MAsGA1UdDwQEAwIBBjANBgkqhkiG9w0BAQsFAAOCAQEAeTLom9CPOPa2dgWDoFgK+/NYNqDHEnqH82d7F6N6xAIq3ufq8obAkz3o70sNNpydk3fj5L/ze+Y0pnfPWaHW0uXoe/g8K6AP0kgpplJMOLIaUq3Ia8k5nHSN1lFgo4ls87wfMe5dC1TIO2x5bizrOsAuxAbC0YDZ5GfA57N6zcjg9qsXHZch1E7sQyqBeZe2qn+aeYrfuz3Z/hGilKFNHn/1MxRD5Yzv+PuzcRmxRcF7N8vgVu5fuBd0oGva2nQVJCG2mSx3ekSv/s22wui7N+pLHl9/EezJQ5lQyVKD9aeDPXVWmQc2k0JgryK5OXU45mS90CUqtOMk4mqOgrznYA==-----END CERTIFICATE-----\n";
const size_t SERVER_BUFFER_LENGTH = 1024;

int readDataCallback(void *ctx, unsigned char *buf, size_t len)
{
	CryptoMng *cryptoMng = static_cast<CryptoMng *>(ctx);
	SocketController* socket = cryptoMng->getSocket();
	size_t ret = socket->readSocket(buf, len);
	return ret;
}

int sendDataCallback(void *ctx, const unsigned char *buf, size_t len)
{
	CryptoMng *cryptoMng = static_cast<CryptoMng *>(ctx);
	SocketController* socket = cryptoMng->getSocket();
	size_t ret = socket->writeSocket(buf, len);
	return ret;
}
}
#endif

namespace business_logic
{
namespace Conectivity
{
CryptoMng::CryptoMng(CryptoConfig cryptoCfg) : cryptoCfg(cryptoCfg)
{
#ifndef TEST_BUILD
	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctrfDrbg);
	mbedtls_x509_crt_init(&caCrtf);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&sslConfig);
#endif
}

CryptoMng::~CryptoMng()
{
	closeSSLContext();
}

void CryptoMng::handshakeSSL()
{
#ifndef TEST_BUILD
	int status = MBEDTLS_ERR_SSL_WANT_READ;
	bool handShakeTimeout = false;
	//PRINT_NOTIF("Getting certificate from server");
	//TODO ADD active waiting
	//while(!handShakeTimeout && (status == MBEDTLS_ERR_SSL_WANT_READ || status == MBEDTLS_ERR_SSL_WANT_WRITE))
	while((status != ERR_OK))
	{
		status = mbedtls_ssl_handshake(&ssl);
		if((status == MBEDTLS_ERR_SSL_WANT_READ || status == MBEDTLS_ERR_SSL_WANT_WRITE))
		{
			THROW_FUNC_EXCEPT(ERROR_CODE::TLS_SERVER_CONNECTION, "TLS connection is down");
		}
	}
	if (status < 0)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_SERVER_HANDSHAKE, "Error happen during handshake with server");
	}
	else
	{
		//PRINT_DEBUG("Certificate verification passed");
	}
#endif
}

void CryptoMng::certificateVerification()
{
#ifndef TEST_BUILD
	char serverCert[SERVER_BUFFER_LENGTH] = " ";
	int ret = mbedtls_x509_crt_info(serverCert, sizeof(serverCert),"\r  ", mbedtls_ssl_get_peer_cert(&ssl));
	if (ret < 0)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_PARSE_CERTF, "Server certificate can not be parsed");
	}
	//PRINT_DEBUG1("Server certificate:\n%s\n", serverCert);

	uint32_t flags = mbedtls_ssl_get_verify_result(&ssl);
	if (flags != ERR_OK)
	{
		ret = mbedtls_x509_crt_verify_info(serverCert, sizeof(serverCert), "\r  ! ", flags);
		//PRINT_DEBUG1("Certificate verification failed: ", serverCert);
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_VERIFY_CHAIN_CERTF, "Chain of certificates can not be validated");
	}
#endif
}

void CryptoMng::sendData(const std::string& getReq)
{
#ifndef TEST_BUILD
	int ret;
	size_t pendingData;
	size_t dataLenght = getReq.length();
	ASSERT_TECH(ERROR_CODE::TLS_INVALID_MSG_SIZE, dataLenght <= MBEDTLS_SSL_OUT_CONTENT_LEN, "SSL content size is higher than the maximum");

	ret = mbedtls_ssl_write(&ssl,reinterpret_cast<const unsigned char *>(getReq.c_str()), dataLenght);
	if ((ret > 0) && (static_cast<size_t>(ret) != dataLenght))
	{
		pendingData = static_cast<size_t>(ret);
		//PRINT_WARN1("The full request was not sent! Pending byte: %d", (dataLenght - pendingData));
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_INCOMPLETE_SEND, "The full request was not sent");
	}
	else if ((ret < 0) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE) || (ret == MBEDTLS_ERR_SSL_WANT_READ))
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_SEND_DATA, "Failed SSL write process");
	}
#endif
}

void CryptoMng::readResponse(std::string& response)
{
#ifndef TEST_BUILD
	char serverResponse[SERVER_BUFFER_LENGTH] = " ";
	size_t readBytes = 0;

	const int readResult = mbedtls_ssl_read(&ssl,reinterpret_cast<unsigned char *>(serverResponse  + readBytes),SERVER_BUFFER_LENGTH - readBytes - 1);
	readBytes = static_cast<size_t>(readResult);

	if(readResult == MBEDTLS_ERR_SSL_CLIENT_RECONNECT)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_CONNECTION_LOST, "Connection lost with the server.");
	}
	else if(readResult == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_CLOSE_CONNECTION, "Close notification to server failed");
	}
	else if (readResult < 0)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_INCOMPLETE_RESPONSE, "Failed HTTPs read process.");
	}

	response = serverResponse;
	//PRINT_DEBUG1("Received message:\n", serverResponse);
#endif
}

void CryptoMng::configureSSL()
{
#ifndef TEST_BUILD
	int ret;

	configureEntropy();

	//TODO add chain of certificates for security. A intermediate one could be interesting.
	if(cryptoCfg.rootCA)
	{
		importRootCA();
	}

	ret = mbedtls_ssl_config_defaults(&sslConfig, MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT);

	if (ret != ERR_OK)
	{
		//PRINT_ERROR1("Error configuring default SSL configuration: ",-ret);
		//THROW exception???
	}

	mbedtls_ssl_conf_rng(&sslConfig, mbedtls_ctr_drbg_random, &ctrfDrbg);

	uint8_t verificationMethod = MBEDTLS_SSL_VERIFY_REQUIRED;
	if(!cryptoCfg.crtVerification)
	{
		verificationMethod = MBEDTLS_SSL_VERIFY_NONE;
	}
	mbedtls_ssl_conf_authmode(&sslConfig, verificationMethod);

	mbedtls_ssl_conf_verify(&sslConfig, crtfVerify, this);

	if ((ret = mbedtls_ssl_setup( &ssl, &sslConfig)) != ERR_OK)
	{
		//PRINT_ERROR1("Error during Secure socket layer ", -ret);
		//THROW exception
	}

	if(cryptoCfg.checkHostName)
	{
		if ((ret = mbedtls_ssl_set_hostname( &ssl, cryptoCfg.hostName.c_str())) != ERR_OK)
		{
			//PRINT_DEBUG("Error assigning the server host name to the crypto layer ");
			//THROW exception
		}
	}
	mbedtls_ssl_set_bio(&ssl, static_cast<void *>(&socket), sendDataCallback, readDataCallback,	NULL);
	mbedtls_ssl_session_reset(&ssl);
#endif
}

void CryptoMng::importRootCA()
{
#ifndef TEST_BUILD
	std::string rootCA;
	int ret = readRootCA(rootCA);
	if(!ret)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_RETRIEVE_ROOT_CA, "Root CA can not be retrieved");
	}
	ret = mbedtls_x509_crt_parse(&caCrtf,reinterpret_cast<const unsigned char *>(ROOT_CA),strlen(ROOT_CA) + 1);

	if (ret != ERR_OK)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_VALIDATE_ROOT_CA, "Root CA can not be validated");
	}

	mbedtls_ssl_conf_ca_chain(&sslConfig, &caCrtf, NULL);
#endif
}
void CryptoMng::configureEntropy()
{
#ifndef TEST_BUILD
	//TODO ADAB check the entropy source --> very important!
	const int ret = mbedtls_ctr_drbg_seed(&ctrfDrbg, mbedtls_entropy_func, &entropy,nullptr, 0);
	if (ret != ERR_OK)
	{
		//TODO check if add CryptoMng as device to throw initialization memory errors
		//PRINT_ERROR1("Error during entropy configuration ", -ret);
	}
#endif
}

void CryptoMng::closeSSLContext()
{
#ifndef TEST_BUILD
	uint8_t closedState = mbedtls_ssl_close_notify(&ssl);
	if(closedState != ERR_OK)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_VALIDATE_ROOT_CA, "Close notification to server failed");
	}

	mbedtls_ssl_session_reset(&ssl);

	mbedtls_entropy_free(&entropy);
	mbedtls_ctr_drbg_free(&ctrfDrbg);
	mbedtls_x509_crt_free(&caCrtf);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&sslConfig);
#endif
}

int CryptoMng::crtfVerify(void *ctx, mbedtls_x509_crt *crt, int depth,uint32_t *flags)
{
	int ret = 0;
#ifndef TEST_BUILD
	char serverCert[SERVER_BUFFER_LENGTH] = "";
	CryptoMng *cryptoMng = static_cast<CryptoMng *>(ctx);

	const bool validityPeriod = cryptoMng->checkCertificateValidityPeriod(crt, flags);
	if(!validityPeriod)
	{
		THROW_FUNC_EXCEPT(ERROR_CODE::TLS_CERTF_PERIOD, "Invalid validity period of server certificate ");
	}
	//TODO: maybe check the others fields of the subject to increase the security??
	ret = mbedtls_x509_crt_info(serverCert, sizeof(serverCert), "\r  ", crt);

	if (ret < 0)
	{
		//PRINT_DEBUG("Error getting server certificate information");
	}

	//PRINT_DEBUG1("Server certificate:\n", serverCert);

	const mbedtls_ssl_context sslCtx = cryptoMng->getSSLContext();
	uint32_t doubleCheck = mbedtls_ssl_get_verify_result(&sslCtx);
	//PRINT_DEBUG2("Checking the parsed certificate flags: ", *flags, "  against the previous one:  ", doubleCheck);

	if (doubleCheck != ERR_OK)
	{
		ret = mbedtls_x509_crt_verify_info(serverCert, sizeof(serverCert), "\r  ! ", flags);
		if (ret < 0)
		{
			//PRINT_DEBUG1("mbedtls_x509_crt_verify_info() returned ", -ret);
		}
		else
		{
			//PRINT_DEBUG1("Certificate verification failed -> flags: ", flags);
		}
	}

#endif
	return ret;
}

bool CryptoMng::checkCertificateValidityPeriod(mbedtls_x509_crt *crt,uint32_t *flags)
{
#ifndef TEST_BUILD
	//TODO: Add the validty check with the RTC is available. For the moment we clear the flags
	*flags &= ~MBEDTLS_X509_BADCERT_FUTURE & ~MBEDTLS_X509_BADCERT_EXPIRED;
#endif
	return true;
}
const mbedtls_ssl_context& CryptoMng::getSSLContext()
{
#ifndef TEST_BUILD
	return ssl;
#endif
}

bool CryptoMng::readRootCA(std::string& rootCA)
{
#ifndef TEST_BUILD
	//TODO implement the method when the FS will be ready and we can retrieve from there instead of hardcoded.
	bool retrieved = true;
	return retrieved;
#endif
}

SocketController* CryptoMng::getSocket()
{
#ifndef TEST_BUILD
	return new SocketController();
#endif
}

void  CryptoMng::reconnect(void)
{
#ifndef TEST_BUILD
	mbedtls_ssl_close_notify( &ssl );

	mbedtls_ssl_session_reset(&ssl);

//	mbedtls_ssl_set_session( &ssl, &saved_session );
#endif
}
}
}
