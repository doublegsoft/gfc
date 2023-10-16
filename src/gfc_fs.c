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
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "gfc_fs.h"
#include "gfc_string.h"

void
gfc_fs_iterate(const char* path, user_data data, void (*resolve)(const char*, user_data data))
{
  if (access(path, F_OK) != 0)
    return;

  struct stat attrib;
  stat(path, &attrib);

  if (S_ISREG(attrib.st_mode))
  {
    resolve(path, data);
    return;
  }
#ifdef WIN32
  char subpath[2048];
  strcpy(subpath, path);
  strcat(subpath, "\\*");
  WIN32_FIND_DATA findFileData;
  HANDLE hFind = FindFirstFile(subpath, &findFileData);

  do {
    subpath[0] = '\0';
    strcpy(subpath, path);
    strcat(subpath, "/");
    strcat(subpath, findFileData.cFileName);
    gfc_fs_iterate(subpath, data, resolve);
  } while (FindNextFile(hFind, &findFileData) != 0);
  FindClose(hFind);

  resolve(path, data);
#else
  DIR* dir = opendir(path);
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char subpath[2048];
    strcpy(subpath, path);
    strcat(subpath, "/");
    strcat(subpath, entry->d_name);
    gfc_fs_iterate(subpath, data, resolve);
  }
  closedir(dir);

  resolve(path, data);
#endif
}

static void
gfc_fs_rm_(const char* path, user_data data)
{
  /*!
  ** non-existing
  */
  if (access(path, F_OK) != 0)
    return;

  struct stat attrib;
  stat(path, &attrib);

  if (S_ISREG(attrib.st_mode))
    remove(path);
  else if (S_ISDIR(attrib.st_mode))
  {
#ifdef WIN32
    RemoveDirectory(path);
#else
    rmdir(path);
#endif
  }

}

void
gfc_fs_rm(const char* path)
{
  if (access(path, F_OK) != 0)
    return;
  gfc_fs_iterate(path, NULL, gfc_fs_rm_);
}

struct gfc_fs_mv_s
{
  char* src;
  char* dst;
};

static void
gfc_fs_mv_(const char* path, user_data data)
{
  struct gfc_fs_mv_s* ctx = (struct gfc_fs_mv_s*) data;

  /*!
  ** not existing
  */
  if (access(path, F_OK) != 0)
    return;

  struct stat attrib;
  stat(path, &attrib);

  if (S_ISDIR(attrib.st_mode)
#ifndef WIN32
      || S_ISLNK(attrib.st_mode)
#endif
  )
    return;

  char* ptr = (char*) path;
  ptr += strlen(ctx->src) + 1;

  char dstpath[4096] = {'\0'};
  strcpy(dstpath, ctx->dst);
  strcat(dstpath, "/");
  strcat(dstpath, ptr);

  gfc_fs_touch(dstpath);

  char buff[4096];
  size_t bytes_read;

  FILE* fsrc = fopen(path, "rb");
  FILE* fdst = fopen(dstpath, "wb");

  while ((bytes_read = fread(buff, sizeof(char), 4096, fsrc)) > 0)
    fwrite(buff, sizeof(char), bytes_read, fdst);

  fclose(fsrc);
  fclose(fdst);
}

void
gfc_fs_mv(const char* src, const char* dst)
{
  struct gfc_fs_mv_s ctx;

  ctx.src = (char*) src;
  ctx.dst = (char*) dst;

  gfc_fs_iterate(src, &ctx, gfc_fs_mv_);
}

void
gfc_fs_mkdirs(const char* path)
{
  if (access(path, F_OK) == 0)
    return;

  char subpath[4096] = {'\0'};
  strcpy(subpath, path);

  const char* delimiter = "/";
  char* token = strtok(subpath, delimiter);

  char dir[4096] = {'\0'};
  if (subpath[0] == '/')
    strcpy(dir, "/");
  while (token != NULL)
  {
    strcat(dir, token);
    if (access(dir, F_OK) != 0)
    {
#ifdef WIN32
      mkdir(dir);
#else
      mkdir(dir, S_IRWXU);
#endif
    }
    strcat(dir, "/");
    token = strtok(NULL, delimiter);
  }
}

void
gfc_fs_touch(const char* path)
{
  char subpath[4096] = {'\0'};
  strcpy(subpath, path);

  char* filename = strrchr(subpath, '/');
  if (filename != NULL)
    *filename = '\0';

  gfc_fs_mkdirs(subpath);
  FILE* fp = fopen(path, "w");
  fclose(fp);
}

void
gfc_fs_write(const char* path, const byte* content, size_t len)
{
  FILE* fp = fopen(path, "w");
  fwrite(content, sizeof(byte), len, fp);
  fclose(fp);
}

int
gfc_fs_read(const char* path, gfc_string_p* str)
{
  FILE* fp = fopen(path, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "%s(%d) %s error: %d\n", __FILE__, __LINE__, "gfc_fs_read", GFC_ERROR_INIT);
    return GFC_ERROR_INIT;
  }

  char buff[GFC_BUFFER_SIZE];
  int len;
  *str = gfc_string_new("");
  while ((len = fread(buff, sizeof(char), GFC_BUFFER_SIZE - 1, fp)) > 0)
  {
    buff[len] = '\0';
    gfc_string_concat(*str, buff);
  }
  return GFC_SUCCESS;
}
