/**
 * \file ssl_misc.h
 *
 * \brief Internal functions shared by the SSL modules
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef MBEDTLS_SSL_MISC_H
#define MBEDTLS_SSL_MISC_H

#include "../../../external/mbedtls/include/mbedtls/build_info.h"
#include "../../../external/mbedtls/include/mbedtls/cipher.h"
#include "../../../external/mbedtls/include/mbedtls/ssl.h"

#if defined(MBEDTLS_USE_PSA_CRYPTO) || defined(MBEDTLS_SSL_PROTO_TLS1_3)
#include "../../../external/mbedtls/include/psa/crypto.h"
#include "../../../external/mbedtls/include/mbedtls/psa_util.h"
#endif

#if defined(MBEDTLS_MD5_C)
#include "../../../external/mbedtls/include/mbedtls/md5.h"
#endif

#if defined(MBEDTLS_SHA1_C)
#include "../../../external/mbedtls/include/mbedtls/sha1.h"
#endif

#if defined(MBEDTLS_SHA256_C)
#include "../../../external/mbedtls/include/mbedtls/sha256.h"
#endif

#if defined(MBEDTLS_SHA512_C)
#include "../../../external/mbedtls/include/mbedtls/sha512.h"
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
#include "../../../external/mbedtls/include/mbedtls/ecjpake.h"
#endif

#include "../../../external/mbedtls/library/common.h"

#if ( defined(__ARMCC_VERSION) || defined(_MSC_VER) ) && \
    !defined(inline) && !defined(__cplusplus)
#define inline __inline
#endif

/* Shorthand for restartable ECC */
#if defined(MBEDTLS_ECP_RESTARTABLE) && \
    defined(MBEDTLS_SSL_CLI_C) && \
    defined(MBEDTLS_SSL_PROTO_TLS1_2) && \
    defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)
#define MBEDTLS_SSL_ECP_RESTARTABLE_ENABLED
#endif

#define MBEDTLS_SSL_INITIAL_HANDSHAKE           0
#define MBEDTLS_SSL_RENEGOTIATION_IN_PROGRESS   1   /* In progress */
#define MBEDTLS_SSL_RENEGOTIATION_DONE          2   /* Done or aborted */
#define MBEDTLS_SSL_RENEGOTIATION_PENDING       3   /* Requested (server only) */

/*
 * Mask of TLS 1.3 handshake extensions used in extensions_present
 * of mbedtls_ssl_handshake_params.
 */
#define MBEDTLS_SSL_EXT_NONE                        0

#define MBEDTLS_SSL_EXT_SERVERNAME                  ( 1 <<  0 )
#define MBEDTLS_SSL_EXT_MAX_FRAGMENT_LENGTH         ( 1 <<  1 )
#define MBEDTLS_SSL_EXT_STATUS_REQUEST              ( 1 <<  2 )
#define MBEDTLS_SSL_EXT_SUPPORTED_GROUPS            ( 1 <<  3 )
#define MBEDTLS_SSL_EXT_SIG_ALG                     ( 1 <<  4 )
#define MBEDTLS_SSL_EXT_USE_SRTP                    ( 1 <<  5 )
#define MBEDTLS_SSL_EXT_HEARTBEAT                   ( 1 <<  6 )
#define MBEDTLS_SSL_EXT_ALPN                        ( 1 <<  7 )
#define MBEDTLS_SSL_EXT_SCT                         ( 1 <<  8 )
#define MBEDTLS_SSL_EXT_CLI_CERT_TYPE               ( 1 <<  9 )
#define MBEDTLS_SSL_EXT_SERV_CERT_TYPE              ( 1 << 10 )
#define MBEDTLS_SSL_EXT_PADDING                     ( 1 << 11 )
#define MBEDTLS_SSL_EXT_PRE_SHARED_KEY              ( 1 << 12 )
#define MBEDTLS_SSL_EXT_EARLY_DATA                  ( 1 << 13 )
#define MBEDTLS_SSL_EXT_SUPPORTED_VERSIONS          ( 1 << 14 )
#define MBEDTLS_SSL_EXT_COOKIE                      ( 1 << 15 )
#define MBEDTLS_SSL_EXT_PSK_KEY_EXCHANGE_MODES      ( 1 << 16 )
#define MBEDTLS_SSL_EXT_CERT_AUTH                   ( 1 << 17 )
#define MBEDTLS_SSL_EXT_OID_FILTERS                 ( 1 << 18 )
#define MBEDTLS_SSL_EXT_POST_HANDSHAKE_AUTH         ( 1 << 19 )
#define MBEDTLS_SSL_EXT_SIG_ALG_CERT                ( 1 << 20 )
#define MBEDTLS_SSL_EXT_KEY_SHARE                   ( 1 << 21 )

/*
 * Helper macros for function call with return check.
 */
/*
 * Exit when return non-zero value
 */
#define MBEDTLS_SSL_PROC_CHK( f )                               \
    do {                                                        \
        ret = ( f );                                            \
        if( ret != 0 )                                          \
        {                                                       \
            goto cleanup;                                       \
        }                                                       \
    } while( 0 )
/*
 * Exit when return negative value
 */
#define MBEDTLS_SSL_PROC_CHK_NEG( f )                           \
    do {                                                        \
        ret = ( f );                                            \
        if( ret < 0 )                                           \
        {                                                       \
            goto cleanup;                                       \
        }                                                       \
    } while( 0 )

/*
 * DTLS retransmission states, see RFC 6347 4.2.4
 *
 * The SENDING state is merged in PREPARING for initial sends,
 * but is distinct for resends.
 *
 * Note: initial state is wrong for server, but is not used anyway.
 */
#define MBEDTLS_SSL_RETRANS_PREPARING       0
#define MBEDTLS_SSL_RETRANS_SENDING         1
#define MBEDTLS_SSL_RETRANS_WAITING         2
#define MBEDTLS_SSL_RETRANS_FINISHED        3

/*
 * Allow extra bytes for record, authentication and encryption overhead:
 * counter (8) + header (5) + IV(16) + MAC (16-48) + padding (0-256).
 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)

/* This macro determines whether CBC is supported. */
#if defined(MBEDTLS_CIPHER_MODE_CBC) &&                               \
    ( defined(MBEDTLS_AES_C)      ||                                  \
      defined(MBEDTLS_CAMELLIA_C) ||                                  \
      defined(MBEDTLS_ARIA_C)     ||                                  \
      defined(MBEDTLS_DES_C) )
#define MBEDTLS_SSL_SOME_SUITES_USE_CBC
#endif

/* This macro determines whether a ciphersuite using a
 * stream cipher can be used. */
#if defined(MBEDTLS_CIPHER_NULL_CIPHER)
#define MBEDTLS_SSL_SOME_SUITES_USE_STREAM
#endif

/* This macro determines whether the CBC construct used in TLS 1.2 is supported. */
#if defined(MBEDTLS_SSL_SOME_SUITES_USE_CBC) && \
      defined(MBEDTLS_SSL_PROTO_TLS1_2)
#define MBEDTLS_SSL_SOME_SUITES_USE_TLS_CBC
#endif

#if defined(MBEDTLS_SSL_SOME_SUITES_USE_STREAM) || \
    defined(MBEDTLS_SSL_SOME_SUITES_USE_CBC)
#define MBEDTLS_SSL_SOME_SUITES_USE_MAC
#endif

/* This macro determines whether a ciphersuite uses Encrypt-then-MAC with CBC */
#if defined(MBEDTLS_SSL_SOME_SUITES_USE_CBC) && \
    defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
#define MBEDTLS_SSL_SOME_SUITES_USE_CBC_ETM
#endif

#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

#if defined(MBEDTLS_SSL_SOME_SUITES_USE_MAC)
/* Ciphersuites using HMAC */
#if defined(MBEDTLS_SHA384_C)
#define MBEDTLS_SSL_MAC_ADD                 48  /* SHA-384 used for HMAC */
#elif defined(MBEDTLS_SHA256_C)
#define MBEDTLS_SSL_MAC_ADD                 32  /* SHA-256 used for HMAC */
#else
#define MBEDTLS_SSL_MAC_ADD                 20  /* SHA-1   used for HMAC */
#endif
#else /* MBEDTLS_SSL_SOME_SUITES_USE_MAC */
/* AEAD ciphersuites: GCM and CCM use a 128 bits tag */
#define MBEDTLS_SSL_MAC_ADD                 16
#endif

#if defined(MBEDTLS_CIPHER_MODE_CBC)
#define MBEDTLS_SSL_PADDING_ADD            256
#else
#define MBEDTLS_SSL_PADDING_ADD              0
#endif

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
#define MBEDTLS_SSL_MAX_CID_EXPANSION      MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY
#else
#define MBEDTLS_SSL_MAX_CID_EXPANSION        0
#endif

#define MBEDTLS_SSL_PAYLOAD_OVERHEAD ( MBEDTLS_MAX_IV_LENGTH +          \
                                       MBEDTLS_SSL_MAC_ADD +            \
                                       MBEDTLS_SSL_PADDING_ADD +        \
                                       MBEDTLS_SSL_MAX_CID_EXPANSION    \
                                       )

#define MBEDTLS_SSL_IN_PAYLOAD_LEN ( MBEDTLS_SSL_PAYLOAD_OVERHEAD + \
                                     ( MBEDTLS_SSL_IN_CONTENT_LEN ) )

#define MBEDTLS_SSL_OUT_PAYLOAD_LEN ( MBEDTLS_SSL_PAYLOAD_OVERHEAD + \
                                      ( MBEDTLS_SSL_OUT_CONTENT_LEN ) )

/* The maximum number of buffered handshake messages. */
#define MBEDTLS_SSL_MAX_BUFFERED_HS 4

/* Maximum length we can advertise as our max content length for
   RFC 6066 max_fragment_length extension negotiation purposes
   (the lesser of both sizes, if they are unequal.)
 */
#define MBEDTLS_TLS_EXT_ADV_CONTENT_LEN (                            \
        (MBEDTLS_SSL_IN_CONTENT_LEN > MBEDTLS_SSL_OUT_CONTENT_LEN)   \
        ? ( MBEDTLS_SSL_OUT_CONTENT_LEN )                            \
        : ( MBEDTLS_SSL_IN_CONTENT_LEN )                             \
        )

/* Maximum size in bytes of list in signature algorithms ext., RFC 5246/8446 */
#define MBEDTLS_SSL_MAX_SIG_ALG_LIST_LEN       65534

/* Minimum size in bytes of list in signature algorithms ext., RFC 5246/8446 */
#define MBEDTLS_SSL_MIN_SIG_ALG_LIST_LEN       2

/* Maximum size in bytes of list in supported elliptic curve ext., RFC 4492 */
#define MBEDTLS_SSL_MAX_CURVE_LIST_LEN         65535

#define MBEDTLS_RECEIVED_SIG_ALGS_SIZE         20

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)

#define MBEDTLS_TLS_SIG_NONE MBEDTLS_TLS1_3_SIG_NONE

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#define MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( sig, hash ) (( hash << 8 ) | sig)
#define MBEDTLS_SSL_TLS12_SIG_ALG_FROM_SIG_AND_HASH_ALG(alg) (alg & 0xFF)
#define MBEDTLS_SSL_TLS12_HASH_ALG_FROM_SIG_AND_HASH_ALG(alg) (alg >> 8)
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */

/*
 * Check that we obey the standard's message size bounds
 */

#if MBEDTLS_SSL_IN_CONTENT_LEN > 16384
#error "Bad configuration - incoming record content too large."
#endif

