/**
 * \file mbedtls/config_psa.h
 * \brief PSA crypto configuration options (set of defines)
 *
 *  This set of compile-time options takes settings defined in
 *  include/mbedtls/mbedtls_config.h and include/psa/crypto_config.h and uses
 *  those definitions to define symbols used in the library code.
 *
 *  Users and integrators should not edit this file, please edit
 *  include/mbedtls/mbedtls_config.h for MBETLS_XXX settings or
 *  include/psa/crypto_config.h for PSA_WANT_XXX settings.
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

#ifndef MBEDTLS_CONFIG_PSA_H
#define MBEDTLS_CONFIG_PSA_H

#if defined(MBEDTLS_PSA_CRYPTO_CONFIG)
#if defined(MBEDTLS_PSA_CRYPTO_CONFIG_FILE)
#include MBEDTLS_PSA_CRYPTO_CONFIG_FILE
#else
#include "../../../../external/mbedtls/include/psa/crypto_config.h"
#endif
#endif /* defined(MBEDTLS_PSA_CRYPTO_CONFIG) */

#if defined(MBEDTLS_PSA_CRYPTO_USER_CONFIG_FILE)
#include MBEDTLS_PSA_CRYPTO_USER_CONFIG_FILE
#endif

#ifdef __cplusplus
extern "C" {
#endif



/****************************************************************/
/* De facto synonyms */
/****************************************************************/

#if defined(PSA_WANT_ALG_ECDSA_ANY) && !defined(PSA_WANT_ALG_ECDSA)
#define PSA_WANT_ALG_ECDSA PSA_WANT_ALG_ECDSA_ANY
#elif !defined(PSA_WANT_ALG_ECDSA_ANY) && defined(PSA_WANT_ALG_ECDSA)
#define PSA_WANT_ALG_ECDSA_ANY PSA_WANT_ALG_ECDSA
#endif

#if defined(PSA_WANT_ALG_CCM_STAR_NO_TAG) && !defined(PSA_WANT_ALG_CCM)
#define PSA_WANT_ALG_CCM PSA_WANT_ALG_CCM_STAR_NO_TAG
#elif !defined(PSA_WANT_ALG_CCM_STAR_NO_TAG) && defined(PSA_WANT_ALG_CCM)
#define PSA_WANT_ALG_CCM_STAR_NO_TAG PSA_WANT_ALG_CCM
#endif

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN_RAW) && !defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN PSA_WANT_ALG_RSA_PKCS1V15_SIGN_RAW
#elif !defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN_RAW) && defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN_RAW PSA_WANT_ALG_RSA_PKCS1V15_SIGN
#endif

#if defined(PSA_WANT_ALG_RSA_PSS_ANY_SALT) && !defined(PSA_WANT_ALG_RSA_PSS)
#define PSA_WANT_ALG_RSA_PSS PSA_WANT_ALG_RSA_PSS_ANY_SALT
#elif !defined(PSA_WANT_ALG_RSA_PSS_ANY_SALT) && defined(PSA_WANT_ALG_RSA_PSS)
#define PSA_WANT_ALG_RSA_PSS_ANY_SALT PSA_WANT_ALG_RSA_PSS
#endif



/****************************************************************/
/* Require built-in implementations based on PSA requirements */
/****************************************************************/

#if defined(MBEDTLS_PSA_CRYPTO_CONFIG)

#if defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_DETERMINISTIC_ECDSA)
#define MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA 1
#define MBEDTLS_ECDSA_DETERMINISTIC
#define MBEDTLS_ECDSA_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_MD_C
#endif /* !MBEDTLS_PSA_ACCEL_ALG_DETERMINISTIC_ECDSA */
#endif /* PSA_WANT_ALG_DETERMINISTIC_ECDSA */

#if defined(PSA_WANT_ALG_ECDH)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_ECDH)
#define MBEDTLS_PSA_BUILTIN_ALG_ECDH 1
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECP_C
#define MBEDTLS_BIGNUM_C
#endif /* !MBEDTLS_PSA_ACCEL_ALG_ECDH */
#endif /* PSA_WANT_ALG_ECDH */

