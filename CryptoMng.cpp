
#include <stdint.h>
#include <string.h>

#include "mbedtls/x509.h"
#include "CryptoMng.h"
#include "Logger.h"
#include <lwip/sockets.h>


//const char *CryptoMng::ROOT_CA = "-----BEGIN CERTIFICATE-----\nMIIDEzCCAfsCFCoP4eoHE17jVrmoc+NxSybT9FL+MA0GCSqGSIb3DQEBCwUAMEYxCzAJBgNVBAYTAlNQMQ8wDQYDVQQIDAZNYWRyaWQxDzANBgNVBAcMBk1hZHJpZDEVMBMGA1UECgwMQW1wYWNpbW9uIENBMB4XDTIyMDgwMjExMDYyMVoXDTMyMDczMDExMDYyMVowRjELMAkGA1UEBhMCU1AxDzANBgNVBAgMBk1hZHJpZDEPMA0GA1UEBwwGTWFkcmlkMRUwEwYDVQQKDAxBbXBhY2ltb24gQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCVN9RU5AWqfHeiCef+1ZbFbKGchHzHSEW57ctTP2lxqHjjHVqEjY57DZO3JoHe8ex8Jd3yC7zlBAUa/ezIkivitAoSXHWWVsrMavMMd9VJzjQ6sVym5PwrnTRt4AyN1y97pctNXKPXRD01CaTAmloniEGs4grft96Q/qbyY9FdOszc3odlHhw00cv0Li/e/wBa137NN8lqSQeK13yyZs2eO/lKJUQ1JAgVdU/aGtQfCX82I6iHB07rts8J7vpd2lm/O4jaWtbBhDPefSdGSJoaZFQkENP6r3D5kmu/S4KWmDe1YYTN/9lJMlnBygwejjKgONurPmgewd9OW+q84qa1AgMBAAEwDQYJKoZIhvcNAQELBQADggEBAEfMX28wkyM+wgWDsjR7YftXt27xVcAkBtGpphPkMhBIE81TYWLtnO7Xj8VNj6jcbb1K/ABcdL5AtO48bhO4/Eh4ACf9oC1WBDI3yYIkVmY8RCNrEsTacmKBUlHq88Q3V50mXOsr9hnJUEzogBoBUZSlrNcUPnuRp05gcFobY0BoGNhDkEobglfC47rMjM3pgF8PWHTMgAQBip2okzD2bW3Ay1Oc2pUFZQpfQD0MEaFExk6a0lyPAaL7GX0CvUyh96e0e7DMn1W/VWPv0vErAJQE7d6vT64xSZNCWnyUxLd/bE+ulJkDTIX6BLp58a8tuaeWT8Nk3Wy2BH4iQqoXnSo=-----END CERTIFICATE-----\n";
const char *CryptoMng::ROOT_CA = "-----BEGIN CERTIFICATE-----\nMIIDFTCCAf0CFCTY8cEiA2q06Kl2y+cYQ75FbEBRMA0GCSqGSIb3DQEBCwUAMEcxCzAJBgNVBAYTAlNQMQ8wDQYDVQQIDAZNYWRyaWQxDzANBgNVBAcMBk1hZHJpZDEWMBQGA1UECgwNTWFzdGVyIFVQTSBDQTAeFw0yMjA4MDMxNjU0MjRaFw0zMjA3MzExNjU0MjRaMEcxCzAJBgNVBAYTAlNQMQ8wDQYDVQQIDAZNYWRyaWQxDzANBgNVBAcMBk1hZHJpZDEWMBQGA1UECgwNTWFzdGVyIFVQTSBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPADdomVQGF/koYds0srgKUVwoG+EWxybxQ0JTuyR1pPGigFJaG9irPGdVAE+5uBcp3q2SQnPjHC2ihlhbBmGW+BUnbOf+zSsqDMnLMKVFtJBvibqQWbz66eHJPkRn8daVZbeOIZekbf5FvREOH76n/cwxoUcY7RL61kUBYYOALfJh9D62cGWw1VjR8ArYCpjJ6pwE1296sWEiPfuWjrUq3cczLgWfjnEuCUUhz07dv9U47JfIYI8h+wP2h3HVv1IculPx1RTizdDnXeKB8oK+k44H0+xco19yCEvT2PPjn0zaIzNDfN2SdljHvL4JcbA9oNZvG4j7a3cTbBBHmpChECAwEAATANBgkqhkiG9w0BAQsFAAOCAQEADEpfw/Y710XTPlEK2aB24APEwxVhaKwkLZQTOfKrx9xKCCT6wVXMgyZAm44Ywi68jo0WPhvMeTkRh8CpjNFYWdrxjfpi2pdpV1MSWUpqa7g/dDoJVRmrqY810J4LbE5JNkXbbB5lGkEY3L2BGSO7SamQ2BMm3blJZ1qSj0fpPstfvhqCeeb0En9vk++vJxwJvuK7l7ESFuDg8FP3hjBblwESBh3cTi9RVax5FEuHBcSbkfisjLcNhW7hEcs+zrr11uBUAyZkvCHFO3H5dKPv1dz3KMpg0LuBvMIhufeAh2i4pBK2wIOUsglyBeHkhwBqt9n8eVigWgazjJCYn9gI5w==-----END CERTIFICATE-----\n";

