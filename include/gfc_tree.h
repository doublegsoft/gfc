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

#ifndef __GFC_TREE_H__
#define __GFC_TREE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct gfc_tree_node_s gfc_tree_node_t;

struct gfc_tree_node_s
{
  gfc_tree_node_t*      parent;

  gfc_tree_node_t*      children;

  uint32_t              child_size;

  void*                 data;

}
gfc_tree_node_t;


#ifdef __cplusplus
}
#endif

#endif // __GFC_TREE_H__
