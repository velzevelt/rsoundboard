#pragma once
#include <raylib.h>

char *TextInputBox(Rectangle bounds, const char *placeholder, char *text, int maxTextSize, int *letterCountPtr);
