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

#include "../include/gfc_gc.h"

static size_t gfc_malloced_memeory = 0;

static size_t gfc_alloc_cookie = 141105; // we can't easily prevent some free calls from coming to us from outside, mark them

void*
gfc_gc_malloc(size_t sz)
{
  void*(*libc_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");
  void* answerplus =  libc_malloc(sz + sizeof(size_t) + sizeof(gfc_alloc_cookie));
  if(answerplus == NULL) return answerplus;// nothing can be done
  gfc_malloced_memeory += sz;
  memcpy(answerplus ,&gfc_alloc_cookie, sizeof(gfc_alloc_cookie));
  memcpy((char *) answerplus + sizeof(gfc_alloc_cookie), &sz, sizeof(sz));
  return ((char *) answerplus) + sizeof(size_t) + sizeof(gfc_alloc_cookie);
}


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

void
gfc_gc_free(void* p)
{
  if(p == NULL) return; // nothing to do
  void (*libc_free)(void*) = dlsym(RTLD_NEXT, "free");
  void* truep = ((char *) p) - sizeof(size_t) - sizeof(gfc_alloc_cookie);
  size_t cookie;
  // the cookie approach is kind of a hack, don't use in production code!
  memcpy(&cookie ,truep,sizeof(gfc_alloc_cookie)); // in some case, this might read data outside of bounds
  if(cookie != gfc_alloc_cookie) {
    libc_free(p);
    return;
  }
  size_t sz;
  memcpy(&sz,(char *) truep + sizeof(gfc_alloc_cookie),sizeof(sz));
  gfc_malloced_memeory -= sz;
  libc_free(truep);
}


void*
gfc_gc_realloc(void *p, size_t sz)
{
  if(p == NULL) return malloc(sz);
  // implement
  void *(*libc_realloc)(void *m,size_t) = dlsym(RTLD_NEXT, "realloc");
  void * truep = ((char *) p) - sizeof(size_t) - sizeof(gfc_alloc_cookie);
  size_t cookie;
  // the cookie approach is kind of a hack, don't use in production code!
  memcpy(&cookie ,truep,sizeof(gfc_alloc_cookie)); // in some case, this might read data outside of bounds
  if(cookie != gfc_alloc_cookie) {
      return libc_realloc(p,sz);
  }
  size_t oldsz;
  memcpy(&oldsz,(char *) truep + sizeof(gfc_alloc_cookie),sizeof(sz));
  gfc_malloced_memeory -= oldsz;
  void * newp = libc_realloc(truep,sz +  sizeof(size_t) + sizeof(gfc_alloc_cookie));
  if(newp == NULL) return newp;// nothing can be done?
  gfc_malloced_memeory += sz;
  memcpy((char *) newp + sizeof(gfc_alloc_cookie),&sz,sizeof(sz));
  return newp + sizeof(size_t) + sizeof(gfc_alloc_cookie);
}

size_t gfc_gc_used()
{
  return gfc_malloced_memeory;
}