#if MBEDTLS_SSL_OUT_CONTENT_LEN > 16384
#error "Bad configuration - outgoing record content too large."
#endif

#if MBEDTLS_SSL_IN_PAYLOAD_LEN > MBEDTLS_SSL_IN_CONTENT_LEN + 2048
#error "Bad configuration - incoming protected record payload too large."
#endif

#if MBEDTLS_SSL_OUT_PAYLOAD_LEN > MBEDTLS_SSL_OUT_CONTENT_LEN + 2048
#error "Bad configuration - outgoing protected record payload too large."
#endif

/* Calculate buffer sizes */

/* Note: Even though the TLS record header is only 5 bytes
   long, we're internally using 8 bytes to store the
   implicit sequence number. */
#define MBEDTLS_SSL_HEADER_LEN 13

#if !defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
#define MBEDTLS_SSL_IN_BUFFER_LEN  \
    ( ( MBEDTLS_SSL_HEADER_LEN ) + ( MBEDTLS_SSL_IN_PAYLOAD_LEN ) )
#else
#define MBEDTLS_SSL_IN_BUFFER_LEN  \
    ( ( MBEDTLS_SSL_HEADER_LEN ) + ( MBEDTLS_SSL_IN_PAYLOAD_LEN ) \
      + ( MBEDTLS_SSL_CID_IN_LEN_MAX ) )
#endif

#if !defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
#define MBEDTLS_SSL_OUT_BUFFER_LEN  \
    ( ( MBEDTLS_SSL_HEADER_LEN ) + ( MBEDTLS_SSL_OUT_PAYLOAD_LEN ) )
#else
#define MBEDTLS_SSL_OUT_BUFFER_LEN                               \
    ( ( MBEDTLS_SSL_HEADER_LEN ) + ( MBEDTLS_SSL_OUT_PAYLOAD_LEN )    \
      + ( MBEDTLS_SSL_CID_OUT_LEN_MAX ) )
#endif

#define MBEDTLS_CLIENT_HELLO_RANDOM_LEN 32
#define MBEDTLS_SERVER_HELLO_RANDOM_LEN 32

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
/**
 * \brief          Return the maximum fragment length (payload, in bytes) for
 *                 the output buffer. For the client, this is the configured
 *                 value. For the server, it is the minimum of two - the
 *                 configured value and the negotiated one.
 *
 * \sa             mbedtls_ssl_conf_max_frag_len()
 * \sa             mbedtls_ssl_get_max_out_record_payload()
 *
 * \param ssl      SSL context
 *
 * \return         Current maximum fragment length for the output buffer.
 */
size_t mbedtls_ssl_get_output_max_frag_len( const mbedtls_ssl_context *ssl );

/**
 * \brief          Return the maximum fragment length (payload, in bytes) for
 *                 the input buffer. This is the negotiated maximum fragment
 *                 length, or, if there is none, MBEDTLS_SSL_IN_CONTENT_LEN.
 *                 If it is not defined either, the value is 2^14. This function
 *                 works as its predecessor, \c mbedtls_ssl_get_max_frag_len().
 *
 * \sa             mbedtls_ssl_conf_max_frag_len()
 * \sa             mbedtls_ssl_get_max_in_record_payload()
 *
 * \param ssl      SSL context
 *
 * \return         Current maximum fragment length for the output buffer.
 */
size_t mbedtls_ssl_get_input_max_frag_len( const mbedtls_ssl_context *ssl );
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
static inline size_t mbedtls_ssl_get_output_buflen( const mbedtls_ssl_context *ctx )
{
#if defined (MBEDTLS_SSL_DTLS_CONNECTION_ID)
    return mbedtls_ssl_get_output_max_frag_len( ctx )
               + MBEDTLS_SSL_HEADER_LEN + MBEDTLS_SSL_PAYLOAD_OVERHEAD
               + MBEDTLS_SSL_CID_OUT_LEN_MAX;
#else
    return mbedtls_ssl_get_output_max_frag_len( ctx )
               + MBEDTLS_SSL_HEADER_LEN + MBEDTLS_SSL_PAYLOAD_OVERHEAD;
#endif
}

static inline size_t mbedtls_ssl_get_input_buflen( const mbedtls_ssl_context *ctx )
{
#if defined (MBEDTLS_SSL_DTLS_CONNECTION_ID)
    return mbedtls_ssl_get_input_max_frag_len( ctx )
               + MBEDTLS_SSL_HEADER_LEN + MBEDTLS_SSL_PAYLOAD_OVERHEAD
               + MBEDTLS_SSL_CID_IN_LEN_MAX;
#else
    return mbedtls_ssl_get_input_max_frag_len( ctx )
               + MBEDTLS_SSL_HEADER_LEN + MBEDTLS_SSL_PAYLOAD_OVERHEAD;
#endif
}
#endif

/*
 * TLS extension flags (for extensions with outgoing ServerHello content
 * that need it (e.g. for RENEGOTIATION_INFO the server already knows because
 * of state of the renegotiation flag, so no indicator is required)
 */
#define MBEDTLS_TLS_EXT_SUPPORTED_POINT_FORMATS_PRESENT (1 << 0)
#define MBEDTLS_TLS_EXT_ECJPAKE_KKPP_OK                 (1 << 1)

/**
 * \brief        This function checks if the remaining size in a buffer is
 *               greater or equal than a needed space.
 *
 * \param cur    Pointer to the current position in the buffer.
 * \param end    Pointer to one past the end of the buffer.
 * \param need   Needed space in bytes.
 *
 * \return       Zero if the needed space is available in the buffer, non-zero
 *               otherwise.
 */
#if ! defined(MBEDTLS_TEST_HOOKS)
static inline int mbedtls_ssl_chk_buf_ptr( const uint8_t *cur,
                                           const uint8_t *end, size_t need )
{
    return( ( cur > end ) || ( need > (size_t)( end - cur ) ) );
}
#else
typedef struct
{
    const uint8_t *cur;
    const uint8_t *end;
    size_t need;
} mbedtls_ssl_chk_buf_ptr_args;

void mbedtls_ssl_set_chk_buf_ptr_fail_args(
    const uint8_t *cur, const uint8_t *end, size_t need );
void mbedtls_ssl_reset_chk_buf_ptr_fail_args( void );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_cmp_chk_buf_ptr_fail_args( mbedtls_ssl_chk_buf_ptr_args *args );

static inline int mbedtls_ssl_chk_buf_ptr( const uint8_t *cur,
                                           const uint8_t *end, size_t need )
{
    if( ( cur > end ) || ( need > (size_t)( end - cur ) ) )
    {
        mbedtls_ssl_set_chk_buf_ptr_fail_args( cur, end, need );
        return( 1 );
    }
    return( 0 );
}
#endif /* MBEDTLS_TEST_HOOKS */

/**
 * \brief        This macro checks if the remaining size in a buffer is
 *               greater or equal than a needed space. If it is not the case,
 *               it returns an SSL_BUFFER_TOO_SMALL error.
 *
 * \param cur    Pointer to the current position in the buffer.
 * \param end    Pointer to one past the end of the buffer.
 * \param need   Needed space in bytes.
 *
 */
#define MBEDTLS_SSL_CHK_BUF_PTR( cur, end, need )                        \
    do {                                                                 \
        if( mbedtls_ssl_chk_buf_ptr( ( cur ), ( end ), ( need ) ) != 0 ) \
        {                                                                \
            return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );                  \
        }                                                                \
    } while( 0 )

/**
 * \brief        This macro checks if the remaining length in an input buffer is
 *               greater or equal than a needed length. If it is not the case, it
 *               returns #MBEDTLS_ERR_SSL_DECODE_ERROR error and pends a
 *               #MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR alert message.
 *
 *               This is a function-like macro. It is guaranteed to evaluate each
 *               argument exactly once.
 *
 * \param cur    Pointer to the current position in the buffer.
 * \param end    Pointer to one past the end of the buffer.
 * \param need   Needed length in bytes.
 *
 */
#define MBEDTLS_SSL_CHK_BUF_READ_PTR( cur, end, need )                          \
    do {                                                                        \
        if( mbedtls_ssl_chk_buf_ptr( ( cur ), ( end ), ( need ) ) != 0 )        \
        {                                                                       \
            MBEDTLS_SSL_DEBUG_MSG( 1,                                           \
                                   ( "missing input data in %s", __func__ ) );  \
            MBEDTLS_SSL_PEND_FATAL_ALERT( MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR,   \
                                          MBEDTLS_ERR_SSL_DECODE_ERROR );       \
            return( MBEDTLS_ERR_SSL_DECODE_ERROR );                             \
        }                                                                       \
    } while( 0 )

