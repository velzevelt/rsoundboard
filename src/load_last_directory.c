#include <raylib.h>

// Get platform-specific application data path
const char* GetAppDataPath(void)
{
    return GetWorkingDirectory(); // Or use platform-specific logic
}

void SaveLastDirectory(const char* directory)
{
    const char* configPath = TextFormat("%s/app_config.ini", GetAppDataPath());
    SaveFileText(configPath, (char*)directory);
}

char* LoadLastDirectory(void)
{
    const char* configPath = TextFormat("%s/app_config.ini", GetAppDataPath());
    
    if (FileExists(configPath))
    {
        char* directory = LoadFileText(configPath);
        if (directory != NULL && DirectoryExists(directory))
        {
            return directory;
        }
        MemFree(directory);
    }
    return NULL;
}