#if defined(PSA_WANT_ALG_ECDSA)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_ECDSA)
#define MBEDTLS_PSA_BUILTIN_ALG_ECDSA 1
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECP_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#endif /* !MBEDTLS_PSA_ACCEL_ALG_ECDSA */
#endif /* PSA_WANT_ALG_ECDSA */

#if defined(PSA_WANT_ALG_HKDF)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_HKDF)
#define MBEDTLS_PSA_BUILTIN_ALG_HMAC 1
#define MBEDTLS_PSA_BUILTIN_ALG_HKDF 1
#endif /* !MBEDTLS_PSA_ACCEL_ALG_HKDF */
#endif /* PSA_WANT_ALG_HKDF */

#if defined(PSA_WANT_ALG_HKDF_EXTRACT)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_HKDF_EXTRACT)
#define MBEDTLS_PSA_BUILTIN_ALG_HMAC 1
#define MBEDTLS_PSA_BUILTIN_ALG_HKDF_EXTRACT 1
#endif /* !MBEDTLS_PSA_ACCEL_ALG_HKDF_EXTRACT */
#endif /* PSA_WANT_ALG_HKDF_EXTRACT */

#if defined(PSA_WANT_ALG_HKDF_EXPAND)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_HKDF_EXPAND)
#define MBEDTLS_PSA_BUILTIN_ALG_HMAC 1
#define MBEDTLS_PSA_BUILTIN_ALG_HKDF_EXPAND 1
#endif /* !MBEDTLS_PSA_ACCEL_ALG_HKDF_EXPAND */
#endif /* PSA_WANT_ALG_HKDF_EXPAND */

#if defined(PSA_WANT_ALG_HMAC)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_HMAC)
#define MBEDTLS_PSA_BUILTIN_ALG_HMAC 1
#endif /* !MBEDTLS_PSA_ACCEL_ALG_HMAC */
#endif /* PSA_WANT_ALG_HMAC */

#if defined(PSA_WANT_ALG_MD5) && !defined(MBEDTLS_PSA_ACCEL_ALG_MD5)
#define MBEDTLS_PSA_BUILTIN_ALG_MD5 1
#define MBEDTLS_MD5_C
#endif

#if defined(PSA_WANT_ALG_RIPEMD160) && !defined(MBEDTLS_PSA_ACCEL_ALG_RIPEMD160)
#define MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160 1
#define MBEDTLS_RIPEMD160_C
#endif

#if defined(PSA_WANT_ALG_RSA_OAEP)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_RSA_OAEP)
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP 1
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_MD_C
#endif /* !MBEDTLS_PSA_ACCEL_ALG_RSA_OAEP */
#endif /* PSA_WANT_ALG_RSA_OAEP */

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_RSA_PKCS1V15_CRYPT)
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT 1
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PKCS1_V15
#endif /* !MBEDTLS_PSA_ACCEL_ALG_RSA_PKCS1V15_CRYPT */
#endif /* PSA_WANT_ALG_RSA_PKCS1V15_CRYPT */

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_RSA_PKCS1V15_SIGN)
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN 1
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_MD_C
#endif /* !MBEDTLS_PSA_ACCEL_ALG_RSA_PKCS1V15_SIGN */
#endif /* PSA_WANT_ALG_RSA_PKCS1V15_SIGN */

#if defined(PSA_WANT_ALG_RSA_PSS)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_RSA_PSS)
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS 1
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_MD_C
#endif /* !MBEDTLS_PSA_ACCEL_ALG_RSA_PSS */
#endif /* PSA_WANT_ALG_RSA_PSS */

#if defined(PSA_WANT_ALG_SHA_1) && !defined(MBEDTLS_PSA_ACCEL_ALG_SHA_1)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_1 1
#define MBEDTLS_SHA1_C
#endif

