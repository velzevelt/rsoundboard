#pragma once
#include <raylib.h>

Rectangle RectToScreen(Rectangle rect, int scrWidth, int scrHeight);
Rectangle RectPadding(Rectangle rect, float pTop, float pBottom, float pRight, float pLeft);
Rectangle RectPaddingAll(Rectangle rect, float padding);
int RectCalcFontSize(Rectangle target, Font font, const char *text, float spacing);

typedef struct
{
    Rectangle *items;
    size_t count;
    size_t capacity;
} RectangleList;

bool IsRectListValid(RectangleList list);
void AddRectListItem(RectangleList list, Rectangle item);
void AddRectListItems(RectangleList list, Rectangle *items, size_t itemsSize);
void UnloadRectList(RectangleList list);
void ClearRectList(RectangleList list);