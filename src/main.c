#include <raylib.h>
#include <keyboard_hook.h>
#include <raygui.h>
#include <nfd.h>
#include <assert.h>
#include <load_last_directory.h>
#include <miniaudio.h>
#include "app.h"
#include <math.h>

typedef enum
{
    NONE,
    MAIN_MENU,
    SOUNDBOARD,
    SETTINGS,
} APP_STATE;

static APP_STATE appState;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

int main(void)
{
    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT | FLAG_VSYNC_HINT);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rsoundboard");

    InitAudioDevice();
    SetExitKey(0);
    InstallKeyboardHook();
    appState = MAIN_MENU;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        BeginKeyboardHook();
        {
            RunApplication();
        }
        EndDrawing();
    }

    RemoveKeyboardHook();
    CloseWindow();
    CloseAudioDevice();
    return 0;
}

void RunApplication()
{
    if (appState == MAIN_MENU)
    {
        RunMainMenu();
    }
    else if (appState == SOUNDBOARD)
    {
        RunSoundboard();
    }
    else if (appState == SETTINGS)
    {
        RunSettings();
    }
    else
    {
        RunInvalidState();
    }
}

void RunMainMenu()
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    Rectangle menuArea = {0, 0, 0.8f, 0.8f};
    menuArea.x = 0.5f - menuArea.width * 0.5f;
    menuArea.y = 0.5f - menuArea.height * 0.5f;

    Rectangle vertButton1 = menuArea;
    vertButton1.width *= 0.5f;
    
    Rectangle vertButton2 = vertButton1;
    vertButton2.x += vertButton1.width;

    vertButton1 = RectPadding(vertButton1, 0.01f, 0.01f, 0.01f, 0.01f);
    Rectangle vertButton1Text = RectPadding(vertButton1, 0.03f, 0.03f, 0.03f, 0.03f);

    vertButton2 = RectPadding(vertButton2, 0.01f, 0.01f, 0.01f, 0.01f);
    Rectangle vertButton2Text = RectPadding(vertButton2, 0.03f, 0.03f, 0.03f, 0.03f);

    ClearBackground(BLACK);

    menuArea = RectToScreen(menuArea, w, h);
    vertButton1 = RectToScreen(vertButton1, w, h);
    vertButton2 = RectToScreen(vertButton2, w, h);
    vertButton1Text = RectToScreen(vertButton1Text, w, h);
    vertButton2Text = RectToScreen(vertButton2Text, w, h);

    int fontSize = RectCalcFontSize(vertButton1Text, GetFontDefault(), "Select Soundboard", 0);
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    if (GuiButton(vertButton1, "Select Soundboard"))
    {
        appState = SOUNDBOARD;
    }

    fontSize = RectCalcFontSize(vertButton2Text, GetFontDefault(), "Settings", 0);
    if (GuiButton(vertButton2, "Settings"))
    {
        appState = SETTINGS;
    }

    DrawRectangleLinesEx(menuArea, 1.0f, RED);
    DrawRectangleLinesEx(vertButton1, 1.0f, YELLOW);
    DrawRectangleLinesEx(vertButton2, 1.0f, ORANGE);
    DrawRectangleLinesEx(vertButton1Text, 1.0f, ORANGE);
    DrawRectangleLinesEx(vertButton2Text, 1.0f, ORANGE);
}

void RunInvalidState()
{
    ClearBackground(BLACK);
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    int textW = MeasureText("INVALID APP STATE", 32);
    DrawText("INVALID APP STATE", w * 0.5f - textW / 2, h * 0.5f, 32, RED);
}

void RunSoundboard()
{
    ClearBackground(BLACK);
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    const char *t = "SOUNDBOARD";
    int textW = MeasureText(t, 32);
    DrawText(t, w * 0.5f - textW / 2, h * 0.5f, 32, GREEN);

    static Sound testSound;
    if (!IsSoundValid(testSound))
    {
        testSound = LoadSound("freeman.wav");
    }

    if (IsKeyDown(KEY_SPACE) && IsSoundValid(testSound) && !IsSoundPlaying(testSound))
    {
        PlaySound(testSound);
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        appState = MAIN_MENU;
    }
}

