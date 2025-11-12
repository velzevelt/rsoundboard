#pragma once

const char* GetAppDataPath(void);
void SaveLastDirectory(const char* directory);
char* LoadLastDirectory(void);