#if defined(PSA_WANT_ALG_SHA_224) && !defined(MBEDTLS_PSA_ACCEL_ALG_SHA_224)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_224 1
#define MBEDTLS_SHA224_C
#endif

#if defined(PSA_WANT_ALG_SHA_256) && !defined(MBEDTLS_PSA_ACCEL_ALG_SHA_256)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_256 1
#define MBEDTLS_SHA256_C
#endif

#if defined(PSA_WANT_ALG_SHA_384) && !defined(MBEDTLS_PSA_ACCEL_ALG_SHA_384)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_384 1
#define MBEDTLS_SHA384_C
#endif

#if defined(PSA_WANT_ALG_SHA_512) && !defined(MBEDTLS_PSA_ACCEL_ALG_SHA_512)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_512 1
#define MBEDTLS_SHA512_C
#endif

#if defined(PSA_WANT_ALG_TLS12_PRF)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_TLS12_PRF)
#define MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF 1
#endif /* !MBEDTLS_PSA_ACCEL_ALG_TLS12_PRF */
#endif /* PSA_WANT_ALG_TLS12_PRF */

#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_TLS12_PSK_TO_MS)
#define MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS 1
#endif /* !MBEDTLS_PSA_ACCEL_ALG_TLS12_PSK_TO_MS */
#endif /* PSA_WANT_ALG_TLS12_PSK_TO_MS */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR 1
#define MBEDTLS_ECP_C
#define MBEDTLS_BIGNUM_C
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR */
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR */

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY 1
#define MBEDTLS_ECP_C
#define MBEDTLS_BIGNUM_C
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY */
#endif /* PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_KEY_PAIR)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR 1
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_GENPRIME
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_PK_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_KEY_PAIR */
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR */

#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_PUBLIC_KEY)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY 1
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_PK_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_PUBLIC_KEY */
#endif /* PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY */

/* If any of the block modes are requested that don't have an
 * associated HW assist, define PSA_HAVE_SOFT_BLOCK_MODE for checking
 * in the block cipher key types. */
#if (defined(PSA_WANT_ALG_CTR) && !defined(MBEDTLS_PSA_ACCEL_ALG_CTR)) || \
    (defined(PSA_WANT_ALG_CFB) && !defined(MBEDTLS_PSA_ACCEL_ALG_CFB)) || \
    (defined(PSA_WANT_ALG_OFB) && !defined(MBEDTLS_PSA_ACCEL_ALG_OFB)) || \
    defined(PSA_WANT_ALG_ECB_NO_PADDING) || \
    (defined(PSA_WANT_ALG_CBC_NO_PADDING) && \
     !defined(MBEDTLS_PSA_ACCEL_ALG_CBC_NO_PADDING)) || \
    (defined(PSA_WANT_ALG_CBC_PKCS7) && \
     !defined(MBEDTLS_PSA_ACCEL_ALG_CBC_PKCS7)) || \
    (defined(PSA_WANT_ALG_CMAC) && !defined(MBEDTLS_PSA_ACCEL_ALG_CMAC))
#define PSA_HAVE_SOFT_BLOCK_MODE 1
#endif

#if (defined(PSA_WANT_ALG_GCM) && !defined(MBEDTLS_PSA_ACCEL_ALG_GCM)) || \
    (defined(PSA_WANT_ALG_CCM) && !defined(MBEDTLS_PSA_ACCEL_ALG_CCM))
#define PSA_HAVE_SOFT_BLOCK_AEAD 1
#endif

#if defined(PSA_WANT_KEY_TYPE_AES)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_AES)
#define PSA_HAVE_SOFT_KEY_TYPE_AES 1
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_AES */
#if defined(PSA_HAVE_SOFT_KEY_TYPE_AES) || \
    defined(PSA_HAVE_SOFT_BLOCK_MODE) || \
    defined(PSA_HAVE_SOFT_BLOCK_AEAD)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_AES 1
#define MBEDTLS_AES_C
#endif /* PSA_HAVE_SOFT_KEY_TYPE_AES || PSA_HAVE_SOFT_BLOCK_MODE */
#endif /* PSA_WANT_KEY_TYPE_AES */

