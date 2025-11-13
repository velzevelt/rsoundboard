#include "app.h"
#include <assert.h>
#include <keyboard_hook.h>
#include <math.h>
#include <miniaudio.h>
#include <nfd.h>
#include <raygui.h>
#include <raylib.h>
#include <font_sdf.h>
#include <resources/hack_regular_font.h>
#include <resources/app_icon.h>

typedef enum
{
    NONE,
    MAIN_MENU,
    SOUNDBOARD_SELECTION,
    SOUNDBOARD_LOADING,
    SOUNDBOARD_STATUS,
    SOUNDBOARD_TO_SEARCH,
    SOUNDBOARD_SEARCH,
    SOUNDBOARD_FROM_SEARCH,
    SETTINGS,
} APP_STATE;

typedef enum
{
    LD_NONE,
    LD_LOADING,
    LD_DONE,
} LOADING_STATE;

static APP_STATE AppState;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const char *SoundboardDir = NULL;
static LOADING_STATE SoundboardDirLoadingState;
static FilePathList SoundboardFiles;

static Rectangle BeforeSearchWindow = {0};
static Font SearchFont;
static Image AppIcon;

// const char *soundExtensions = ".wav;.mp3";
#define SUPPORTED_SOUND_EXTENSIONS ".wav"

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rsoundboard");

    AppIcon = LoadImageFromMemory(".png", APP_ICON_DATA, APP_ICON_DATA_SIZE);

    if (IsImageValid(AppIcon))
    {
        if (AppIcon.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
        {
            TraceLog(LOG_WARNING, "App icon has incorrect format %i %s", AppIcon.format, rlGetPixelFormatName(AppIcon.format));
        }

        SetWindowIcon(AppIcon);
    }

    InitAudioDevice();
    SetExitKey(0);
    InstallKeyboardHook();
    // AppState = MAIN_MENU;
    AppState = SOUNDBOARD_STATUS;

    // SearchFont = LoadFont_HackRegularFont();
    // SearchFont = LoadFontSDF("Hack-Regular.ttf", 32);
    SearchFont = LoadFontSDFFromMemory(HACK_REGULAR_FONT_DATA, HACK_REGULAR_FONT_DATA_SIZE, 32);

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
    if (AppState == MAIN_MENU)
    {
        RunMainMenu();
    }
    else if (AppState == SOUNDBOARD_SELECTION)
    {
        RunSoundboardSelection();
    }
    else if (AppState == SETTINGS)
    {
        RunSettings();
    }
    else if (AppState == SOUNDBOARD_LOADING)
    {
        RunSoundboardLoading();
    }
    else if (AppState == SOUNDBOARD_STATUS)
    {
        RunSoundboardStatus();
    }
    else if (AppState == SOUNDBOARD_TO_SEARCH)
    {
        RunToSearch();
    }
    else if (AppState == SOUNDBOARD_SEARCH)
    {
        RunSearch();
    }
    else if (AppState == SOUNDBOARD_FROM_SEARCH)
    {
        RunFromSearch();
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
        AppState = SOUNDBOARD_SELECTION;
    }

    fontSize = RectCalcFontSize(vertButton2Text, GetFontDefault(), "Settings", 0);
    if (GuiButton(vertButton2, "Settings"))
    {
        AppState = SETTINGS;
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

    Vector2 textSize = MeasureTextEx(GetFontDefault(), "INVALID APP STATE", 32, 1);
    DrawText("INVALID APP STATE", w * 0.5f - textSize.x * 0.5f, h * 0.5f - textSize.y * 0.5f, 32, RED);
}

void RunSoundboardSelection()
{
    ClearBackground(BLACK);
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // const char *t = "SOUNDBOARD SELECTION";
    // int textW = MeasureText(t, 32);
    // DrawText(t, w * 0.5f - textW / 2, h * 0.5f, 32, GREEN);

    Rectangle menuArea = {0, 0, 0.9f, 0.9f};
    menuArea.x = 0.5f - menuArea.width * 0.5f;
    menuArea.y = 0.5f - menuArea.height * 0.5f;

    Rectangle backButton = menuArea;
    backButton.width = menuArea.width * 0.3f;
    backButton.height = menuArea.height * 0.3f;
    backButton.x = menuArea.x + menuArea.width * 0.5f - backButton.width * 0.5f;
    backButton.y = menuArea.y + menuArea.height - backButton.height;

    Rectangle buttonsArea = menuArea;
    buttonsArea.height -= backButton.height;
    buttonsArea = RectPadding(buttonsArea, 0, 0.03f, 0, 0);

    Rectangle loadSoundBoardButton = buttonsArea;
    loadSoundBoardButton.width = buttonsArea.width * 0.5f;
    loadSoundBoardButton.height = buttonsArea.height * 0.3f;

    Rectangle backButtonText = RectPadding(backButton, 0.03f, 0.03f, 0.03f, 0.03f);
    loadSoundBoardButton = RectPadding(loadSoundBoardButton, 0.03f, 0.03f, 0.03f, 0.03f);

    menuArea = RectToScreen(menuArea, w, h);
    backButton = RectToScreen(backButton, w, h);
    backButtonText = RectToScreen(backButtonText, w, h);
    buttonsArea = RectToScreen(buttonsArea, w, h);
    loadSoundBoardButton = RectToScreen(loadSoundBoardButton, w, h);

    const char *backText = "Back";
    int fontSize = 36 * GetScreenScale();
    int spacing = 8 * GetScreenScale();

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(DEFAULT, TEXT_SPACING, spacing);
    if (GuiButton(backButtonText, backText))
    {
        AppState = MAIN_MENU;
    }

    fontSize = 20 * GetScreenScale();
    spacing = 5 * GetScreenScale();
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(DEFAULT, TEXT_SPACING, spacing);

    if (GuiButton(loadSoundBoardButton, "Load soundboard"))
    {
        nfdresult_t result = NFD_PickFolder(NULL, &SoundboardDir);

        if (result == NFD_OKAY)
        {
            TraceLog(LOG_INFO, "Selected directory: %s", SoundboardDir);
            AppState = SOUNDBOARD_LOADING;
            SoundboardDirLoadingState = LD_NONE;
        }
        else if (result == NFD_CANCEL)
        {
            TraceLog(LOG_INFO, "User pressed cancel.");
        }
        else
        {
            TraceLog(LOG_INFO, "Error: %s", NFD_GetError());
        }
    }

    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);

    DrawRectangleLinesEx(menuArea, 1.0f, RED);
    DrawRectangleLinesEx(backButton, 1.0f, YELLOW);
    DrawRectangleLinesEx(backButtonText, 1.0f, ORANGE);
    DrawRectangleLinesEx(buttonsArea, 1.0f, YELLOW);
    DrawRectangleLinesEx(loadSoundBoardButton, 1.0f, ORANGE);
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
        AppState = MAIN_MENU;
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

    // TODO: Make this shite scrollable
    for (int i = 0; i < playbackDevicesCount; i++)
    {
        const char *deviceName = playbackDevices[i].name;

        Color hoverColor = YELLOW;
        Color normalColor = RED;
        normalColor.a = 255 / 2;

        Color clickedColor = WHITE;
        Color c = normalColor;

        Rectangle rec = {tPos.x, tPos.y, scrollArea.width, tSize.y};

        if (CheckCollisionPointRec(GetMousePosition(), rec))
        {
            c = hoverColor;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                clickedId = i;
            }
            else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                c = clickedColor;
            }
            else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && clickedId == i)
            {
                ChangePlaybackDevice(playbackDevices[i]);
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

    DrawRectangleLinesEx(menuArea, 1.0f, RED);
    DrawRectangleLinesEx(backButton, 1.0f, YELLOW);
    DrawRectangleLinesEx(backButtonText, 1.0f, ORANGE);
    DrawRectangleLinesEx(selectAudioDeviceButton, 1.0f, YELLOW);
    DrawRectangleLinesEx(scrollArea, 1.0f, GREEN);
}

