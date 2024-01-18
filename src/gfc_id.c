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
#include "gfc_id.h"

#define START_TIMESTAMP 1497453478000

#define SEQ_BIT 12
#define MACHINE_BIT 5
#define DATACENTER_BIT 5

#define MAX_DATACENTER_NUM ((1 << DATACENTER_BIT) - 1)
#define MAX_MACHINE_NUM ((1 << MACHINE_BIT) - 1)
#define MAX_SEQ_NUM ((1 << SEQ_BIT) - 1)

#define MACHINE_LEFT_OFFSET SEQ_BIT
#define DATACENTER_LEFT_OFFSET (SEQ_BIT + MACHINE_BIT)
#define TIMESTAMP_LEFT_OFFSET (DATACENTER_LEFT_OFFSET + DATACENTER_BIT)

typedef gfc_id_t snowflake_st;

static uint64_t snowflake_next_timestamp(uint64_t last_timestamp);
static uint64_t snowflake_timestamp();

static
uint64_t snowflake_id(snowflake_st* snowflake_st)
{
  uint64_t current_timestamp = snowflake_timestamp();

  if (current_timestamp == snowflake_st->last_timestamp)
  {
    snowflake_st->seq = (snowflake_st->seq + 1) & MAX_SEQ_NUM;
    if (snowflake_st->seq == 0)
      current_timestamp = snowflake_next_timestamp(snowflake_st->last_timestamp);
  } else
    snowflake_st->seq = 0;
  snowflake_st->last_timestamp = current_timestamp;
  return (current_timestamp - START_TIMESTAMP) << TIMESTAMP_LEFT_OFFSET | snowflake_st->datacenter << DATACENTER_LEFT_OFFSET | snowflake_st->machine << MACHINE_LEFT_OFFSET | snowflake_st->seq;
}

static uint64_t snowflake_timestamp()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

static uint64_t snowflake_next_timestamp(uint64_t last_timestamp)
{
  uint64_t cur;
  do
  {
    cur = snowflake_timestamp();
  }
  while (cur <= last_timestamp);
  return cur;
}

gfc_id_t* g_id_ctx;

GFC_API void
gfc_id_init(gfc_id_t* ctx)
{
  g_id_ctx = ctx;
}

GFC_API uint64_t
gfc_id_value()
{
  return snowflake_id(g_id_ctx);
}

GFC_API void
gfc_id_string(char* buff)
{
  uint64_t id = gfc_id_value();
  sprintf(buff, "%ld", id);
}
