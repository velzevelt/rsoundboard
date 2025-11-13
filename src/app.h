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

Rectangle RectToScreen(Rectangle rect, int scrWidth, int scrHeight);
Rectangle RectPadding(Rectangle rect, float pTop, float pBottom, float pRight, float pLeft);
int RectCalcFontSize(Rectangle target, Font font, const char *text, float spacing);

float GetAbsoluteScreenScaleX();
float GetAbsoluteScreenScaleY();
float GetScreenScale();