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
#include <dlfcn.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "../include/gfc_gc.h"

typedef struct gfc_gc_ptr_s
{
  /*!
  ** the pointer to memory which is using or freed.
  */
  char          used;

  /*!
  ** the pointer to memory.
  */
  void*         ptr;

  /*!
  ** the array length if the pointer to an array; otherwise is 1
  */
  size_t        len;

  /*!
  ** the element size.
  */
  size_t        size;

  /*!
  ** the total byte size of which pointer to memory
  */
  size_t        total;

  /*!
  ** the timestamp to allocate memory
  */
  time_t            ts;
}
gfc_gc_ptr_t;

typedef struct gfc_gc_ctx_s
{

  /*!
  ** the managed memory pointers.
  */
  gfc_gc_ptr_t*     ptrs;

  /*!
  ** the length of ptrs which are allocated.
  */
  size_t            len;

  /*!
  ** the total bytes of allocated memory.
  */
  size_t            total;

  /*!
  ** the last timestamp to allocate memory.
  */
  time_t            ts;
}
gfc_gc_ctx_t;

static size_t gfc_malloced_memeory = 0;

static size_t gfc_alloc_cookie = 141105; // we can't easily prevent some free calls from coming to us from outside, mark them

static gfc_gc_ctx_t* gc_ctx = NULL;


void
gfc_gc_init(void)
{
  gc_ctx = (gfc_gc_ctx_t*)malloc(sizeof(gfc_gc_ctx_t));
  gc_ctx->len = 10;
  gc_ctx->total = 0;
  gc_ctx->ptrs = calloc(10, sizeof(gfc_gc_ptr_t));
}

void
gfc_gc_close(void)
{
  free(gc_ctx);
}

void*
gfc_gc_malloc(size_t size, size_t len)
{
  if (gc_ctx == NULL)
  {
    fprintf(stderr, "gc context not initialized, and please call gfc_gc_init at first.\n");
    return NULL;
  }
  size_t total = len * size;
  void* ret = malloc(total);
  if (ret == NULL)
  {
    fprintf(stderr, "error to allocate memory in file (%s) at line number (%d).\n", __FILE__, __LINE__);
    return NULL;
  }
  int idx = 0;
  int is_set = 0;

  for (; idx < gc_ctx->len; idx++)
  {
    if ((int)gc_ctx->ptrs[idx].used == 0)
    {
      memset(&gc_ctx->ptrs[idx], 0, sizeof(gfc_gc_ptr_t));
      gc_ctx->ptrs[idx].ptr = ret;
      gc_ctx->ptrs[idx].used = 1;
      gc_ctx->ptrs[idx].size = size;
      gc_ctx->ptrs[idx].total = total;
      gc_ctx->ptrs[idx].len = len;
      time(&gc_ctx->ptrs[idx].ts);
      is_set = 1;
      break;
    }
  }
  if (is_set == 0)
  {
    gc_ctx->len += 10;
    gc_ctx->ptrs = realloc(gc_ctx->ptrs, gc_ctx->len * sizeof(gfc_gc_ptr_t));
    memset(&gc_ctx->ptrs[idx], 0, sizeof(gfc_gc_ptr_t));
    gc_ctx->ptrs[idx].ptr = ret;
    gc_ctx->ptrs[idx].used = 1;
    gc_ctx->ptrs[idx].size = size;
    gc_ctx->ptrs[idx].total = total;
    gc_ctx->ptrs[idx].len = len;
    time(&gc_ctx->ptrs[idx].ts);
  }
  gc_ctx->ts = gc_ctx->ptrs[idx].ts;
  gc_ctx->total += total;

  assert(gc_ctx->ptrs[idx].ptr == ret);

  return ret;
}

void*
gfc_gc_realloc(void* ptr, size_t size, size_t len)
{
  if (gc_ctx == NULL)
  {
    fprintf(stderr, "gc context not initialized, and please call gfc_gc_init at first.\n");
    return NULL;
  }

  if (ptr == NULL)
  {
    fprintf(stderr, "the given ptr is null in file (%s) at line number (%d).\n", __FILE__, __LINE__);
    return NULL;
  }

  size_t total = len * size;
  void* ret = realloc(ptr, total);

  int i = 0;
  for (i = 0; i < gc_ctx->len; i++)
  {
    if (gc_ctx->ptrs[i].ptr == ptr)
    {
      // adjust total size
      gc_ctx->total += total - gc_ctx->ptrs[i].total;
      // set new size
      gc_ctx->ptrs[i].size = size;
      gc_ctx->ptrs[i].total = total;
      gc_ctx->ptrs[i].len = len;
      gc_ctx->ptrs[i].ptr = ret;
      time(&gc_ctx->ptrs[i].ts);
      return ret;
    }
  }

  // never be here if correct
  assert(1 == 0);
}


size_t
gfc_gc_total(void)
{
  return gc_ctx->total;
}

size_t
gfc_gc_length(void)
{
  return gc_ctx->len;
}

