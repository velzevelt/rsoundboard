#pragma once
#include <raylib.h>

typedef struct {
    FilePathList *originalList;  // Reference to original list
    char **filteredPaths;        // Pointers to filtered paths in original list
    int count;                   // Number of filtered items
    int capacity;                // Maximum capacity for filtered results
} FilteredFilePathList;

FilteredFilePathList SearchInFileListNoAlloc(FilePathList *list, const char *search, int limit);