const size_t SERVER_BUFFER_LENGTH = 1024;

CryptoMng::CryptoMng(bool crtVerification, int socket, std::string serverIP) : crtVerification(crtVerification),  socketNumber(socketNumber), serverIP(serverIP)
{
	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctrfDrbg);
	mbedtls_x509_crt_init(&caCrtf);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&sslConfig);
}

CryptoMng::~CryptoMng()
{
	mbedtls_entropy_free(&entropy);
	mbedtls_ctr_drbg_free(&ctrfDrbg);
	mbedtls_x509_crt_free(&caCrtf);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&sslConfig);
}

void CryptoMng::tlsHandshake()
{
	int status = MBEDTLS_ERR_SSL_WANT_READ;
	bool handShakeTimeout = false;
	PRINT_NOTIF("Getting certificate from server");
	//TODO ADD active waiting
	while(!handShakeTimeout && (status == MBEDTLS_ERR_SSL_WANT_READ || status == MBEDTLS_ERR_SSL_WANT_WRITE))
	{
		status = mbedtls_ssl_handshake(&ssl);
		handShakeTimeout = false;
	}

	if (status < 0)
	{
		//PRINT_WARN1("Error happen during handshake: ", -ret);
		//Throw
	}
	else
	{
		PRINT_NOTIF("Certificate received from server\n");
	}
}

void CryptoMng::certificateVerification()
{
	char serverCert[SERVER_BUFFER_LENGTH] = " ";
	int ret = mbedtls_x509_crt_info(serverCert, sizeof(serverCert),"\r  ", mbedtls_ssl_get_peer_cert(&ssl));
	if (ret < 0)
	{
		////PRINT_DEBUG1("Error geting server certificate info returned ", -ret);
		//THROW
	}
	////PRINT_DEBUG1("Server certificate:\n%s\n", serverCert);

	uint32_t flags = mbedtls_ssl_get_verify_result(&ssl);
	if (flags != 0)
	{
		ret = mbedtls_x509_crt_verify_info(serverCert, sizeof(serverCert), "\r  ! ", flags);
		if (ret < 0)
		{
			////PRINT_DEBUG1("mbedtls_x509_crt_verify_info() returned ", -ret);
			//THROW
		}
		else
		{
			////PRINT_DEBUG1("Certificate verification failed -> flags: ", flags);
			//THROW
		}
	}

	PRINT_INFO("Certificate verification passed\n");
	PRINT_INFO1("Established TLS connection to %s\n", serverIP.c_str());

}

void CryptoMng::sendData(const std::string& getReq)
{
	int ret = 0;
	size_t pendingData = 0;
	size_t dataLenght = getReq.length();
	ret = mbedtls_ssl_write(&ssl,reinterpret_cast<const unsigned char *>(getReq.c_str()), dataLenght);
	if ((ret > 0) && (static_cast<size_t>(ret) != dataLenght))
	{
		//TODO check if throw an exception when not full request is send and then retry!
		pendingData = static_cast<size_t>(ret);
		PRINT_WARN1("The full request was not sent! Pending byte: %d", (dataLenght - pendingData));
	}
	else if (ret < 0)
	{
		//PRINT_ERROR1("Error during SSL write process ", -ret);
		//Throw exception with the send data error!
	}
}

void CryptoMng::readResponse()
{
	//TODO review read response method
	char serverResponse[SERVER_BUFFER_LENGTH] = " ";
	constexpr char* HTTP_OKEY_RESPONSE = "200 OK";
	constexpr char* HTTP_ERROR_RESPONSE = "501 ERROR";

	bool timeout = false;
	size_t pendingBytes = 0;
	int readResult = MBEDTLS_ERR_SSL_WANT_READ;
	//TODO add active wait and check status(MBEDTLS_ERR_SSL_WANT_READ, MBEDTLS_ERR_SSL_WANT_WRITE)
	while(!timeout && (readResult == MBEDTLS_ERR_SSL_WANT_READ || readResult == MBEDTLS_ERR_SSL_WANT_WRITE))
	{
		readResult = mbedtls_ssl_read(&ssl,reinterpret_cast<unsigned char *>(serverResponse  + pendingBytes),sizeof(serverResponse) - pendingBytes - 1);
		serverResponse[pendingBytes] = '\0';
		pendingBytes += static_cast<size_t>(readResult);
		timeout = false;
	}
	if (readResult < 0)
	{
		//Throw error during read response
	}

	//PRINT_DEBUG1("Received %u chars from server\n", offset);
	//PRINT_DEBUG1("Received message:\n", serverResponse);
}

