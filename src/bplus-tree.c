
#include <stdio.h>
#include <stdlib.h>

#define MAX_KEYS 3  // B+树的阶
#define MIN_KEYS ((MAX_KEYS + 1) / 2)  // 最小关键字数量

// B+树的节点结构
typedef struct BPlusTreeNode {
  int isLeaf;  // 是否为叶子节点
  int numKeys; // 当前节点中的键数量
  int keys[MAX_KEYS]; // 键
  struct BPlusTreeNode* children[MAX_KEYS + 1]; // 子节点的指针
  struct BPlusTreeNode* next; // 指向下一个叶子节点的指针
  struct BPlusTreeNode* parent; // 指向父节点的指针
} BPlusTreeNode;

// 创建一个新的节点
BPlusTreeNode* createNode(int isLeaf) {
  BPlusTreeNode* newNode = (BPlusTreeNode*)malloc(sizeof(BPlusTreeNode));
  newNode->isLeaf = isLeaf;
  newNode->numKeys = 0;
  for (int i = 0; i < MAX_KEYS + 1; i++) {
    newNode->children[i] = NULL;
  }
  newNode->next = NULL;
  newNode->parent = NULL;
  return newNode;
}

// 查找键在B+树中的位置
BPlusTreeNode* find(BPlusTreeNode* root, int key) {
  if (root == NULL) {
    return NULL;
  }
  
  BPlusTreeNode* current = root;
  while (!current->isLeaf) {
    int i = 0;
    while (i < current->numKeys && key >= current->keys[i]) {
      i++;
    }
    current = current->children[i];
  }
  
  int i = 0;
  while (i < current->numKeys && key > current->keys[i]) {
    i++;
  }
  if (i < current->numKeys && key == current->keys[i]) {
    return current; // 找到键，返回叶子节点
  } else {
    return NULL; // 未找到键
  }
}

void deleteInternal(BPlusTreeNode** root, BPlusTreeNode* parent) {
  if (parent->numKeys >= MIN_KEYS) {
    return;
  }

  BPlusTreeNode* grandParent = parent->parent;
  int i = 0;
  while (i < grandParent->numKeys && grandParent->children[i] != parent) {
    i++;
  }

  if (i > 0 && grandParent->children[i - 1]->numKeys > MIN_KEYS) {
    BPlusTreeNode* leftSibling = grandParent->children[i - 1];
    for (int j = parent->numKeys; j > 0; j--) {
      parent->keys[j] = parent->keys[j - 1];
      parent->children[j + 1] = parent->children[j];
    }
    parent->keys[0] = grandParent->keys[i - 1];
    parent->children[1] = parent->children[0];
    parent->children[0] = leftSibling->children[leftSibling->numKeys];
    parent->numKeys++;
    grandParent->keys[i - 1] = leftSibling->keys[leftSibling->numKeys - 1];
    leftSibling->numKeys--;
  } else if (i < grandParent->numKeys && grandParent->children[i + 1]->numKeys > MIN_KEYS) {
    BPlusTreeNode* rightSibling = grandParent->children[i + 1];
    parent->keys[parent->numKeys] = grandParent->keys[i];
    parent->children[parent->numKeys + 1] = rightSibling->children[0];
    parent->numKeys++;
    grandParent->keys[i] = rightSibling->keys[0];
    for (int j = 0; j < rightSibling->numKeys - 1; j++) {
      rightSibling->keys[j] = rightSibling->keys[j + 1];
      rightSibling->children[j] = rightSibling->children[j + 1];
    }
    rightSibling->children[rightSibling->numKeys - 1] = rightSibling->children[rightSibling->numKeys];
    rightSibling->numKeys--;
  } else {
    if (i > 0) {
      BPlusTreeNode* leftSibling = grandParent->children[i - 1];
      leftSibling->keys[leftSibling->numKeys] = grandParent->keys[i - 1];
      for (int j = 0; j < parent->numKeys; j++) {
        leftSibling->keys[leftSibling->numKeys + 1 + j] = parent->keys[j];
        leftSibling->children[leftSibling->numKeys + 1 + j] = parent->children[j];
      }
      leftSibling->children[leftSibling->numKeys + 1 + parent->numKeys] = parent->children[parent->numKeys];
      leftSibling->numKeys += parent->numKeys + 1;
      for (int j = i - 1; j < grandParent->numKeys - 1; j++) {
        grandParent->keys[j] = grandParent->keys[j + 1];
        grandParent->children[j + 1] = grandParent->children[j + 2];
      }
      grandParent->numKeys--;
      free(parent);
    } else {
      BPlusTreeNode* rightSibling = grandParent->children[i + 1];
      parent->keys[parent->numKeys] = grandParent->keys[i];
      for (int j = 0; j < rightSibling->numKeys; j++) {
        parent->keys[parent->numKeys + 1 + j] = rightSibling->keys[j];
        parent->children[parent->numKeys + 1 + j] = rightSibling->children[j];
      }
      parent->children[parent->numKeys + 1 + rightSibling->numKeys] = rightSibling->children[rightSibling->numKeys];
      parent->numKeys += rightSibling->numKeys + 1;
      for (int j = i; j < grandParent->numKeys - 1; j++) {
        grandParent->keys[j] = grandParent->keys[j + 1];
        grandParent->children[j + 1] = grandParent->children[j + 2];
      }
      grandParent->numKeys--;
      free(rightSibling);
    }
  }

  if (grandParent->numKeys == 0 && grandParent == *root) {
    *root = grandParent->children[0];
    free(grandParent);
  } else if (grandParent->numKeys < MIN_KEYS) {
    deleteInternal(root, grandParent);
  }
}

