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
#include <stdlib.h>
#include <pthread.h>

#include "gfc_threads.h"

struct gfc_threads_s
{
  /*!
  ** the present size.
  */
  uint                size;

  /*!
  ** max thread count.
  */
  uint                max;

  pthread_mutex_t     lock;

  pthread_cond_t      cond;
};

typedef user_data (*THREAD_ENTRY)(user_data);

typedef struct  gfc_threads_data_s     gfc_threads_data_t;
typedef         gfc_threads_data_t*    gfc_threads_data_p;

struct gfc_threads_data_s
{

  gfc_threads_p       threads;

  void*               userdata;

  THREAD_ENTRY        entry;
};

/*!
** It is a framework thread function.
*/
static void*
gfc_threads_entry(void* data)
{
  gfc_threads_data_t* threads_data = (gfc_threads_data_t*) data;
  // invoke user function
  threads_data->entry(threads_data->userdata);

  pthread_mutex_lock(&threads_data->threads->lock);
  threads_data->threads->size--;
  pthread_cond_signal(&threads_data->threads->cond);
  pthread_mutex_unlock(&threads_data->threads->lock);

  free(threads_data);
  return NULL;
}

gfc_threads_p
gfc_threads_new(uint count)
{
  gfc_threads_p ret = (gfc_threads_p) malloc(sizeof(gfc_threads_t));
  pthread_mutex_init(&ret->lock, NULL);
  pthread_cond_init(&ret->cond, NULL);

  ret->max = count;
  ret->size = 0;
  return ret;
}

void
gfc_threads_do(gfc_threads_p threads, void*(*fun)(void* data), void* data)
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
  gfc_threads_data_p threads_data = (gfc_threads_data_p) malloc(sizeof(gfc_threads_data_t));
  threads_data->threads = threads;
  threads_data->userdata = data;
  threads_data->entry = fun;
  res = pthread_create(&thid, NULL, gfc_threads_entry, threads_data);
  if (res != 0)
  {
    threads->size--;
  }
}

void
gfc_threads_free(gfc_threads_p threads)
{
  pthread_mutex_lock(&threads->lock);
  while (threads->size > 0)
  {
    pthread_cond_wait(&threads->cond, &threads->lock);
  }
  pthread_mutex_unlock(&threads->lock);

  free(threads);
}

