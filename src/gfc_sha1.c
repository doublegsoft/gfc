/*
 **           .d888
 **          d88P"
 **          888
 **  .d88b.  888888 .d8888b
 ** d88P"88b 888   d88P"
 ** 888  888 888   888
 ** Y88b 888 888   Y88b.
 **  "Y88888 888    "Y8888P
 **      888
 ** Y8b d88P
 **  "Y88P"
 **
 ** Copyright (C) 2022 doublegsoft.open
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
#include <gfc_sha1.h>

/*
 *  Define the SHA1 circular left shift macro
 */
#define SHA1CircularShift(bits,word) \
                (((word) << (bits)) | ((word) >> (32-(bits))))

/* Local Function Prototyptes */
static void
SHA1PadMessage (SHA1Context*);

static void
SHA1ProcessMessageBlock (SHA1Context*);

/*
 *  SHA1Reset
 *
 *  Description:
 *      This function will initialize the SHA1Context in preparation
 *      for computing a new SHA1 message digest.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to reset.
 *
 *  Returns:
 *      sha Error Code.
 *
 */
int
gfc_sha1_reset (SHA1Context *context)
{
  if (!context)
    return shaNull;

  context->length_low = 0;
  context->length_high = 0;
  context->message_block_index = 0;

  context->intermediate_hash[0] = 0x67452301;
  context->intermediate_hash[1] = 0xEFCDAB89;
  context->intermediate_hash[2] = 0x98BADCFE;
  context->intermediate_hash[3] = 0x10325476;
  context->intermediate_hash[4] = 0xC3D2E1F0;

  context->computed = 0;
  context->corrupted = 0;

  return shaSuccess;
}

/*
 *  SHA1Result
 *
 *  Description:
 *      This function will return the 160-bit message digest into the
 *      Message_Digest array  provided by the caller.
 *      NOTE: The first octet of hash is stored in the 0th element,
 *            the last octet of hash in the 19th element.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to use to calculate the SHA-1 hash.
 *      Message_Digest: [out]
 *          Where the digest is returned.
 *
 *  Returns:
 *      sha Error Code.
 *
 */
int
gfc_sha1_result (SHA1Context *context, uint8_t message_digest[SHA1HashSize])
{
  int i;

  if (!context || !message_digest)
  {
    return shaNull;
  }

  if (context->corrupted)
  {
    return context->corrupted;
  }

  if (!context->computed)
  {
    SHA1PadMessage (context);
    for (i = 0; i < 64; ++i)
    {
      /* message may be sensitive, clear it out */
      context->message_block[i] = 0;
    }
    context->length_low = 0; /* and clear length */
    context->length_high = 0;
    context->computed = 1;

  }

  for (i = 0; i < SHA1HashSize; ++i)
  {
    message_digest[i] = context->intermediate_hash[i >> 2]
        >> 8 * (3 - (i & 0x03));
  }

  return shaSuccess;
}

/*
 *  SHA1Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion
 *      of the message.
 *
 *  Parameters:
 *      context: [in/out]
 *          The SHA context to update
 *      message_array: [in]
 *          An array of characters representing the next portion of
 *          the message.
 *      length: [in]
 *          The length of the message in message_array
 *
 *  Returns:
 *      sha Error Code.
 *
 */
int
SHA1Input (SHA1Context *context, const uint8_t *message_array, unsigned length)
{
  if (!length)
  {
    return shaSuccess;
  }

  if (!context || !message_array)
  {
    return shaNull;
  }

  if (context->computed)
  {
    context->corrupted = shaStateError;
    return shaStateError;
  }

  if (context->corrupted)
  {
    return context->corrupted;
  }
  while (length-- && !context->corrupted)
  {
    context->message_block[context->message_block_index++] =
        (*message_array & 0xFF);

    context->length_low += 8;
    if (context->length_low == 0)
    {
      context->length_high++;
      if (context->length_high == 0)
      {
        /* Message is too long */
        context->corrupted = 1;
      }
    }

    if (context->message_block_index == 64)
    {
      SHA1ProcessMessageBlock (context);
    }

    message_array++;
  }

  return shaSuccess;
}

