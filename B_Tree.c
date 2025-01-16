#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"

// 创建一个新的B树节点
BTreeNode *create_node(int is_leaf)
{
    BTreeNode *node = (BTreeNode *)malloc(sizeof(BTreeNode));
    node->num = 0;
    node->keys = (char **)malloc((2 * MIN_DEGREE - 1) * sizeof(char *));
    node->values = (char **)malloc((2 * MIN_DEGREE - 1) * sizeof(char *));
    for (int i = 0; i < 2 * MIN_DEGREE - 1; i++)
    {
        node->keys[i] = NULL;
        node->values[i] = NULL;
    }
  //  if (!is_leaf)
    {
        node->children = (struct BTreeNode **)malloc(2 * MIN_DEGREE * sizeof(struct BTreeNode *));
        for (int i = 0; i < 2 * MIN_DEGREE; i++)
        {
            node->children[i] = NULL;
        }
    }
    node->is_leaf = is_leaf;
    return node;
}

// 分裂节点
void split_child(BTreeNode *parent, int index, BTreeNode *child)
{
    BTreeNode *new_child = create_node(child->is_leaf);
    new_child->num = MIN_DEGREE - 1;

    // 复制数据到新节点
    for (int j = 0; j < MIN_DEGREE - 1; j++)
    {
        new_child->keys[j] = child->keys[j + MIN_DEGREE];
        new_child->values[j] = child->values[j + MIN_DEGREE];
    }

    if (!child->is_leaf)
    {
        for (int j = 0; j < MIN_DEGREE; j++)
        {
            new_child->children[j] = child->children[j + MIN_DEGREE];
        }
    }

    child->num = MIN_DEGREE - 1;

    // 插入新子节点到父节点
    for (int j = parent->num; j >= index + 1; j--)
    {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[index + 1] = new_child;

    // 移动父节点的对应键
    for (int j = parent->num - 1; j >= index; j--)
    {
        parent->keys[j + 1] = parent->keys[j];
        parent->values[j + 1] = parent->values[j];
    }
    parent->keys[index] = strdup(child->keys[MIN_DEGREE - 1]);
    parent->values[index] = strdup(child->values[MIN_DEGREE - 1]);
    parent->num++;
}

// 插入未满节点
void insert_non_full(BTreeNode *node, const char *key, const char *value)
{
    int i = node->num - 1;

    if (node->is_leaf)
    {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0)
        {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        node->keys[i + 1] = strdup(key);
        node->values[i + 1] = strdup(value);
        node->num++;
    }
    else
    {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0)
        {
            i--;
        }
        i++;
        if (node->children[i]->num == 2 * MIN_DEGREE - 1)
        {
            split_child(node, i, node->children[i]);
            if (strcmp(key, node->keys[i]) > 0)
            {
                i++;
            }
        }
        insert_non_full(node->children[i], key, value);
    }
}

// 插入
void B_insert(BTree *tree, const char *key, const char *value)
{
    if (tree->root == NULL)
    {
        tree->root = create_node(1);
        tree->root->keys[0] = strdup(key);
        tree->root->values[0] = strdup(value);
        tree->root->num = 1;
    }
    else
    {
        if (tree->root->num == 2 * MIN_DEGREE - 1)
        {
            BTreeNode *new_root = create_node(0);
            new_root->children[0] = tree->root;
            split_child(new_root, 0, tree->root);
            tree->root = new_root;
            insert_non_full(new_root, key, value);
        }
        else
        {
            insert_non_full(tree->root, key, value);
        }
    }
}

// 从叶子节点中删除键和值
void remove_from_leaf(BTreeNode *node, int index)
{
    for (int i = index; i < node->num - 1; i++)
    {
        node->keys[i] = node->keys[i + 1];
        node->values[i] = node->values[i + 1];
    }
    node->keys[node->num - 1] = NULL;
    node->values[node->num - 1] = NULL;
    node->num--;
}

