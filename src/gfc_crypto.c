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
#ifdef GFC_USE_OPENSSL
#include <openssl/des.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/pem.h>
#endif

#include "gfc_crypto.h"
#include "gfc_type.h"


static byte gfc_b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*!
** ASCII to base64_int
** 65-90  Upper Case  >>  0-25
** 97-122 Lower Case  >>  26-51
** 48-57  Numbers     >>  52-61
** 43     Plus (+)    >>  62
** 47     Slash (/)   >>  63
** 61     Equal (=)   >>  64~
*/
static uint
gfc_crypto_base64_to_int(char ch)
{
  if (ch == 43)
  return 62;
  if (ch == 47)
  return 63;
  if (ch == 61)
  return 64;
  if ((ch > 47) && (ch < 58))
  return ch + 4;
  if ((ch > 64) && (ch < 91))
  return ch - 'A';
  if ((ch > 96) && (ch < 123))
  return (ch - 'a') + 26;
  return 0;
}

/*!
** @brief 使用Base64对图片二进制数据编码。
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
gfc_crypto_base64_encode(const byte* in, uint in_len, char* out) {

  unsigned int i = 0, j = 0, k = 0, s[3];

  for (i = 0; i < in_len; i++)
  {
    s[j++] = *(in + i);
    if (j == 3)
    {
      out[k + 0] = gfc_b64_chars[(s[0] & 255) >> 2];
      out[k + 1] = gfc_b64_chars[((s[0] & 0x03) << 4) + ((s[1] & 0xF0) >> 4)];
      out[k + 2] = gfc_b64_chars[((s[1] & 0x0F) << 2) + ((s[2] & 0xC0) >> 6)];
      out[k + 3] = gfc_b64_chars[s[2] & 0x3F];
      j = 0;
      k += 4;
    }
  }

  if (j)
  {
    if ( j==1 )
      s[1] = 0;
    out[k + 0] = gfc_b64_chars[(s[0] & 255) >> 2];
    out[k + 1] = gfc_b64_chars[((s[0] & 0x03) << 4) + ((s[1] & 0xF0) >> 4)];
    if (j == 2)
      out[k + 2] = gfc_b64_chars[((s[1] & 0x0F) << 2)];
    else
      out[k + 2] = '=';
    out[k + 3] = '=';
    k += 4;
  }

  out[k] = '\0';

  return k;
}

GFC_API uint
gfc_crypto_base64_decode(const byte* in, uint in_len, byte* out) {

  uint i = 0, j = 0, k = 0, s[4];

  for (i = 0; i < in_len; i++)
  {
    s[j++] = gfc_crypto_base64_to_int(*(in + i));
    if (j == 4)
    {
      out[k + 0] = ((s[0] & 255) << 2) + ((s[1] & 0x30) >> 4);
      if (s[2] != 64)
      {
        out[k + 1] = ((s[1] & 0x0F) << 4) + ((s[2] & 0x3C) >> 2);
        if ((s[3] != 64))
        {
          out[k + 2] = ((s[2] & 0x03) << 6) + (s[3]);
          k += 3;
        }
        else
        {
          k += 2;
        }
      }
      else
      {
        k += 1;
      }
      j=0;
    }
  }
  return k;
}

#ifdef GFC_USE_OPENSSL

#define TDES_CIPHER_LENGTH           24

/*!
**
*/
GFC_API uint
gfc_crypto_des_ecb3_encrypt(const char* plaintext,
                            const char* cipher,
                            uint cipher_length,
                            byte* ciphertext)
{
  size_t                plaintext_length                        = strlen(plaintext);

  byte*                 padding_plaintext                       = NULL;
  byte                  padding_cipher[TDES_CIPHER_LENGTH];
  size_t                padding_length                          = plaintext_length + 8 - (plaintext_length % 8);
  byte                  filling_byte                            = 8 - (plaintext_length % 8);
  size_t                encryption_count                        = padding_length / 8;

  size_t                i;
  byte                  block_plaintext[8];
  byte                  block_ciphertext[8];
  byte                  block[8];

  DES_key_schedule      ks1, ks2, ks3;

  // pad the cipher
  memcpy(padding_cipher, cipher, cipher_length);
  memset(padding_cipher + cipher_length, 0x00, TDES_CIPHER_LENGTH - cipher_length);

  padding_plaintext = (byte*) malloc(padding_length * sizeof(byte));

  // pad the cipher text
  memset(padding_plaintext, 0x00, padding_length);
  memcpy(padding_plaintext, plaintext, plaintext_length);
  memset(padding_plaintext + plaintext_length, filling_byte, (int) filling_byte);

  memset(block, 0x00, 8);

  memcpy(block, padding_cipher + 0, 8);
  DES_set_key_unchecked((const_DES_cblock*) block, &ks1);

  memcpy(block, padding_cipher + 8, 8);
  DES_set_key_unchecked((const_DES_cblock*) block, &ks2);

  memcpy(block, padding_cipher + 16, 8);
  DES_set_key_unchecked((const_DES_cblock*) block, &ks3);

  for (i = 0; i < encryption_count; i++)
  {
    memcpy(block_plaintext, padding_plaintext + 8 * i, 8);
    /* 加密 */
    DES_ecb3_encrypt((const_DES_cblock*) block_plaintext,
                     (DES_cblock*) block_ciphertext,
                     &ks1, &ks2, &ks3,
                     DES_ENCRYPT);

    memcpy(ciphertext + 8 * i, block_ciphertext, 8);
  }

  free(padding_plaintext);
  return padding_length;
}