#if defined(PSA_WANT_KEY_TYPE_ARIA)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ARIA)
#define PSA_HAVE_SOFT_KEY_TYPE_ARIA 1
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_ARIA */
#if defined(PSA_HAVE_SOFT_KEY_TYPE_ARIA) || \
    defined(PSA_HAVE_SOFT_BLOCK_MODE) || \
    defined(PSA_HAVE_SOFT_BLOCK_AEAD)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_ARIA 1
#define MBEDTLS_ARIA_C
#endif /* PSA_HAVE_SOFT_KEY_TYPE_ARIA || PSA_HAVE_SOFT_BLOCK_MODE */
#endif /* PSA_WANT_KEY_TYPE_ARIA */

#if defined(PSA_WANT_KEY_TYPE_CAMELLIA)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_CAMELLIA)
#define PSA_HAVE_SOFT_KEY_TYPE_CAMELLIA 1
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_CAMELLIA */
#if defined(PSA_HAVE_SOFT_KEY_TYPE_CAMELLIA) || \
    defined(PSA_HAVE_SOFT_BLOCK_MODE) || \
    defined(PSA_HAVE_SOFT_BLOCK_AEAD)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_CAMELLIA 1
#define MBEDTLS_CAMELLIA_C
#endif /* PSA_HAVE_SOFT_KEY_TYPE_CAMELLIA || PSA_HAVE_SOFT_BLOCK_MODE */
#endif /* PSA_WANT_KEY_TYPE_CAMELLIA */

#if defined(PSA_WANT_KEY_TYPE_DES)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_DES)
#define PSA_HAVE_SOFT_KEY_TYPE_DES 1
#endif /* !MBEDTLS_PSA_ACCEL_KEY_TYPE_DES */
#if defined(PSA_HAVE_SOFT_KEY_TYPE_DES) || \
    defined(PSA_HAVE_SOFT_BLOCK_MODE)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES 1
#define MBEDTLS_DES_C
#endif /*PSA_HAVE_SOFT_KEY_TYPE_DES || PSA_HAVE_SOFT_BLOCK_MODE */
#endif /* PSA_WANT_KEY_TYPE_DES */

#if defined(PSA_WANT_KEY_TYPE_CHACHA20)
#if !defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_CHACHA20)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_CHACHA20 1
#define MBEDTLS_CHACHA20_C
#endif /*!MBEDTLS_PSA_ACCEL_KEY_TYPE_CHACHA20 */
#endif /* PSA_WANT_KEY_TYPE_CHACHA20 */

/* If any of the software block ciphers are selected, define
 * PSA_HAVE_SOFT_BLOCK_CIPHER, which can be used in any of these
 * situations. */
#if defined(PSA_HAVE_SOFT_KEY_TYPE_AES) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_ARIA) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_DES) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_CAMELLIA)
#define PSA_HAVE_SOFT_BLOCK_CIPHER 1
#endif

#if defined(PSA_WANT_ALG_STREAM_CIPHER)
#define MBEDTLS_PSA_BUILTIN_ALG_STREAM_CIPHER 1
#endif /* PSA_WANT_ALG_STREAM_CIPHER */

#if defined(PSA_WANT_ALG_CBC_MAC)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CBC_MAC)
#error "CBC-MAC is not yet supported via the PSA API in Mbed TLS."
#define MBEDTLS_PSA_BUILTIN_ALG_CBC_MAC 1
#endif /* !MBEDTLS_PSA_ACCEL_ALG_CBC_MAC */
#endif /* PSA_WANT_ALG_CBC_MAC */

#if defined(PSA_WANT_ALG_CMAC)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CMAC) || \
    defined(PSA_HAVE_SOFT_BLOCK_CIPHER)
#define MBEDTLS_PSA_BUILTIN_ALG_CMAC 1
#define MBEDTLS_CMAC_C
#endif /* !MBEDTLS_PSA_ACCEL_ALG_CMAC */
#endif /* PSA_WANT_ALG_CMAC */