// 从非叶子节点中删除键
void remove_from_non_leaf(BTreeNode *node, int index)
{
    char *key = node->keys[index];
    if (node->children[index]->num >= MIN_DEGREE)
    {
        BTreeNode *prev = node->children[index];
        char *prev_key = prev->keys[prev->num - 1];
        node->keys[index] = prev_key;
        remove_key(prev, prev_key);
    }
    else if (node->children[index + 1]->num >= MIN_DEGREE)
    {
        BTreeNode *next = node->children[index + 1];
        char *next_key = next->keys[0];
        node->keys[index] = next_key;
        remove_key(next, next_key);
    }
    else
    {
        merge(node, index);
        remove_key(node->children[index], key);
    }
}

// 从当前节点借一个键和值（从前一个兄弟节点）
void borrow_from_prev(BTreeNode *node, int index)
{
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index - 1];

    for (int i = child->num - 1; i >= 0; i--)
    {
        child->keys[i + 1] = child->keys[i];
        child->values[i + 1] = child->values[i];
    }
   if (!child->is_leaf) {
        for (int i = child->num; i >= 0; i--) {
            child->children[i + 1] = child->children[i];
        }
        child->children[0] = sibling->children[sibling->num];
    }
    child->keys[0] = node->keys[index - 1];
    child->values[0] = node->values[index - 1];
    node->keys[index - 1] = sibling->keys[sibling->num - 1];
    node->values[index - 1] = sibling->values[sibling->num - 1];
    child->num++;
    sibling->num--;
}

// 从当前节点借一个键和值（从后一个兄弟节点）
void borrow_from_next(BTreeNode *node, int index)
{
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index + 1];

    child->keys[child->num] = node->keys[index];
    child->values[child->num] = node->values[index];
    if (!child->is_leaf)
    {
        child->children[child->num + 1] = sibling->children[0];
    }
    node->keys[index] = sibling->keys[0];
    node->values[index] = sibling->values[0];
    for (int i = 0; i < sibling->num - 1; i++)
    {
        sibling->keys[i] = sibling->keys[i + 1];
        sibling->values[i] = sibling->values[i + 1];
    }
    if (!sibling->is_leaf)
    {
        for (int i = 0; i < sibling->num; i++)
        {
            sibling->children[i] = sibling->children[i + 1];
        }
    }
    child->num++;
    sibling->num--;
}

// 合并两个子节点
void merge(BTreeNode *node, int index)
{
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index + 1];

    child->keys[MIN_DEGREE - 1] = node->keys[index];
    child->values[MIN_DEGREE - 1] = node->values[index];

    for (int i = 0; i < sibling->num; i++)
    {
        child->keys[i + MIN_DEGREE] = sibling->keys[i];
        child->values[i + MIN_DEGREE] = sibling->values[i];
    }
    if (!child->is_leaf)
    {
        for (int i = 0; i <= sibling->num; i++)
        {
            child->children[i + MIN_DEGREE] = sibling->children[i];
        }
    }
    for (int i = index; i < node->num - 1; i++)
    {
        node->keys[i] = node->keys[i + 1];
        node->values[i] = node->values[i + 1];
    }
    for (int i = index + 1; i < node->num; i++)
    {
        node->children[i] = node->children[i + 1];
    }
    child->num += sibling->num + 1;
    node->num--;
    free(sibling->keys);
    free(sibling->values);
    free(sibling->children);
    free(sibling);
}

// 在B树中删除键
void delete_key(BTree *tree, const char *key)
{
    if (tree->root == NULL)
    {
        return;
    }
    remove_key(tree->root, key);
    if (tree->root->num == 0)
    {
        BTreeNode *old_root = tree->root;
        if (tree->root->is_leaf)
        {
            tree->root = NULL;
        }
        else
        {
            tree->root = tree->root->children[0];
        }
        free(old_root->keys);
        free(old_root->values);
        free(old_root->children);
        free(old_root);
    }
}

