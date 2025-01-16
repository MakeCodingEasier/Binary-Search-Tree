#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"
#define BUFFER_SIZE 8192  // 数据块大小


// 操作指南
void print_help()
{
    printf("Chinese-English Dictionary\n");
    printf("------------------------------------------------------------------\n");
    printf("i <document>        initialize with document\n"
           "d <document>        delete words with document\n"
           "n <document>        insert new words with document\n"
           "R                   use Red-Black Tree\n"
           "B                   use B Tree\n"
           "s <word1> <word2>   search words from word1 to word2\n"
           "s <word>            search a word and its meaning\n"
           "I <word> <meaning>  insert a single word\n"
           "D <word>            delete a single word\n"
           "pr                  print RedBlackTree to rbt.txt\n"
           "pb                  print B-Tree to bt.txt\n"
           "q                   quit\n"
           "h                   print help\n");
    printf("------------------------------------------------------------------\n");
}

// 前序打印树
void print_tree_to_file(RedBlackTree *rb_tree, BTree *b_tree, int tree_type)
{
    FILE *file;
    if (tree_type == 0)
    { // B-tree
        file = fopen("bt.txt", "w");
        if (file != NULL)
        {
            B_preorderPrintToFile(b_tree->root, 0, 0, file);
            printf("It has been printed to bt.txt\n");
            fclose(file);
        }
    }
    else
    { // Red-Black tree
        file = fopen("rbt.txt", "w");
        if (file != NULL)
        {
            RB_preorderPrintToFile(rb_tree, rb_tree->root, 0, 0, file);
            printf("It has been printed to rbt.txt\n");
            fclose(file);
        }
    }
}

// 初始化树，将之前存储数据清零
void initialize_tree(const char *path, int tree_type, RedBlackTree **rb_tree, BTree **b_tree)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Could not open file %s\n", path);
        return;
    }
    if (tree_type == 0)
    { // B - tree
        *b_tree = malloc(sizeof(BTree));
        (*b_tree)->root = NULL; // 初始化，分配内存
    }
    else
    { // Red - Black tree
        *rb_tree = newRedBlackTree();
    }
    char line[300]; // 行缓冲区，用于存储每行的内容
    char word[100], meaning[200];
    // 逐行读取文件并插入词汇
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // 处理每一行
        char *separator = strchr(line, ' ');
        if (separator != NULL)
        {
            // 分隔出单词
            *separator = '\0';
            strcpy(word, line);
            // 取出意思部分，去掉换行符
            strcpy(meaning, separator + 1);
            meaning[strcspn(meaning, "\n")] = '\0'; // 去除换行符
        }
        // Insert
        if (tree_type == 0)
            B_insert(*b_tree, word, meaning);
        else
            RB_insert(*rb_tree, word, meaning);
    }
    fclose(file);
    printf("Dictionary has been initialized.\n");
}

// 插入单词
void insert_tree(const char *path, int tree_type, RedBlackTree **rb_tree, BTree **b_tree)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Could not open file %s\n", path);
        return;
    }
    char line[300]; // 行缓冲区，用于存储每行的内容
    char word[100], meaning[200];
    char operation[10];
    // 读取第一行，检查操作类型
    if (fgets(line, sizeof(line), file) != NULL)
    {
        sscanf(line, "%s", operation);
        if (strcmp(operation, "INSERT") == 0)
        {
            // 逐行读取文件并插入词汇
            while (fgets(line, sizeof(line), file) != NULL)
            {
                // 处理每一行
                char *separator = strchr(line, ' ');
                if (separator != NULL)
                {
                    // 分隔出单词
                    *separator = '\0';
                    strcpy(word, line);
                    // 取出意思部分，去掉换行符
                    strcpy(meaning, separator + 1);
                    meaning[strcspn(meaning, "\n")] = '\0'; // 去除换行符
                }
                // Insert
                if (tree_type == 0)
                    B_insert(*b_tree, word, meaning);
                else
                    RB_insert(*rb_tree, word, meaning);
            }
            fclose(file);
        }
        else
        {
            printf("Wrong Input!\n");
            fclose(file);
            return;
        }
    }
    else
    {
        printf("Failed to read operation line.\n");
        fclose(file);
        return;
    }
    printf("Dictionary has been updated.\n");
}

void delete_tree(const char *path, int tree_type, RedBlackTree **rb_tree, BTree **b_tree)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Could not open file %s\n", path);
        return;
    }
    char word[100];
    char firstLine[10];
    fscanf(file, "%s", firstLine);
    if (strcmp(firstLine, "DELETE") == 0)
    {
        while (fscanf(file, "%s", word) != EOF)
        { // Delete
            if (tree_type == 0)
                delete_key(*b_tree, word);
            else
                RB_delete(*rb_tree, word);
        }
        fclose(file);
        printf("Dictionary has been updated.\n");
        return;
    }
    else
    {
        printf("Wrong Input!\n");
        return;
    }
}