#if defined(PSA_WANT_ALG_CTR)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CTR) || \
    defined(PSA_HAVE_SOFT_BLOCK_CIPHER)
#define MBEDTLS_PSA_BUILTIN_ALG_CTR 1
#define MBEDTLS_CIPHER_MODE_CTR
#endif
#endif /* PSA_WANT_ALG_CTR */

#if defined(PSA_WANT_ALG_CFB)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CFB) || \
    defined(PSA_HAVE_SOFT_BLOCK_CIPHER)
#define MBEDTLS_PSA_BUILTIN_ALG_CFB 1
#define MBEDTLS_CIPHER_MODE_CFB
#endif
#endif /* PSA_WANT_ALG_CFB */

#if defined(PSA_WANT_ALG_OFB)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_OFB) || \
    defined(PSA_HAVE_SOFT_BLOCK_CIPHER)
#define MBEDTLS_PSA_BUILTIN_ALG_OFB 1
#define MBEDTLS_CIPHER_MODE_OFB
#endif
#endif /* PSA_WANT_ALG_OFB */

#if defined(PSA_WANT_ALG_ECB_NO_PADDING) &&     \
    !defined(MBEDTLS_PSA_ACCEL_ALG_ECB_NO_PADDING)
#define MBEDTLS_PSA_BUILTIN_ALG_ECB_NO_PADDING 1
#endif

#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CBC_NO_PADDING) || \
    defined(PSA_HAVE_SOFT_BLOCK_CIPHER)
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_PSA_BUILTIN_ALG_CBC_NO_PADDING 1
#endif
#endif /* PSA_WANT_ALG_CBC_NO_PADDING */

#if defined(PSA_WANT_ALG_CBC_PKCS7)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CBC_PKCS7) || \
    defined(PSA_HAVE_SOFT_BLOCK_CIPHER)
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_PSA_BUILTIN_ALG_CBC_PKCS7 1
#define MBEDTLS_CIPHER_PADDING_PKCS7
#endif
#endif /* PSA_WANT_ALG_CBC_PKCS7 */

#if defined(PSA_WANT_ALG_CCM)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CCM) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_AES) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_ARIA) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_CAMELLIA)
#define MBEDTLS_PSA_BUILTIN_ALG_CCM 1
#define MBEDTLS_PSA_BUILTIN_ALG_CCM_STAR_NO_TAG 1
#define MBEDTLS_CCM_C
#endif
#endif /* PSA_WANT_ALG_CCM */

#if defined(PSA_WANT_ALG_GCM)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_GCM) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_AES) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_ARIA) || \
    defined(PSA_HAVE_SOFT_KEY_TYPE_CAMELLIA)
#define MBEDTLS_PSA_BUILTIN_ALG_GCM 1
#define MBEDTLS_GCM_C
#endif
#endif /* PSA_WANT_ALG_GCM */

#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
#if !defined(MBEDTLS_PSA_ACCEL_ALG_CHACHA20_POLY1305)
#if defined(PSA_WANT_KEY_TYPE_CHACHA20)
#define MBEDTLS_CHACHAPOLY_C
#define MBEDTLS_PSA_BUILTIN_ALG_CHACHA20_POLY1305 1
#endif /* PSA_WANT_KEY_TYPE_CHACHA20 */
#endif /* !MBEDTLS_PSA_ACCEL_ALG_CHACHA20_POLY1305 */
#endif /* PSA_WANT_ALG_CHACHA20_POLY1305 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_256)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_BRAINPOOL_P_R1_256)
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_BRAINPOOL_P_R1_256 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_BRAINPOOL_P_R1_256 */
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_256 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_384)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_BRAINPOOL_P_R1_384)
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_BRAINPOOL_P_R1_384 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_BRAINPOOL_P_R1_384 */
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_384 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_512)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_BRAINPOOL_P_R1_512)
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_BRAINPOOL_P_R1_512 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_BRAINPOOL_P_R1_512 */
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_512 */

