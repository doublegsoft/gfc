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
** Copyright (C) 2023 doublegsoft.open
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
#include <stdlib.h>
#include <pthread.h>

#include "gfc_mt.h"
#include "gfc_gc.h"

struct gfc_mt_s
{
  /*!
  ** the present size.
  */
  uint                size;

  /*!
  ** max thread count.
  */
  uint                max;

  /*!
  ** thread lock.
  */
  pthread_mutex_t     lock;

  /*!
  ** thread condition race.
  */
  pthread_cond_t      cond;
};

typedef user_data (*THREAD_ENTRY)(user_data);

typedef struct  gfc_mt_data_s     gfc_mt_data_t;
typedef         gfc_mt_data_t*    gfc_mt_data_p;

struct gfc_mt_data_s
{

  gfc_mt_p       threads;

  void*               userdata;

  THREAD_ENTRY        entry;
};

/*!
** It is a framework thread function.
*/
static void*
gfc_mt_entry(void* data)
{
  gfc_mt_data_t* threads_data = (gfc_mt_data_t*) data;
  // invoke user function
  threads_data->entry(threads_data->userdata);

  pthread_mutex_lock(&threads_data->threads->lock);
  threads_data->threads->size--;
  pthread_cond_signal(&threads_data->threads->cond);
  pthread_mutex_unlock(&threads_data->threads->lock);

  gfc_gc_free(threads_data);
  return NULL;
}

GFC_API gfc_mt_p
gfc_mt_new(uint count)
{
  gfc_mt_p ret = (gfc_mt_p) gfc_gc_malloc(sizeof(gfc_mt_t), 1);
  pthread_mutex_init(&ret->lock, NULL);
  pthread_cond_init(&ret->cond, NULL);

  ret->max = count;
  ret->size = 0;
  return ret;
}

GFC_API void
gfc_mt_do(gfc_mt_p threads, void*(*fun)(void* data), void* data)
{
  int res = 0;
  pthread_mutex_lock(&threads->lock);
  while (threads->size >= threads->max)
  {
    pthread_cond_wait(&threads->cond, &threads->lock);
  }
  pthread_mutex_unlock(&threads->lock);

  threads->size++;
  pthread_t thid;
  gfc_mt_data_p threads_data = (gfc_mt_data_p) gfc_gc_malloc(sizeof(gfc_mt_data_t), 1);
  threads_data->threads = threads;
  threads_data->userdata = data;
  threads_data->entry = fun;
  res = pthread_create(&thid, NULL, gfc_mt_entry, threads_data);
  if (res != 0)
  {
    threads->size--;
  }
}

GFC_API void
gfc_mt_free(gfc_mt_p threads)
{
  pthread_mutex_lock(&threads->lock);
  while (threads->size > 0)
  {
    pthread_cond_wait(&threads->cond, &threads->lock);
  }
  pthread_mutex_unlock(&threads->lock);

  gfc_gc_free(threads);
}

GFC_API int
gfc_mt_size(gfc_mt_p mt)
{
  return mt->size;
}

