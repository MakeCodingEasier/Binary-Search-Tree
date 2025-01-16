#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"
#include <windows.h>//在windows中使用

// 将树打印至文件
void print_tree_to_file(RedBlackTree *rb_tree, BTree *b_tree, int tree_type)
{
    FILE *file = NULL;
    if (tree_type == 0)
    {
        file = fopen("bt.txt", "w");
        if (file != NULL)
        {
            B_preorderPrintToFile(b_tree->root, 0, 0, file);
            printf("It has been printed to bt.txt\n");
            fclose(file);
        }
        else
        {
            fprintf(stderr, "Failed to open bt.txt for writing\n");
        }
    }
    else
    {
        file = fopen("rbt.txt", "w");
        if (file != NULL)
        {
            RB_preorderPrintToFile(rb_tree, rb_tree->root, 0, 0, file);
            printf("It has been printed to rbt.txt\n");
            fclose(file);
        }
        else
        {
            fprintf(stderr, "Failed to open rbt.txt for writing\n");
        }
    }
}

// 向树中插入、删除数据以及计时
void process_file(const char *filepath, int tree_type, RedBlackTree *rb_tree, BTree *b_tree)
{
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        fprintf(stderr, "Could not open file %s\n", filepath);
        return;
    }
    char word[100];
    char meaning[100];
    int count = 0;
    double total_time = 0.0;
    LARGE_INTEGER frequency; // 用于计时（微秒）,windows系统
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER begin_time, end_time;
    QueryPerformanceCounter(&begin_time);

    char firstLine[10];
    fscanf(file, "%s", firstLine); // 操作类型insert or delete
    int operation = (strcmp(firstLine, "INSERT") == 0) ? 0 : 1;
    if (operation == 0)
    {
        while (fscanf(file, "%s %s", word, meaning) != EOF)
        {
            // Insert
            if (tree_type == 0)
                B_insert(b_tree, word, meaning);
            else
                RB_insert(rb_tree, word, meaning);
            count++;
            if (count % 100 == 0)
            {
                QueryPerformanceCounter(&end_time);
                printf("%s\n", tree_type == 0 ? "B Tree" : "RedBlack Tree");
                printf("Processed 100 entries, time used: %lf us\n", ((double)(end_time.QuadPart - begin_time.QuadPart) / (double)frequency.QuadPart) * 1000000.0);
                total_time += ((double)(end_time.QuadPart - begin_time.QuadPart) / (double)frequency.QuadPart) * 1000000.0;
                QueryPerformanceCounter(&begin_time); // 重新计时
            }
        }
        fclose(file);
    }
    else
    {
        count = 0;
        while (fscanf(file, "%s", word) != EOF)
        { // Delete
            if (tree_type == 0)
                delete_key(b_tree, word);
            else
                RB_delete(rb_tree, word);
            count++;
            if (count % 100 == 0)
            {
                QueryPerformanceCounter(&end_time);
                printf("%s\n", tree_type == 0 ? "B Tree" : "RedBlack Tree");
                printf("Processed 100 entries, time used: %lf us\n", ((double)(end_time.QuadPart - begin_time.QuadPart) / (double)frequency.QuadPart) * 1000000.0);
                total_time += ((double)(end_time.QuadPart - begin_time.QuadPart) / (double)frequency.QuadPart) * 1000000.0;
                QueryPerformanceCounter(&begin_time); // 重新计时
            }
        }
        fclose(file);
    }
    printf("Total time: %lf us\n", total_time);
    print_tree_to_file(rb_tree, b_tree, tree_type);
}

// 查询单词
void query_words(RedBlackTree *rb_tree, BTree *b_tree, int tree_type, char *words[], int num_words)
{
    if (tree_type == 0)
        printf("B Tree \n");
    else
        printf("RedBlackTree \n");
    if (num_words == 1) // 仅查询一个单词
    {
        char *query = words[0];
        if (tree_type == 0)
        {
            char *meaning = B_search(b_tree->root, query);
            if (meaning)
                printf("Meaning of '%s' : %s \n", query, meaning);
            else
                printf("'%s' not found.\n", query);
        }
        else
        {
            if (RB_search(rb_tree, query) == NULL)
                printf("'%s' not found in RedBlackTree.\n", query);
            else
                printf("Meaning of '%s' : %s \n", query, RB_search(rb_tree, query)->value);
        }
    }
    else // 查询word1、word2之间所有单词
    {
        char *word1 = words[0], *word2 = words[1];
        if (tree_type == 0)
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
}

int main()
{ // Initialize trees
    BTree *b_tree = malloc(sizeof(BTree));
    if (!b_tree)
    {
        fprintf(stderr, "Memory allocation failed for BTree\n");
        free(b_tree);
        return EXIT_FAILURE;
    }
    b_tree->root = NULL;

    RedBlackTree *rb_tree = newRedBlackTree();
    if (!rb_tree)
    {
        fprintf(stderr, "Memory allocation failed for RedBlackTree\n");
        free(rb_tree);
        return EXIT_FAILURE;
    }
    // Step 1: Insert into trees from 1_initial.txt
    printf("Step 1: Insert initial data\n");
    process_file("1_initial.txt", 0, rb_tree, b_tree); // B-tree
    process_file("1_initial.txt", 1, rb_tree, b_tree); // Red-Black tree

    // Step 2: Delete from trees based on 2_delete.txt
    printf("Step 2: Delete data\n");
    process_file("2_delete.txt", 0, rb_tree, b_tree); // B-tree
    process_file("2_delete.txt", 1, rb_tree, b_tree); // Red-Black tree

    // Step 3: Add data from 3_insert.txt
    printf("Step 3: Add data\n");
    process_file("3_insert.txt", 0, rb_tree, b_tree); // B-tree
    process_file("3_insert.txt", 1, rb_tree, b_tree); // Red-Black tree

    // Step 4: Query a word
    char *single_query[] = {"android"}; // Replace with an actual word to query
    printf("Step 4: Query a word\n");
    query_words(rb_tree, b_tree, 0, single_query, 1); // B-tree
    query_words(rb_tree, b_tree, 1, single_query, 1); // Red-Black tree

    // Step 5: Query some words
    char *multi_query[] = {"and", "android"}; // search words between word[0] and word[1]
    printf("Step 5: Query some words\n");
    query_words(rb_tree, b_tree, 0, multi_query, 2); // B-tree
    query_words(rb_tree, b_tree, 1, multi_query, 2); // Red-Black tree

    // 显式释放内存
    if (rb_tree)
        RB_free_tree(rb_tree);
    if (b_tree)
        B_free_tree(b_tree);

    return 0;
}