#if defined(PSA_WANT_ECC_MONTGOMERY_255)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_MONTGOMERY_255)
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_MONTGOMERY_255 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_MONTGOMERY_255 */
#endif /* PSA_WANT_ECC_MONTGOMERY_255 */

#if defined(PSA_WANT_ECC_MONTGOMERY_448)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_MONTGOMERY_448)
#define MBEDTLS_ECP_DP_CURVE448_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_MONTGOMERY_448 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_MONTGOMERY_448 */
#endif /* PSA_WANT_ECC_MONTGOMERY_448 */

#if defined(PSA_WANT_ECC_SECP_R1_192)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_R1_192)
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_192 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_R1_192 */
#endif /* PSA_WANT_ECC_SECP_R1_192 */

#if defined(PSA_WANT_ECC_SECP_R1_224)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_R1_224)
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_224 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_R1_224 */
#endif /* PSA_WANT_ECC_SECP_R1_224 */

#if defined(PSA_WANT_ECC_SECP_R1_256)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_R1_256)
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_256 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_R1_256 */
#endif /* PSA_WANT_ECC_SECP_R1_256 */

#if defined(PSA_WANT_ECC_SECP_R1_384)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_R1_384)
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_384 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_R1_384 */
#endif /* PSA_WANT_ECC_SECP_R1_384 */

#if defined(PSA_WANT_ECC_SECP_R1_521)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_R1_521)
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_521 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_R1_521 */
#endif /* PSA_WANT_ECC_SECP_R1_521 */

#if defined(PSA_WANT_ECC_SECP_K1_192)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_K1_192)
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_K1_192 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_K1_192 */
#endif /* PSA_WANT_ECC_SECP_K1_192 */

#if defined(PSA_WANT_ECC_SECP_K1_224)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_K1_224)
/*
 * SECP224K1 is buggy via the PSA API in Mbed TLS
 * (https://github.com/Mbed-TLS/mbedtls/issues/3541).
 */
#error "SECP224K1 is buggy via the PSA API in Mbed TLS."
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_K1_224 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_K1_224 */
#endif /* PSA_WANT_ECC_SECP_K1_224 */

#if defined(PSA_WANT_ECC_SECP_K1_256)
#if !defined(MBEDTLS_PSA_ACCEL_ECC_SECP_K1_256)
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_K1_256 1
#endif /* !MBEDTLS_PSA_ACCEL_ECC_SECP_K1_256 */
#endif /* PSA_WANT_ECC_SECP_K1_256 */



/****************************************************************/
/* Infer PSA requirements from Mbed TLS capabilities */
/****************************************************************/

#else /* MBEDTLS_PSA_CRYPTO_CONFIG */

/*
 * Ensure PSA_WANT_* defines are setup properly if MBEDTLS_PSA_CRYPTO_CONFIG
 * is not defined
 */

#if defined(MBEDTLS_CCM_C)
#define MBEDTLS_PSA_BUILTIN_ALG_CCM 1
#define MBEDTLS_PSA_BUILTIN_ALG_CCM_STAR_NO_TAG 1
#define PSA_WANT_ALG_CCM 1
#define PSA_WANT_ALG_CCM_STAR_NO_TAG 1
#endif /* MBEDTLS_CCM_C */

#if defined(MBEDTLS_CMAC_C)
#define MBEDTLS_PSA_BUILTIN_ALG_CMAC 1
#define PSA_WANT_ALG_CMAC 1
#endif /* MBEDTLS_CMAC_C */

#if defined(MBEDTLS_ECDH_C)
#define MBEDTLS_PSA_BUILTIN_ALG_ECDH 1
#define PSA_WANT_ALG_ECDH 1
#endif /* MBEDTLS_ECDH_C */

#if defined(MBEDTLS_ECDSA_C)
#define MBEDTLS_PSA_BUILTIN_ALG_ECDSA 1
#define PSA_WANT_ALG_ECDSA 1
#define PSA_WANT_ALG_ECDSA_ANY 1

