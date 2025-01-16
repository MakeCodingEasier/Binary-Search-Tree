#ifndef TREE_H
#define TREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"

// B树部分
#define MIN_DEGREE 10 // 最小度数

// B树节点
typedef struct BTreeNode
{
    int num;                     // 当前节点的键的个数
    char **keys;                 // 键数组
    char **values;               // 值数组
    struct BTreeNode **children; // 子指针数组
    int is_leaf;                 // 是否为叶子节点
} BTreeNode;
// B树
typedef struct BTree
{
    BTreeNode *root; // B树的根节点
} BTree;
// 函数声明
BTreeNode *create_node(int is_leaf);
char *B_search(BTreeNode *node, const char *key);
int B_search_in_range(BTreeNode *node, const char *start, const char *end);
void B_preorderPrintToFile(BTreeNode *node, int level, int child_num, FILE *file);
void split_child(BTreeNode *parent, int index, BTreeNode *child);
void insert_non_full(BTreeNode *node, const char *key, const char *value);
void B_insert(BTree *tree, const char *key, const char *value);
void remove_key(BTreeNode *node, const char *key);
void remove_from_leaf(BTreeNode *node, int index);
void remove_from_non_leaf(BTreeNode *node, int index);
void borrow_from_prev(BTreeNode *node, int index);
void borrow_from_next(BTreeNode *node, int index);
void merge(BTreeNode *node, int index);
void delete_key(BTree *tree, const char *key);
void B_free_tree(BTree *tree);
void free_tree_b(BTreeNode *node);

// 红黑树部分
#define RED 0
#define BLACK 1
// 节点结构
typedef struct Node
{
    char *key;   // 英语单词
    char *value; // 中文翻译
    int color;   // 节点颜色
    struct Node *parent;
    struct Node *left;
    struct Node *right;
} Node;

// 红黑树结构
typedef struct RedBlackTree
{
    Node *root;
} RedBlackTree;
// 函数声明
Node *newNode(char *key, char *value);
RedBlackTree *newRedBlackTree();
void leftRotate(RedBlackTree *tree, Node *x);
void rightRotate(RedBlackTree *tree, Node *x);
void insertFixup(RedBlackTree *tree, Node *z);
void RB_insert(RedBlackTree *tree, char *key, char *value);
Node *treeMinimum(Node *x);
Node *treeSuccessor(Node *x);
Node *RB_search(RedBlackTree *tree, const char *key);
int RB_searchRange(Node *node, const char *low, const char *high);
void deleteFixup(RedBlackTree *tree, Node *node, Node *parent);
void RB_delete(RedBlackTree *tree, const char *key);
void RB_preorderPrintToFile(RedBlackTree *tree, Node *node, int level, int child, FILE *file);
void RB_free_tree(RedBlackTree *tree);
void free_tree_rb(Node *node);

#endif