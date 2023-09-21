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

void
gfc_fs_rm(const char* path)
{
  if (access(path, F_OK) != 0)
    return;

  struct stat path_stat;
  stat(path, &path_stat);

  if (S_ISREG(path_stat.st_mode))
  {
    remove(path);
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
    gfc_fs_rm(subpath);
  } while (FindNextFile(hFind, &findFileData) != 0);
  FindClose(hFind);

  RemoveDirectory(path);
#else
  DIR* dir = opendir(path);
  if (ENOENT == errno)
    return;
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char subpath[2048];
    strcpy(subpath, path);
    strcat(subpath, "/");
    strcat(subpath, entry->d_name);
    gfc_fs_rm(subpath);
  }
  rmdir(path);
#endif
}

void
gfc_fs_mkdirs(const char* path)
{
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
    strcat(dir, "/");
    if (access(dir, F_OK) != 0)
#ifdef WIN32
      mkdir(dir);
#else
      mkdir(dir, 0755);
#endif
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
