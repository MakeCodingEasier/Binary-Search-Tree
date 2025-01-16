#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "B_Tree.h"
#include <windows.h>

#define BUFFER_SIZE 8192 // 增大缓冲区大小

// 读取超大文件b树使用磁盘存取大数据集
int main()
{
    // B - tree
    BTree *b_tree = malloc(sizeof(BTree));
    if (b_tree == NULL)
    {
        perror("Memory allocation failed for BTree");
        return 1;
    }
    b_tree->root = NULL;
    // 读取文件，并插入B树
    const char *path = "D:\\dictionary_big.txt"; // 路径可根据存储路径修改，注意路径为\\

    // Create file mapping
    HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Could not open file %s\n", path);
        free(b_tree);
        return 0;
    }

    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapping == NULL)
    {
        printf("Could not create file mapping.\n");
        CloseHandle(hFile);
        free(b_tree);
        return 0;
    }

    LPVOID pMap = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (pMap == NULL)
    {
        printf("Could not map view of file.\n");
        CloseHandle(hMapping);
        CloseHandle(hFile);
        free(b_tree);
        return 0;
    }

    // 逐行处理映射的内存
    char *ptr = (char *)pMap;
    char *line_end;
    char word[100], meaning[200];

    while ((line_end = strchr(ptr, '\n')) != NULL)
    {
        *line_end = '\0'; // 替换换行符为字符串结束符
        char *separator = strchr(ptr, ' ');
        if (separator != NULL)
        {
            *separator = '\0'; // 分别处理键和值
            strcpy(word, ptr);
            strcpy(meaning, separator + 1);
            B_insert(b_tree, word, meaning);
        }
        ptr = line_end + 1; // 移动到下一行
    }

    // 释放资源
    UnmapViewOfFile(pMap);
    CloseHandle(hMapping);
    CloseHandle(hFile);

    B_free_tree(b_tree);
    return 0;
}