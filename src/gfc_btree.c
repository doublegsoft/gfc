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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdatomic.h>
#include <assert.h>

#include "gfc_btree.h"
#include "gfc_gc.h"

typedef struct gfc_btree_node_s*   gfc_btree_node_p;

typedef struct gfc_btree_node_s {

  /*!
  ** the user data
  */
  user_data                 data;

  /*!
  ** the left child
  */
  gfc_btree_node_p          left;

  /*!
  ** the right child
  */
  gfc_btree_node_p          right;

  /*!
  **
  */
  int                       height;
}
gfc_btree_node_t;

typedef struct gfc_btree_s {

  int (*compare)(const user_data a, const user_data b);

  /*!
  ** the user data of node
  */
  void* udata;

  /*!
  ** the size of user element.
  */
  size_t el_size;

  /*!
  ** the root node.
  */
  gfc_btree_node_t*     root;

  /*!
  **
  */
  int                   in_order;

  size_t count;         // number of items in tree
  size_t height;        // height of tree from root to leaf
  size_t max_items;     // max items allowed per node before needing split
  size_t min_items;     // min items allowed per node before needing join


  bool oom;             // last write operation failed due to no memory
  size_t spare_elsize;  // size of each spare element. This is aligned
  char spare_data[];    // spare element spaces for various operations
}
gfc_btree_t;

/*!
**
*/
static gfc_btree_node_p
gfc_btree_node_new(user_data data)
{
  gfc_btree_node_p ret = (gfc_btree_node_p)gfc_gc_malloc(sizeof(gfc_btree_node_t), 1);
  ret->left = NULL;
  ret->right = NULL;
  ret->data = data;
  ret->height = 1; // New node is initially added as a leaf
  return ret;
}

int gfc_btree_height(gfc_btree_node_p node)
{
  if (node == NULL)
    return 0;
  return node->height;
}

// Utility function to get the maximum of two integers
int gfc_btree_max(int a, int b) {
  return (a > b) ? a : b;
}

/*!
** get the balance factor of a node
*/
static int
gfc_btree_balance(gfc_btree_node_p node) {
  if (node == NULL)
    return 0;
  return gfc_btree_height(node->left) - gfc_btree_height(node->right);
}

/*!
** rotate right to fix imbalance
*/
static gfc_btree_node_p
gfc_btree_right_rotate(gfc_btree_node_p y) {
  gfc_btree_node_p x = y->left;
  gfc_btree_node_p T2 = x->right;

  x->right = y;
  y->left = T2;

  // Update heights
  y->height = gfc_btree_max(gfc_btree_height(y->left), gfc_btree_height(y->right)) + 1;
  x->height = gfc_btree_max(gfc_btree_height(x->left), gfc_btree_height(x->right)) + 1;

  return x;
}

/*!
** rotate left to fix imbalance
*/
static gfc_btree_node_p
gfc_btree_left_rotate(gfc_btree_node_p x) {
  gfc_btree_node_p y = x->right;
  gfc_btree_node_p T2 = y->left;

  y->left = x;
  x->right = T2;

  // Update heights
  x->height = gfc_btree_max(gfc_btree_height(x->left), gfc_btree_height(x->right)) + 1;
  y->height = gfc_btree_max(gfc_btree_height(y->left), gfc_btree_height(y->right)) + 1;

  return y;
}

gfc_btree_p
gfc_btree_new(int (*compare)(const user_data a, const user_data b))
{
  size_t size = sizeof(struct gfc_btree_s);
  gfc_btree_p ret = gfc_gc_malloc(sizeof(gfc_btree_t), 1);
  memset(ret, 0, size);
  ret->compare = compare;
  return ret;
}

static gfc_btree_node_p
gfc_btree_insert(gfc_btree_node_p node, user_data data, int (*compare)(const user_data a, const user_data b))
{
  if (node == NULL)
    node = gfc_btree_node_new(data);

  int cmp = compare(data, node->data);
  if (cmp < 0)
    node->left = gfc_btree_insert(node->left, data, compare);
  else if (cmp > 0)
    node->right = gfc_btree_insert(node->right, data, compare);
  else
    return node;

  node->height = 1 + gfc_btree_max(gfc_btree_height(node->left), gfc_btree_height(node->right));

  int balance = gfc_btree_balance(node);

  /*!
  ** left-left
  */
  if (balance > 1 && compare(data, node->left->data) < 0)
    return gfc_btree_right_rotate(node);

  /*!
  ** right-right
  */
  if (balance < -1 && compare(data, node->right->data) > 0)
    return gfc_btree_left_rotate(node);

  /*!
  ** left-right
  */
  if (balance > 1 && compare(data, node->left->data) > 0)
  {
    node->left = gfc_btree_left_rotate(node->left);
    return gfc_btree_right_rotate(node);
  }

  /*!
  ** right-left
  */
  if (balance < -1 && compare(data, node->right->data) < 0)
  {
    node->right = gfc_btree_right_rotate(node->right);
    return gfc_btree_left_rotate(node);
  }

  // If the node is balanced, return it
  return node;
}

/*!
** insert data to avl tree
*/
void
gfc_btree_append(gfc_btree_p ctx, user_data data)
{
  ctx->root = gfc_btree_insert(ctx->root, data, ctx->compare);
}

void
gfc_btree_transverse0(gfc_btree_node_p node)
{
  if (node) {
    gfc_btree_transverse0(node->left);
    printf("%d ", *((int*)node->data));
    gfc_btree_transverse0(node->right);
  }
}

void
gfc_btree_transverse(gfc_btree_p ctx)
{
  gfc_btree_transverse0(ctx->root);
}