#ifdef __cplusplus
extern "C" {
#endif

typedef int  mbedtls_ssl_tls_prf_cb( const unsigned char *secret, size_t slen,
                                     const char *label,
                                     const unsigned char *random, size_t rlen,
                                     unsigned char *dstbuf, size_t dlen );

/* cipher.h exports the maximum IV, key and block length from
 * all ciphers enabled in the config, regardless of whether those
 * ciphers are actually usable in SSL/TLS. Notably, XTS is enabled
 * in the default configuration and uses 64 Byte keys, but it is
 * not used for record protection in SSL/TLS.
 *
 * In order to prevent unnecessary inflation of key structures,
 * we introduce SSL-specific variants of the max-{key,block,IV}
 * macros here which are meant to only take those ciphers into
 * account which can be negotiated in SSL/TLS.
 *
 * Since the current definitions of MBEDTLS_MAX_{KEY|BLOCK|IV}_LENGTH
 * in cipher.h are rough overapproximations of the real maxima, here
 * we content ourselves with replicating those overapproximations
 * for the maximum block and IV length, and excluding XTS from the
 * computation of the maximum key length. */
#define MBEDTLS_SSL_MAX_BLOCK_LENGTH 16
#define MBEDTLS_SSL_MAX_IV_LENGTH    16
#define MBEDTLS_SSL_MAX_KEY_LENGTH   32

/**
 * \brief   The data structure holding the cryptographic material (key and IV)
 *          used for record protection in TLS 1.3.
 */
struct mbedtls_ssl_key_set
{
    /*! The key for client->server records. */
    unsigned char client_write_key[ MBEDTLS_SSL_MAX_KEY_LENGTH ];
    /*! The key for server->client records. */
    unsigned char server_write_key[ MBEDTLS_SSL_MAX_KEY_LENGTH ];
    /*! The IV  for client->server records. */
    unsigned char client_write_iv[ MBEDTLS_SSL_MAX_IV_LENGTH ];
    /*! The IV  for server->client records. */
    unsigned char server_write_iv[ MBEDTLS_SSL_MAX_IV_LENGTH ];

    size_t key_len; /*!< The length of client_write_key and
                     *   server_write_key, in Bytes. */
    size_t iv_len;  /*!< The length of client_write_iv and
                     *   server_write_iv, in Bytes. */
};
typedef struct mbedtls_ssl_key_set mbedtls_ssl_key_set;

typedef struct
{
    unsigned char binder_key                  [ MBEDTLS_TLS1_3_MD_MAX_SIZE ];
    unsigned char client_early_traffic_secret [ MBEDTLS_TLS1_3_MD_MAX_SIZE ];
    unsigned char early_exporter_master_secret[ MBEDTLS_TLS1_3_MD_MAX_SIZE ];
} mbedtls_ssl_tls13_early_secrets;

typedef struct
{
    unsigned char client_handshake_traffic_secret[ MBEDTLS_TLS1_3_MD_MAX_SIZE ];
    unsigned char server_handshake_traffic_secret[ MBEDTLS_TLS1_3_MD_MAX_SIZE ];
} mbedtls_ssl_tls13_handshake_secrets;

/*
 * This structure contains the parameters only needed during handshake.
 */
struct mbedtls_ssl_handshake_params
{
    /* Frequently-used boolean or byte fields (placed early to take
     * advantage of smaller code size for indirect access on Arm Thumb) */
    uint8_t resume;                     /*!<  session resume indicator*/
    uint8_t cli_exts;                   /*!< client extension presence*/

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    uint8_t sni_authmode;               /*!< authmode from SNI callback     */
#endif

#if defined(MBEDTLS_SSL_SRV_C)
    /* Flag indicating if a CertificateRequest message has been sent
     * to the client or not. */
    uint8_t certificate_request_sent;
#endif /* MBEDTLS_SSL_SRV_C */

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    uint8_t new_session_ticket;         /*!< use NewSessionTicket?    */
#endif /* MBEDTLS_SSL_SESSION_TICKETS */

#if defined(MBEDTLS_SSL_CLI_C)
    /** Minimum TLS version to be negotiated.
     *
     *  It is set up in the ClientHello writing preparation stage and used
     *  throughout the ClientHello writing. Not relevant anymore as soon as
     *  the protocol version has been negotiated thus as soon as the
     *  ServerHello is received.
     *  For a fresh handshake not linked to any previous handshake, it is
     *  equal to the configured minimum minor version to be negotiated. When
     *  renegotiating or resuming a session, it is equal to the previously
     *  negotiated minor version.
     *
     *  There is no maximum TLS version field in this handshake context.
     *  From the start of the handshake, we need to define a current protocol
     *  version for the record layer which we define as the maximum TLS
     *  version to be negotiated. The `tls_version` field of the SSL context is
     *  used to store this maximum value until it contains the actual
     *  negotiated value.
     */
    mbedtls_ssl_protocol_version min_tls_version;
#endif

#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    uint8_t extended_ms;                /*!< use Extended Master Secret? */
#endif

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    uint8_t async_in_progress; /*!< an asynchronous operation is in progress */
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    unsigned char retransmit_state;     /*!<  Retransmission state           */
#endif

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
    unsigned char group_list_heap_allocated;
    unsigned char sig_algs_heap_allocated;
#endif

#if defined(MBEDTLS_SSL_ECP_RESTARTABLE_ENABLED)
    uint8_t ecrs_enabled;               /*!< Handshake supports EC restart? */
    enum { /* this complements ssl->state with info on intra-state operations */
        ssl_ecrs_none = 0,              /*!< nothing going on (yet)         */
        ssl_ecrs_crt_verify,            /*!< Certificate: crt_verify()      */
        ssl_ecrs_ske_start_processing,  /*!< ServerKeyExchange: pk_verify() */
        ssl_ecrs_cke_ecdh_calc_secret,  /*!< ClientKeyExchange: ECDH step 2 */
        ssl_ecrs_crt_vrfy_sign,         /*!< CertificateVerify: pk_sign()   */
    } ecrs_state;                       /*!< current (or last) operation    */
    mbedtls_x509_crt *ecrs_peer_cert;   /*!< The peer's CRT chain.          */
    size_t ecrs_n;                      /*!< place for saving a length      */
#endif

    size_t pmslen;                      /*!<  premaster length        */

    mbedtls_ssl_ciphersuite_t const *ciphersuite_info;

    void (*update_checksum)(mbedtls_ssl_context *, const unsigned char *, size_t);
    void (*calc_verify)(const mbedtls_ssl_context *, unsigned char *, size_t *);
    void (*calc_finished)(mbedtls_ssl_context *, unsigned char *, int);
    mbedtls_ssl_tls_prf_cb *tls_prf;

    /*
     * Handshake specific crypto variables
     */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    uint8_t key_exchange_mode; /*!< Selected key exchange mode */

    /** Number of HelloRetryRequest messages received/sent from/to the server. */
    int hello_retry_request_count;

#if defined(MBEDTLS_SSL_SRV_C)
    /** selected_group of key_share extension in HelloRetryRequest message. */
    uint16_t hrr_selected_group;
#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
    uint8_t tls13_kex_modes; /*!< Key exchange modes supported by the client */
#endif
#endif /* MBEDTLS_SSL_SRV_C */

#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
    uint16_t received_sig_algs[MBEDTLS_RECEIVED_SIG_ALGS_SIZE];
#endif

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
    const uint16_t *group_list;
    const uint16_t *sig_algs;
#endif

#if defined(MBEDTLS_DHM_C)
    mbedtls_dhm_context dhm_ctx;                /*!<  DHM key exchange        */
#endif

/* Adding guard for MBEDTLS_ECDSA_C to ensure no compile errors due
 * to guards in client and server code. There is a gap in functionality that
 * access to ecdh_ctx structure is needed for MBEDTLS_ECDSA_C which does not
 * seem correct.
 */
#if defined(MBEDTLS_ECDH_C) || defined(MBEDTLS_ECDSA_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
    mbedtls_ecdh_context ecdh_ctx;              /*!<  ECDH key exchange       */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */

#if defined(MBEDTLS_USE_PSA_CRYPTO) || defined(MBEDTLS_SSL_PROTO_TLS1_3)
    psa_key_type_t ecdh_psa_type;
    size_t ecdh_bits;
    mbedtls_svc_key_id_t ecdh_psa_privkey;
    uint8_t ecdh_psa_privkey_is_external;
    unsigned char ecdh_psa_peerkey[MBEDTLS_PSA_MAX_EC_PUBKEY_LENGTH];
    size_t ecdh_psa_peerkey_len;
#endif /* MBEDTLS_USE_PSA_CRYPTO || MBEDTLS_SSL_PROTO_TLS1_3 */
#endif /* MBEDTLS_ECDH_C || MBEDTLS_ECDSA_C */

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    mbedtls_ecjpake_context ecjpake_ctx;        /*!< EC J-PAKE key exchange */
#if defined(MBEDTLS_SSL_CLI_C)
    unsigned char *ecjpake_cache;               /*!< Cache for ClientHello ext */
    size_t ecjpake_cache_len;                   /*!< Length of cached data */
#endif
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_ECDH_C) || defined(MBEDTLS_ECDSA_C) ||      \
    defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    const mbedtls_ecp_curve_info **curves;      /*!<  Supported elliptic curves */
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    mbedtls_svc_key_id_t psk_opaque;            /*!< Opaque PSK from the callback   */
    uint8_t psk_opaque_is_internal;
#else
    unsigned char *psk;                 /*!<  PSK from the callback         */
    size_t psk_len;                     /*!<  Length of PSK from callback   */
#endif /* MBEDTLS_USE_PSA_CRYPTO */
    uint16_t    selected_identity;
#endif /* MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED */

#if defined(MBEDTLS_SSL_ECP_RESTARTABLE_ENABLED)
    mbedtls_x509_crt_restart_ctx ecrs_ctx;  /*!< restart context            */
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_key_cert *key_cert;     /*!< chosen key/cert pair (server)  */
#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    mbedtls_ssl_key_cert *sni_key_cert; /*!< key/cert list from SNI         */
    mbedtls_x509_crt *sni_ca_chain;     /*!< trusted CAs from SNI callback  */
    mbedtls_x509_crl *sni_ca_crl;       /*!< trusted CAs CRLs from SNI      */
#endif /* MBEDTLS_SSL_SERVER_NAME_INDICATION */
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_X509_CRT_PARSE_C) &&        \
    !defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
    mbedtls_pk_context peer_pubkey;     /*!< The public key from the peer.  */
#endif /* MBEDTLS_X509_CRT_PARSE_C && !MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */

    struct
    {
        size_t total_bytes_buffered; /*!< Cumulative size of heap allocated
                                      *   buffers used for message buffering. */

        uint8_t seen_ccs;               /*!< Indicates if a CCS message has
                                         *   been seen in the current flight. */

        struct mbedtls_ssl_hs_buffer
        {
            unsigned is_valid      : 1;
            unsigned is_fragmented : 1;
            unsigned is_complete   : 1;
            unsigned char *data;
            size_t data_len;
        } hs[MBEDTLS_SSL_MAX_BUFFERED_HS];

        struct
        {
            unsigned char *data;
            size_t len;
            unsigned epoch;
        } future_record;

    } buffering;

#if defined(MBEDTLS_SSL_CLI_C) && \
    ( defined(MBEDTLS_SSL_PROTO_DTLS) || defined(MBEDTLS_SSL_PROTO_TLS1_3) )
    unsigned char *cookie;              /*!<  HelloVerifyRequest cookie for DTLS
                                         *    HelloRetryRequest cookie for TLS 1.3 */
#endif /* MBEDTLS_SSL_CLI_C &&
          ( MBEDTLS_SSL_PROTO_DTLS || MBEDTLS_SSL_PROTO_TLS1_3 ) */
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    unsigned char verify_cookie_len;    /*!<  Cli: HelloVerifyRequest cookie
                                         *    length
                                         *    Srv: flag for sending a cookie */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
#if defined(MBEDTLS_SSL_CLI_C) && defined(MBEDTLS_SSL_PROTO_TLS1_3)
    uint16_t hrr_cookie_len;            /*!<  HelloRetryRequest cookie length */
#endif /* MBEDTLS_SSL_CLI_C && MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    unsigned int out_msg_seq;           /*!<  Outgoing handshake sequence number */
    unsigned int in_msg_seq;            /*!<  Incoming handshake sequence number */

    uint32_t retransmit_timeout;        /*!<  Current value of timeout       */
    mbedtls_ssl_flight_item *flight;    /*!<  Current outgoing flight        */
    mbedtls_ssl_flight_item *cur_msg;   /*!<  Current message in flight      */
    unsigned char *cur_msg_p;           /*!<  Position in current message    */
    unsigned int in_flight_start_seq;   /*!<  Minimum message sequence in the
                                              flight being received          */
    mbedtls_ssl_transform *alt_transform_out;   /*!<  Alternative transform for
                                              resending messages             */
    unsigned char alt_out_ctr[MBEDTLS_SSL_SEQUENCE_NUMBER_LEN]; /*!<  Alternative record epoch/counter
                                                                      for resending messages         */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    /* The state of CID configuration in this handshake. */

    uint8_t cid_in_use; /*!< This indicates whether the use of the CID extension
                         *   has been negotiated. Possible values are
                         *   #MBEDTLS_SSL_CID_ENABLED and
                         *   #MBEDTLS_SSL_CID_DISABLED. */
    unsigned char peer_cid[ MBEDTLS_SSL_CID_OUT_LEN_MAX ]; /*! The peer's CID */
    uint8_t peer_cid_len;                                  /*!< The length of
                                                            *   \c peer_cid.  */
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

    uint16_t mtu;                       /*!<  Handshake mtu, used to fragment outgoing messages */
#endif /* MBEDTLS_SSL_PROTO_DTLS */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    /*! TLS 1.3 transforms for 0-RTT and encrypted handshake messages.
     *  Those pointers own the transforms they reference. */
    mbedtls_ssl_transform *transform_handshake;
    mbedtls_ssl_transform *transform_earlydata;
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

    /*
     * Checksum contexts
     */
#if defined(MBEDTLS_SHA256_C)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    psa_hash_operation_t fin_sha256_psa;
#else
    mbedtls_sha256_context fin_sha256;
#endif
#endif
#if defined(MBEDTLS_SHA384_C)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    psa_hash_operation_t fin_sha384_psa;
#else
    mbedtls_sha512_context fin_sha512;
