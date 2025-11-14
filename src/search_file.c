#include <raylib.h>
#include <string.h>

typedef struct {
    FilePathList *originalList;  // Reference to original list
    char **filteredPaths;        // Pointers to filtered paths in original list
    int count;                   // Number of filtered items
    int capacity;                // Maximum capacity for filtered results
} FilteredFilePathList;

FilteredFilePathList SearchInFileListNoAlloc(FilePathList *list, const char *search, int limit) {
    FilteredFilePathList result = {0};
    
    if (list == NULL || list->paths == NULL || search == NULL || list->count == 0) {
        return result;
    }
    
    // Use a static array to avoid allocation (adjust size as needed)
    #define MAX_FILTERED_RESULTS 256
    static char *filteredPaths[MAX_FILTERED_RESULTS];
    
    result.originalList = list;
    result.filteredPaths = filteredPaths;
    result.capacity = MAX_FILTERED_RESULTS;
    
    int matchCount = 0;
    const char *lowerSearch = TextToLower(search);
    
    // First pass: exact matches at beginning (higher priority)
    for (int i = 0; i < list->count && matchCount < result.capacity && matchCount < limit; i++) {
        const char *filename = TextToLower(GetFileName(list->paths[i]));
        const char *found = strstr(filename, lowerSearch);
        
        if (found != NULL && found == filename) { // Match at beginning
            result.filteredPaths[matchCount++] = list->paths[i];
        }
    }
    
    // Second pass: other matches
    for (int i = 0; i < list->count && matchCount < result.capacity && matchCount < limit; i++) {
        const char *filename = TextToLower(GetFileName(list->paths[i]));
        const char *found = strstr(filename, lowerSearch);
        
        if (found != NULL && found != filename) { // Match not at beginning
            // Check if not already added in first pass
            bool alreadyAdded = false;
            for (int j = 0; j < matchCount; j++) {
                if (result.filteredPaths[j] == list->paths[i]) {
                    alreadyAdded = true;
                    break;
                }
            }
            
            if (!alreadyAdded) {
                result.filteredPaths[matchCount++] = list->paths[i];
            }
        }
    }
    
    result.count = matchCount;
    return result;
}

// // Alternative version if you want to modify the function signature to return FilePathList
// FilePathList SearchInFileListSimple(FilePathList *list, const char *search, int limit, char **preAllocatedBuffer, int bufferSize) {
//     FilePathList result = {0};
    
//     if (list == NULL || list->paths == NULL || search == NULL || list->count == 0 || preAllocatedBuffer == NULL) {
//         return result;
//     }
    
//     result.paths = preAllocatedBuffer;
//     result.capacity = bufferSize;
    
//     const char *lowerSearch = TextToLower(search);
//     int matchCount = 0;
    
//     // First pass: exact matches at beginning
//     for (int i = 0; i < list->count && matchCount < bufferSize && (limit <= 0 || matchCount < limit); i++) {
//         const char *filename = TextToLower(GetFileName(list->paths[i]));
//         const char *found = strstr(filename, lowerSearch);
        
//         if (found != NULL && found == filename) {
//             result.paths[matchCount++] = (char*)list->paths[i];
//         }
//     }
    
//     // Second pass: other matches
//     for (int i = 0; i < list->count && matchCount < bufferSize && (limit <= 0 || matchCount < limit); i++) {
//         const char *filename = TextToLower(GetFileName(list->paths[i]));
//         const char *found = strstr(filename, lowerSearch);
        
//         if (found != NULL && found != filename) {
//             // Check if not already added
//             bool alreadyAdded = false;
//             for (int j = 0; j < matchCount; j++) {
//                 if (result.paths[j] == list->paths[i]) {
//                     alreadyAdded = true;
//                     break;
//                 }
//             }
            
//             if (!alreadyAdded) {
//                 result.paths[matchCount++] = (char*)list->paths[i];
//             }
//         }
//     }
    
//     result.count = matchCount;
//     return result;
// }

// // Simplest version using static buffer internally
// FilePathList SearchInFileListStatic(FilePathList *list, const char *search, int limit) {
//     FilePathList result = {0};
    
//     if (list == NULL || list->paths == NULL || search == NULL || list->count == 0) {
//         return result;
//     }
    
//     static char *filteredPaths[256];  // Static buffer for results
//     result.paths = filteredPaths;
//     result.capacity = 256;
    
//     const char *lowerSearch = TextToLower(search);
//     int matchCount = 0;
    
//     // First pass: matches at beginning of filename
//     for (int i = 0; i < list->count && matchCount < result.capacity && (limit <= 0 || matchCount < limit); i++) {
//         const char *filename = TextToLower(GetFileName(list->paths[i]));
//         const char *found = strstr(filename, lowerSearch);
        
//         if (found != NULL && found == filename) {
//             result.paths[matchCount++] = (char*)list->paths[i];
//         }
//     }
    
//     // Second pass: matches anywhere in filename
//     for (int i = 0; i < list->count && matchCount < result.capacity && (limit <= 0 || matchCount < limit); i++) {
//         const char *filename = TextToLower(GetFileName(list->paths[i]));
//         const char *found = strstr(filename, lowerSearch);
        
//         if (found != NULL && found != filename) {
//             // Check if not already added
//             bool alreadyAdded = false;
//             for (int j = 0; j < matchCount; j++) {
//                 if (result.paths[j] == list->paths[i]) {
//                     alreadyAdded = true;
//                     break;
//                 }
//             }
            
//             if (!alreadyAdded) {
//                 result.paths[matchCount++] = (char*)list->paths[i];
//             }
//         }
//     }
    
//     result.count = matchCount;
//     return result;
// }