/*
 *  SHA1ProcessMessageBlock
 *
 *  Description:
 *      This function will process the next 512 bits of the message
 *      stored in the message_block array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:

 *      Many of the variable names in this code, especially the
 *      single character names, were used because those were the
 *      names used in the publication.
 *
 *
 */
static void
SHA1ProcessMessageBlock (SHA1Context *context)
{
  const uint32_t K[] =
    { /* Constants defined in SHA-1   */
    0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
  int t; /* Loop counter                */
  uint32_t temp; /* Temporary word value        */
  uint32_t W[80]; /* Word sequence               */
  uint32_t A, B, C, D, E; /* Word buffers                */

  /*
   *  Initialize the first 16 words in the array W
   */
  for (t = 0; t < 16; t++)
  {
    W[t] = context->message_block[t * 4] << 24;
    W[t] |= context->message_block[t * 4 + 1] << 16;
    W[t] |= context->message_block[t * 4 + 2] << 8;
    W[t] |= context->message_block[t * 4 + 3];
  }

  for (t = 16; t < 80; t++)
  {
    W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
  }

  A = context->intermediate_hash[0];
  B = context->intermediate_hash[1];
  C = context->intermediate_hash[2];
  D = context->intermediate_hash[3];
  E = context->intermediate_hash[4];

  for (t = 0; t < 20; t++)
  {
    temp = SHA1CircularShift(5,A) +
        ((B & C) | ((~B) & D)) + E + W[t] + K[0];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);

    B = A;
    A = temp;
  }

  for (t = 20; t < 40; t++)
  {
    temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);
    B = A;
    A = temp;
  }

  for (t = 40; t < 60; t++)
  {
    temp = SHA1CircularShift(5,A) +
        ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);
    B = A;
    A = temp;
  }

  for (t = 60; t < 80; t++)
  {
    temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
    E = D;
    D = C;
    C = SHA1CircularShift(30, B);
    B = A;
    A = temp;
  }

  context->intermediate_hash[0] += A;
  context->intermediate_hash[1] += B;
  context->intermediate_hash[2] += C;
  context->intermediate_hash[3] += D;
  context->intermediate_hash[4] += E;

  context->message_block_index = 0;
}

/*
 *  SHA1PadMessage
 *

 *  Description:
 *      According to the standard, the message must be padded to an even
 *      512 bits.  The first padding bit must be a '1'.  The last 64
 *      bits represent the length of the original message.  All bits in
 *      between should be 0.  This function will pad the message
 *      according to those rules by filling the message_block array
 *      accordingly.  It will also call the ProcessMessageBlock function
 *      provided appropriately.  When it returns, it can be assumed that
 *      the message digest has been computed.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to pad
 *      ProcessMessageBlock: [in]
 *          The appropriate SHA*ProcessMessageBlock function
 *  Returns:
 *      Nothing.
 *
 */

static void
SHA1PadMessage (SHA1Context *context)
{
  /*
   *  Check to see if the current message block is too small to hold
   *  the initial padding bits and length.  If so, we will pad the
   *  block, process it, and then continue padding into a second
   *  block.
   */
  if (context->message_block_index > 55)
  {
    context->message_block[context->message_block_index++] = 0x80;
    while (context->message_block_index < 64)
    {
      context->message_block[context->message_block_index++] = 0;
    }

    SHA1ProcessMessageBlock (context);

    while (context->message_block_index < 56)
    {
      context->message_block[context->message_block_index++] = 0;
    }
  }
  else
  {
    context->message_block[context->message_block_index++] = 0x80;
    while (context->message_block_index < 56)
    {

      context->message_block[context->message_block_index++] = 0;
    }
  }

  /*
   *  Store the message length as the last 8 octets
   */
  context->message_block[56] = context->length_high >> 24;
  context->message_block[57] = context->length_high >> 16;
  context->message_block[58] = context->length_high >> 8;
  context->message_block[59] = context->length_high;
  context->message_block[60] = context->length_low >> 24;
  context->message_block[61] = context->length_low >> 16;
  context->message_block[62] = context->length_low >> 8;
  context->message_block[63] = context->length_low;

  SHA1ProcessMessageBlock (context);
}
