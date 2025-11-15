#pragma once
#include <raylib.h>

Rectangle RectToScreen(Rectangle rect, int scrWidth, int scrHeight);
Rectangle RectPadding(Rectangle rect, float pTop, float pBottom, float pRight, float pLeft);
Rectangle RectPaddingAll(Rectangle rect, float padding);
int RectCalcFontSize(Rectangle target, Font font, const char *text, float spacing);