// 插入键到内部节点
void insertInternal(BPlusTreeNode** root, BPlusTreeNode* parent, int key, BPlusTreeNode* leftChild, BPlusTreeNode* rightChild) {
  // 找到插入位置
  int i = 0;
  while (i < parent->numKeys && key >= parent->keys[i]) {
    i++;
  }

  // 移动键和子节点指针以腾出插入位置
  for (int j = parent->numKeys; j > i; j--) {
    parent->keys[j] = parent->keys[j - 1];
    parent->children[j + 1] = parent->children[j];
  }

  // 插入新键和右子节点指针
  parent->keys[i] = key;
  parent->children[i + 1] = rightChild;
  parent->numKeys++;

  // 更新子节点的父节点指针
  leftChild->parent = parent;
  rightChild->parent = parent;

  // 检查是否需要分裂内部节点
  if (parent->numKeys == MAX_KEYS) {
    // 创建新的内部节点
    BPlusTreeNode* newInternal = createNode(0);
    int mid = MAX_KEYS / 2;

    // 将后半部分的键和子节点移动到新内部节点
    newInternal->numKeys = MAX_KEYS - mid - 1;
    for (int j = 0; j < newInternal->numKeys; j++) {
      newInternal->keys[j] = parent->keys[mid + 1 + j];
      newInternal->children[j] = parent->children[mid + 1 + j];
      newInternal->children[j]->parent = newInternal;
    }
    newInternal->children[newInternal->numKeys] = parent->children[MAX_KEYS];
    newInternal->children[newInternal->numKeys]->parent = newInternal;

    // 更新旧节点的键数量
    parent->numKeys = mid;

    // 如果父节点是根节点，创建新的根节点
    if (parent == *root) {
      BPlusTreeNode* newRoot = createNode(0);
      newRoot->keys[0] = parent->keys[mid];
      newRoot->children[0] = parent;
      newRoot->children[1] = newInternal;
      newRoot->numKeys = 1;
      *root = newRoot;
      parent->parent = newRoot;
      newInternal->parent = newRoot;
    } else {
      // 否则，将中间键插入到父节点中
      insertInternal(root, parent->parent, parent->keys[mid], parent, newInternal);
    }
  }
}

