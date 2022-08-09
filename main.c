
#include "mbedtls/platform.h"
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#include "psa/crypto.h"
#endif /* MBEDTLS_USE_PSA_CRYPTO */

#include "HelloHttpsClient.h"

/* Domain/IP address of the server to contact */
const char SERVER_NAME[] = "os.mbed.com";
const char SERVER_ADDR[] = "os.mbed.com";

/* Port used to connect to the server */
const int SERVER_PORT = 443;

/**
 * The main function driving the HTTPS client.
 */
int main()
{
    int exit_code = MBEDTLS_EXIT_FAILURE;

    if((exit_code = mbedtls_platform_setup(NULL)) != 0) {
        printf("Platform initialization failed with error %d\r\n", exit_code);
        return MBEDTLS_EXIT_FAILURE;
    }

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    /*
     * Initialize underlying PSA Crypto implementation.
     * Even if the HTTPS client doesn't make use of
     * PSA-specific API, for example for setting opaque PSKs
     * or opaque private keys, Mbed TLS will use PSA
     * for public and symmetric key operations as well as
     * hashing.
     */
    psa_status_t status;
    status = psa_crypto_init();
    if( status != PSA_SUCCESS )
    {
        printf("psa_crypto_init() failed with %d\r\n", status );
        return MBEDTLS_EXIT_FAILURE;
    }
#endif /* MBEDTLS_USE_PSA_CRYPTO */

    /*
     * The default 9600 bps is too slow to print full TLS debug info and could
     * cause the other party to time out.
     */

    HelloHttpsClient *client;

    mbedtls_printf("Starting mbed-os-example-tls/tls-client\n");

#if defined(MBED_MAJOR_VERSION)
    mbedtls_printf("Using Mbed OS %d.%d.%d\n",
                   MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#else
    printf("Using Mbed OS from master.\n");
#endif /* MBEDTLS_MAJOR_VERSION */

    /* Allocate a HTTPS client */
    client = new HelloHttpsClient(SERVER_NAME, SERVER_ADDR, SERVER_PORT);

    if (client == NULL) {
        mbedtls_printf("Failed to allocate HelloHttpsClient object\n"
                       "\nFAIL\n");
        mbedtls_platform_teardown(NULL);
        return exit_code;
    }

    /* Run the client */
    if (client->run() != 0) {
        mbedtls_printf("\nFAIL\n");
    } else {
        exit_code = MBEDTLS_EXIT_SUCCESS;
        mbedtls_printf("\nDONE\n");
    }

    delete client;

    mbedtls_platform_teardown(NULL);
    return exit_code;
}
