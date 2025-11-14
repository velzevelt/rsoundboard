#include "app.h"
#include <assert.h>
#include <font_sdf.h>
#include <keyboard_hook.h>
#include <miniaudio.h>
#include <nfd.h>
#include <raygui.h>
#include <raylib.h>
#include <rect_utils.h>
#include <resources/app_icon.h>
#include <resources/hack_regular_font.h>
#include <string.h>
#include <text_input_box.h>
#include <search_file.h>

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
static const char *SoundboardDir = NULL;
static LOADING_STATE SoundboardDirLoadingState;
static FilePathList SoundboardFiles;
static FilteredFilePathList SearchCandidates;

static Rectangle BeforeSearchWindow = {0};
static Font SearchFont;
static Image AppIcon;

#define SEARCH_TEXT_SIZE 32
static char SearchText[SEARCH_TEXT_SIZE + 1] = "\0";
static int SearchTextLetterCount = 0;

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
            TraceLog(LOG_WARNING, "App icon has incorrect format %i %s", AppIcon.format,
                     rlGetPixelFormatName(AppIcon.format));
        }

        SetWindowIcon(AppIcon);
    }

    InitAudioDevice();
    SetExitKey(0);
    InstallKeyboardHook();
    AppState = MAIN_MENU;
    // AppState = SOUNDBOARD_STATUS;

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
            UnloadDirectoryFiles(SoundboardFiles);
            SoundboardFiles = LoadDirectoryFilesEx(SoundboardDir, SUPPORTED_SOUND_EXTENSIONS, true);
            SoundboardDirLoadingState = LD_DONE;
            SearchCandidates = SearchInFileListNoAlloc(&SoundboardFiles, "", 16);
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

    const char *text = TextFormat("Soundboard is ready (%i),\nuse alt + f \nrestore space + f (only focused)", SoundboardFiles.count);
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, spacing);
    Vector2 textPos = {w * 0.5f - textSize.x * 0.5f, h * 0.5f - textSize.y};
    // RLAPI void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);
    // // Draw text using font and additional parameters
    DrawTextEx(GetFontDefault(), text, textPos, fontSize, spacing, GREEN);

    Vector2 backButtonSize = {w * 0.15f, h * 0.15f};
    Rectangle backButton = {w * 0.5f - backButtonSize.x * 0.5f, h - backButtonSize.y, backButtonSize.x,
                            backButtonSize.y};

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

    if (IsKeyboardHookKeyDown(HK_ALT) && IsKeyboardHookKeyPressed(HK_F))
    {
        AppState = SOUNDBOARD_TO_SEARCH;
    }

    bool wantToRestoreBeforeWindow =
        IsWindowFocused() && IsKeyboardHookKeyDown(HK_SPACE) && IsKeyboardHookKeyPressed(HK_F);

    // if (IsKeyboardHookKeyDown(HK_ALT) && IsKeyboardHookKeyPressed(HK_F))
    // {
    //     printf("%i, %f, %f\n", IsWindowFocused(), BeforeSearchWindow.width, BeforeSearchWindow.height);
    // }

    wantToRestoreBeforeWindow =
        wantToRestoreBeforeWindow && (BeforeSearchWindow.width > 0 && BeforeSearchWindow.height > 0);

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

    strcpy(SearchText, "");
    SearchTextLetterCount = 0;
}