GFC_API uint
gfc_crypto_des_ecb3_decrypt(const byte* ciphertext,
                            uint ciphertext_length,
                            const byte* cipher,
                            uint cipher_length,
                            char* plaintext)
{
  byte                  padding_cipher[TDES_CIPHER_LENGTH];
  size_t                padding_length                          = ciphertext_length;
  size_t                decryption_count                        = padding_length / 8;

  size_t                i;
  byte                  block_plaintext[8];
  byte                  block_ciphertext[8];
  byte                  block[8];

  DES_key_schedule      ks1, ks2, ks3;

  memcpy(padding_cipher, cipher, cipher_length);
  memset(padding_cipher + cipher_length, 0x00, TDES_CIPHER_LENGTH - cipher_length);

  memset(block, 0x00, 8);

  memcpy(block, padding_cipher + 0, 8);
  DES_set_key_unchecked((const_DES_cblock*) block, &ks1);

  memcpy(block, padding_cipher + 8, 8);
  DES_set_key_unchecked((const_DES_cblock*) block, &ks2);

  memcpy(block, padding_cipher + 16, 8);
  DES_set_key_unchecked((const_DES_cblock*) block, &ks3);

  for (i = 0; i < decryption_count; i++)
  {
    memset(block_plaintext, 0, 8);
    memcpy(block_ciphertext, ciphertext + 8 * i, 8);
    DES_ecb3_encrypt((const_DES_cblock*) block_ciphertext,
                     (DES_cblock*) block_plaintext,
                     &ks1, &ks2, &ks3,
                     DES_DECRYPT);
    memcpy(plaintext + 8 * i, block_plaintext, 8);
  }

  for (i = 0; i < padding_length; i++)
  {
    if (plaintext[i] <= 32)
    {
      plaintext[i] = '\0';
      break;
    }
  }
  return strlen(plaintext);
}

GFC_API int
gfc_crypto_rsa_public_encrypt(const byte*   plaintext,
                              uint          plaintext_length,
                              const byte*   cipher,
                              byte*         ciphertext)
{
  RSA*  rsa         = NULL;
  BIO*  keybio      = BIO_new_mem_buf(cipher, -1);

  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
  int ret = RSA_public_encrypt(plaintext_length, plaintext, ciphertext, rsa, RSA_PKCS1_PADDING);

  free(keybio);
  free(rsa);

  return ret;
}

GFC_API int
gfc_crypto_rsa_public_decrypt(const byte*   ciphertext,
                              uint          ciphertext_length,
                              const byte*   cipher,
                              byte*         plaintext)
{
  RSA*  rsa         = NULL;
  BIO*  keybio      = BIO_new_mem_buf(cipher, -1);

  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
  int ret = RSA_public_decrypt(ciphertext_length, ciphertext, plaintext, rsa, RSA_PKCS1_PADDING);

  free(keybio);
  free(rsa);

  return ret;
}


GFC_API int
gfc_crypto_rsa_private_encrypt(const byte*   plaintext,
                               uint          plaintext_length,
                               const byte*   cipher,
                               byte*         ciphertext)
{
  RSA*  rsa         = NULL;
  BIO*  keybio      = BIO_new_mem_buf(cipher, -1);

  rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
  int ret = RSA_private_encrypt(plaintext_length, plaintext, ciphertext, rsa, RSA_PKCS1_PADDING);

  free(keybio);
  free(rsa);

  return ret;
}

GFC_API int
gfc_crypto_rsa_private_decrypt(const byte*   ciphertext,
                               uint          ciphertext_length,
                               const byte*   cipher,
                               byte*         plaintext)
{
  RSA*  rsa         = NULL;
  BIO*  keybio      = BIO_new_mem_buf(cipher, -1);

  rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
  int ret = RSA_private_decrypt(ciphertext_length, ciphertext, plaintext, rsa, RSA_PKCS1_PADDING);

  free(keybio);
  free(rsa);

  return ret;
}

#endif // GFC_USE_OPENSSL