// 插入键到B+树中
void insert(BPlusTreeNode** root, int key) {
  if (*root == NULL) {
    *root = createNode(1);
    (*root)->keys[0] = key;
    (*root)->numKeys = 1;
    return;
  }

  BPlusTreeNode* current = *root;
  BPlusTreeNode* parent = NULL;

  while (!current->isLeaf) {
    parent = current;
    int i = 0;
    while (i < current->numKeys && key >= current->keys[i]) {
      i++;
    }
    current = current->children[i];
  }

  int i = 0;
  while (i < current->numKeys && key > current->keys[i]) {
    i++;
  }
  for (int j = current->numKeys; j > i; j--) {
    current->keys[j] = current->keys[j - 1];
  }
  current->keys[i] = key;
  current->numKeys++;

  if (current->numKeys == MAX_KEYS) {
    BPlusTreeNode* newLeaf = createNode(1);
    int mid = (MAX_KEYS + 1) / 2;

    newLeaf->numKeys = current->numKeys - mid;
    for (int j = 0; j < newLeaf->numKeys; j++) {
      newLeaf->keys[j] = current->keys[mid + j];
    }
    current->numKeys = mid;

    newLeaf->next = current->next;
    current->next = newLeaf;

    if (parent == NULL) {
      BPlusTreeNode* newRoot = createNode(0);
      newRoot->keys[0] = newLeaf->keys[0];
      newRoot->children[0] = current;
      newRoot->children[1] = newLeaf;
      newRoot->numKeys = 1;
      *root = newRoot;
      current->parent = newRoot;
      newLeaf->parent = newRoot;
    } else {
      insertInternal(root, parent, newLeaf->keys[0], current, newLeaf);
    }
  }
}

// 删除B+树中的键
void delete(BPlusTreeNode** root, int key) {
  if (*root == NULL) {
    printf("B+树为空。\n");
    return;
  }

  BPlusTreeNode* current = *root;
  BPlusTreeNode* parent = NULL;

  while (!current->isLeaf) {
    parent = current;
    int i = 0;
    while (i < current->numKeys && key >= current->keys[i]) {
      i++;
    }
    current = current->children[i];
  }

  int i = 0;
  while (i < current->numKeys && key > current->keys[i]) {
    i++;
  }
  if (i >= current->numKeys || key != current->keys[i]) {
    printf("键不在树中。\n");
    return;
  }

  for (int j = i; j < current->numKeys - 1; j++) {
    current->keys[j] = current->keys[j + 1];
  }
  current->numKeys--;

  if (current->numKeys < MIN_KEYS) {
    if (current->next && current->next->numKeys > MIN_KEYS) {
      current->keys[current->numKeys] = current->next->keys[0];
      current->numKeys++;
      for (int j = 0; j < current->next->numKeys - 1; j++) {
        current->next->keys[j] = current->next->keys[j + 1];
      }
      current->next->numKeys--;
    } else {
      if (parent) {
        for (int j = 0; j < current->next->numKeys; j++) {
          current->keys[current->numKeys + j] = current->next->keys[j];
        }
        current->numKeys += current->next->numKeys;
        BPlusTreeNode* temp = current->next;
        current->next = current->next->next;
        free(temp);

        for (int j = i; j < parent->numKeys - 1; j++) {
          parent->keys[j] = parent->keys[j + 1];
          parent->children[j + 1] = parent->children[j + 2];
        }
        parent->numKeys--;

        if (parent->numKeys < MIN_KEYS) {
          deleteInternal(root, parent);
        }
      } else {
        if (current->numKeys == 0) {
          *root = current->next;
          free(current);
        }
      }
    }
  }
}

// 测试B+树的基本功能
void testBPlusTree() {
  BPlusTreeNode* root = NULL;

  insert(&root, 10);
  insert(&root, 20);
  insert(&root, 5);
  insert(&root, 6);
  insert(&root, 12);
  insert(&root, 30);
  insert(&root, 7);
  insert(&root, 17);

  BPlusTreeNode* found = find(root, 6);
  if (found != NULL) {
    printf("Found key 6 in B+ tree.\n");
  } else {
    printf("Key 6 not found in B+ tree.\n");
  }

  delete(&root, 6);
  found = find(root, 6);
  if (found != NULL) {
    printf("Found key 6 in B+ tree.\n");
  } else {
    printf("Key 6 not found in B+ tree.\n");
  }
}


int main(int argc, char* argv[])
{
	testBPlusTree();
	return 0;
}