void RunSoundboardLoading()
{
    ClearBackground(BLACK);

    bool hasErrors = false;

    if (SoundboardDirLoadingState == LD_NONE)
    {
        SoundboardDirLoadingState = LD_LOADING;
    }
    else if (SoundboardDirLoadingState == LD_LOADING)
    {
        if (SoundboardDir == NULL)
        {
            TraceLog(LOG_WARNING, "Directory is null");
            hasErrors = true;
            SoundboardDirLoadingState = LD_DONE;
        }
        else if (!DirectoryExists(SoundboardDir))
        {
            TraceLog(LOG_WARNING, "Directory does not exists %s", SoundboardDir);
            hasErrors = true;
            SoundboardDirLoadingState = LD_DONE;
        }
        else
        {
            SoundboardFiles = LoadDirectoryFilesEx(SoundboardDir, SUPPORTED_SOUND_EXTENSIONS, true);
            SoundboardDirLoadingState = LD_DONE;
            AppState = SOUNDBOARD_STATUS;
        }
    }

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    char *statusText = "Soundboard is loading";
    Color statusColor = YELLOW;

    if (hasErrors)
    {
        statusText = "Soundboard loading failed";
        statusColor = RED;
    }

    int fontSize = 32 * GetScreenScale();
    int spacing = 6 * GetScreenScale();
    Vector2 textSize = MeasureTextEx(GetFontDefault(), statusText, fontSize, 1);
    Vector2 textPos = {w * 0.5f - textSize.x * 0.5f, h * 0.45f - textSize.y * 0.5f};

    DrawTextEx(GetFontDefault(), statusText, textPos, fontSize, spacing, statusColor);

    if (hasErrors)
    {
        Vector2 buttonSize = {w * 0.2f, h * 0.2f};
        Rectangle backButton = {w * 0.5f - buttonSize.x, h - buttonSize.y};

        if (GuiButton(backButton, "Back"))
        {
            AppState = MAIN_MENU;
            SoundboardDirLoadingState = LD_NONE;
        }
    }
}

