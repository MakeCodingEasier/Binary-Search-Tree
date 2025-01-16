#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"

// 创建一个新的节点
Node *newNode(char *key, char *value)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->key = strdup(key);
    node->value = strdup(value);
    node->color = RED; // 新插入必为红色
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 创建一个新的红黑树
RedBlackTree *newRedBlackTree()
{
    RedBlackTree *tree = (RedBlackTree *)malloc(sizeof(RedBlackTree));
    tree->root = NULL;
    return tree;
}

// 左旋操作
void leftRotate(RedBlackTree *tree, Node *x)
{
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NULL)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL)
    {
        tree->root = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// 右旋操作
void rightRotate(RedBlackTree *tree, Node *x)
{
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NULL)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL)
    {
        tree->root = y;
    }
    else if (x == x->parent->right)
    {
        x->parent->right = y;
    }
    else
    {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

// 插入后的修复操作
void insertFixup(RedBlackTree *tree, Node *z)
{
    while (z->parent != NULL && z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            Node *y = z->parent->parent->right;
            if (y != NULL && y->color == RED)
            {
                // Case 1: 父节点和叔节点都是红色
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    // Case 2: 父节点是红色，叔节点是黑色，当前节点是父节点的右孩子
                    z = z->parent;
                    leftRotate(tree, z);
                }
                // Case 3: 父节点是红色，叔节点是黑色，当前节点是父节点的左孩子
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(tree, z->parent->parent);
            }
        }
        else
        {
            // 对称情况
            Node *y = z->parent->parent->left;
            if (y != NULL && y->color == RED)
            {
                // Case 1: 父节点和叔节点都是红色
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    // Case 2: 父节点是红色，叔节点是黑色，当前节点是父节点的左孩子
                    z = z->parent;
                    rightRotate(tree, z);
                }
                // Case 3: 父节点是红色，叔节点是黑色，当前节点是父节点的右孩子
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

// 红黑树插入操作
void RB_insert(RedBlackTree *tree, char *key, char *value)
{
    Node *z = newNode(key, value);
    Node *y = NULL;
    Node *x = tree->root;

    while (x != NULL)
    {
        y = x;
        if (strcmp(z->key, x->key) < 0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NULL)
    {
        tree->root = z;
    }
    else if (strcmp(z->key, y->key) < 0)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }
    // 修复操作
    insertFixup(tree, z);
}

// 寻找树的最小节点
Node *treeMinimum(Node *x)
{
    while (x->left != NULL)
    {
        x = x->left;
    }
    return x;
}

// 寻找节点的后继节点
Node *treeSuccessor(Node *x)
{
    if (x->right != NULL)
    {
        return treeMinimum(x->right);
    }
    Node *y = x->parent;
    while (y != NULL && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

// 红黑树搜索操作
Node *RB_search(RedBlackTree *tree, const char *key)
{
    Node *x = tree->root;
    while (x != NULL && strcmp(key, x->key) != 0)
    {
        if (strcmp(key, x->key) < 0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    return x;
}

// 在红黑树中搜索指定范围内的键，并计数
int RB_searchRange(Node *node, const char *low, const char *high)
{
    int count = 0;
    if (node == NULL)
        return count;
    if (strcmp(low, node->key) < 0)
    {
        count += RB_searchRange(node->left, low, high);
    }
    // 当前单词在范围里
    if (strcmp(low, node->key) <= 0 && strcmp(node->key, high) <= 0)
    {
        printf("Meaning of '%s': %s\n", node->key, node->value);
        count++;
    }
    if (strcmp(node->key, high) < 0)
    {
        count += RB_searchRange(node->right, low, high);
    }
    return count;
}

// 删除修复函数
void deleteFixup(RedBlackTree *tree, Node *node, Node *parent)
{
    Node *other;

    while ((!node || node->color == BLACK) && node != tree->root)
    {
        if (parent->left == node)
        {
            other = parent->right;
            if (other->color == RED)
            {
                // Case 1: x的兄弟w是红色的
                other->color = BLACK;
                parent->color = RED;
                leftRotate(tree, parent);
                other = parent->right;
            }
            if ((!other->left || (other->left->color == BLACK)) &&
                (!other->right || (other->right->color == BLACK)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                other->color = RED;
                node = parent;
                parent = node->parent;
            }
            else
            {
                if (!other->right || (other->right->color == BLACK))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    other->left->color = BLACK;
                    other->color = RED;
                    leftRotate(tree, other);
                    other = parent->right;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                other->color = parent->color;
                parent->color = BLACK;
                other->right->color = BLACK;
                leftRotate(tree, parent);
                node = tree->root;
                break;
            }
        }
        else
        {
            other = parent->left;
            if (other->color == RED)
            {
                // Case 1: x的兄弟w是红色的
                other->color = BLACK;
                parent->color = RED;
                rightRotate(tree, parent);
                other = parent->left;
            }
            if ((!other->left || (other->left->color == BLACK)) &&
                (!other->right || (other->right->color == BLACK)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
                other->color = RED;
                node = parent;
                parent = node->parent;
            }
            else
            {
                if (!other->left || (other->left->color == BLACK))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
                    other->right->color = BLACK;
                    other->color = RED;
                    leftRotate(tree, other);
                    other = parent->left;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                other->color = parent->color;
                parent->color = BLACK;
                other->left->color = BLACK;
                rightRotate(tree, parent);
                node = tree->root;
                break;
            }
        }
    }
    if (node)
        node->color = BLACK;
}

// 红黑树删除节点
void RB_delete(RedBlackTree *tree, const char *key)
{
    // 直接搜索指定键的节点
    Node *node = RB_search(tree, key);
    if (node == NULL)
    {
        printf("Word '%s' not found.\n", key);
        return;
    }

    Node *child, *parent;
    int color;

    // 当被删除的节点有两个子节点时
    if ((node->left != NULL) && (node->right != NULL))
    {
        Node *newNode = node;

        // 获取后继节点
        newNode = newNode->right;
        while (newNode->left != NULL)
            newNode = newNode->left;

        // 更新父节点的指针
        if (node->parent)
        {
            if (node->parent->left == node)
                node->parent->left = newNode;
            else
                node->parent->right = newNode;
        }
        else
            tree->root = newNode;

        // 准备替换节点
        child = newNode->right;
        parent = newNode->parent;
        color = newNode->color;
        if (parent == node)
        {
            parent = newNode;
        }
        else
        {
            // child不为空
            if (child)
                child->parent = parent;
            parent->left = child;

            newNode->right = node->right;
            node->right->parent = newNode;
        }

        // 更新新节点的父节点和颜色信息
        newNode->parent = node->parent;
        newNode->color = node->color;
        newNode->left = node->left;
        node->left->parent = newNode;

        // 如果被替换的节点是黑色，可能需要进行修复
        if (color == BLACK)
            deleteFixup(tree, child, parent);
        free(node->key);
        free(node->value);
        free(node);

        return;
    }

    // 当被删除的节点有一个或没有子节点时
    if (node->left != NULL)
        child = node->left;
    else
        child = node->right;
    parent = node->parent;
    color = node->color; // 保存颜色
    if (child)
        child->parent = parent;

    // 更新父节点或根节点的指针
    if (parent)
    {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    }
    else
        tree->root = child;

    // 如果被删除的节点是黑色，需要进行修复
    if (color == BLACK)
        deleteFixup(tree, child, parent);
    free(node->key);
    free(node->value);
    free(node);
}

// 打印红黑树（先序遍历）并写入文件
void RB_preorderPrintToFile(RedBlackTree *tree, Node *node, int level, int child, FILE *file)
{
    if (file == NULL)
    {
        return;
    }
    fprintf(file, "level=%d child=%d ", level, child);
    if (node == NULL)
    {
        fprintf(file, "null\n");
        return;
    }
    if (node->color == RED)
    {
        fprintf(file, "%s(RED)\n", node->key);
    }
    else
    {
        fprintf(file, "%s(BLACK)\n", node->key);
    }
    RB_preorderPrintToFile(tree, node->left, level + 1, 0, file);
    RB_preorderPrintToFile(tree, node->right, level + 1, 1, file);
}

// 递归释放红黑树节点资源
void free_tree_rb(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    free_tree_rb(node->left);
    free_tree_rb(node->right);
    free(node->key);
    free(node->value);
    free(node);
}

// 释放红黑树资源
void RB_free_tree(RedBlackTree *tree)
{
    if (tree == NULL)
    {
        return;
    }
    free_tree_rb(tree->root);
    free(tree);
}
