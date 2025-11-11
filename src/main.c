#include <raylib.h>
#include <keyboard_hook.h>

int main(void)
{
    const int screenWidth = 1920 / 2;
    const int screenHeight = 1080 / 2;

    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    InstallKeyboardHook();

    // TraceLog(LOG_INFO, "Size is %i", hookKeysSize);

    int i = 0;

    while (!WindowShouldClose())
    {
        BeginKeyboardHook();

        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawText("Congrats! You created your first window!", 190, 200, 20, WHITE);

            HookKey lastPressed = GetLastPressedKey();
            bool searchRequested = IsKeyboardHookKeyDown(HK_SPACE) && IsKeyboardHookKeyDown(HK_F) && (lastPressed == HK_SPACE || lastPressed == HK_F);

            if (searchRequested)
            {
                TraceLog(LOG_INFO, "Search pressed is %i. Last pressed %i", i, lastPressed);
                i++;
            }
        }
        EndDrawing();
        EndKeyboardHook();
    }

    RemoveKeyboardHook();
    CloseWindow();
    return 0;
}