#endif
#endif

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    uint16_t offered_group_id; /* The NamedGroup value for the group
                                * that is being used for ephemeral
                                * key exchange.
                                *
                                * On the client: Defaults to the first
                                * entry in the client's group list,
                                * but can be overwritten by the HRR. */
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_CLI_C)
    uint8_t client_auth;       /*!< used to check if CertificateRequest has been
                                    received from server side. If CertificateRequest
                                    has been received, Certificate and CertificateVerify
                                    should be sent to server */
#endif /* MBEDTLS_SSL_CLI_C */
    /*
     * State-local variables used during the processing
     * of a specific handshake state.
     */
    union
    {
        /* Outgoing Finished message */
        struct
        {
            uint8_t preparation_done;

            /* Buffer holding digest of the handshake up to
             * but excluding the outgoing finished message. */
            unsigned char digest[MBEDTLS_TLS1_3_MD_MAX_SIZE];
            size_t digest_len;
        } finished_out;

        /* Incoming Finished message */
        struct
        {
            uint8_t preparation_done;

            /* Buffer holding digest of the handshake up to but
             * excluding the peer's incoming finished message. */
            unsigned char digest[MBEDTLS_TLS1_3_MD_MAX_SIZE];
            size_t digest_len;
        } finished_in;

    } state_local;

    /* End of state-local variables. */

    unsigned char randbytes[MBEDTLS_CLIENT_HELLO_RANDOM_LEN +
                            MBEDTLS_SERVER_HELLO_RANDOM_LEN];
                                        /*!<  random bytes            */
    unsigned char premaster[MBEDTLS_PREMASTER_SIZE];
                                        /*!<  premaster secret        */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    int extensions_present;             /*!< extension presence; Each bitfield
                                             represents an extension and defined
                                             as \c MBEDTLS_SSL_EXT_XXX */

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
    unsigned char certificate_request_context_len;
    unsigned char *certificate_request_context;
#endif

    union
    {
        unsigned char early    [MBEDTLS_TLS1_3_MD_MAX_SIZE];
        unsigned char handshake[MBEDTLS_TLS1_3_MD_MAX_SIZE];
        unsigned char app      [MBEDTLS_TLS1_3_MD_MAX_SIZE];
    } tls13_master_secrets;

    mbedtls_ssl_tls13_handshake_secrets tls13_hs_secrets;
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    /** Asynchronous operation context. This field is meant for use by the
     * asynchronous operation callbacks (mbedtls_ssl_config::f_async_sign_start,
     * mbedtls_ssl_config::f_async_decrypt_start,
     * mbedtls_ssl_config::f_async_resume, mbedtls_ssl_config::f_async_cancel).
     * The library does not use it internally. */
    void *user_async_ctx;
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    const unsigned char *sni_name;      /*!< raw SNI                        */
    size_t sni_name_len;                /*!< raw SNI len                    */
#if defined(MBEDTLS_KEY_EXCHANGE_CERT_REQ_ALLOWED_ENABLED)
    const mbedtls_x509_crt *dn_hints;   /*!< acceptable client cert issuers */
#endif
#endif /* MBEDTLS_SSL_SERVER_NAME_INDICATION */
};

typedef struct mbedtls_ssl_hs_buffer mbedtls_ssl_hs_buffer;

/*
 * Representation of decryption/encryption transformations on records
 *
 * There are the following general types of record transformations:
 * - Stream transformations (TLS versions == 1.2 only)
 *   Transformation adding a MAC and applying a stream-cipher
 *   to the authenticated message.
 * - CBC block cipher transformations ([D]TLS versions == 1.2 only)
 *   For TLS 1.2, no IV is generated at key extraction time, but every
 *   encrypted record is explicitly prefixed by the IV with which it was
 *   encrypted.
 * - AEAD transformations ([D]TLS versions == 1.2 only)
 *   These come in two fundamentally different versions, the first one
 *   used in TLS 1.2, excluding ChaChaPoly ciphersuites, and the second
 *   one used for ChaChaPoly ciphersuites in TLS 1.2 as well as for TLS 1.3.
 *   In the first transformation, the IV to be used for a record is obtained
 *   as the concatenation of an explicit, static 4-byte IV and the 8-byte
 *   record sequence number, and explicitly prepending this sequence number
 *   to the encrypted record. In contrast, in the second transformation
 *   the IV is obtained by XOR'ing a static IV obtained at key extraction
 *   time with the 8-byte record sequence number, without prepending the
 *   latter to the encrypted record.
 *
 * Additionally, DTLS 1.2 + CID as well as TLS 1.3 use an inner plaintext
 * which allows to add flexible length padding and to hide a record's true
 * content type.
 *
 * In addition to type and version, the following parameters are relevant:
 * - The symmetric cipher algorithm to be used.
 * - The (static) encryption/decryption keys for the cipher.
 * - For stream/CBC, the type of message digest to be used.
 * - For stream/CBC, (static) encryption/decryption keys for the digest.
 * - For AEAD transformations, the size (potentially 0) of an explicit,
 *   random initialization vector placed in encrypted records.
 * - For some transformations (currently AEAD) an implicit IV. It is static
 *   and (if present) is combined with the explicit IV in a transformation-
 *   -dependent way (e.g. appending in TLS 1.2 and XOR'ing in TLS 1.3).
 * - For stream/CBC, a flag determining the order of encryption and MAC.
 * - The details of the transformation depend on the SSL/TLS version.
 * - The length of the authentication tag.
 *
 * The struct below refines this abstract view as follows:
 * - The cipher underlying the transformation is managed in
 *   cipher contexts cipher_ctx_{enc/dec}, which must have the
 *   same cipher type. The mode of these cipher contexts determines
 *   the type of the transformation in the sense above: e.g., if
 *   the type is MBEDTLS_CIPHER_AES_256_CBC resp. MBEDTLS_CIPHER_AES_192_GCM
 *   then the transformation has type CBC resp. AEAD.
 * - The cipher keys are never stored explicitly but
 *   are maintained within cipher_ctx_{enc/dec}.
 * - For stream/CBC transformations, the message digest contexts
 *   used for the MAC's are stored in md_ctx_{enc/dec}. These contexts
 *   are unused for AEAD transformations.
 * - For stream/CBC transformations, the MAC keys are not stored explicitly
 *   but maintained within md_ctx_{enc/dec}.
 * - The mac_enc and mac_dec fields are unused for EAD transformations.
 * - For transformations using an implicit IV maintained within
 *   the transformation context, its contents are stored within
 *   iv_{enc/dec}.
 * - The value of ivlen indicates the length of the IV.
 *   This is redundant in case of stream/CBC transformations
 *   which always use 0 resp. the cipher's block length as the
 *   IV length, but is needed for AEAD ciphers and may be
 *   different from the underlying cipher's block length
 *   in this case.
 * - The field fixed_ivlen is nonzero for AEAD transformations only
 *   and indicates the length of the static part of the IV which is
 *   constant throughout the communication, and which is stored in
 *   the first fixed_ivlen bytes of the iv_{enc/dec} arrays.
 * - tls_version denotes the 2-byte TLS version
 * - For stream/CBC transformations, maclen denotes the length of the
 *   authentication tag, while taglen is unused and 0.
 * - For AEAD transformations, taglen denotes the length of the
 *   authentication tag, while maclen is unused and 0.
 * - For CBC transformations, encrypt_then_mac determines the
 *   order of encryption and authentication. This field is unused
 *   in other transformations.
 *
 */
struct mbedtls_ssl_transform
{
    /*
     * Session specific crypto layer
     */
    size_t minlen;                      /*!<  min. ciphertext length  */
    size_t ivlen;                       /*!<  IV length               */
    size_t fixed_ivlen;                 /*!<  Fixed part of IV (AEAD) */
    size_t maclen;                      /*!<  MAC(CBC) len            */
    size_t taglen;                      /*!<  TAG(AEAD) len           */

    unsigned char iv_enc[16];           /*!<  IV (encryption)         */
    unsigned char iv_dec[16];           /*!<  IV (decryption)         */

#if defined(MBEDTLS_SSL_SOME_SUITES_USE_MAC)

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    mbedtls_svc_key_id_t psa_mac_enc;           /*!<  MAC (encryption)        */
    mbedtls_svc_key_id_t psa_mac_dec;           /*!<  MAC (decryption)        */
    psa_algorithm_t psa_mac_alg;                /*!<  psa MAC algorithm       */
#else
    mbedtls_md_context_t md_ctx_enc;            /*!<  MAC (encryption)        */
    mbedtls_md_context_t md_ctx_dec;            /*!<  MAC (decryption)        */
#endif /* MBEDTLS_USE_PSA_CRYPTO */

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    int encrypt_then_mac;       /*!< flag for EtM activation                */
#endif

#endif /* MBEDTLS_SSL_SOME_SUITES_USE_MAC */

    mbedtls_ssl_protocol_version tls_version;

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    mbedtls_svc_key_id_t psa_key_enc;           /*!<  psa encryption key      */
    mbedtls_svc_key_id_t psa_key_dec;           /*!<  psa decryption key      */
    psa_algorithm_t psa_alg;                    /*!<  psa algorithm           */
#else
    mbedtls_cipher_context_t cipher_ctx_enc;    /*!<  encryption context      */
    mbedtls_cipher_context_t cipher_ctx_dec;    /*!<  decryption context      */
#endif /* MBEDTLS_USE_PSA_CRYPTO */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    uint8_t in_cid_len;
    uint8_t out_cid_len;
    unsigned char in_cid [ MBEDTLS_SSL_CID_OUT_LEN_MAX ];
    unsigned char out_cid[ MBEDTLS_SSL_CID_OUT_LEN_MAX ];
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    /* We need the Hello random bytes in order to re-derive keys from the
     * Master Secret and other session info,
     * see ssl_tls12_populate_transform() */
    unsigned char randbytes[MBEDTLS_SERVER_HELLO_RANDOM_LEN +
                            MBEDTLS_CLIENT_HELLO_RANDOM_LEN];
                            /*!< ServerHello.random+ClientHello.random */
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */
};

/*
 * Return 1 if the transform uses an AEAD cipher, 0 otherwise.
 * Equivalently, return 0 if a separate MAC is used, 1 otherwise.
 */
static inline int mbedtls_ssl_transform_uses_aead(
        const mbedtls_ssl_transform *transform )
{
#if defined(MBEDTLS_SSL_SOME_SUITES_USE_MAC)
    return( transform->maclen == 0 && transform->taglen != 0 );
#else
    (void) transform;
    return( 1 );
#endif
}

/*
 * Internal representation of record frames
 *
 * Instances come in two flavors:
 * (1) Encrypted
 *     These always have data_offset = 0
 * (2) Unencrypted
 *     These have data_offset set to the amount of
 *     pre-expansion during record protection. Concretely,
 *     this is the length of the fixed part of the explicit IV
 *     used for encryption, or 0 if no explicit IV is used
 *     (e.g. for stream ciphers).
 *
 * The reason for the data_offset in the unencrypted case
 * is to allow for in-place conversion of an unencrypted to
 * an encrypted record. If the offset wasn't included, the
 * encrypted content would need to be shifted afterwards to
 * make space for the fixed IV.
 *
 */
#if MBEDTLS_SSL_CID_OUT_LEN_MAX > MBEDTLS_SSL_CID_IN_LEN_MAX
#define MBEDTLS_SSL_CID_LEN_MAX MBEDTLS_SSL_CID_OUT_LEN_MAX
#else
#define MBEDTLS_SSL_CID_LEN_MAX MBEDTLS_SSL_CID_IN_LEN_MAX
#endif

