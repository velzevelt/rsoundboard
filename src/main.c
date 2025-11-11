#include <raylib.h>
#include <keyboard_hook.h>
#include <raygui.h>

int main(void)
{
    const int screenWidth = 1920 / 2;
    const int screenHeight = 1080 / 2;

    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "rsoundboard");
    InstallKeyboardHook();

    bool isSearchMenuVisible = false;
    char *selectedDir = NULL;

    while (!WindowShouldClose())
    {
        BeginKeyboardHook();
        BeginDrawing();
        {
            ClearBackground(BLACK);
            // int w = MeasureText("rsoundboard", 20);
            // DrawText("rsoundboard", screenWidth / 2 - w / 2, screenHeight / 2, 20, WHITE);

            HookKey lastPressed = GetLastPressedKey();
            bool searchRequested = IsKeyboardHookKeyDown(HK_SPACE) && IsKeyboardHookKeyDown(HK_F) && (lastPressed == HK_SPACE || lastPressed == HK_F);

            if (GuiButton((Rectangle){300, 200, 200, 40}, "Select Directory"))
            {
                // Open directory selection dialog
                selectedDir = tinyfd_selectFolderDialog("Select a directory", NULL);

                if (selectedDir)
                {
                    TraceLog(LOG_INFO, "Selected directory: %s", selectedDir);
                }
            }

            if (searchRequested && !isSearchMenuVisible)
            {
                isSearchMenuVisible = true;

                TraceLog(LOG_INFO, "Search menu triggered");

                // if (selectedDir)
                // {
                //     DrawText(TextFormat("Selected: %s", selectedDir), 50, 250, 20, DARKGRAY);
                // }
            }
        }
        EndDrawing();
    }

    RemoveKeyboardHook();
    CloseWindow();
    return 0;
}