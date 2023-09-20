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

#ifdef __MINGW32__
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

#ifdef __MINGW32__
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
    if (entry->d_type == DT_DIR)
    {
      gfc_fs_rm(subpath);
      continue;
    }
    remove(subpath);
  }
  rmdir(path);
#endif
}