// Only add in DETERMINISTIC support if ECDSA is also enabled
#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
#define MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA 1
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA 1
#endif /* MBEDTLS_ECDSA_DETERMINISTIC */

#endif /* MBEDTLS_ECDSA_C */

#if defined(MBEDTLS_ECP_C)
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR 1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR 1
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY 1
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY 1
#endif /* MBEDTLS_ECP_C */

#if defined(MBEDTLS_GCM_C)
#define MBEDTLS_PSA_BUILTIN_ALG_GCM 1
#define PSA_WANT_ALG_GCM 1
#endif /* MBEDTLS_GCM_C */

/* Enable PSA HKDF algorithm if mbedtls HKDF is supported.
 * PSA HKDF EXTRACT and PSA HKDF EXPAND have minimal cost when
 * PSA HKDF is enabled, so enable both algorithms together
 * with PSA HKDF. */
#if defined(MBEDTLS_HKDF_C)
#define MBEDTLS_PSA_BUILTIN_ALG_HMAC 1
#define PSA_WANT_ALG_HMAC 1
#define MBEDTLS_PSA_BUILTIN_ALG_HKDF 1
#define PSA_WANT_ALG_HKDF 1
#define MBEDTLS_PSA_BUILTIN_ALG_HKDF_EXTRACT 1
#define PSA_WANT_ALG_HKDF_EXTRACT 1
#define MBEDTLS_PSA_BUILTIN_ALG_HKDF_EXPAND 1
#define PSA_WANT_ALG_HKDF_EXPAND 1
#endif /* MBEDTLS_HKDF_C */

#if defined(MBEDTLS_MD_C)
#define MBEDTLS_PSA_BUILTIN_ALG_HMAC 1
#define PSA_WANT_ALG_HMAC 1
#define PSA_WANT_KEY_TYPE_HMAC
#define MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF 1
#define PSA_WANT_ALG_TLS12_PRF 1
#define MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS 1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS 1
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_MD5_C)
#define MBEDTLS_PSA_BUILTIN_ALG_MD5 1
#define PSA_WANT_ALG_MD5 1
#endif

#if defined(MBEDTLS_RIPEMD160_C)
#define MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160 1
#define PSA_WANT_ALG_RIPEMD160 1
#endif

#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_PKCS1_V15)
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT 1
#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT 1
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN 1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN 1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN_RAW 1
#endif /* MBEDTLS_PKCS1_V15 */
#if defined(MBEDTLS_PKCS1_V21)
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP 1
#define PSA_WANT_ALG_RSA_OAEP 1
#define MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS 1
#define PSA_WANT_ALG_RSA_PSS 1
#endif /* MBEDTLS_PKCS1_V21 */
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR 1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR 1
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY 1
#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY 1
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_SHA1_C)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_1 1
#define PSA_WANT_ALG_SHA_1 1
#endif

#if defined(MBEDTLS_SHA224_C)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_224 1
#define PSA_WANT_ALG_SHA_224 1
#endif

#if defined(MBEDTLS_SHA256_C)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_256 1
#define PSA_WANT_ALG_SHA_256 1
#endif

#if defined(MBEDTLS_SHA384_C)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_384 1
#define PSA_WANT_ALG_SHA_384 1
#endif

#if defined(MBEDTLS_SHA512_C)
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_512 1
#define PSA_WANT_ALG_SHA_512 1
#endif

#if defined(MBEDTLS_AES_C)
#define PSA_WANT_KEY_TYPE_AES 1
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_AES 1
#endif

#if defined(MBEDTLS_ARIA_C)
#define PSA_WANT_KEY_TYPE_ARIA 1
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_ARIA 1
#endif

#if defined(MBEDTLS_CAMELLIA_C)
#define PSA_WANT_KEY_TYPE_CAMELLIA 1
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_CAMELLIA 1
#endif

#if defined(MBEDTLS_DES_C)
#define PSA_WANT_KEY_TYPE_DES 1
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES 1
#endif

