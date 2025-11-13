#include <windows.h>
#include <stdio.h>
#include "keyboard_hook.h"
#include <assert.h>
#include <stdbool.h>

static HHOOK keyboardHook = NULL;

#define KEYS_SIZE 256

static int keyStates[KEYS_SIZE] = {HK_STATE_RELEASED}; // Track state of all virtual keys (0-255)

static int thisFrameStates[KEYS_SIZE] = {HK_STATE_RELEASED}; // Track state of all virtual keys (0-255)
// static int lastFrameStates[KEYS_SIZE] = {HK_STATE_RELEASED}; // Track state of all virtual keys (0-255)

static int lastPressedKey = 0;

// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        KBDLLHOOKSTRUCT *kbStruct = (KBDLLHOOKSTRUCT *)lParam;
        int key = (int)kbStruct->vkCode;

        if (key >= 0 && key < KEYS_SIZE)
        {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
            {
                // HookKeyState keyState = keyStates[key];
                keyStates[key] = HK_STATE_PRESSED;    

                lastPressedKey = key;
            }
            else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
            {
                // HookKeyState keyState = keyStates[key];
                keyStates[key] = HK_STATE_UP;
            }
        }
    }

    // Pass to next hook
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

// Install the global hook
void InstallKeyboardHook()
{
    // Initialize all key states to 0
    for (int i = 0; i < KEYS_SIZE; i++)
    {
        keyStates[i] = 0;
    }

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (!keyboardHook)
    {
        printf("Failed to install keyboard hook\n");
    }
    else
    {
        printf("Global keyboard hook installed successfully\n");
    }
}

// Remove the hook
void RemoveKeyboardHook()
{
    if (keyboardHook)
    {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = NULL;
        printf("Global keyboard hook removed\n");
    }
}

// Check if a specific key is currently pressed
bool IsKeyboardHookKeyPressed(HookKey key)
{
    if (key >= 0 && key < KEYS_SIZE)
    {
        return thisFrameStates[(int)key] == HK_STATE_PRESSED;
    }
    return false;
}

bool IsKeyboardHookKeyDown(HookKey key)
{
    if (key >= 0 && key < KEYS_SIZE)
    {
        return thisFrameStates[(int)key] == HK_STATE_DOWN;
    }
    return false;
}

void BeginKeyboardHook()
{
    for (int i = 0; i < KEYS_SIZE; i++)
    {
        HookKeyState actualState = keyStates[i];
        HookKeyState prevFrameState = thisFrameStates[i];

        if (prevFrameState == HK_STATE_RELEASED && actualState == HK_STATE_PRESSED)
        {
            thisFrameStates[i] = HK_STATE_PRESSED;
        }
        else if (prevFrameState == HK_STATE_PRESSED)
        {
            thisFrameStates[i] = HK_STATE_DOWN;
        }
        else if (prevFrameState == HK_STATE_DOWN && actualState == HK_STATE_UP)
        {
            thisFrameStates[i] = HK_STATE_UP;
        }
        else if (prevFrameState == HK_STATE_UP)
        {
            thisFrameStates[i] = HK_STATE_RELEASED;
        }
    }
}

// void EndKeyboardHook()
// {
// }

HookKey GetLastPressedKey()
{
    return lastPressedKey;
}