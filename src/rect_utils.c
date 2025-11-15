#include "rect_utils.h"
#include <math.h>

Rectangle RectToScreen(Rectangle rect, int scrWidth, int scrHeight)
{
    rect.height *= scrHeight;
    rect.y *= scrHeight;
    rect.width *= scrWidth;
    rect.x *= scrWidth;
    return rect;
}

Rectangle RectPadding(Rectangle rect, float pTop, float pBottom, float pRight, float pLeft)
{
    // Move origin and adjust size for left padding
    rect.x += pLeft;
    rect.width -= pLeft;

    // Move origin and adjust size for top padding
    rect.y += pTop;
    rect.height -= pTop;

    // Adjust size for right and bottom padding
    rect.width -= pRight;
    rect.height -= pBottom;

    // Ensure the rectangle doesn't have negative dimensions
    if (rect.width < 0)
        rect.width = 0;
    if (rect.height < 0)
        rect.height = 0;

    return rect;
}

Rectangle RectPaddingAll(Rectangle rect, float padding)
{
    return RectPadding(rect, padding, padding, padding, padding);
}

// Calculate fontsize to fit in rect
int RectCalcFontSize(Rectangle target, Font font, const char *text, float spacing)
{
    Vector2 baseTextSize1 = MeasureTextEx(font, text, 1, spacing);
    Vector2 baseTextSize2 = MeasureTextEx(font, text, 2, spacing);

    // a_n = a_1 + (n - 1) * d
    // a_1 + (n - 1) * d > x
    // n > (x - a_1) / d + 1

    int nX = 1;
    int nY = 1;

    {
        float a_1 = baseTextSize1.x;
        float d = baseTextSize2.x - baseTextSize1.x;
        float x = target.width;

        float v = (x - a_1) / d + 1;
        nX = (int)ceilf(v) - 1;
    }

    {
        float a_1 = baseTextSize1.y;
        float d = baseTextSize2.y - baseTextSize1.y;
        float x = target.height;

        float v = (x - a_1) / d + 1;
        nY = (int)ceilf(v) - 1;
    }

    int n = nX > nY ? nY : nX;
    return n;
}



bool IsRectListValid(RectangleList list)
{
    return list.items != NULL && list.capacity > 0;
}

void AddRectListItem(RectangleList list, Rectangle item)
{
    if (!IsRectListValid(list))
    {
        return;
    }
    
    // Check if we need to resize
    if (list.count >= list.capacity)
    {
        // Double the capacity (or set to 1 if it was 0)
        size_t new_capacity = list.capacity == 0 ? 1 : list.capacity * 2;
        Rectangle *new_items = realloc(list.items, new_capacity * sizeof(Rectangle));
        
        if (new_items == NULL)
        {
            return; // Allocation failed
        }
        
        list.items = new_items;
        list.capacity = new_capacity;
    }
    
    // Add the new item
    list.items[list.count++] = item;
}

void AddRectListItems(RectangleList list, Rectangle *items, size_t itemsSize)
{
    if (!IsRectListValid(list) || items == NULL || itemsSize == 0)
    {
        return;
    }
    
    // Check if we have enough capacity
    if (list.count + itemsSize > list.capacity)
    {
        size_t new_capacity = list.count + itemsSize;
        Rectangle *new_items = realloc(list.items, new_capacity * sizeof(Rectangle));
        
        if (new_items == NULL)
        {
            return; // Allocation failed
        }
        
        list.items = new_items;
        list.capacity = new_capacity;
    }
    
    // Copy all items
    memcpy(list.items + list.count, items, itemsSize * sizeof(Rectangle));
    list.count += itemsSize;
}

void UnloadRectList(RectangleList list)
{
    if (list.items != NULL)
    {
        free(list.items);
        // Note: The list struct itself still exists, but its items are freed
        // The caller should set list.items to NULL and list.count/capacity to 0
    }
}

void ClearRectList(RectangleList list)
{
    if (IsRectListValid(list))
    {
        memset(list.items, 0, list.count);
        list.count = 0;
        // Memory remains allocated, capacity unchanged
    }
}