typedef struct
{
    uint8_t ctr[MBEDTLS_SSL_SEQUENCE_NUMBER_LEN];  /* In TLS:  The implicit record sequence number.
                                                    * In DTLS: The 2-byte epoch followed by
                                                    *          the 6-byte sequence number.
                                                    * This is stored as a raw big endian byte array
                                                    * as opposed to a uint64_t because we rarely
                                                    * need to perform arithmetic on this, but do
                                                    * need it as a Byte array for the purpose of
                                                    * MAC computations.                             */
    uint8_t type;           /* The record content type.                      */
    uint8_t ver[2];         /* SSL/TLS version as present on the wire.
                             * Convert to internal presentation of versions
                             * using mbedtls_ssl_read_version() and
                             * mbedtls_ssl_write_version().
                             * Keep wire-format for MAC computations.        */

    unsigned char *buf;     /* Memory buffer enclosing the record content    */
    size_t buf_len;         /* Buffer length                                 */
    size_t data_offset;     /* Offset of record content                      */
    size_t data_len;        /* Length of record content                      */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    uint8_t cid_len;        /* Length of the CID (0 if not present)          */
    unsigned char cid[ MBEDTLS_SSL_CID_LEN_MAX ]; /* The CID                 */
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
} mbedtls_record;

#if defined(MBEDTLS_X509_CRT_PARSE_C)
/*
 * List of certificate + private key pairs
 */
struct mbedtls_ssl_key_cert
{
    mbedtls_x509_crt *cert;                 /*!< cert                       */
    mbedtls_pk_context *key;                /*!< private key                */
    mbedtls_ssl_key_cert *next;             /*!< next key/cert pair         */
};
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
/*
 * List of handshake messages kept around for resending
 */
struct mbedtls_ssl_flight_item
{
    unsigned char *p;       /*!< message, including handshake headers   */
    size_t len;             /*!< length of p                            */
    unsigned char type;     /*!< type of the message: handshake or CCS  */
    mbedtls_ssl_flight_item *next;  /*!< next handshake message(s)              */
};
#endif /* MBEDTLS_SSL_PROTO_DTLS */

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
/**
 * \brief Given an SSL context and its associated configuration, write the TLS
 *        1.2 specific extensions of the ClientHello message.
 *
 * \param[in]   ssl     SSL context
 * \param[in]   buf     Base address of the buffer where to write the extensions
 * \param[in]   end     End address of the buffer where to write the extensions
 * \param       uses_ec Whether one proposed ciphersuite uses an elliptic curve
 *                      (<> 0) or not ( 0 ).
 * \param[out]  out_len Length of the data written into the buffer \p buf
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls12_write_client_hello_exts( mbedtls_ssl_context *ssl,
                                               unsigned char *buf,
                                               const unsigned char *end,
                                               int uses_ec,
                                               size_t *out_len );
#endif

#if defined(MBEDTLS_SSL_PROTO_TLS1_2) && \
    defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)

/**
 * \brief Find the preferred hash for a given signature algorithm.
 *
 * \param[in]   ssl     SSL context
 * \param[in]   sig_alg A signature algorithm identifier as defined in the
 *                      TLS 1.2 SignatureAlgorithm enumeration.
 *
 * \return  The preferred hash algorithm for \p sig_alg. It is a hash algorithm
 *          identifier as defined in the TLS 1.2 HashAlgorithm enumeration.
 */
unsigned int mbedtls_ssl_tls12_get_preferred_hash_for_sig_alg(
                mbedtls_ssl_context *ssl,
                unsigned int sig_alg );

#endif /* MBEDTLS_SSL_PROTO_TLS1_2 &&
          MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */

/**
 * \brief           Free referenced items in an SSL transform context and clear
 *                  memory
 *
 * \param transform SSL transform context
 */
void mbedtls_ssl_transform_free( mbedtls_ssl_transform *transform );

/**
 * \brief           Free referenced items in an SSL handshake context and clear
 *                  memory
 *
 * \param ssl       SSL context
 */
void mbedtls_ssl_handshake_free( mbedtls_ssl_context *ssl );

/* set inbound transform of ssl context */
void mbedtls_ssl_set_inbound_transform( mbedtls_ssl_context *ssl,
                                        mbedtls_ssl_transform *transform );

/* set outbound transform of ssl context */
void mbedtls_ssl_set_outbound_transform( mbedtls_ssl_context *ssl,
                                         mbedtls_ssl_transform *transform );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_handshake_client_step( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_handshake_server_step( mbedtls_ssl_context *ssl );
void mbedtls_ssl_handshake_wrapup( mbedtls_ssl_context *ssl );
static inline void mbedtls_ssl_handshake_set_state( mbedtls_ssl_context *ssl,
                                                    mbedtls_ssl_states state )
{
    ssl->state = ( int ) state;
}

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_send_fatal_handshake_failure( mbedtls_ssl_context *ssl );

void mbedtls_ssl_reset_checksum( mbedtls_ssl_context *ssl );

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_derive_keys( mbedtls_ssl_context *ssl );
#endif /* MBEDTLS_SSL_PROTO_TLS1_2  */

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_handle_message_type( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_prepare_handshake_record( mbedtls_ssl_context *ssl );
void mbedtls_ssl_update_handshake_status( mbedtls_ssl_context *ssl );

/**
 * \brief       Update record layer
 *
 *              This function roughly separates the implementation
 *              of the logic of (D)TLS from the implementation
 *              of the secure transport.
 *
 * \param  ssl              The SSL context to use.
 * \param  update_hs_digest This indicates if the handshake digest
 *                          should be automatically updated in case
 *                          a handshake message is found.
 *
 * \return      0 or non-zero error code.
 *
 * \note        A clarification on what is called 'record layer' here
 *              is in order, as many sensible definitions are possible:
 *
 *              The record layer takes as input an untrusted underlying
 *              transport (stream or datagram) and transforms it into
 *              a serially multiplexed, secure transport, which
 *              conceptually provides the following:
 *
 *              (1) Three datagram based, content-agnostic transports
 *                  for handshake, alert and CCS messages.
 *              (2) One stream- or datagram-based transport
 *                  for application data.
 *              (3) Functionality for changing the underlying transform
 *                  securing the contents.
 *
 *              The interface to this functionality is given as follows:
 *
 *              a Updating
 *                [Currently implemented by mbedtls_ssl_read_record]
 *
 *                Check if and on which of the four 'ports' data is pending:
 *                Nothing, a controlling datagram of type (1), or application
 *                data (2). In any case data is present, internal buffers
 *                provide access to the data for the user to process it.
 *                Consumption of type (1) datagrams is done automatically
 *                on the next update, invalidating that the internal buffers
 *                for previous datagrams, while consumption of application
 *                data (2) is user-controlled.
 *
 *              b Reading of application data
 *                [Currently manual adaption of ssl->in_offt pointer]
 *
 *                As mentioned in the last paragraph, consumption of data
 *                is different from the automatic consumption of control
 *                datagrams (1) because application data is treated as a stream.
 *
 *              c Tracking availability of application data
 *                [Currently manually through decreasing ssl->in_msglen]
 *
 *                For efficiency and to retain datagram semantics for
 *                application data in case of DTLS, the record layer
 *                provides functionality for checking how much application
 *                data is still available in the internal buffer.
 *
 *              d Changing the transformation securing the communication.
 *
 *              Given an opaque implementation of the record layer in the
 *              above sense, it should be possible to implement the logic
 *              of (D)TLS on top of it without the need to know anything
 *              about the record layer's internals. This is done e.g.
 *              in all the handshake handling functions, and in the
 *              application data reading function mbedtls_ssl_read.
 *
 * \note        The above tries to give a conceptual picture of the
 *              record layer, but the current implementation deviates
 *              from it in some places. For example, our implementation of
 *              the update functionality through mbedtls_ssl_read_record
 *              discards datagrams depending on the current state, which
 *              wouldn't fall under the record layer's responsibility
 *              following the above definition.
 *
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_read_record( mbedtls_ssl_context *ssl,
                             unsigned update_hs_digest );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_fetch_input( mbedtls_ssl_context *ssl, size_t nb_want );

/*
 * Write handshake message header
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_start_handshake_msg( mbedtls_ssl_context *ssl, unsigned hs_type,
                                     unsigned char **buf, size_t *buf_len );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_write_handshake_msg_ext( mbedtls_ssl_context *ssl,
                                         int update_checksum,
                                         int force_flush );
static inline int mbedtls_ssl_write_handshake_msg( mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_write_handshake_msg_ext( ssl, 1 /* update checksum */, 1 /* force flush */ ) );
}

/*
 * Write handshake message tail
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_finish_handshake_msg( mbedtls_ssl_context *ssl,
                                      size_t buf_len, size_t msg_len );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_write_record( mbedtls_ssl_context *ssl, int force_flush );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_flush_output( mbedtls_ssl_context *ssl );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_parse_certificate( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_write_certificate( mbedtls_ssl_context *ssl );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_parse_change_cipher_spec( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_write_change_cipher_spec( mbedtls_ssl_context *ssl );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_parse_finished( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_write_finished( mbedtls_ssl_context *ssl );

void mbedtls_ssl_optimize_checksum( mbedtls_ssl_context *ssl,
                            const mbedtls_ssl_ciphersuite_t *ciphersuite_info );

/*
 * Update checksum of handshake messages.
 */
void mbedtls_ssl_add_hs_msg_to_checksum( mbedtls_ssl_context *ssl,
                                         unsigned hs_type,
                                         unsigned char const *msg,
                                         size_t msg_len );

void mbedtls_ssl_add_hs_hdr_to_checksum( mbedtls_ssl_context *ssl,
                                         unsigned hs_type,
                                         size_t total_hs_len );

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_psk_derive_premaster( mbedtls_ssl_context *ssl,
                                      mbedtls_key_exchange_type_t key_ex );
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#if defined(MBEDTLS_SSL_CLI_C) && defined(MBEDTLS_SSL_PROTO_TLS1_2)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_conf_has_static_psk( mbedtls_ssl_config const *conf );
#endif

#if defined(MBEDTLS_USE_PSA_CRYPTO)
/**
 * Get the first defined opaque PSK by order of precedence:
 * 1. handshake PSK set by \c mbedtls_ssl_set_hs_psk_opaque() in the PSK
 *    callback
 * 2. static PSK configured by \c mbedtls_ssl_conf_psk_opaque()
 * Return an opaque PSK
 */
static inline mbedtls_svc_key_id_t mbedtls_ssl_get_opaque_psk(
    const mbedtls_ssl_context *ssl )
{
    if( ! mbedtls_svc_key_id_is_null( ssl->handshake->psk_opaque ) )
        return( ssl->handshake->psk_opaque );

    if( ! mbedtls_svc_key_id_is_null( ssl->conf->psk_opaque ) )
        return( ssl->conf->psk_opaque );

    return( MBEDTLS_SVC_KEY_ID_INIT );
}
#else
/**
 * Get the first defined PSK by order of precedence:
 * 1. handshake PSK set by \c mbedtls_ssl_set_hs_psk() in the PSK callback
 * 2. static PSK configured by \c mbedtls_ssl_conf_psk()
 * Return a code and update the pair (PSK, PSK length) passed to this function
 */