void RunSoundboardStatus()
{
    ClearBackground(BLACK);
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    int fontSize = 32 * GetScreenScale();
    int spacing = 4 * GetScreenScale();

    const char *text = TextFormat("Soundboard is ready (%i),\nuse space + f \nrestore alt + f (only focused)", 7);
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, spacing);
    Vector2 textPos = {w * 0.5f - textSize.x * 0.5f, h * 0.5f - textSize.y};
    // RLAPI void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);
    // // Draw text using font and additional parameters
    DrawTextEx(GetFontDefault(), text, textPos, fontSize, spacing, GREEN);

    Vector2 backButtonSize = {w * 0.15f, h * 0.15f};
    Rectangle backButton = {w * 0.5f - backButtonSize.x * 0.5f, h - backButtonSize.y, backButtonSize.x, backButtonSize.y};

    fontSize = 32 * GetScreenScale();
    spacing = 4 * GetScreenScale();

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(DEFAULT, TEXT_SPACING, spacing);
    if (GuiButton(backButton, "Back"))
    {
        AppState = MAIN_MENU;
        return;
    }
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);

    if (IsKeyboardHookKeyDown(HK_SPACE) && IsKeyboardHookKeyPressed(HK_F))
    {
        AppState = SOUNDBOARD_TO_SEARCH;
    }

    bool wantToRestoreBeforeWindow = IsWindowFocused() && IsKeyboardHookKeyDown(HK_ALT) && IsKeyboardHookKeyPressed(HK_F);
    
    // if (IsKeyboardHookKeyDown(HK_ALT) && IsKeyboardHookKeyPressed(HK_F))
    // {
    //     printf("%i, %f, %f\n", IsWindowFocused(), BeforeSearchWindow.width, BeforeSearchWindow.height);
    // }

    wantToRestoreBeforeWindow = wantToRestoreBeforeWindow && (BeforeSearchWindow.width > 0 && BeforeSearchWindow.height > 0);

    if (wantToRestoreBeforeWindow)
    {    
        SetWindowSize(BeforeSearchWindow.width, BeforeSearchWindow.height);
        SetWindowPosition(BeforeSearchWindow.x, BeforeSearchWindow.y);
    }
}

void RunToSearch()
{
    if (!IsWindowMinimized())
    {
        Vector2 currentWindowPos = GetWindowPosition();
        Vector2 currentWindowSize = {GetScreenWidth(), GetScreenHeight()};
        BeforeSearchWindow =
            (Rectangle){currentWindowPos.x, currentWindowPos.y, currentWindowSize.x, currentWindowSize.y};
    }

    // TraceLog(LOG_INFO, "Xuy %f %f %f %f", BeforeSearchWindow.x, BeforeSearchWindow.y, BeforeSearchWindow.width,
    //          BeforeSearchWindow.height);

    int currentMonitorId = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(currentMonitorId);
    int monitorHeight = GetMonitorHeight(currentMonitorId);

    Vector2 newWindowSize = {monitorWidth, monitorHeight};

    RestoreWindow();

    SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
    SetWindowPosition(0, 0);
    SetWindowSize(newWindowSize.x, newWindowSize.y);

    SetWindowFocused();
    AppState = SOUNDBOARD_SEARCH;
}

void RunSearch()
{
    Color c = BLACK;
    c.a = 255 * 0.5f;

    ClearBackground(c);

    if (IsKeyboardHookKeyPressed(HK_ESCAPE))
    {
        AppState = SOUNDBOARD_FROM_SEARCH;
    }

    Vector2 optionPosition = {0, 0};
    Vector2 optionSize = {30, 5};

    // int size = SoundboardFiles.count >= 10 ? 10 : SoundboardFiles.count;
    int size = 10;
    for (int i = 0; i < size; i++)
    {
        Rectangle rec = {optionPosition.x, optionPosition.y, optionSize.x, optionSize.y};
        DrawRectangleRec(rec, RED);


        optionPosition.y += 10;
    }

    if (IsFontValid(SearchFont))
    {
        BeginShaderMode(GetSDFShader());
        DrawTextEx(SearchFont, "TEST", (Vector2){300, 300}, 64, 5, WHITE);
        EndShaderMode();
    }
    else
    {
        DrawTextEx(GetFontDefault(), "TEST", (Vector2){300, 300}, 64, 5, WHITE);
    }
}

void RunFromSearch()
{
    SetWindowSize(0, 0);
    SetWindowPosition(0, 0);

    ClearWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);

    // SetWindowSize(BeforeSearchWindow.width, BeforeSearchWindow.height);
    // SetWindowPosition(BeforeSearchWindow.x, BeforeSearchWindow.y);

    MinimizeWindow();
    AppState = SOUNDBOARD_STATUS;
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
    if (rect.width < 0)
        rect.width = 0;
    if (rect.height < 0)
        rect.height = 0;

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