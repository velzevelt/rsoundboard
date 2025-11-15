#pragma once
#include <raylib.h>

void RunApplication();
void RunMainMenu();
void RunInvalidState();
void RunSoundboardSelection();
void RunSettings();
void RunSoundboardLoading();
void RunSoundboardStatus();
void RunToSearch();
void RunSearch();
void RunFromSearch();

void CalculateSquareTableLayout(size_t items, size_t* columns, size_t* rows);
const char *FindLongestFileName(FilePathList *list);

float GetAbsoluteScreenScaleX();
float GetAbsoluteScreenScaleY();
float GetScreenScale();