//TODO refactor this method to get the SSL config as parameters
void CryptoMng::configureSSL(int socket)
{
	int ret;

	configureEntropy();

	//TODO add chain of certificates for security. A intermediate one could be interesting.
	if(rootCA)
	{
		importRootCA();
	}

	ret = mbedtls_ssl_config_defaults(&sslConfig, MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT);

	if (ret != 0)
	{
		//PRINT_ERROR1("Error configuring default SSL configuration: ",-ret);
		//THROW exception
	}

	mbedtls_ssl_conf_rng(&sslConfig, mbedtls_ctr_drbg_random, &ctrfDrbg);

	uint8_t verificationMethod = MBEDTLS_SSL_VERIFY_REQUIRED;
	if(!crtVerification)
	{
		verificationMethod = MBEDTLS_SSL_VERIFY_NONE;
	}
	mbedtls_ssl_conf_authmode(&sslConfig, verificationMethod);

	mbedtls_ssl_conf_verify(&sslConfig, crtfVerify, this);

	if ((ret = mbedtls_ssl_setup( &ssl, &sslConfig)) != 0)
	{
		//PRINT_ERROR1("Error during Secure socket layer ", -ret);
		//THROW exception
	}

	//TODO check to enable
	if(checkHostName)
	{
		if ((ret = mbedtls_ssl_set_hostname( &ssl, serverIP.c_str() )) != 0)
		{
			//PRINT_ERROR1("Error assigning the server host name returned ", -ret);
			//THROW exception
		}
	}
	mbedtls_ssl_set_bio(&ssl, static_cast<void *>(&socket), sendDataCallback, readDataCallback,	NULL);

}

void CryptoMng::importRootCA()
{
	//const std::string rootCA = readRootCA();
	const int ret = mbedtls_x509_crt_parse(&caCrtf,reinterpret_cast<const unsigned char *>(ROOT_CA),strlen(ROOT_CA) + 1);
	if (ret != 0)
	{
		PRINT_FATAL1("Error during import root CA ", -ret);
		//THROW EXCEPTION
	}

	mbedtls_ssl_conf_ca_chain(&sslConfig, &caCrtf, NULL);

}
void CryptoMng::configureEntropy()
{
	//TODO ADAB check the entropy source --> very important!
	const std::string DRBG_CUSTOM = "TLS client";
	const int ret = mbedtls_ctr_drbg_seed(&ctrfDrbg, mbedtls_entropy_func, &entropy,DRBG_CUSTOM.c_str(),DRBG_CUSTOM.length() + 1);
	if (ret != 0)
	{
		////PRINT_ERROR1("Error during entropy configuration ", -ret);
		//THROW ERROR;
	}
}

int CryptoMng::readDataCallback(void *ctx, unsigned char *buf, size_t len)
{
	int socket = 0 ;
	int ret = recv(socket,buf, len,0);
	return ret;
}

int CryptoMng::sendDataCallback(void *ctx, const unsigned char *buf, size_t len)
{
	int socket = 0;
	int ret = send(socket,buf, len,0);
	return ret;
}


int CryptoMng::crtfVerify(void *ctx, mbedtls_x509_crt *crt, int depth,uint32_t *flags)
{
	int ret = 0;
	char serverCert[SERVER_BUFFER_LENGTH] = "";

	//TODO: Add the validty check with the RTC is available. For the moment we clear the flags
	*flags &= ~MBEDTLS_X509_BADCERT_FUTURE & ~MBEDTLS_X509_BADCERT_EXPIRED;


	//TODO: maybe is needed add a revocation ticket (OCSP?).
	ret = mbedtls_x509_crt_info(serverCert, sizeof(serverCert), "\r  ", crt);

	if (ret < 0)
	{
		PRINT_DEBUG("Error getting server certificate information");
	}

	////PRINT_DEBUG1("Server certificate:\n", serverCert);

	CryptoMng *cryptoMng = static_cast<CryptoMng *>(ctx);
	const mbedtls_ssl_context sslCtx = cryptoMng->getSslContext();
	uint32_t doubleCheck = mbedtls_ssl_get_verify_result(&sslCtx);
	PRINT_DEBUG2("Checking the parsed certificate flags: ", flags, "  against the previous one:  ", doubleCheck);

	if (doubleCheck != 0)
	{
		ret = mbedtls_x509_crt_verify_info(serverCert, sizeof(serverCert), "\r  ! ", flags);
		if (ret < 0)
		{
			////PRINT_DEBUG1("mbedtls_x509_crt_verify_info() returned ", -ret);
			//THROW
		}
		else
		{
			////PRINT_DEBUG1("Certificate verification failed -> flags: ", flags);
			//THROW
		}
	}

	PRINT_INFO("Certificate verification passed\n");
	ret = 0;
	return ret;
}

const mbedtls_ssl_context& CryptoMng::getSslContext()
{
	return ssl;
}
std::string& CryptoMng::readRootCA()
{
	//TODO implement the method when the FS will be ready and we can retrieve from there instead of hardcoded.
	std::string rootCA{ROOT_CA};
	return rootCA;
}
