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
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "gfc_cli.h"


void
gfc_cli_progress(const char* label, int progress, int total)
{
  //progress width
  const int pwidth = 72;

  //minus label len
  int width = pwidth - strlen( label );
  int pos = ( progress * width ) / total ;


  int percent = ( progress * 100 ) / total;

#ifdef WIN32
  //set green text color, only on Windows
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
#endif
  printf( "%s[", label);

  for ( int i = 0; i < pos; i++ )  printf("%c", '=');
  printf( "% *c", width - pos + 1, ']');
  if (progress < total)
    printf(" %3d%%\r", percent);
  else
    printf(" 100%%\n");

  fflush(stdout);

#ifdef WIN32
  //reset text color, only on Windows
  SetConsoleTextAttribute(GetStdHandle( STD_OUTPUT_HANDLE ), 0x08);
#endif
}
