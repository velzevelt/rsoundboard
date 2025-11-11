#pragma once
#include <stdbool.h>

// Custom key codes to avoid conflicts with Raylib
typedef enum {
    // Letters
    HK_A = 0x41,
    HK_B = 0x42,
    HK_C = 0x43,
    HK_D = 0x44,
    HK_E = 0x45,
    HK_F = 0x46,
    HK_G = 0x47,
    HK_H = 0x48,
    HK_I = 0x49,
    HK_J = 0x4A,
    HK_K = 0x4B,
    HK_L = 0x4C,
    HK_M = 0x4D,
    HK_N = 0x4E,
    HK_O = 0x4F,
    HK_P = 0x50,
    HK_Q = 0x51,
    HK_R = 0x52,
    HK_S = 0x53,
    HK_T = 0x54,
    HK_U = 0x55,
    HK_V = 0x56,
    HK_W = 0x57,
    HK_X = 0x58,
    HK_Y = 0x59,
    HK_Z = 0x5A,

    // Numbers
    HK_0 = 0x30,
    HK_1 = 0x31,
    HK_2 = 0x32,
    HK_3 = 0x33,
    HK_4 = 0x34,
    HK_5 = 0x35,
    HK_6 = 0x36,
    HK_7 = 0x37,
    HK_8 = 0x38,
    HK_9 = 0x39,

    // Function keys
    HK_F1 = 0x70,
    HK_F2 = 0x71,
    HK_F3 = 0x72,
    HK_F4 = 0x73,
    HK_F5 = 0x74,
    HK_F6 = 0x75,
    HK_F7 = 0x76,
    HK_F8 = 0x77,
    HK_F9 = 0x78,
    HK_F10 = 0x79,
    HK_F11 = 0x7A,
    HK_F12 = 0x7B,

    // Special keys
    HK_SPACE = 0x20,
    HK_SHIFT = 0x10,
    HK_CONTROL = 0x11,
    HK_ALT = 0x12,
    HK_ESCAPE = 0x1B,
    HK_ENTER = 0x0D,
    HK_BACKSPACE = 0x08,
    HK_TAB = 0x09,
    HK_CAPS_LOCK = 0x14,
    HK_WINDOWS = 0x5B,

    // Arrow keys
    HK_LEFT = 0x25,
    HK_UP = 0x26,
    HK_RIGHT = 0x27,
    HK_DOWN = 0x28,

    // Numpad
    HK_NUMPAD_0 = 0x60,
    HK_NUMPAD_1 = 0x61,
    HK_NUMPAD_2 = 0x62,
    HK_NUMPAD_3 = 0x63,
    HK_NUMPAD_4 = 0x64,
    HK_NUMPAD_5 = 0x65,
    HK_NUMPAD_6 = 0x66,
    HK_NUMPAD_7 = 0x67,
    HK_NUMPAD_8 = 0x68,
    HK_NUMPAD_9 = 0x69,

    // Other
    HK_INSERT = 0x2D,
    HK_DELETE = 0x2E,
    HK_HOME = 0x24,
    HK_END = 0x23,
    HK_PAGE_UP = 0x21,
    HK_PAGE_DOWN = 0x22,

    // Symbols
    HK_GRAVE = 0xC0,        // `
    HK_MINUS = 0xBD,        // -
    HK_EQUALS = 0xBB,       // =
    HK_LEFT_BRACKET = 0xDB, // [
    HK_RIGHT_BRACKET = 0xDD, // ]
    HK_BACKSLASH = 0xDC,    // \
    HK_SEMICOLON = 0xBA,    // ;
    HK_APOSTROPHE = 0xDE,   // '
    HK_COMMA = 0xBC,        // ,
    HK_PERIOD = 0xBE,       // .
    HK_SLASH = 0xBF,        // /

    // Media keys (may require additional handling)
    HK_VOLUME_UP = 0xAF,
    HK_VOLUME_DOWN = 0xAE,
    HK_VOLUME_MUTE = 0xAD,
    HK_MEDIA_PLAY_PAUSE = 0xB3,
    HK_MEDIA_NEXT = 0xB0,
    HK_MEDIA_PREV = 0xB1
} HookKey;

typedef enum
{
    HK_STATE_RELEASED, // released
    HK_STATE_PRESSED, // pressed once
    HK_STATE_DOWN, // being held
    HK_STATE_UP, // released once
} HookKeyState;

void InstallKeyboardHook();
void RemoveKeyboardHook();
bool IsKeyboardHookKeyPressed(HookKey key);
bool IsKeyboardHookKeyDown(HookKey key);
void BeginKeyboardHook();
void EndKeyboardHook();
HookKey GetLastPressedKey();