#if defined(MBEDTLS_CHACHA20_C)
#define PSA_WANT_KEY_TYPE_CHACHA20 1
#define PSA_WANT_ALG_STREAM_CIPHER 1
#define MBEDTLS_PSA_BUILTIN_KEY_TYPE_CHACHA20 1
#define MBEDTLS_PSA_BUILTIN_ALG_STREAM_CIPHER 1
#if defined(MBEDTLS_CHACHAPOLY_C)
#define PSA_WANT_ALG_CHACHA20_POLY1305 1
#define MBEDTLS_PSA_BUILTIN_ALG_CHACHA20_POLY1305 1
#endif
#endif

#if defined(MBEDTLS_CIPHER_MODE_CBC)
#define MBEDTLS_PSA_BUILTIN_ALG_CBC_NO_PADDING 1
#define PSA_WANT_ALG_CBC_NO_PADDING 1
#if defined(MBEDTLS_CIPHER_PADDING_PKCS7)
#define MBEDTLS_PSA_BUILTIN_ALG_CBC_PKCS7 1
#define PSA_WANT_ALG_CBC_PKCS7 1
#endif
#endif

#if defined(MBEDTLS_AES_C) || defined(MBEDTLS_DES_C) || \
    defined(MBEDTLS_ARIA_C) || defined(MBEDTLS_CAMELLIA_C)
#define MBEDTLS_PSA_BUILTIN_ALG_ECB_NO_PADDING 1
#define PSA_WANT_ALG_ECB_NO_PADDING 1
#endif

#if defined(MBEDTLS_CIPHER_MODE_CFB)
#define MBEDTLS_PSA_BUILTIN_ALG_CFB 1
#define PSA_WANT_ALG_CFB 1
#endif

#if defined(MBEDTLS_CIPHER_MODE_CTR)
#define MBEDTLS_PSA_BUILTIN_ALG_CTR 1
#define PSA_WANT_ALG_CTR 1
#endif

#if defined(MBEDTLS_CIPHER_MODE_OFB)
#define MBEDTLS_PSA_BUILTIN_ALG_OFB 1
#define PSA_WANT_ALG_OFB 1
#endif

#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_BRAINPOOL_P_R1_256 1
#define PSA_WANT_ECC_BRAINPOOL_P_R1_256
#endif

#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_BRAINPOOL_P_R1_384 1
#define PSA_WANT_ECC_BRAINPOOL_P_R1_384
#endif

#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_BRAINPOOL_P_R1_512 1
#define PSA_WANT_ECC_BRAINPOOL_P_R1_512
#endif

#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_MONTGOMERY_255 1
#define PSA_WANT_ECC_MONTGOMERY_255
#endif

#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_MONTGOMERY_448 1
#define PSA_WANT_ECC_MONTGOMERY_448
#endif

#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_192 1
#define PSA_WANT_ECC_SECP_R1_192
#endif

#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_224 1
#define PSA_WANT_ECC_SECP_R1_224
#endif

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_256 1
#define PSA_WANT_ECC_SECP_R1_256
#endif

#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_384 1
#define PSA_WANT_ECC_SECP_R1_384
#endif

#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_R1_521 1
#define PSA_WANT_ECC_SECP_R1_521
#endif

#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_K1_192 1
#define PSA_WANT_ECC_SECP_K1_192
#endif

/* SECP224K1 is buggy via the PSA API (https://github.com/Mbed-TLS/mbedtls/issues/3541) */
#if 0 && defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_K1_224 1
#define PSA_WANT_ECC_SECP_K1_224
#endif

#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
#define MBEDTLS_PSA_BUILTIN_ECC_SECP_K1_256 1
#define PSA_WANT_ECC_SECP_K1_256
#endif

#endif /* MBEDTLS_PSA_CRYPTO_CONFIG */

/* These features are always enabled. */
#define PSA_WANT_KEY_TYPE_DERIVE 1
#define PSA_WANT_KEY_TYPE_RAW_DATA 1

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CONFIG_PSA_H */