static inline int mbedtls_ssl_get_psk( const mbedtls_ssl_context *ssl,
    const unsigned char **psk, size_t *psk_len )
{
    if( ssl->handshake->psk != NULL && ssl->handshake->psk_len > 0 )
    {
        *psk = ssl->handshake->psk;
        *psk_len = ssl->handshake->psk_len;
    }

    else if( ssl->conf->psk != NULL && ssl->conf->psk_len > 0 )
    {
        *psk = ssl->conf->psk;
        *psk_len = ssl->conf->psk_len;
    }

    else
    {
        *psk = NULL;
        *psk_len = 0;
        return( MBEDTLS_ERR_SSL_PRIVATE_KEY_REQUIRED );
    }

    return( 0 );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */

#endif /* MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED */

#if defined(MBEDTLS_PK_C)
unsigned char mbedtls_ssl_sig_from_pk( mbedtls_pk_context *pk );
unsigned char mbedtls_ssl_sig_from_pk_alg( mbedtls_pk_type_t type );
mbedtls_pk_type_t mbedtls_ssl_pk_alg_from_sig( unsigned char sig );
#endif

mbedtls_md_type_t mbedtls_ssl_md_alg_from_hash( unsigned char hash );
unsigned char mbedtls_ssl_hash_from_md_alg( int md );

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_set_calc_verify_md( mbedtls_ssl_context *ssl, int md );
#endif

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_check_curve_tls_id( const mbedtls_ssl_context *ssl, uint16_t tls_id );
#if defined(MBEDTLS_ECP_C)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_check_curve( const mbedtls_ssl_context *ssl, mbedtls_ecp_group_id grp_id );
#endif

#if defined(MBEDTLS_SSL_DTLS_SRTP)
static inline mbedtls_ssl_srtp_profile mbedtls_ssl_check_srtp_profile_value
                                                    ( const uint16_t srtp_profile_value )
{
    switch( srtp_profile_value )
    {
        case MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80:
        case MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32:
        case MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_80:
        case MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_32:
            return srtp_profile_value;
        default: break;
    }
    return( MBEDTLS_TLS_SRTP_UNSET );
}
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
static inline mbedtls_pk_context *mbedtls_ssl_own_key( mbedtls_ssl_context *ssl )
{
    mbedtls_ssl_key_cert *key_cert;

    if( ssl->handshake != NULL && ssl->handshake->key_cert != NULL )
        key_cert = ssl->handshake->key_cert;
    else
        key_cert = ssl->conf->key_cert;

    return( key_cert == NULL ? NULL : key_cert->key );
}

static inline mbedtls_x509_crt *mbedtls_ssl_own_cert( mbedtls_ssl_context *ssl )
{
    mbedtls_ssl_key_cert *key_cert;

    if( ssl->handshake != NULL && ssl->handshake->key_cert != NULL )
        key_cert = ssl->handshake->key_cert;
    else
        key_cert = ssl->conf->key_cert;

    return( key_cert == NULL ? NULL : key_cert->cert );
}

/*
 * Check usage of a certificate wrt extensions:
 * keyUsage, extendedKeyUsage (later), and nSCertType (later).
 *
 * Warning: cert_endpoint is the endpoint of the cert (ie, of our peer when we
 * check a cert we received from them)!
 *
 * Return 0 if everything is OK, -1 if not.
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_check_cert_usage( const mbedtls_x509_crt *cert,
                          const mbedtls_ssl_ciphersuite_t *ciphersuite,
                          int cert_endpoint,
                          uint32_t *flags );
#endif /* MBEDTLS_X509_CRT_PARSE_C */

void mbedtls_ssl_write_version( unsigned char version[2], int transport,
                                mbedtls_ssl_protocol_version tls_version );
uint16_t mbedtls_ssl_read_version( const unsigned char version[2],
                                   int transport );

static inline size_t mbedtls_ssl_in_hdr_len( const mbedtls_ssl_context *ssl )
{
#if !defined(MBEDTLS_SSL_PROTO_DTLS)
    ((void) ssl);
#endif

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
        return( 13 );
    }
    else
#endif /* MBEDTLS_SSL_PROTO_DTLS */
    {
        return( 5 );
    }
}

static inline size_t mbedtls_ssl_out_hdr_len( const mbedtls_ssl_context *ssl )
{
    return( (size_t) ( ssl->out_iv - ssl->out_hdr ) );
}

static inline size_t mbedtls_ssl_hs_hdr_len( const mbedtls_ssl_context *ssl )
{
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        return( 12 );
#else
    ((void) ssl);
#endif
    return( 4 );
}

#if defined(MBEDTLS_SSL_PROTO_DTLS)
void mbedtls_ssl_send_flight_completed( mbedtls_ssl_context *ssl );
void mbedtls_ssl_recv_flight_completed( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_resend( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_flight_transmit( mbedtls_ssl_context *ssl );
#endif

/* Visible for testing purposes only */
#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_dtls_replay_check( mbedtls_ssl_context const *ssl );
void mbedtls_ssl_dtls_replay_update( mbedtls_ssl_context *ssl );
#endif

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_session_copy( mbedtls_ssl_session *dst,
                              const mbedtls_ssl_session *src );

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
/* The hash buffer must have at least MBEDTLS_MD_MAX_SIZE bytes of length. */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_get_key_exchange_md_tls1_2( mbedtls_ssl_context *ssl,
                                            unsigned char *hash, size_t *hashlen,
                                            unsigned char *data, size_t data_len,
                                            mbedtls_md_type_t md_alg );
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

#ifdef __cplusplus
}
#endif

void mbedtls_ssl_transform_init( mbedtls_ssl_transform *transform );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_encrypt_buf( mbedtls_ssl_context *ssl,
                             mbedtls_ssl_transform *transform,
                             mbedtls_record *rec,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_decrypt_buf( mbedtls_ssl_context const *ssl,
                             mbedtls_ssl_transform *transform,
                             mbedtls_record *rec );

/* Length of the "epoch" field in the record header */
static inline size_t mbedtls_ssl_ep_len( const mbedtls_ssl_context *ssl )
{
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        return( 2 );
#else
    ((void) ssl);
#endif
    return( 0 );
}

#if defined(MBEDTLS_SSL_PROTO_DTLS)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_resend_hello_request( mbedtls_ssl_context *ssl );
#endif /* MBEDTLS_SSL_PROTO_DTLS */

void mbedtls_ssl_set_timer( mbedtls_ssl_context *ssl, uint32_t millisecs );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_check_timer( mbedtls_ssl_context *ssl );

void mbedtls_ssl_reset_in_out_pointers( mbedtls_ssl_context *ssl );
void mbedtls_ssl_update_out_pointers( mbedtls_ssl_context *ssl,
                              mbedtls_ssl_transform *transform );
void mbedtls_ssl_update_in_pointers( mbedtls_ssl_context *ssl );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_session_reset_int( mbedtls_ssl_context *ssl, int partial );
void mbedtls_ssl_session_reset_msg_layer( mbedtls_ssl_context *ssl,
                                          int partial );

/*
 * Send pending alert
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_handle_pending_alert( mbedtls_ssl_context *ssl );

/*
 * Set pending fatal alert flag.
 */
void mbedtls_ssl_pend_fatal_alert( mbedtls_ssl_context *ssl,
                                   unsigned char alert_type,
                                   int alert_reason );

/* Alias of mbedtls_ssl_pend_fatal_alert */
#define MBEDTLS_SSL_PEND_FATAL_ALERT( type, user_return_value )         \
            mbedtls_ssl_pend_fatal_alert( ssl, type, user_return_value )

#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
void mbedtls_ssl_dtls_replay_reset( mbedtls_ssl_context *ssl );
#endif

void mbedtls_ssl_handshake_wrapup_free_hs_transform( mbedtls_ssl_context *ssl );

#if defined(MBEDTLS_SSL_RENEGOTIATION)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_start_renegotiation( mbedtls_ssl_context *ssl );
#endif /* MBEDTLS_SSL_RENEGOTIATION */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
size_t mbedtls_ssl_get_current_mtu( const mbedtls_ssl_context *ssl );
void mbedtls_ssl_buffering_free( mbedtls_ssl_context *ssl );
void mbedtls_ssl_flight_free( mbedtls_ssl_flight_item *flight );
#endif /* MBEDTLS_SSL_PROTO_DTLS */

/**
 * ssl utils functions for checking configuration.
 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
static inline int mbedtls_ssl_conf_is_tls13_only( const mbedtls_ssl_config *conf )
{
    return( conf->min_tls_version == MBEDTLS_SSL_VERSION_TLS1_3 &&
            conf->max_tls_version == MBEDTLS_SSL_VERSION_TLS1_3 );
}

#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
static inline int mbedtls_ssl_conf_is_tls12_only( const mbedtls_ssl_config *conf )
{
    return( conf->min_tls_version == MBEDTLS_SSL_VERSION_TLS1_2 &&
            conf->max_tls_version == MBEDTLS_SSL_VERSION_TLS1_2 );
}

#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

static inline int mbedtls_ssl_conf_is_tls13_enabled( const mbedtls_ssl_config *conf )
{
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    return( conf->min_tls_version <= MBEDTLS_SSL_VERSION_TLS1_3 &&
            conf->max_tls_version >= MBEDTLS_SSL_VERSION_TLS1_3 );
#else
    ((void) conf);
    return( 0 );
#endif
}

static inline int mbedtls_ssl_conf_is_tls12_enabled( const mbedtls_ssl_config *conf )
{
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    return( conf->min_tls_version <= MBEDTLS_SSL_VERSION_TLS1_2 &&
            conf->max_tls_version >= MBEDTLS_SSL_VERSION_TLS1_2 );
#else
    ((void) conf);
    return( 0 );
#endif
}

#if defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_SSL_PROTO_TLS1_3)
static inline int mbedtls_ssl_conf_is_hybrid_tls12_tls13( const mbedtls_ssl_config *conf )
{
    return( conf->min_tls_version == MBEDTLS_SSL_VERSION_TLS1_2 &&
            conf->max_tls_version == MBEDTLS_SSL_VERSION_TLS1_3 );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 && MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
extern const uint8_t mbedtls_ssl_tls13_hello_retry_request_magic[
                        MBEDTLS_SERVER_HELLO_RANDOM_LEN ];
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_process_finished_message( mbedtls_ssl_context *ssl );
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_write_finished_message( mbedtls_ssl_context *ssl );
void mbedtls_ssl_tls13_handshake_wrapup( mbedtls_ssl_context *ssl );

/**
 * \brief Given an SSL context and its associated configuration, write the TLS
 *        1.3 specific extensions of the ClientHello message.
 *
 * \param[in]   ssl     SSL context
 * \param[in]   buf     Base address of the buffer where to write the extensions
 * \param[in]   end     End address of the buffer where to write the extensions
 * \param[out]  out_len Length of the data written into the buffer \p buf
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_write_client_hello_exts( mbedtls_ssl_context *ssl,
                                               unsigned char *buf,
                                               unsigned char *end,
                                               size_t *out_len );

/**
 * \brief           TLS 1.3 client side state machine entry
 *
 * \param ssl       SSL context
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_handshake_client_step( mbedtls_ssl_context *ssl );

/**
 * \brief           TLS 1.3 server side state machine entry
 *
 * \param ssl       SSL context
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_handshake_server_step( mbedtls_ssl_context *ssl );


/*
 * Helper functions around key exchange modes.
 */
static inline unsigned mbedtls_ssl_conf_tls13_check_kex_modes( mbedtls_ssl_context *ssl,
                                                               int kex_mode_mask )
{
    return( ( ssl->conf->tls13_kex_modes & kex_mode_mask ) != 0 );
}

static inline int mbedtls_ssl_conf_tls13_psk_enabled( mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_conf_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK ) );
}

static inline int mbedtls_ssl_conf_tls13_psk_ephemeral_enabled( mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_conf_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL ) );
}

