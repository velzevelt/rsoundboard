#include <raylib.h>
#include <keyboard_hook.h>
#include <raygui.h>
#include <nfd.h>
#include <assert.h>
#include <load_last_directory.h>

int main(void)
{
    const int screenWidth = 1920 / 2;
    const int screenHeight = 1080 / 2;

    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT
    );

    InitWindow(screenWidth, screenHeight, "rsoundboard");
    SetExitKey(0);
    InstallKeyboardHook();

    bool isSearchMenuVisible = false;
    char *selectedDir = NULL;
    // const char *soundExtensions = ".wav;.mp3";
    const char *soundExtensions = ".wav";
    FilePathList soundFiles = {0};

    selectedDir = LoadLastDirectory();
    if (selectedDir && DirectoryExists(selectedDir))
    {
        TraceLog(LOG_INFO, "Selected directory: %s", selectedDir);
        soundFiles = LoadDirectoryFilesEx(selectedDir, soundExtensions, true);
        SaveLastDirectory(selectedDir);
    }
    else
    {
        selectedDir = NULL;
    }

    while (!WindowShouldClose())
    {
        BeginKeyboardHook();
        HookKey lastPressed = GetLastPressedKey();

        bool searchRequested = IsKeyboardHookKeyDown(HK_SPACE) && IsKeyboardHookKeyPressed(HK_F) && (lastPressed == HK_SPACE || lastPressed == HK_F); 

        BeginDrawing();
        {
            Color b = BLACK;
            b.a = 255 * 0.5f;
            ClearBackground(b);

            if (searchRequested)
            {
                isSearchMenuVisible = !isSearchMenuVisible;
                TraceLog(LOG_INFO, "XXX %i", isSearchMenuVisible);

                if (isSearchMenuVisible)
                {
                    TraceLog(LOG_INFO, "X1");
                    SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
                    SetWindowFocused();
                }
                else
                {
                    ClearWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
                }
            }

            if (IsKeyboardHookKeyPressed(HK_ESCAPE) && IsWindowState(FLAG_WINDOW_TOPMOST))
            {
                ClearWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
                isSearchMenuVisible = false;
            }

            if (IsKeyPressed(KEY_C))
            {
                TraceLog(LOG_INFO, "C");
            }

            // if (selectedDir == NULL)
            // {
            //     if (GuiButton((Rectangle){300, 200, 200, 40}, "Select Sounds Directory"))
            //     {
            //         nfdresult_t result = NFD_PickFolder(NULL, &selectedDir);

            //         if (result == NFD_OKAY)
            //         {
            //             TraceLog(LOG_INFO, "Selected directory: %s", selectedDir);
            //             assert(DirectoryExists(selectedDir));
            //             soundFiles = LoadDirectoryFilesEx(selectedDir, soundExtensions, true);
            //             SaveLastDirectory(selectedDir);
            //         }
            //         else if (result == NFD_CANCEL)
            //         {
            //             TraceLog(LOG_INFO, "User pressed cancel.");
            //         }
            //         else
            //         {
            //             TraceLog(LOG_INFO, "Error: %s", NFD_GetError());
            //         }
            //     }
            // }

            // if (selectedDir)
            // {
            //     DrawText(TextFormat("Selected: %s files %i", selectedDir, soundFiles.count), 10, 10, 20, DARKGRAY);
            // }
        }
        EndDrawing();
    }

    RemoveKeyboardHook();
    CloseWindow();
    return 0;
}
