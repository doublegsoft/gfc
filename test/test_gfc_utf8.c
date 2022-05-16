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

#include <stdio.h>

#include "gfc.h"


int
main()
{
  char* str = "意义未明的单克隆丙种球蛋白病abc";
  printf("length(%s) = %d\n", str, gfc_utf8_length(str));

  gfc_utf8_string_t utf8str;

  gfc_utf8_convert(str, &utf8str);

  printf("%s\n", utf8str.chars[0].bytes);
}