int main()
{
    print_help();
    printf("First you should choose which tree to implement: R or B\n");
    int run = 1;  // Running state
    int tree = 1; // 1 for RedBlackTree, 0 for BTree,default RedBlackTree
    // B-tree
    BTree *b_tree = NULL;
    b_tree = malloc(sizeof(BTree));
    b_tree->root = NULL;
    // Red-Black tree
    RedBlackTree *rb_tree = newRedBlackTree();
    while (run != 0)
    {
        char cmd[100];
        scanf(" %[^\n]", cmd); // Read the whole line command
        switch (cmd[0])
        {
        case 'i':
        {
            char path[100];
            sscanf(cmd, "i %s", path);
            initialize_tree(path, tree, &rb_tree, &b_tree);
            print_help();
            break;
        }
        case 'B':
        {
            tree = 0;
            printf("You are now using B tree.\n");
            break;
        }
        case 'R':
        {
            tree = 1;
            printf("You are now using Red-Black tree.\n");
            break;
        }
        case 's':
        {
            char word1[100], word2[100];
            // 范围搜索
            if (sscanf(cmd, "s %s %s", word1, word2) == 2)
            {
                if (tree == 0)
                {
                    // B-tree
                    int count1 = B_search_in_range(b_tree->root, word1, word2);
                    printf("%d words in range [%s, %s] found.\n", count1, word1, word2);
                }
                else
                {
                    // Red-Black tree
                    int count2 = RB_searchRange(rb_tree->root, word1, word2);
                    printf("%d words in range [%s, %s] found.\n", count2, word1, word2);
                }
            }
            else
            { // 单个搜索
                char word[100];
                sscanf(cmd, "s %s", word);
                if (tree == 0)
                {
                    char *meaning = B_search(b_tree->root, word);
                    if (meaning != NULL)
                        printf("Meaning of '%s': %s. \n", word, meaning);
                    else
                        printf("Word '%s' not found.\n", word);
                }
                else
                {
                    Node *result = RB_search(rb_tree, word);
                    if (result != NULL)
                        printf("Meaning of '%s': %s. \n", word, result->value);
                    else
                        printf("Word '%s' not found. \n", word);
                }
            }
            break;
        }
        case 'd':
        {
            char path[100];
            sscanf(cmd, "d %s", path);
            delete_tree(path, tree, &rb_tree, &b_tree);
            print_help();
            break;
        }
        case 'n':
        {
            char path[100];
            sscanf(cmd, "n %s", path);
            insert_tree(path, tree, &rb_tree, &b_tree);
            print_help();
            break;
        }
        case 'D':
        {
            char word[100];
            sscanf(cmd, "D %s", word);
            if (tree == 0)
            {
                if (B_search(b_tree->root, word) == NULL)
                    printf("'%s' not found.\n", word);
                else
                {
                    delete_key(b_tree, word);
                    printf("'%s' deleted.\n", word);
                }
            }
            else
            {
                if (RB_search(rb_tree, word) == NULL)
                    printf("'%s' not found.\n", word);
                else
                {
                    RB_delete(rb_tree, word);
                    printf("'%s' deleted.\n", word);
                }
            }
            break;
        }
        case 'I':
        {
            char word[100], meaning[100];
            sscanf(cmd, "I %s %s", word, meaning);
            if (tree == 0)
            {
                if (B_search(b_tree->root, word) != NULL)
                    printf("'%s' already exists.\n", word);
                else
                {
                    B_insert(b_tree, word, meaning);
                    printf("Inserted '%s' into B-tree.\n", word);
                }
            }
            else
            {
                if (RB_search(rb_tree, word) != NULL)
                    printf("'%s' already exists.\n", word);
                else
                {
                    RB_insert(rb_tree, word, meaning);
                    printf("Inserted '%s' into Red-Black tree.\n", word);
                }
            }
            break;
        }
        case 'p':
        {
            if (strcmp(cmd, "pr") == 0)
            {
                print_tree_to_file(rb_tree, b_tree, 1);
            }
            else if (strcmp(cmd, "pb") == 0)
            {
                print_tree_to_file(rb_tree, b_tree, 0);
            }
            else
                printf("Invalid command. Enter 'h' for help.\n");
            break;
        }
        case 'q':
        {
            printf("Quit !");
            run = 0;
            break;
        }
        case 'h':
        {
            print_help();
            break;
        }
        default:
        {
            printf("Invalid command. Enter 'h' for help.\n");
            break;
        }
        }
    }

    // 显式释放内存
    if (rb_tree)
        RB_free_tree(rb_tree);
    if (b_tree)
        B_free_tree(b_tree);

    return 0;
}