static inline int mbedtls_ssl_conf_tls13_ephemeral_enabled( mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_conf_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL ) );
}

static inline int mbedtls_ssl_conf_tls13_some_ephemeral_enabled( mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_conf_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ALL ) );
}

static inline int mbedtls_ssl_conf_tls13_some_psk_enabled( mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_conf_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ALL ) );
}

#if defined(MBEDTLS_SSL_SRV_C) && defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
/**
 * Given a list of key exchange modes, check if at least one of them is
 * supported.
 *
 * \param[in] ssl  SSL context
 * \param kex_modes_mask  Mask of the key exchange modes to check
 *
 * \return 0 if at least one of the key exchange modes is supported,
 *         !=0 otherwise.
 */
static inline unsigned mbedtls_ssl_tls13_check_kex_modes( mbedtls_ssl_context *ssl,
                                                          int kex_modes_mask )
{
    return( ( ssl->handshake->tls13_kex_modes & kex_modes_mask ) == 0 );
}

static inline int mbedtls_ssl_tls13_psk_enabled( mbedtls_ssl_context *ssl )
{
    return( ! mbedtls_ssl_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK ) );
}

static inline int mbedtls_ssl_tls13_psk_ephemeral_enabled(
                                                    mbedtls_ssl_context *ssl )
{
    return( ! mbedtls_ssl_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL ) );
}

static inline int mbedtls_ssl_tls13_ephemeral_enabled( mbedtls_ssl_context *ssl )
{
    return( ! mbedtls_ssl_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL ) );
}

static inline int mbedtls_ssl_tls13_some_ephemeral_enabled( mbedtls_ssl_context *ssl )
{
    return( ! mbedtls_ssl_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ALL ) );
}

static inline int mbedtls_ssl_tls13_some_psk_enabled( mbedtls_ssl_context *ssl )
{
    return( ! mbedtls_ssl_tls13_check_kex_modes( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ALL ) );
}
#endif /* MBEDTLS_SSL_SRV_C && MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED */

/*
 * Helper functions to check the selected key exchange mode.
 */
static inline int mbedtls_ssl_tls13_key_exchange_mode_check(
    mbedtls_ssl_context *ssl, int kex_mask )
{
    return( ( ssl->handshake->key_exchange_mode & kex_mask ) != 0 );
}

static inline int mbedtls_ssl_tls13_key_exchange_mode_with_psk(
    mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_tls13_key_exchange_mode_check( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ALL ) );
}

static inline int mbedtls_ssl_tls13_key_exchange_mode_with_ephemeral(
    mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_tls13_key_exchange_mode_check( ssl,
                   MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ALL ) );
}

/*
 * Fetch TLS 1.3 handshake message header
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_fetch_handshake_msg( mbedtls_ssl_context *ssl,
                                           unsigned hs_type,
                                           unsigned char **buf,
                                           size_t *buf_len );

/*
 * Handler of TLS 1.3 server certificate message
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_process_certificate( mbedtls_ssl_context *ssl );

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
/*
 * Handler of TLS 1.3 write Certificate message
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_write_certificate( mbedtls_ssl_context *ssl );

/*
 * Handler of TLS 1.3 write Certificate Verify message
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_write_certificate_verify( mbedtls_ssl_context *ssl );

#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */

/*
 * Generic handler of Certificate Verify
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_process_certificate_verify( mbedtls_ssl_context *ssl );

/*
 * Write of dummy-CCS's for middlebox compatibility
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_write_change_cipher_spec( mbedtls_ssl_context *ssl );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_reset_transcript_for_hrr( mbedtls_ssl_context *ssl );

#if defined(MBEDTLS_ECDH_C)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_generate_and_write_ecdh_key_exchange(
                mbedtls_ssl_context *ssl,
                uint16_t named_group,
                unsigned char *buf,
                unsigned char *end,
                size_t *out_len );
#endif /* MBEDTLS_ECDH_C */


#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
/*
 * Parse TLS Signature Algorithm extension
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_parse_sig_alg_ext( mbedtls_ssl_context *ssl,
                                   const unsigned char *buf,
                                   const unsigned char *end );
#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */

/* Get handshake transcript */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_get_handshake_transcript( mbedtls_ssl_context *ssl,
                                          const mbedtls_md_type_t md,
                                          unsigned char *dst,
                                          size_t dst_len,
                                          size_t *olen );

/*
 * Return supported groups.
 *
 * In future, invocations can be changed to ssl->conf->group_list
 * when mbedtls_ssl_conf_curves() is deleted.
 *
 * ssl->handshake->group_list is either a translation of curve_list to IANA TLS group
 * identifiers when mbedtls_ssl_conf_curves() has been used, or a pointer to
 * ssl->conf->group_list when mbedtls_ssl_conf_groups() has been more recently invoked.
 *
 */
static inline const void *mbedtls_ssl_get_groups( const mbedtls_ssl_context *ssl )
{
    #if defined(MBEDTLS_DEPRECATED_REMOVED) || !defined(MBEDTLS_ECP_C)
    return( ssl->conf->group_list );
    #else
    if( ( ssl->handshake != NULL ) && ( ssl->handshake->group_list != NULL ) )
        return( ssl->handshake->group_list );
    else
        return( ssl->conf->group_list );
    #endif
}

/*
 * Helper functions for NamedGroup.
 */
static inline int mbedtls_ssl_tls12_named_group_is_ecdhe( uint16_t named_group )
{
    /*
     * RFC 8422 section 5.1.1
     */
    return( named_group == MBEDTLS_SSL_IANA_TLS_GROUP_X25519    ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_BP256R1   ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_BP384R1   ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_BP512R1   ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_X448      ||
            /* Below deprecated curves should be removed with notice to users */
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP192K1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP192R1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP224K1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP224R1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP256K1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP256R1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP384R1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP521R1 );
}

static inline int mbedtls_ssl_tls13_named_group_is_ecdhe( uint16_t named_group )
{
    return( named_group == MBEDTLS_SSL_IANA_TLS_GROUP_X25519    ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP256R1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP384R1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_SECP521R1 ||
            named_group == MBEDTLS_SSL_IANA_TLS_GROUP_X448 );
}

static inline int mbedtls_ssl_tls13_named_group_is_dhe( uint16_t named_group )
{
    return( named_group >= MBEDTLS_SSL_IANA_TLS_GROUP_FFDHE2048 &&
            named_group <= MBEDTLS_SSL_IANA_TLS_GROUP_FFDHE8192 );
}

static inline int mbedtls_ssl_named_group_is_offered(
                        const mbedtls_ssl_context *ssl, uint16_t named_group )
{
    const uint16_t *group_list = mbedtls_ssl_get_groups( ssl );

    if( group_list == NULL )
        return( 0 );

    for( ; *group_list != 0; group_list++ )
    {
        if( *group_list == named_group )
            return( 1 );
    }

    return( 0 );
}

static inline int mbedtls_ssl_named_group_is_supported( uint16_t named_group )
{
#if defined(MBEDTLS_ECDH_C)
    if( mbedtls_ssl_tls13_named_group_is_ecdhe( named_group ) )
    {
        const mbedtls_ecp_curve_info *curve_info =
            mbedtls_ecp_curve_info_from_tls_id( named_group );
        if( curve_info != NULL )
            return( 1 );
    }
#else
    ((void) named_group);
#endif /* MBEDTLS_ECDH_C */
    return( 0 );
}

/*
 * Return supported signature algorithms.
 *
 * In future, invocations can be changed to ssl->conf->sig_algs when
 * mbedtls_ssl_conf_sig_hashes() is deleted.
 *
 * ssl->handshake->sig_algs is either a translation of sig_hashes to IANA TLS
 * signature algorithm identifiers when mbedtls_ssl_conf_sig_hashes() has been
 * used, or a pointer to ssl->conf->sig_algs when mbedtls_ssl_conf_sig_algs() has
 * been more recently invoked.
 *
 */
static inline const void *mbedtls_ssl_get_sig_algs(
                                                const mbedtls_ssl_context *ssl )
{
#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
    if( ssl->handshake != NULL &&
        ssl->handshake->sig_algs_heap_allocated == 1 &&
        ssl->handshake->sig_algs != NULL )
    {
        return( ssl->handshake->sig_algs );
    }
#endif
    return( ssl->conf->sig_algs );

#else /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */

    ((void) ssl);
    return( NULL );
#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */
}