void RunSettings()
{
    ClearBackground(BLACK);
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    Rectangle menuArea = {0, 0, 0.9f, 0.9f};
    menuArea.x = 0.5f - menuArea.width * 0.5f;
    menuArea.y = 0.5f - menuArea.height * 0.5f;

    Rectangle backButton = menuArea;
    backButton.width = menuArea.width * 0.3f;
    backButton.height = menuArea.height * 0.3f;
    backButton.x = menuArea.x + menuArea.width * 0.5f - backButton.width * 0.5f;
    backButton.y = menuArea.y + menuArea.height - backButton.height;

    Rectangle selectAudioDeviceButton = menuArea;
    selectAudioDeviceButton.height -= backButton.height;
    selectAudioDeviceButton = RectPadding(selectAudioDeviceButton, 0, 0.03f, 0, 0);

    Rectangle backButtonText = RectPadding(backButton, 0.03f, 0.03f, 0.03f, 0.03f);

    menuArea = RectToScreen(menuArea, w, h);
    backButton = RectToScreen(backButton, w, h);
    backButtonText = RectToScreen(backButtonText, w, h);
    selectAudioDeviceButton = RectToScreen(selectAudioDeviceButton, w, h);

    const char *backText = "Back";
    int fontSize = 36 * GetScreenScale();
    int spacing = 8 * GetScreenScale();

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(DEFAULT, TEXT_SPACING, spacing);
    if (GuiButton(backButtonText, backText))
    {
        appState = MAIN_MENU;
    }
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);

    fontSize = 22 * GetScreenScale();
    spacing = 6 * GetScreenScale();

    Vector2 tPos = {selectAudioDeviceButton.x, selectAudioDeviceButton.y};
    const char *selectText = "Select new audio device";
    Vector2 tSize = MeasureTextEx(GetFontDefault(), selectText, fontSize, spacing);
    DrawTextEx(GetFontDefault(), selectText, tPos, fontSize, spacing, RED);

    const char *currentAudioDevice = GetCurrentPlaybackDeviceName();
    const char *fullCurrentDeviceName = TextFormat("Current: %s", currentAudioDevice);
    tPos.y += tSize.y + tSize.y * 0.1f;
    DrawTextEx(GetFontDefault(), fullCurrentDeviceName, tPos, fontSize, spacing, YELLOW);

    Rectangle scrollArea = {tPos.x, tPos.y, selectAudioDeviceButton.width, selectAudioDeviceButton.height - tPos.y};
    
    ma_device_info *playbackDevices = 0;
    ma_uint32 playbackDevicesCount = 0;
    GetPlaybackDevices(&playbackDevices, &playbackDevicesCount);

    tSize = MeasureTextEx(GetFontDefault(), fullCurrentDeviceName, fontSize, spacing);
    tPos.y += tSize.y + tSize.y * 0.5f;

    static int clickedId;

    for (int i = 0; i < playbackDevicesCount; i++)
    {
        const char *deviceName = playbackDevices[i].name;

        Color hoverColor = YELLOW;
        Color normalColor = RED;
        Color clickedColor = WHITE;
        Color c = normalColor;

        Rectangle rec = {tPos.x, tPos.y, scrollArea.width, tSize.y};

        if (CheckCollisionPointRec(GetMousePosition(), rec))
        {
            c = hoverColor;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                c = clickedColor;
                clickedId = i;
            }
            else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && clickedId == i)
            {
                ChangePlaybackDevice(playbackDevices[i]);
                // TraceLog(LOG_INFO, "CLICK %i", clickedId);
            }
        }
        else if (clickedId == i)
        {
            clickedId = -1;
        }

        DrawRectangleRec(rec, c);

        DrawTextEx(GetFontDefault(), deviceName, tPos, fontSize, spacing, ORANGE);  
        tSize = MeasureTextEx(GetFontDefault(), deviceName, fontSize, spacing);
        tPos.y += tSize.y + tSize.y * 0.5f;
    }

    // for (int i = 0; i < playbackDevicesCount; i++)
    // {
    //     const char *deviceName = playbackDevices[i].name;
    //     DrawTextEx(GetFontDefault(), deviceName, tPos, fontSize, spacing, ORANGE);  

    //     tSize = MeasureTextEx(GetFontDefault(), deviceName, fontSize, spacing);
    //     tPos.y += tSize.y + tSize.y * 0.5f;
    // }


    DrawRectangleLinesEx(menuArea, 1.0f, RED);
    DrawRectangleLinesEx(backButton, 1.0f, YELLOW);
    DrawRectangleLinesEx(backButtonText, 1.0f, ORANGE);
    DrawRectangleLinesEx(selectAudioDeviceButton, 1.0f, YELLOW);
    DrawRectangleLinesEx(scrollArea, 1.0f, GREEN);
}

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
    if (rect.width < 0) rect.width = 0;
    if (rect.height < 0) rect.height = 0;

    return rect;
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
        nX = (int) ceilf(v) - 1;
    }

    {
        float a_1 = baseTextSize1.y;
        float d = baseTextSize2.y - baseTextSize1.y;
        float x = target.height;

        float v = (x - a_1) / d + 1;
        nY = (int) ceilf(v) - 1;
    }

    int n = nX > nY ? nY : nX;
    return n;
}

float GetAbsoluteScreenScaleX()
{
    return (float)GetScreenWidth() / SCREEN_WIDTH;
}

float GetAbsoluteScreenScaleY()
{
    return (float)GetScreenHeight() / SCREEN_HEIGHT;
}

float GetScreenScale()
{
    return GetScreenWidth() > GetScreenHeight() ? GetAbsoluteScreenScaleY() : GetAbsoluteScreenScaleX();
}


inline bool IsRectClicked(Rectangle collider, Vector2 mouseP)
{
    bool collision = CheckCollisionPointRec(mouseP, collider);
    bool click = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    return collision && click;
}