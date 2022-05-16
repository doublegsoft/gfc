/*
 * gfc
 *
 * Copyright (C) 2019 doublegsoft.open
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef _WIN32
#include <windows.h>
#include <rpc.h>
#include <ctype.h>

#include "gfc_uuid.h"

GFC_API void
gfc_uuid_clear(gfc_uuid_t uu)
{
  UuidCreateNil((UUID*)uu);
}

GFC_API int
gfc_uuid_compare(const gfc_uuid_t uu1, const gfc_uuid_t uu2)
{
  RPC_STATUS rpc_status;
  return UuidCompare((UUID*)uu1, (UUID*)uu2, &rpc_status);
}

GFC_API void
gfc_uuid_copy(gfc_uuid_t dst, const gfc_uuid_t src)
{
  memcpy(dst, src, UUID_T_LENGTH);
}

GFC_API void
gfc_uuid_generate(gfc_uuid_t out)
{
  UuidCreateNil((UUID*)out);
  UuidCreate((UUID*)out);
}

GFC_API void
gfc_uuid_random(gfc_uuid_t out)
{
  char* buf = (char *) out;
  int i;
  for(i = 0; i < UUID_T_LENGTH; i++)
  {
    buf[i] = (rand() >> 20) & 0xff;
  }
  UUID* uuid = (UUID*)out;

  //see also original libuuid source.
  unsigned short* clock_seq = (unsigned short*) uuid->Data4;
  *clock_seq = (*clock_seq & 0x3FFF) | 0x8000;

  unsigned short* time_hi_and_version = (unsigned short*) uuid->Data3;
  *time_hi_and_version = (*time_hi_and_version & 0x0FFF) | 0x4000;
}

GFC_API int
gfc_uuid_null(const gfc_uuid_t uu)
{
  RPC_STATUS rpc_status;
  if(UuidIsNil((UUID*)uu, &rpc_status))
    return 1;
  return 0;
}

GFC_API int
gfc_uuid_parse(const char *in, gfc_uuid_t uu)
{
  RPC_STATUS rpc_status = UuidFromStringA((unsigned char*)in, (UUID*)uu);
  if(rpc_status == RPC_S_OK)
    return 0;
  return -1;
}

GFC_API void
gfc_uuid_lower(const gfc_uuid_t uu, char* out)
{
  unsigned char *p;
  if(UuidToStringA((UUID*)uu,&p) == RPC_S_OK)
  {
    int i;
    for(i=0;i<UUID_STR_LENGTH;i++)
      out[i] = tolower(p[i]);
    out[UUID_STR_LENGTH] = '\0';
    RpcStringFree(&p );
  }
  else
    out[0] = '\0';
}

GFC_API void
gfc_uuid_upper(const gfc_uuid_t uu, char* out)
{
  unsigned char* p;
  if(UuidToStringA((UUID*)uu,&p) == RPC_S_OK)
  {
    int i;
    for(i=0;i<UUID_STR_LENGTH;i++)
      out[i] = toupper(p[i]);

    out[UUID_STR_LENGTH] = '\0';
    RpcStringFree(&p);
  }
  else
    out[0] = '\0';
}
#endif