#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
static inline int mbedtls_ssl_sig_alg_is_received( const mbedtls_ssl_context *ssl,
                                                   uint16_t own_sig_alg )
{
    const uint16_t *sig_alg = ssl->handshake->received_sig_algs;
    if( sig_alg == NULL )
        return( 0 );

    for( ; *sig_alg != MBEDTLS_TLS_SIG_NONE; sig_alg++ )
    {
        if( *sig_alg == own_sig_alg )
            return( 1 );
    }
    return( 0 );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

static inline int mbedtls_ssl_sig_alg_is_offered( const mbedtls_ssl_context *ssl,
                                                  uint16_t proposed_sig_alg )
{
    const uint16_t *sig_alg = mbedtls_ssl_get_sig_algs( ssl );
    if( sig_alg == NULL )
        return( 0 );

    for( ; *sig_alg != MBEDTLS_TLS_SIG_NONE; sig_alg++ )
    {
        if( *sig_alg == proposed_sig_alg )
            return( 1 );
    }
    return( 0 );
}

static inline int mbedtls_ssl_tls13_get_pk_type_and_md_alg_from_sig_alg(
    uint16_t sig_alg, mbedtls_pk_type_t *pk_type, mbedtls_md_type_t *md_alg )
{
    *pk_type = mbedtls_ssl_pk_alg_from_sig( sig_alg & 0xff );
    *md_alg = mbedtls_ssl_md_alg_from_hash( ( sig_alg >> 8 ) & 0xff );

    if( *pk_type != MBEDTLS_PK_NONE && *md_alg != MBEDTLS_MD_NONE )
        return( 0 );

    switch( sig_alg )
    {
#if defined(MBEDTLS_PKCS1_V21)
#if defined(MBEDTLS_SHA256_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA256:
            *md_alg = MBEDTLS_MD_SHA256;
            *pk_type = MBEDTLS_PK_RSASSA_PSS;
            break;
#endif /* MBEDTLS_SHA256_C  */
#if defined(MBEDTLS_SHA384_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA384:
            *md_alg = MBEDTLS_MD_SHA384;
            *pk_type = MBEDTLS_PK_RSASSA_PSS;
            break;
#endif /* MBEDTLS_SHA384_C */
#if defined(MBEDTLS_SHA512_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA512:
            *md_alg = MBEDTLS_MD_SHA512;
            *pk_type = MBEDTLS_PK_RSASSA_PSS;
            break;
#endif /* MBEDTLS_SHA512_C */
#endif /* MBEDTLS_PKCS1_V21 */
            default:
                return( MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE );
        }
        return( 0 );
}

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
static inline int mbedtls_ssl_tls13_sig_alg_for_cert_verify_is_supported(
                                                    const uint16_t sig_alg )
{
    switch( sig_alg )
    {
#if defined(MBEDTLS_ECDSA_C)
#if defined(MBEDTLS_SHA256_C) && defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
        case MBEDTLS_TLS1_3_SIG_ECDSA_SECP256R1_SHA256:
            break;
#endif /* MBEDTLS_SHA256_C && MBEDTLS_ECP_DP_SECP256R1_ENABLED */
#if defined(MBEDTLS_SHA384_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
        case MBEDTLS_TLS1_3_SIG_ECDSA_SECP384R1_SHA384:
            break;
#endif /* MBEDTLS_SHA384_C && MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
        case MBEDTLS_TLS1_3_SIG_ECDSA_SECP521R1_SHA512:
            break;
#endif /* MBEDTLS_SHA512_C && MBEDTLS_ECP_DP_SECP521R1_ENABLED */
#endif /* MBEDTLS_ECDSA_C */

#if defined(MBEDTLS_PKCS1_V21)
#if defined(MBEDTLS_SHA256_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA256:
            break;
#endif /* MBEDTLS_SHA256_C  */
#if defined(MBEDTLS_SHA384_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA384:
            break;
#endif /* MBEDTLS_SHA384_C */
#if defined(MBEDTLS_SHA512_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA512:
            break;
#endif /* MBEDTLS_SHA512_C */
#endif /* MBEDTLS_PKCS1_V21 */
        default:
            return( 0 );
    }
    return( 1 );

}

static inline int mbedtls_ssl_tls13_sig_alg_is_supported(
                                                    const uint16_t sig_alg )
{
    switch( sig_alg )
    {
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SHA256_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA256:
            break;
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA384_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA384:
            break;
#endif /* MBEDTLS_SHA384_C */
#if defined(MBEDTLS_SHA512_C)
        case MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA512:
            break;
#endif /* MBEDTLS_SHA512_C */
#endif /* MBEDTLS_PKCS1_V15 */
        default:
            return( mbedtls_ssl_tls13_sig_alg_for_cert_verify_is_supported(
                                                                    sig_alg ) );
    }
    return( 1 );
}

#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
static inline int mbedtls_ssl_tls12_sig_alg_is_supported(
                                                    const uint16_t sig_alg )
{
    /* High byte is hash */
    unsigned char hash = MBEDTLS_BYTE_1( sig_alg );
    unsigned char sig = MBEDTLS_BYTE_0( sig_alg );

    switch( hash )
    {
#if defined(MBEDTLS_MD5_C)
        case MBEDTLS_SSL_HASH_MD5:
            break;
#endif

#if defined(MBEDTLS_SHA1_C)
        case MBEDTLS_SSL_HASH_SHA1:
            break;
#endif

#if defined(MBEDTLS_SHA224_C)
        case MBEDTLS_SSL_HASH_SHA224:
            break;
#endif

#if defined(MBEDTLS_SHA256_C)
        case MBEDTLS_SSL_HASH_SHA256:
            break;
#endif

#if defined(MBEDTLS_SHA384_C)
        case MBEDTLS_SSL_HASH_SHA384:
            break;
#endif

#if defined(MBEDTLS_SHA512_C)
        case MBEDTLS_SSL_HASH_SHA512:
            break;
#endif

        default:
            return( 0 );
    }

    switch( sig )
    {
#if defined(MBEDTLS_RSA_C)
        case MBEDTLS_SSL_SIG_RSA:
            break;
#endif

#if defined(MBEDTLS_ECDSA_C)
        case MBEDTLS_SSL_SIG_ECDSA:
            break;
#endif

    default:
        return( 0 );
    }

    return( 1 );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

static inline int mbedtls_ssl_sig_alg_is_supported(
                                                const mbedtls_ssl_context *ssl,
                                                const uint16_t sig_alg )
{

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    if( ssl->tls_version == MBEDTLS_SSL_VERSION_TLS1_2 )
    {
        return( mbedtls_ssl_tls12_sig_alg_is_supported( sig_alg ) );
    }
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    if( ssl->tls_version == MBEDTLS_SSL_VERSION_TLS1_3 )
    {
       return( mbedtls_ssl_tls13_sig_alg_is_supported( sig_alg ) );
    }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
    ((void) ssl);
    ((void) sig_alg);
    return( 0 );
}

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_check_sig_alg_cert_key_match( uint16_t sig_alg,
                                                    mbedtls_pk_context *key );

#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */

#if defined(MBEDTLS_USE_PSA_CRYPTO) || defined(MBEDTLS_SSL_PROTO_TLS1_3)
/* Corresponding PSA algorithm for MBEDTLS_CIPHER_NULL.
 * Same value is used for PSA_ALG_CATEGORY_CIPHER, hence it is
 * guaranteed to not be a valid PSA algorithm identifier.
 */
#define MBEDTLS_SSL_NULL_CIPHER 0x04000000

/**
 * \brief       Translate mbedtls cipher type/taglen pair to psa:
 *              algorithm, key type and key size.
 *
 * \param  mbedtls_cipher_type [in] given mbedtls cipher type
 * \param  taglen              [in] given tag length
 *                                  0 - default tag length
 * \param  alg                 [out] corresponding PSA alg
 *                                   There is no corresponding PSA
 *                                   alg for MBEDTLS_CIPHER_NULL, so
 *                                   in this case MBEDTLS_SSL_NULL_CIPHER
 *                                   is returned via this parameter
 * \param  key_type            [out] corresponding PSA key type
 * \param  key_size            [out] corresponding PSA key size
 *
 * \return                     PSA_SUCCESS on success or PSA_ERROR_NOT_SUPPORTED if
 *                             conversion is not supported.
 */
psa_status_t mbedtls_ssl_cipher_to_psa( mbedtls_cipher_type_t mbedtls_cipher_type,
                                    size_t taglen,
                                    psa_algorithm_t *alg,
                                    psa_key_type_t *key_type,
                                    size_t *key_size );

/**
 * \brief       Convert given PSA status to mbedtls error code.
 *
 * \param  status      [in] given PSA status
 *
 * \return             corresponding mbedtls error code
 */
static inline int psa_ssl_status_to_mbedtls( psa_status_t status )
{
    switch( status )
    {
        case PSA_SUCCESS:
            return( 0 );
        case PSA_ERROR_INSUFFICIENT_MEMORY:
            return( MBEDTLS_ERR_SSL_ALLOC_FAILED );
        case PSA_ERROR_NOT_SUPPORTED:
            return( MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE );
        case PSA_ERROR_INVALID_SIGNATURE:
            return( MBEDTLS_ERR_SSL_INVALID_MAC );
        case PSA_ERROR_INVALID_ARGUMENT:
            return( MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
        case PSA_ERROR_BAD_STATE:
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        case PSA_ERROR_BUFFER_TOO_SMALL:
            return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        default:
            return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }
}
#endif /* MBEDTLS_USE_PSA_CRYPTO || MBEDTLS_SSL_PROTO_TLS1_3 */

/**
 * \brief       TLS record protection modes
 */
typedef enum {
    MBEDTLS_SSL_MODE_STREAM = 0,
    MBEDTLS_SSL_MODE_CBC,
    MBEDTLS_SSL_MODE_CBC_ETM,
    MBEDTLS_SSL_MODE_AEAD
} mbedtls_ssl_mode_t;

mbedtls_ssl_mode_t mbedtls_ssl_get_mode_from_transform(
        const mbedtls_ssl_transform *transform );

#if defined(MBEDTLS_SSL_SOME_SUITES_USE_CBC_ETM)
mbedtls_ssl_mode_t mbedtls_ssl_get_mode_from_ciphersuite(
        int encrypt_then_mac,
        const mbedtls_ssl_ciphersuite_t *suite );
#else
mbedtls_ssl_mode_t mbedtls_ssl_get_mode_from_ciphersuite(
        const mbedtls_ssl_ciphersuite_t *suite );
#endif /* MBEDTLS_SSL_SOME_SUITES_USE_CBC_ETM */

#if defined(MBEDTLS_ECDH_C)

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_read_public_ecdhe_share( mbedtls_ssl_context *ssl,
                                               const unsigned char *buf,
                                               size_t buf_len );

#endif /* MBEDTLS_ECDH_C */

static inline int mbedtls_ssl_tls13_cipher_suite_is_offered(
        mbedtls_ssl_context *ssl, int cipher_suite )
{
    const int *ciphersuite_list = ssl->conf->ciphersuite_list;

    /* Check whether we have offered this ciphersuite */
    for ( size_t i = 0; ciphersuite_list[i] != 0; i++ )
    {
        if( ciphersuite_list[i] == cipher_suite )
        {
            return( 1 );
        }
    }
    return( 0 );
}

/**
 * \brief Validate cipher suite against config in SSL context.
 *
 * \param ssl              SSL context
 * \param suite_info       Cipher suite to validate
 * \param min_tls_version  Minimal TLS version to accept a cipher suite
 * \param max_tls_version  Maximal TLS version to accept a cipher suite
 *
 * \return 0 if valid, negative value otherwise.
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_validate_ciphersuite(
    const mbedtls_ssl_context *ssl,
    const mbedtls_ssl_ciphersuite_t *suite_info,
    mbedtls_ssl_protocol_version min_tls_version,
    mbedtls_ssl_protocol_version max_tls_version );

MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_write_sig_alg_ext( mbedtls_ssl_context *ssl, unsigned char *buf,
                                   const unsigned char *end, size_t *out_len );

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_parse_server_name_ext( mbedtls_ssl_context *ssl,
                                       const unsigned char *buf,
                                       const unsigned char *end );
#endif /* MBEDTLS_SSL_SERVER_NAME_INDICATION */

#if defined(MBEDTLS_SSL_ALPN)
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_parse_alpn_ext( mbedtls_ssl_context *ssl,
                                const unsigned char *buf,
                                const unsigned char *end );


MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_write_alpn_ext( mbedtls_ssl_context *ssl,
                                unsigned char *buf,
                                unsigned char *end,
                                size_t *out_len );
#endif /* MBEDTLS_SSL_ALPN */

#if defined(MBEDTLS_TEST_HOOKS)
int mbedtls_ssl_check_dtls_clihlo_cookie(
                           mbedtls_ssl_context *ssl,
                           const unsigned char *cli_id, size_t cli_id_len,
                           const unsigned char *in, size_t in_len,
                           unsigned char *obuf, size_t buf_len, size_t *olen );
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
/* Check if we have any PSK to offer, returns 0 if PSK is available.
 * Assign the psk and ticket if pointers are present.
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_get_psk_to_offer(
        const mbedtls_ssl_context *ssl,
        int *psk_type,
        const unsigned char **psk, size_t *psk_len,
        const unsigned char **psk_identity, size_t *psk_identity_len );

/**
 * \brief Given an SSL context and its associated configuration, write the TLS
 *        1.3 specific Pre-Shared key extension.
 *
 * \param[in]   ssl     SSL context
 * \param[in]   buf     Base address of the buffer where to write the extension
 * \param[in]   end     End address of the buffer where to write the extension
 * \param[out]  out_len Length in bytes of the Pre-Shared key extension: data
 *                      written into the buffer \p buf by this function plus
 *                      the length of the binders to be written.
 * \param[out]  binders_len Length of the binders to be written at the end of
 *                          the extension.
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_write_identities_of_pre_shared_key_ext(
    mbedtls_ssl_context *ssl,
    unsigned char *buf, unsigned char *end,
    size_t *out_len, size_t *binders_len );

/**
 * \brief Given an SSL context and its associated configuration, write the TLS
 *        1.3 specific Pre-Shared key extension binders at the end of the
 *        ClientHello.
 *
 * \param[in]   ssl     SSL context
 * \param[in]   buf     Base address of the buffer where to write the binders
 * \param[in]   end     End address of the buffer where to write the binders
 */
MBEDTLS_CHECK_RETURN_CRITICAL
int mbedtls_ssl_tls13_write_binders_of_pre_shared_key_ext(
    mbedtls_ssl_context *ssl,
    unsigned char *buf, unsigned char *end );
#endif /* MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED */

#endif /* ssl_misc.h */