void RunSearch()
{
    Color c = BLACK;
    c.a = 255 * 0.5f;
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float scale = GetScreenScale();

    ClearBackground(c);

    if (IsKeyboardHookKeyPressed(HK_ESCAPE))
    {
        AppState = SOUNDBOARD_FROM_SEARCH;
    }

    Rectangle searchBoxOuter = {0, 0, 1.0f, 0.2f};
    Rectangle soundBoxOuter = {0, searchBoxOuter.height, 1.0f, 1.0f - searchBoxOuter.height};

    Rectangle searchBoxInner = RectPaddingAll(searchBoxOuter, 0.065f);

    searchBoxOuter = RectToScreen(searchBoxOuter, w, h);
    soundBoxOuter = RectToScreen(soundBoxOuter, w, h);
    searchBoxInner = RectToScreen(searchBoxInner, w, h);

    // printf("%f %f %f %f\n", searchBoxInner.x, searchBoxInner.y, searchBoxInner.width, searchBoxInner.height);
    int fontSize = 16 * GetScreenScale();

    GuiSetFont(SearchFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    BeginShaderMode(GetSDFShader());
    char *searchOut = TextInputBox(searchBoxInner, "Enter song name...", SearchText, SEARCH_TEXT_SIZE, &SearchTextLetterCount);
    EndShaderMode();
    GuiSetFont(GetFontDefault());
    
    // if (searchOut)
    // {
    //     printf("Search out %s\n", searchOut);
    // }

    if (searchOut && strnlen(SearchText, SEARCH_TEXT_SIZE) > 0)
    {
        SearchCandidates = SearchInFileListNoAlloc(&SoundboardFiles, searchOut, 16);
        printf("Search can %i\n", SearchCandidates.count);
    }

    Rectangle itemRect = {0, 0, 30, 30};
    for (int i = 0; i < SearchCandidates.count; i++)
    {
        DrawRectangleRec(itemRect, RED);
        itemRect.y += 33;
    }

    // printf("Count %i\n", SearchSoundboardFiles.count);
    DrawRectangleLinesEx(searchBoxOuter, 1, RED);
    DrawRectangleLinesEx(searchBoxInner, 1, GREEN);
    DrawRectangleLinesEx(soundBoxOuter, 1, ORANGE);
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

// returns *text if it was changed, null otherwise 
char *TextInputBox(Rectangle bounds, const char *placeholder, char *text, int maxTextSize, int *letterCountPtr)
{
    assert(placeholder);
    assert(text);
    assert(letterCountPtr);

    int letterCount = *letterCountPtr;
    int key = GetCharPressed();

    bool wasChanged = false;
    bool pasted = false;

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V))
    {
        char *clipboardText = GetClipboardText();
        int clipboardTextLen = strlen(clipboardText);
        int currentTextLen = strnlen(text, maxTextSize);
        if (clipboardTextLen > 0)
        {
            strncat(text, clipboardText, maxTextSize);
            int newLetterCount = clipboardTextLen + currentTextLen;
            letterCount = newLetterCount > maxTextSize ? maxTextSize : newLetterCount;
            wasChanged = true;
            pasted = true;
            TraceLog(LOG_INFO, "Text pasted %i", letterCount);
        }
    }

    // Check if more characters have been pressed on the same frame
    while (key > 0 && !pasted)
    {
        // printf("Key pressed %c\n", (char) key);

        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (letterCount < maxTextSize))
        {
            text[letterCount] = (char)key;
            text[letterCount + 1] = '\0'; // Add null terminator at the end of the string
            letterCount++;
            wasChanged = true;
        }

        key = GetCharPressed(); // Check next character in the queue
    }

    bool processBackSpace = false;
    static double pressedTime;

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        pressedTime = GetTime();
        processBackSpace = true;
    }
    else if (IsKeyDown(KEY_BACKSPACE))
    {
        double holdTime = GetTime();
        
        if (holdTime - pressedTime > 0.5)
        {
            processBackSpace = true;
            pressedTime = holdTime - 0.48;
            // printf("XUY\n");
        }
    }

    if (processBackSpace)
    {
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            letterCount = 0;
        }
        else
        {
            letterCount--;
        }

        if (letterCount < 0) 
        {
            letterCount = 0;
        }
        else
        {
            wasChanged = true;
        }

        text[letterCount] = '\0';
    }

    int textLen = strnlen(text, maxTextSize); 
    char *xuy = textLen == 0 ? placeholder : text;

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C) && textLen > 0)
    {
        SetClipboardText(text);
        TraceLog(LOG_INFO, "Text copied");
    }

    // printf("XUY %s\n", xuy);

    Font f = GuiGetFont();
    int fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    Vector2 textSize = MeasureTextEx(f, xuy, fontSize, 1);
    Vector2 textPosition = {bounds.x + bounds.width * 0.5f - textSize.x * 0.5f,
                            bounds.y + bounds.height * 0.5f - textSize.y * 0.5f};
    DrawTextEx(f, xuy, textPosition, fontSize, 1, WHITE);

    if (wasChanged)
    {
        *letterCountPtr = letterCount;
        return text;
    }
    else
    {
        return NULL;
    }
}