// 从节点中删除指定键
void remove_key(BTreeNode *node, const char *key)
{
    int i = 0;
    while (i < node->num && strcmp(node->keys[i], key) < 0)
    {
        i++;
    }
    if (i < node->num && strcmp(node->keys[i], key) == 0)
    {
        if (node->is_leaf)
        {
            remove_from_leaf(node, i);
        }
        else
        {
            remove_from_non_leaf(node, i);
        }
    }
    else
    {
        if (node->is_leaf)
        {
            return;
        }
        int flag = (i == node->num) ? 1 : 0;
        BTreeNode *child = node->children[i];
        if (child->num < MIN_DEGREE)
        {
            if (i != 0 && node->children[i - 1]->num >= MIN_DEGREE)
            {
                borrow_from_prev(node, i);
            }
            else if (i != node->num && node->children[i + 1]->num >= MIN_DEGREE)
            {
                borrow_from_next(node, i);
            }
            else
            {
                if (flag ==1)
                {
                    merge(node, i - 1);
                    child = node->children[i - 1];
                }
                else
                {
                    merge(node, i);
                }
            }
        }
        remove_key(child, key);
    }
}

// 前序遍历打印并写入文件
void B_preorderPrintToFile(BTreeNode *node, int level, int child_num, FILE *file)
{
    if (node == NULL || file == NULL)
        return;

    fprintf(file, "level=%d child=%d ", level, child_num);

    for (int i = 0; i < node->num; i++)
    {
        fprintf(file, "/%s", node->keys[i]); // 输出为单词
    }
    fprintf(file, "/\n");

    if (!node->is_leaf)
    {
        for (int i = 0; i <= node->num; i++)
        {
            B_preorderPrintToFile(node->children[i], level + 1, i, file);
        }
    }
}

// 查找功能
char *B_search(BTreeNode *node, const char *key)
{
    if (!node || !key)
    {
        return NULL; // 确保输入有效
    }
    int i = 0;
    while (i < node->num && strcmp(key, node->keys[i]) > 0)
        i++;

    if (i < node->num && strcmp(key, node->keys[i]) == 0)
    {
        return node->values[i]; // 找到了
    }

    if (node->is_leaf)
    {
        return NULL; // 关键字不在树中
    }

    return B_search(node->children[i], key); // 在子树中查找
}

// 范围搜索函数
int B_search_in_range(BTreeNode *node, const char *start, const char *end)
{
    int count = 0;
    if (node == NULL)
        return count;
    for (int i = 0; i < node->num; i++)
    {
        if (strcmp(node->keys[i], start) >= 0 && strcmp(node->keys[i], end) <= 0)
        {
            count++;
            // 打印在范围内的单词和对应意义
            printf("Meaning of '%s': %s\n", node->keys[i], node->values[i]);
        }
    } // 如果不是叶节点，就递归处理叶节点
    if (!node->is_leaf)
    {
        for (int i = 0; i <= node->num; i++)
        {
            // 确定要递归哪个子节点
            if (i == node->num || strcmp(node->keys[i], start) > 0)
            {
                count += B_search_in_range(node->children[i], start, end);
                if (i < node->num && strcmp(node->keys[i], end) > 0)
                {
                    break; // 到达end的范围，停止进一步搜索
                }
            }
        }
    }
    return count;
}

// 释放B树节点的内存
void free_tree_b(BTreeNode *node)
{
    if (node == NULL)
        return;
    // 递归释放子节点
    for (int i = 0; i <= node->num; i++)
    {
        if (!node->is_leaf && node->children[i] != NULL)
        {
            free_tree_b(node->children[i]);
        }
    }
    // 释放键和值数组
    for (int i = 0; i < node->num; i++)
    {
        free(node->keys[i]);
        free(node->values[i]);
    }
    // 释放键和值数组本身
    free(node->keys);
    free(node->values);
    // 释放子指针数组
   // if (!node->is_leaf)
    {
        free(node->children);
    }
    // 释放节点本身
    free(node);
}

// 释放整个B树内存
void B_free_tree(BTree *tree)
{
    if (tree == NULL)
        return;
    free_tree_b(tree->root); // 释放根节点及其所有子节点
    free(tree);              // 释放树结构本身
}