int
gfc_gc_free(void* ptr)
{
  if (ptr == NULL)
    return GFC_GC_IS_NULL;

  int i = 0;
  for (i = 0; i < gc_ctx->len; i++)
  {
    if (gc_ctx->ptrs[i].ptr == ptr)
    {
      gc_ctx->total -= gc_ctx->ptrs[i].total;
      gc_ctx->ptrs[i].used = 0;
      gc_ctx->ptrs[i].size = 0;
      gc_ctx->ptrs[i].len = 0;
      gc_ctx->ptrs[i].total = 0;

      free(gc_ctx->ptrs[i].ptr);
      gc_ctx->ptrs[i].ptr = NULL;
      ptr = NULL;
      return GFC_GC_OK;
    }
  }
  return GFC_GC_NOT_FOUND;
}

//void*
//gfc_gc_malloc(size_t sz)
//{
//  void*(*libc_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");
//  void* answerplus =  libc_malloc(sz + sizeof(size_t) + sizeof(gfc_alloc_cookie));
//  if(answerplus == NULL) return answerplus;// nothing can be done
//  gfc_malloced_memeory += sz;
//  memcpy(answerplus ,&gfc_alloc_cookie, sizeof(gfc_alloc_cookie));
//  memcpy((char *) answerplus + sizeof(gfc_alloc_cookie), &sz, sizeof(sz));
//  return ((char *) answerplus) + sizeof(size_t) + sizeof(gfc_alloc_cookie);
//}


// can fail to produce an aligned result if alignment does not divide 2 * size_t
int
gfc_gc_memalign(void **memptr, size_t alignment, size_t size)
{
  int(*libc_posix_memalign)(void **, size_t, size_t) = dlsym(RTLD_NEXT, "posix_memalign");
  size_t offset = (sizeof(size_t) + sizeof(gfc_alloc_cookie) );
  void * answerplus;
  int ret = libc_posix_memalign(&answerplus,alignment, size + offset);
  if(ret) return ret;// nothing can be done
  gfc_malloced_memeory += size;
  memcpy(answerplus ,&gfc_alloc_cookie,sizeof(gfc_alloc_cookie));
  memcpy((char *) answerplus + sizeof(gfc_alloc_cookie),&size,sizeof(size));
  *memptr = (char *) answerplus + offset;
  return ret;
}

void*
gfc_gc_calloc(size_t count, size_t size) {
  size_t sz = count * size;
  void *(*libc_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");
  size_t volume = sz + sizeof(size_t) + sizeof(gfc_alloc_cookie);
  void * answerplus =  libc_malloc(volume);
  memset(answerplus, 0, volume);
  if(answerplus == NULL) return answerplus;// nothing can be done
  gfc_malloced_memeory += sz;
  memcpy(answerplus ,&gfc_alloc_cookie,sizeof(gfc_alloc_cookie));
  memcpy((char *) answerplus + sizeof(gfc_alloc_cookie),&sz,sizeof(sz));
  return ((char *) answerplus) + sizeof(size_t) + sizeof(gfc_alloc_cookie);
}

//void
//gfc_gc_free(void* p)
//{
//  if(p == NULL) return; // nothing to do
//  void (*libc_free)(void*) = dlsym(RTLD_NEXT, "free");
//  void* truep = ((char *) p) - sizeof(size_t) - sizeof(gfc_alloc_cookie);
//  size_t cookie;
//  // the cookie approach is kind of a hack, don't use in production code!
//  memcpy(&cookie ,truep,sizeof(gfc_alloc_cookie)); // in some case, this might read data outside of bounds
//  if(cookie != gfc_alloc_cookie) {
//    libc_free(p);
//    return;
//  }
//  size_t sz;
//  memcpy(&sz,(char *) truep + sizeof(gfc_alloc_cookie),sizeof(sz));
//  gfc_malloced_memeory -= sz;
//  libc_free(truep);
//}


//void*
//gfc_gc_realloc(void *p, size_t sz)
//{
//  if(p == NULL) return malloc(sz);
//  // implement
//  void *(*libc_realloc)(void *m,size_t) = dlsym(RTLD_NEXT, "realloc");
//  void * truep = ((char *) p) - sizeof(size_t) - sizeof(gfc_alloc_cookie);
//  size_t cookie;
//  // the cookie approach is kind of a hack, don't use in production code!
//  // in some case, this might read data outside of bounds
//  memcpy(&cookie ,truep,sizeof(gfc_alloc_cookie));
//  if(cookie != gfc_alloc_cookie) {
//      return libc_realloc(p,sz);
//  }
//  size_t oldsz;
//  memcpy(&oldsz,(char *) truep + sizeof(gfc_alloc_cookie),sizeof(sz));
//  gfc_malloced_memeory -= oldsz;
//  void * newp = libc_realloc(truep,sz +  sizeof(size_t) + sizeof(gfc_alloc_cookie));
//  if(newp == NULL) return newp;// nothing can be done?
//  gfc_malloced_memeory += sz;
//  memcpy((char *) newp + sizeof(gfc_alloc_cookie),&sz,sizeof(sz));
//  return newp + sizeof(size_t) + sizeof(gfc_alloc_cookie);
//}

size_t gfc_gc_used()
{
  return gfc_malloced_memeory;
}

