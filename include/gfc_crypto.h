/*
** gfc
**
** Copyright (C) 2019 doublegsoft.open
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
** @file gfc_crypto.h
**
** @author Christian Gann <guo.guo.gan@gmail.com>
**
** @date 2019-03-15
**
** @since 1.0
*/

#ifndef __GFC_CRYPTO_H__
#define __GFC_CRYPTO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef GFC_USE_OPENSSL
#include <openssl/bn.h>
#include <openssl/des.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#endif

#include "gfc_type.h"

/*!
** @brief Using BASE64 to encode the byte block.
**
** @param in
**        the input data
**
** @param in_len
**        the input data length
**
** @param out
**        the output data buffer
**
** @return the length of encoded string
*/
GFC_API uint
gfc_crypto_base64_encode(const byte* in, uint in_len, char* out);

/*!
** @brief Using BASE64 to decode the byte block.
**
** @param in
**        the input data
**
** @param in_len
**        the input data length
**
** @param out
**        the output data buffer
**
** @return the length of encoded string
*/
GFC_API uint
gfc_crypto_base64_decode(const byte* in, uint in_len, byte* out);

#ifdef GFC_USE_OPENSSL

#define TDES_CIPHER_LENGTH           24

/*!
** Using DES with ECB mode to encrypt the plain text.
**
** @param plaintext
**        the plain text to encrypt
**
** @param cipher
**        the cipher (password)
**
** @param cipher_length
**        the length of cipher
**
** @param [out] ciphertext
**        the encrypted text
**
** @return padding length
*/
GFC_API uint
gfc_crypto_des_ecb3_encrypt(const char* plaintext,
                            const char* cipher,
                            uint cipher_length,
                            byte* ciphertext);

GFC_API uint
gfc_crypto_des_ecb3_decrypt(const byte* ciphertext,
                            uint ciphertext_length,
                            const byte* cipher,
                            uint cipher_length,
                            char* plaintext);

GFC_API int
gfc_crypto_rsa_public_encrypt(const byte*   plaintext,
                              uint          plaintext_length,
                              const byte*   cipher,
                              byte*         ciphertext);

GFC_API int
gfc_crypto_rsa_public_decrypt(const byte*   ciphertext,
                              uint          ciphertext_length,
                              const byte*   cipher,
                              byte*         plaintext);


GFC_API int
gfc_crypto_rsa_private_encrypt(const byte*   plaintext,
                               uint          plaintext_length,
                               const byte*   cipher,
                               byte*         ciphertext);

GFC_API int
gfc_crypto_rsa_private_decrypt(const byte*   ciphertext,
                               uint          ciphertext_length,
                               const byte*   cipher,
                               byte*         plaintext);

#endif

#ifdef __cplusplus
}
#endif

#endif
