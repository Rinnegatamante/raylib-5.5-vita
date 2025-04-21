/**********************************************************************************************
*
*   rcore_vita - Functions to manage window, graphics device and inputs
*
*   PLATFORM: VITA
*
*   LIMITATIONS:
*       - Limitation 01
*       - Limitation 02
*
*   POSSIBLE IMPROVEMENTS:
*       - Improvement 01
*       - Improvement 02
*
*   ADDITIONAL NOTES:
*       - TRACELOG() function is located in raylib [utils] module
*
*   CONFIGURATION:
*       #define RCORE_PLATFORM_CUSTOM_FLAG
*           Custom flag for rcore on target platform -not used-
*
*   DEPENDENCIES:
*       - SDL 2 or SLD 3 (main library): Windowing and inputs management
*       - gestures: Gestures system for touch-ready devices (or simulated from mouse inputs)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5) and contributors
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "SDL2/SDL.h"                // SDL base library (window/rendered, input, timing... functionality)
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>


//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#ifndef MAX_CLIPBOARD_BUFFER_LENGTH
    #define MAX_CLIPBOARD_BUFFER_LENGTH 1024 // Size of the clipboard buffer used on GetClipboardText()
#endif

#if ((defined(SDL_MAJOR_VERSION) && SDL_MAJOR_VERSION == 3) && (defined(SDL_MINOR_VERSION) && SDL_MINOR_VERSION >= 1))
    #ifndef PLATFORM_DESKTOP_SDL3
        #define PLATFORM_DESKTOP_SDL3
    #endif
#endif

typedef struct {
    SDL_Window *window;
    SDL_GLContext glContext;

    SDL_GameController *gamepad[1];
    SDL_Cursor *cursor;
    bool cursorRelative;
} PlatformData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern CoreData CORE;                   // Global CORE state context

static PlatformData platform = { 0 };   // Platform specific data

//----------------------------------------------------------------------------------
// Local Variables Definition
//----------------------------------------------------------------------------------
#define SCANCODE_MAPPED_NUM 232
static const KeyboardKey mapScancodeToKey[SCANCODE_MAPPED_NUM] = {
    KEY_NULL,           // SDL_SCANCODE_UNKNOWN
    0,
    0,
    0,
    KEY_A,              // SDL_SCANCODE_A
    KEY_B,              // SDL_SCANCODE_B
    KEY_C,              // SDL_SCANCODE_C
    KEY_D,              // SDL_SCANCODE_D
    KEY_E,              // SDL_SCANCODE_E
    KEY_F,              // SDL_SCANCODE_F
    KEY_G,              // SDL_SCANCODE_G
    KEY_H,              // SDL_SCANCODE_H
    KEY_I,              // SDL_SCANCODE_I
    KEY_J,              // SDL_SCANCODE_J
    KEY_K,              // SDL_SCANCODE_K
    KEY_L,              // SDL_SCANCODE_L
    KEY_M,              // SDL_SCANCODE_M
    KEY_N,              // SDL_SCANCODE_N
    KEY_O,              // SDL_SCANCODE_O
    KEY_P,              // SDL_SCANCODE_P
    KEY_Q,              // SDL_SCANCODE_Q
    KEY_R,              // SDL_SCANCODE_R
    KEY_S,              // SDL_SCANCODE_S
    KEY_T,              // SDL_SCANCODE_T
    KEY_U,              // SDL_SCANCODE_U
    KEY_V,              // SDL_SCANCODE_V
    KEY_W,              // SDL_SCANCODE_W
    KEY_X,              // SDL_SCANCODE_X
    KEY_Y,              // SDL_SCANCODE_Y
    KEY_Z,              // SDL_SCANCODE_Z
    KEY_ONE,            // SDL_SCANCODE_1
    KEY_TWO,            // SDL_SCANCODE_2
    KEY_THREE,          // SDL_SCANCODE_3
    KEY_FOUR,           // SDL_SCANCODE_4
    KEY_FIVE,           // SDL_SCANCODE_5
    KEY_SIX,            // SDL_SCANCODE_6
    KEY_SEVEN,          // SDL_SCANCODE_7
    KEY_EIGHT,          // SDL_SCANCODE_8
    KEY_NINE,           // SDL_SCANCODE_9
    KEY_ZERO,           // SDL_SCANCODE_0
    KEY_ENTER,          // SDL_SCANCODE_RETURN
    KEY_ESCAPE,         // SDL_SCANCODE_ESCAPE
    KEY_BACKSPACE,      // SDL_SCANCODE_BACKSPACE
    KEY_TAB,            // SDL_SCANCODE_TAB
    KEY_SPACE,          // SDL_SCANCODE_SPACE
    KEY_MINUS,          // SDL_SCANCODE_MINUS
    KEY_EQUAL,          // SDL_SCANCODE_EQUALS
    KEY_LEFT_BRACKET,   // SDL_SCANCODE_LEFTBRACKET
    KEY_RIGHT_BRACKET,  // SDL_SCANCODE_RIGHTBRACKET
    KEY_BACKSLASH,      // SDL_SCANCODE_BACKSLASH
    0,                  // SDL_SCANCODE_NONUSHASH
    KEY_SEMICOLON,      // SDL_SCANCODE_SEMICOLON
    KEY_APOSTROPHE,     // SDL_SCANCODE_APOSTROPHE
    KEY_GRAVE,          // SDL_SCANCODE_GRAVE
    KEY_COMMA,          // SDL_SCANCODE_COMMA
    KEY_PERIOD,         // SDL_SCANCODE_PERIOD
    KEY_SLASH,          // SDL_SCANCODE_SLASH
    KEY_CAPS_LOCK,      // SDL_SCANCODE_CAPSLOCK
    KEY_F1,             // SDL_SCANCODE_F1
    KEY_F2,             // SDL_SCANCODE_F2
    KEY_F3,             // SDL_SCANCODE_F3
    KEY_F4,             // SDL_SCANCODE_F4
    KEY_F5,             // SDL_SCANCODE_F5
    KEY_F6,             // SDL_SCANCODE_F6
    KEY_F7,             // SDL_SCANCODE_F7
    KEY_F8,             // SDL_SCANCODE_F8
    KEY_F9,             // SDL_SCANCODE_F9
    KEY_F10,            // SDL_SCANCODE_F10
    KEY_F11,            // SDL_SCANCODE_F11
    KEY_F12,            // SDL_SCANCODE_F12
    KEY_PRINT_SCREEN,   // SDL_SCANCODE_PRINTSCREEN
    KEY_SCROLL_LOCK,    // SDL_SCANCODE_SCROLLLOCK
    KEY_PAUSE,          // SDL_SCANCODE_PAUSE
    KEY_INSERT,         // SDL_SCANCODE_INSERT
    KEY_HOME,           // SDL_SCANCODE_HOME
    KEY_PAGE_UP,        // SDL_SCANCODE_PAGEUP
    KEY_DELETE,         // SDL_SCANCODE_DELETE
    KEY_END,            // SDL_SCANCODE_END
    KEY_PAGE_DOWN,      // SDL_SCANCODE_PAGEDOWN
    KEY_RIGHT,          // SDL_SCANCODE_RIGHT
    KEY_LEFT,           // SDL_SCANCODE_LEFT
    KEY_DOWN,           // SDL_SCANCODE_DOWN
    KEY_UP,             // SDL_SCANCODE_UP
    KEY_NUM_LOCK,       // SDL_SCANCODE_NUMLOCKCLEAR
    KEY_KP_DIVIDE,      // SDL_SCANCODE_KP_DIVIDE
    KEY_KP_MULTIPLY,    // SDL_SCANCODE_KP_MULTIPLY
    KEY_KP_SUBTRACT,    // SDL_SCANCODE_KP_MINUS
    KEY_KP_ADD,         // SDL_SCANCODE_KP_PLUS
    KEY_KP_ENTER,       // SDL_SCANCODE_KP_ENTER
    KEY_KP_1,           // SDL_SCANCODE_KP_1
    KEY_KP_2,           // SDL_SCANCODE_KP_2
    KEY_KP_3,           // SDL_SCANCODE_KP_3
    KEY_KP_4,           // SDL_SCANCODE_KP_4
    KEY_KP_5,           // SDL_SCANCODE_KP_5
    KEY_KP_6,           // SDL_SCANCODE_KP_6
    KEY_KP_7,           // SDL_SCANCODE_KP_7
    KEY_KP_8,           // SDL_SCANCODE_KP_8
    KEY_KP_9,           // SDL_SCANCODE_KP_9
    KEY_KP_0,           // SDL_SCANCODE_KP_0
    KEY_KP_DECIMAL,     // SDL_SCANCODE_KP_PERIOD
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0,
    KEY_LEFT_CONTROL,   //SDL_SCANCODE_LCTRL
    KEY_LEFT_SHIFT,     //SDL_SCANCODE_LSHIFT
    KEY_LEFT_ALT,       //SDL_SCANCODE_LALT
    KEY_LEFT_SUPER,     //SDL_SCANCODE_LGUI
    KEY_RIGHT_CONTROL,  //SDL_SCANCODE_RCTRL
    KEY_RIGHT_SHIFT,    //SDL_SCANCODE_RSHIFT
    KEY_RIGHT_ALT,      //SDL_SCANCODE_RALT
    KEY_RIGHT_SUPER     //SDL_SCANCODE_RGUI
};

static const int CursorsLUT[] = {
    SDL_SYSTEM_CURSOR_ARROW,       // 0  MOUSE_CURSOR_DEFAULT
    SDL_SYSTEM_CURSOR_ARROW,       // 1  MOUSE_CURSOR_ARROW
    SDL_SYSTEM_CURSOR_IBEAM,       // 2  MOUSE_CURSOR_IBEAM
    SDL_SYSTEM_CURSOR_CROSSHAIR,   // 3  MOUSE_CURSOR_CROSSHAIR
    SDL_SYSTEM_CURSOR_HAND,        // 4  MOUSE_CURSOR_POINTING_HAND
    SDL_SYSTEM_CURSOR_SIZEWE,      // 5  MOUSE_CURSOR_RESIZE_EW
    SDL_SYSTEM_CURSOR_SIZENS,      // 6  MOUSE_CURSOR_RESIZE_NS
    SDL_SYSTEM_CURSOR_SIZENWSE,    // 7  MOUSE_CURSOR_RESIZE_NWSE
    SDL_SYSTEM_CURSOR_SIZENESW,    // 8  MOUSE_CURSOR_RESIZE_NESW
    SDL_SYSTEM_CURSOR_SIZEALL,     // 9  MOUSE_CURSOR_RESIZE_ALL
    SDL_SYSTEM_CURSOR_NO           // 10 MOUSE_CURSOR_NOT_ALLOWED
    //SDL_SYSTEM_CURSOR_WAIT,      // No equivalent implemented on MouseCursor enum on raylib.h
    //SDL_SYSTEM_CURSOR_WAITARROW, // No equivalent implemented on MouseCursor enum on raylib.h
};


// SDL3 Migration Layer made to avoid `ifdefs` inside functions when we can.

void* SDL_GetClipboardData(const char *mime_type, size_t *size) {
    TRACELOG(LOG_WARNING, "Getting clipboard data that is not text is only available in SDL3");
    // We could possibly implement it ourselves in this case for some easier platforms
    return NULL;
}



//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
int InitPlatform(void);                                      // Initialize platform (graphics, inputs and more)
void ClosePlatform(void);                                    // Close platform

static KeyboardKey ConvertScancodeToKey(SDL_Scancode sdlScancode);  // Help convert SDL scancodes to raylib key

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
// NOTE: Functions declaration is provided by raylib.h

//----------------------------------------------------------------------------------
// Module Functions Definition: Window and Graphics Device
//----------------------------------------------------------------------------------

// Check if application should close
bool WindowShouldClose(void)
{
    if (CORE.Window.ready) return CORE.Window.shouldClose;
    else return true;
}

// Toggle fullscreen mode
void ToggleFullscreen(void)
{
    TRACELOG(LOG_WARNING, "ToggleFullscreen() not available on target platform");
}

// Toggle borderless windowed mode
void ToggleBorderlessWindowed(void)
{
    TRACELOG(LOG_WARNING, "ToggleBorderlessWindowed() not available on target platform");
}

// Set window state: maximized, if resizable
void MaximizeWindow(void)
{
    TRACELOG(LOG_WARNING, "MaximizeWindow() not available on target platform");
}

// Set window state: minimized
void MinimizeWindow(void)
{
    TRACELOG(LOG_WARNING, "MinimizeWindow() not available on target platform");
}

// Set window state: minimized
void RestoreWindow(void)
{
    TRACELOG(LOG_WARNING, "RestoreWindow() not available on target platform");
}


// Set window configuration state using flags
void SetWindowState(unsigned int flags)
{
    TRACELOG(LOG_WARNING, "SetWindowState() not available on target platform");
}

// Clear window configuration state flags
void ClearWindowState(unsigned int flags)
{
    TRACELOG(LOG_WARNING, "ClearWindowState() not available on target platform");
}

// Set icon for window
void SetWindowIcon(Image image)
{
    TRACELOG(LOG_WARNING, "SetWindowIcon() not available on target platform");
}

// Set icon for window
void SetWindowIcons(Image *images, int count)
{
    TRACELOG(LOG_WARNING, "SetWindowIcons() not available on target platform");
}

// Set title for window
void SetWindowTitle(const char *title)
{
    SDL_SetWindowTitle(platform.window, title);

    CORE.Window.title = title;
}

// Set window position on screen (windowed mode)
void SetWindowPosition(int x, int y)
{
    TRACELOG(LOG_WARNING, "SetWindowPosition() not available on target platform");
}

// Set monitor for the current window
void SetWindowMonitor(int monitor)
{
    TRACELOG(LOG_WARNING, "SetWindowMonitor() not available on target platform");
}

// Set window minimum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMinSize(int width, int height)
{
    SDL_SetWindowMinimumSize(platform.window, width, height);

    CORE.Window.screenMin.width = width;
    CORE.Window.screenMin.height = height;
}

// Set window maximum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMaxSize(int width, int height)
{
    SDL_SetWindowMaximumSize(platform.window, width, height);

    CORE.Window.screenMax.width = width;
    CORE.Window.screenMax.height = height;
}

// Set window dimensions
void SetWindowSize(int width, int height)
{
    TRACELOG(LOG_WARNING, "SetWindowSize() not available on target platform");
}

// Set window opacity, value opacity is between 0.0 and 1.0
void SetWindowOpacity(float opacity)
{
    TRACELOG(LOG_WARNING, "SetWindowOpacity() not available on target platform");
}

// Set window focused
void SetWindowFocused(void)
{
    TRACELOG(LOG_WARNING, "SetWindowFocused() not available on target platform");
}

// Get native window handle
void *GetWindowHandle(void)
{
    TRACELOG(LOG_WARNING, "GetWindowHandle() not implemented on target platform");
    return NULL;
}

// Get number of monitors
int GetMonitorCount(void)
{
    TRACELOG(LOG_WARNING, "GetMonitorCount() not implemented on target platform");
    return 1;
}

// Get number of monitors
int GetCurrentMonitor(void)
{
    TRACELOG(LOG_WARNING, "GetCurrentMonitor() not implemented on target platform");
    return 0;
}

// Get selected monitor position
Vector2 GetMonitorPosition(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPosition() not implemented on target platform");
    return (Vector2){ 0, 0 };
}

// Get selected monitor width (currently used by monitor)
int GetMonitorWidth(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorWidth() not implemented on target platform");
    return 0;
}

// Get selected monitor height (currently used by monitor)
int GetMonitorHeight(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorHeight() not implemented on target platform");
    return 0;
}

// Get selected monitor physical width in millimetres
int GetMonitorPhysicalWidth(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPhysicalWidth() not implemented on target platform");
    return 0;
}

// Get selected monitor physical height in millimetres
int GetMonitorPhysicalHeight(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPhysicalHeight() not implemented on target platform");
    return 0;
}

// Get selected monitor refresh rate
int GetMonitorRefreshRate(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorRefreshRate() not implemented on target platform");
    return 0;
}

// Get the human-readable, UTF-8 encoded name of the selected monitor
const char *GetMonitorName(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorName() not implemented on target platform");
    return "";
}

// Get window position XY on monitor
Vector2 GetWindowPosition(void)
{
    TRACELOG(LOG_WARNING, "GetWindowPosition() not implemented on target platform");
    return (Vector2){ 0, 0 };
}

// Get window scale DPI factor for current monitor
Vector2 GetWindowScaleDPI(void)
{
    TRACELOG(LOG_WARNING, "GetWindowScaleDPI() not implemented on target platform");
    return (Vector2){ 1.0f, 1.0f };
}

// Set clipboard text content
void SetClipboardText(const char *text)
{
    TRACELOG(LOG_WARNING, "SetClipboardText() not implemented on target platform");
}

// Get clipboard text content
const char *GetClipboardText(void)
{
    TRACELOG(LOG_WARNING, "GetClipboardText() not implemented on target platform");
    return NULL;
}

// Show mouse cursor
void ShowCursor(void)
{
#ifdef PLATFORM_DESKTOP_SDL3
    SDL_ShowCursor();
#else
    SDL_ShowCursor(SDL_ENABLE);
#endif
    CORE.Input.Mouse.cursorHidden = false;
}

// Hides mouse cursor
void HideCursor(void)
{
#ifdef PLATFORM_DESKTOP_SDL3
    SDL_HideCursor();
#else
    SDL_ShowCursor(SDL_DISABLE);
#endif
    CORE.Input.Mouse.cursorHidden = true;
}

// Enables cursor (unlock cursor)
void EnableCursor(void)
{
    SDL_SetRelativeMouseMode(SDL_FALSE);

#ifdef PLATFORM_DESKTOP_SDL3
    // SDL_ShowCursor() has been split into three functions: SDL_ShowCursor(), SDL_HideCursor(), and SDL_CursorVisible()
    SDL_ShowCursor();
#else
    SDL_ShowCursor(SDL_ENABLE);
#endif

    platform.cursorRelative = false;
    CORE.Input.Mouse.cursorHidden = false;
}

// Disables cursor (lock cursor)
void DisableCursor(void)
{
    SDL_SetRelativeMouseMode(SDL_TRUE);

    platform.cursorRelative = true;
    CORE.Input.Mouse.cursorHidden = true;
}

// Swap back buffer with front buffer (screen drawing)
void SwapScreenBuffer(void)
{
    SDL_GL_SwapWindow(platform.window);
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Misc
//----------------------------------------------------------------------------------

// Get elapsed time measure in seconds
double GetTime(void)
{
    unsigned int ms = SDL_GetTicks();    // Elapsed time in milliseconds since SDL_Init()
    double time = (double)ms/1000;
    return time;
}

// Open URL with default system browser (if available)
// NOTE: This function is only safe to use if you control the URL given.
// A user could craft a malicious string performing another action.
// Only call this function yourself not with user input or make sure to check the string yourself.
// Ref: https://github.com/raysan5/raylib/issues/686
void OpenURL(const char *url)
{
    // Security check to (partially) avoid malicious code
    if (strchr(url, '\'') != NULL) TRACELOG(LOG_WARNING, "SYSTEM: Provided URL could be potentially malicious, avoid [\'] character");
    else {
        //todo
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Inputs
//----------------------------------------------------------------------------------

// Set internal gamepad mappings
int SetGamepadMappings(const char *mappings)
{
    return SDL_GameControllerAddMapping(mappings);
}


// Set mouse position XY
void SetMousePosition(int x, int y)
{
    SDL_WarpMouseInWindow(platform.window, x, y);

    CORE.Input.Mouse.currentPosition = (Vector2){ (float)x, (float)y };
    CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
}

// Set mouse cursor
void SetMouseCursor(int cursor)
{
    TRACELOG(LOG_WARNING, "SetMouseCursor() not implemented on target platform");
}

// Get physical key name.
const char *GetKeyName(int key)
{
    return SDL_GetKeyName(key);
}

static void UpdateTouchPointsSDL(SDL_TouchFingerEvent event)
{
#ifdef PLATFORM_DESKTOP_SDL3 // SDL3
    int count = 0;
    SDL_Finger **fingers = SDL_GetTouchFingers(event.touchID, &count);
    CORE.Input.Touch.pointCount = count;

    for (int i = 0; i < CORE.Input.Touch.pointCount; i++)
    {
        SDL_Finger *finger = fingers[i];
        CORE.Input.Touch.pointId[i] = finger->id;
        CORE.Input.Touch.position[i].x = finger->x*CORE.Window.screen.width;
        CORE.Input.Touch.position[i].y = finger->y*CORE.Window.screen.height;
        CORE.Input.Touch.currentTouchState[i] = 1;
    }
    SDL_free(fingers);
#else // SDL2

    CORE.Input.Touch.pointCount = SDL_GetNumTouchFingers(event.touchId);

    for (int i = 0; i < CORE.Input.Touch.pointCount; i++)
    {
        SDL_Finger *finger = SDL_GetTouchFinger(event.touchId, i);
        CORE.Input.Touch.pointId[i] = finger->id;
        CORE.Input.Touch.position[i].x = finger->x*CORE.Window.screen.width;
        CORE.Input.Touch.position[i].y = finger->y*CORE.Window.screen.height;
        CORE.Input.Touch.currentTouchState[i] = 1;
    }
#endif
    for (int i = CORE.Input.Touch.pointCount; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.currentTouchState[i] = 0;
}

// Register all input events
void PollInputEvents(void)
{
#if defined(SUPPORT_GESTURES_SYSTEM)
    // NOTE: Gestures update must be called every frame to reset gestures correctly
    // because ProcessGestureEvent() is just called on an event, not every frame
    UpdateGestures();
#endif

    // Reset keys/chars pressed registered
    CORE.Input.Keyboard.keyPressedQueueCount = 0;
    CORE.Input.Keyboard.charPressedQueueCount = 0;

    // Reset mouse wheel
    CORE.Input.Mouse.currentWheelMove.x = 0;
    CORE.Input.Mouse.currentWheelMove.y = 0;

    // Register previous mouse position
    if (platform.cursorRelative) CORE.Input.Mouse.currentPosition = (Vector2){ 0.0f, 0.0f };
    else CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;

    // Reset last gamepad button/axis registered state
    for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++)
    {
        // Check if gamepad is available
        if (CORE.Input.Gamepad.ready[i])
        {
            // Register previous gamepad button states
            for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++)
            {
                CORE.Input.Gamepad.previousButtonState[i][k] = CORE.Input.Gamepad.currentButtonState[i][k];
            }
        }
    }

    // Register previous touch states
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.previousTouchState[i] = CORE.Input.Touch.currentTouchState[i];

    // Map touch position to mouse position for convenience
    //CORE.Input.Touch.position[0] = CORE.Input.Mouse.currentPosition;

    int touchAction = -1;       // 0-TOUCH_ACTION_UP, 1-TOUCH_ACTION_DOWN, 2-TOUCH_ACTION_MOVE
    bool realTouch = false;     // Flag to differentiate real touch gestures from mouse ones

    // Register previous keys states
    // NOTE: Android supports up to 260 keys
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++)
    {
        CORE.Input.Keyboard.previousKeyState[i] = CORE.Input.Keyboard.currentKeyState[i];
        CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;
    }

    // Register previous mouse states
    for (int i = 0; i < MAX_MOUSE_BUTTONS; i++) CORE.Input.Mouse.previousButtonState[i] = CORE.Input.Mouse.currentButtonState[i];

    // Poll input events for current platform
    //-----------------------------------------------------------------------------
    /*
    // WARNING: Indexes into this array are obtained by using SDL_Scancode values, not SDL_Keycode values
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < 256; ++i)
    {
        CORE.Input.Keyboard.currentKeyState[i] = keys[i];
        //if (keys[i]) TRACELOG(LOG_WARNING, "Pressed key: %i", i);
    }
    */

    CORE.Window.resizedLastFrame = false;

    SDL_Event event = { 0 };
    while (SDL_PollEvent(&event) != 0)
    {
        // All input events can be processed after polling
        switch (event.type)
        {
            case SDL_QUIT: CORE.Window.shouldClose = true; break;

            case SDL_DROPFILE:      // Dropped file
            {
                if (CORE.Window.dropFileCount == 0)
                {
                    // When a new file is dropped, we reserve a fixed number of slots for all possible dropped files
                    // at the moment we limit the number of drops at once to 1024 files but this behaviour should probably be reviewed
                    // TODO: Pointers should probably be reallocated for any new file added...
                    CORE.Window.dropFilepaths = (char **)RL_CALLOC(1024, sizeof(char *));

                    CORE.Window.dropFilepaths[CORE.Window.dropFileCount] = (char *)RL_CALLOC(MAX_FILEPATH_LENGTH, sizeof(char));
                #ifdef PLATFORM_DESKTOP_SDL3
                    // const char *data;   /**< The text for SDL_EVENT_DROP_TEXT and the file name for SDL_EVENT_DROP_FILE, NULL for other events */
                    // Event memory is now managed by SDL, so you should not free the data in SDL_EVENT_DROP_FILE, and if you want to hold onto the text in SDL_EVENT_TEXT_EDITING and SDL_EVENT_TEXT_INPUT events, you should make a copy of it. SDL_TEXTINPUTEVENT_TEXT_SIZE is no longer necessary and has been removed.
                    strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.data);
                #else
                    strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.file);
                    SDL_free(event.drop.file);
                #endif

                    CORE.Window.dropFileCount++;
                }
                else if (CORE.Window.dropFileCount < 1024)
                {
                    CORE.Window.dropFilepaths[CORE.Window.dropFileCount] = (char *)RL_CALLOC(MAX_FILEPATH_LENGTH, sizeof(char));
                #ifdef PLATFORM_DESKTOP_SDL3
                    strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.data);
                #else
                    strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.file);
                    SDL_free(event.drop.file);
                #endif

                    CORE.Window.dropFileCount++;
                }
                else TRACELOG(LOG_WARNING, "FILE: Maximum drag and drop files at once is limited to 1024 files!");

            } break;

            // Window events are also polled (Minimized, maximized, close...)

        #ifndef PLATFORM_DESKTOP_SDL3
            // SDL3 states:
            //     The SDL_WINDOWEVENT_* events have been moved to top level events,
            //     and SDL_WINDOWEVENT has been removed.
            //     In general, handling this change just means checking for the individual events instead of first checking for SDL_WINDOWEVENT
            //     and then checking for window events. You can compare the event >= SDL_EVENT_WINDOW_FIRST and <= SDL_EVENT_WINDOW_LAST if you need to see whether it's a window event.
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
        #endif
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {
                        const int width = event.window.data1;
                        const int height = event.window.data2;
                        SetupViewport(width, height);
                        CORE.Window.screen.width = width;
                        CORE.Window.screen.height = height;
                        CORE.Window.currentFbo.width = width;
                        CORE.Window.currentFbo.height = height;
                        CORE.Window.resizedLastFrame = true;
                    } break;
                    case SDL_WINDOWEVENT_ENTER:
                    {
                        CORE.Input.Mouse.cursorOnScreen = true;
                    } break;
                    case SDL_WINDOWEVENT_LEAVE:
                    {
                        CORE.Input.Mouse.cursorOnScreen = false;
                    } break;
                    case SDL_WINDOWEVENT_HIDDEN:
                    case SDL_WINDOWEVENT_MINIMIZED:
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                    case SDL_WINDOWEVENT_SHOWN:
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                    case SDL_WINDOWEVENT_MAXIMIZED:
                    case SDL_WINDOWEVENT_RESTORED:
            #ifdef PLATFORM_DESKTOP_SDL3
                        break;
            #else
                    default: break;
                }
            } break;
            #endif

            // Keyboard events
            case SDL_KEYDOWN:
            {
            #ifdef PLATFORM_DESKTOP_SDL3
                // SDL3 Migration: The following structures have been removed: * SDL_Keysym
                KeyboardKey key = ConvertScancodeToKey(event.key.scancode);
            #else
                KeyboardKey key = ConvertScancodeToKey(event.key.keysym.scancode);
            #endif

                if (key != KEY_NULL)
                {
                    // If key was up, add it to the key pressed queue
                    if ((CORE.Input.Keyboard.currentKeyState[key] == 0) && (CORE.Input.Keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE))
                    {
                        CORE.Input.Keyboard.keyPressedQueue[CORE.Input.Keyboard.keyPressedQueueCount] = key;
                        CORE.Input.Keyboard.keyPressedQueueCount++;
                    }

                    CORE.Input.Keyboard.currentKeyState[key] = 1;
                }

                if (event.key.repeat) CORE.Input.Keyboard.keyRepeatInFrame[key] = 1;

                // TODO: Put exitKey verification outside the switch?
                if (CORE.Input.Keyboard.currentKeyState[CORE.Input.Keyboard.exitKey])
                {
                    CORE.Window.shouldClose = true;
                }
            } break;

            case SDL_KEYUP:
            {

            #ifdef PLATFORM_DESKTOP_SDL3
                KeyboardKey key = ConvertScancodeToKey(event.key.scancode);
            #else
                KeyboardKey key = ConvertScancodeToKey(event.key.keysym.scancode);
            #endif
                if (key != KEY_NULL) CORE.Input.Keyboard.currentKeyState[key] = 0;
            } break;

            case SDL_TEXTINPUT:
            {
                // NOTE: event.text.text data comes an UTF-8 text sequence but we register codepoints (int)

                int codepointSize = 0;

                // Check if there is space available in the queue
                if (CORE.Input.Keyboard.charPressedQueueCount < MAX_CHAR_PRESSED_QUEUE)
                {
                    // Add character (codepoint) to the queue
                    CORE.Input.Keyboard.charPressedQueue[CORE.Input.Keyboard.charPressedQueueCount] = GetCodepointNext(event.text.text, &codepointSize);
                    CORE.Input.Keyboard.charPressedQueueCount++;
                }
            } break;

            // Check mouse events
            case SDL_MOUSEBUTTONDOWN:
            {
                // NOTE: SDL2 mouse button order is LEFT, MIDDLE, RIGHT, but raylib uses LEFT, RIGHT, MIDDLE like GLFW
                //       The following conditions align SDL with raylib.h MouseButton enum order
                int btn = event.button.button - 1;
                if (btn == 2) btn = 1;
                else if (btn == 1) btn = 2;

                CORE.Input.Mouse.currentButtonState[btn] = 1;
                CORE.Input.Touch.currentTouchState[btn] = 1;

                touchAction = 1;
            } break;
            case SDL_MOUSEBUTTONUP:
            {
                // NOTE: SDL2 mouse button order is LEFT, MIDDLE, RIGHT, but raylib uses LEFT, RIGHT, MIDDLE like GLFW
                //       The following conditions align SDL with raylib.h MouseButton enum order
                int btn = event.button.button - 1;
                if (btn == 2) btn = 1;
                else if (btn == 1) btn = 2;

                CORE.Input.Mouse.currentButtonState[btn] = 0;
                CORE.Input.Touch.currentTouchState[btn] = 0;

                touchAction = 0;
            } break;
            case SDL_MOUSEWHEEL:
            {
                CORE.Input.Mouse.currentWheelMove.x = (float)event.wheel.x;
                CORE.Input.Mouse.currentWheelMove.y = (float)event.wheel.y;
            } break;
            case SDL_MOUSEMOTION:
            {
                if (platform.cursorRelative)
                {
                    CORE.Input.Mouse.currentPosition.x = (float)event.motion.xrel;
                    CORE.Input.Mouse.currentPosition.y = (float)event.motion.yrel;
                    CORE.Input.Mouse.previousPosition = (Vector2){ 0.0f, 0.0f };
                }
                else
                {
                    CORE.Input.Mouse.currentPosition.x = (float)event.motion.x;
                    CORE.Input.Mouse.currentPosition.y = (float)event.motion.y;
                }

                //CORE.Input.Touch.position[0] = CORE.Input.Mouse.currentPosition;
                //touchAction = 2;
            } break;

            case SDL_FINGERDOWN:
            {
                UpdateTouchPointsSDL(event.tfinger);
                touchAction = 1;
                realTouch = true;
            } break;
            case SDL_FINGERUP:
            {
                UpdateTouchPointsSDL(event.tfinger);
                touchAction = 0;
                realTouch = true;
            } break;
            case SDL_FINGERMOTION:
            {
                UpdateTouchPointsSDL(event.tfinger);
                touchAction = 2;
                realTouch = true;
            } break;

            // Check gamepad events
            case SDL_JOYDEVICEADDED:
            {
                int jid = event.jdevice.which;

                if (!CORE.Input.Gamepad.ready[jid] && (jid < MAX_GAMEPADS))
                {
                    platform.gamepad[jid] = SDL_GameControllerOpen(jid);

                    if (platform.gamepad[jid])
                    {
                        CORE.Input.Gamepad.ready[jid] = true;
                        CORE.Input.Gamepad.axisCount[jid] = SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(platform.gamepad[jid]));
                        CORE.Input.Gamepad.axisState[jid][GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
                        CORE.Input.Gamepad.axisState[jid][GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
                        strncpy(CORE.Input.Gamepad.name[jid], SDL_GameControllerNameForIndex(jid), 63);
                        CORE.Input.Gamepad.name[jid][63] = '\0';
                    }
                    else
                    {
                        TRACELOG(LOG_WARNING, "PLATFORM: Unable to open game controller [ERROR: %s]", SDL_GetError());
                    }
                }
            } break;
            case SDL_JOYDEVICEREMOVED:
            {
                int jid = event.jdevice.which;

                if (jid == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(platform.gamepad[jid])))
                {
                    SDL_GameControllerClose(platform.gamepad[jid]);
                    platform.gamepad[jid] = SDL_GameControllerOpen(0);
                    CORE.Input.Gamepad.ready[jid] = false;
                    memset(CORE.Input.Gamepad.name[jid], 0, 64);
                }
            } break;
            case SDL_CONTROLLERBUTTONDOWN:
            {
                int button = -1;

                switch (event.jbutton.button)
                {
                    case SDL_CONTROLLER_BUTTON_Y: button = GAMEPAD_BUTTON_RIGHT_FACE_UP; break;
                    case SDL_CONTROLLER_BUTTON_B: button = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; break;
                    case SDL_CONTROLLER_BUTTON_A: button = GAMEPAD_BUTTON_RIGHT_FACE_DOWN; break;
                    case SDL_CONTROLLER_BUTTON_X: button = GAMEPAD_BUTTON_RIGHT_FACE_LEFT; break;

                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: button = GAMEPAD_BUTTON_LEFT_TRIGGER_1; break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: button = GAMEPAD_BUTTON_RIGHT_TRIGGER_1; break;

                    case SDL_CONTROLLER_BUTTON_BACK: button = GAMEPAD_BUTTON_MIDDLE_LEFT; break;
                    case SDL_CONTROLLER_BUTTON_GUIDE: button = GAMEPAD_BUTTON_MIDDLE; break;
                    case SDL_CONTROLLER_BUTTON_START: button = GAMEPAD_BUTTON_MIDDLE_RIGHT; break;

                    case SDL_CONTROLLER_BUTTON_DPAD_UP: button = GAMEPAD_BUTTON_LEFT_FACE_UP; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: button = GAMEPAD_BUTTON_LEFT_FACE_RIGHT; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: button = GAMEPAD_BUTTON_LEFT_FACE_DOWN; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: button = GAMEPAD_BUTTON_LEFT_FACE_LEFT; break;

                    case SDL_CONTROLLER_BUTTON_LEFTSTICK: button = GAMEPAD_BUTTON_LEFT_THUMB; break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: button = GAMEPAD_BUTTON_RIGHT_THUMB; break;
                    default: break;
                }

                if (button >= 0)
                {
                    CORE.Input.Gamepad.currentButtonState[event.jbutton.which][button] = 1;
                    CORE.Input.Gamepad.lastButtonPressed = button;
                }
            } break;
            case SDL_CONTROLLERBUTTONUP:
            {
                int button = -1;

                switch (event.jbutton.button)
                {
                    case SDL_CONTROLLER_BUTTON_Y: button = GAMEPAD_BUTTON_RIGHT_FACE_UP; break;
                    case SDL_CONTROLLER_BUTTON_B: button = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; break;
                    case SDL_CONTROLLER_BUTTON_A: button = GAMEPAD_BUTTON_RIGHT_FACE_DOWN; break;
                    case SDL_CONTROLLER_BUTTON_X: button = GAMEPAD_BUTTON_RIGHT_FACE_LEFT; break;

                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: button = GAMEPAD_BUTTON_LEFT_TRIGGER_1; break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: button = GAMEPAD_BUTTON_RIGHT_TRIGGER_1; break;

                    case SDL_CONTROLLER_BUTTON_BACK: button = GAMEPAD_BUTTON_MIDDLE_LEFT; break;
                    case SDL_CONTROLLER_BUTTON_GUIDE: button = GAMEPAD_BUTTON_MIDDLE; break;
                    case SDL_CONTROLLER_BUTTON_START: button = GAMEPAD_BUTTON_MIDDLE_RIGHT; break;

                    case SDL_CONTROLLER_BUTTON_DPAD_UP: button = GAMEPAD_BUTTON_LEFT_FACE_UP; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: button = GAMEPAD_BUTTON_LEFT_FACE_RIGHT; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: button = GAMEPAD_BUTTON_LEFT_FACE_DOWN; break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: button = GAMEPAD_BUTTON_LEFT_FACE_LEFT; break;

                    case SDL_CONTROLLER_BUTTON_LEFTSTICK: button = GAMEPAD_BUTTON_LEFT_THUMB; break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: button = GAMEPAD_BUTTON_RIGHT_THUMB; break;
                    default: break;
                }

                if (button >= 0)
                {
                    CORE.Input.Gamepad.currentButtonState[event.jbutton.which][button] = 0;
                    if (CORE.Input.Gamepad.lastButtonPressed == button) CORE.Input.Gamepad.lastButtonPressed = 0;
                }
            } break;
            case SDL_CONTROLLERAXISMOTION:
            {
                int axis = -1;

                switch (event.jaxis.axis)
                {
                    case SDL_CONTROLLER_AXIS_LEFTX: axis = GAMEPAD_AXIS_LEFT_X; break;
                    case SDL_CONTROLLER_AXIS_LEFTY: axis = GAMEPAD_AXIS_LEFT_Y; break;
                    case SDL_CONTROLLER_AXIS_RIGHTX: axis = GAMEPAD_AXIS_RIGHT_X; break;
                    case SDL_CONTROLLER_AXIS_RIGHTY: axis = GAMEPAD_AXIS_RIGHT_Y; break;
                    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: axis = GAMEPAD_AXIS_LEFT_TRIGGER; break;
                    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: axis = GAMEPAD_AXIS_RIGHT_TRIGGER; break;
                    default: break;
                }

                if (axis >= 0)
                {
                    // SDL axis value range is -32768 to 32767, we normalize it to RayLib's -1.0 to 1.0f range
                    float value = event.jaxis.value/(float)32767;
                    CORE.Input.Gamepad.axisState[event.jaxis.which][axis] = value;

                    // Register button state for triggers in addition to their axes
                    if ((axis == GAMEPAD_AXIS_LEFT_TRIGGER) || (axis == GAMEPAD_AXIS_RIGHT_TRIGGER))
                    {
                        int button = (axis == GAMEPAD_AXIS_LEFT_TRIGGER)? GAMEPAD_BUTTON_LEFT_TRIGGER_2 : GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
                        int pressed = (value > 0.1f);
                        CORE.Input.Gamepad.currentButtonState[event.jaxis.which][button] = pressed;
                        if (pressed) CORE.Input.Gamepad.lastButtonPressed = button;
                        else if (CORE.Input.Gamepad.lastButtonPressed == button) CORE.Input.Gamepad.lastButtonPressed = 0;
                    }
                }
            } break;
            default: break;
        }

#if defined(SUPPORT_GESTURES_SYSTEM)
        if (touchAction > -1)
        {
            // Process mouse events as touches to be able to use mouse-gestures
            GestureEvent gestureEvent = { 0 };

            // Register touch actions
            gestureEvent.touchAction = touchAction;

            // Assign a pointer ID
            gestureEvent.pointId[0] = 0;

            // Register touch points count
            gestureEvent.pointCount = 1;

            // Register touch points position, only one point registered
            gestureEvent.position[0] = CORE.Input.Touch.position[0];

            // Normalize gestureEvent.position[0] for CORE.Window.screen.width and CORE.Window.screen.height
            gestureEvent.position[0].x /= (float)GetScreenWidth();
            gestureEvent.position[0].y /= (float)GetScreenHeight();

            // Gesture data is sent to gestures-system for processing
            ProcessGestureEvent(gestureEvent);

            touchAction = -1;
        }
#endif
    }
    //-----------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

#define HAVE_GL_HEADERS
#include <vitaGL.h>

// Initialize platform: graphics, inputs and more
int InitPlatform(void)
{
	vglSetSemanticBindingMode(VGL_MODE_POSTPONED);
	vglInitWithCustomThreshold(0, 960, 544, 8 * 1024 * 1024, 0, 0, 26 * 1024 * 1024, SCE_GXM_MULTISAMPLE_4X);
	
    //SDL_setenv("VITA_DISABLE_TOUCH_FRONT", "1", 1);
    SDL_setenv("VITA_DISABLE_TOUCH_BACK", "1", 1);
    // Initialize SDL internal global state, only required systems
    // NOTE: Not all systems need to be initialized, SDL_INIT_AUDIO is not required, managed by miniaudio
    int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    if (result < 0) { TRACELOG(LOG_WARNING, "SDL: Failed to initialize SDL"); return -1; }

    // Initialize graphic device: display/window and graphic context
    //----------------------------------------------------------------------------
    unsigned int flags = 0;
    flags = SDL_WINDOW_OPENGL;

    //if ((CORE.Window.flags & FLAG_WINDOW_TRANSPARENT) > 0) flags |= SDL_WINDOW_TRANSPARENT;     // Alternative: SDL_GL_ALPHA_SIZE = 8

    //if ((CORE.Window.flags & FLAG_FULLSCREEN_DESKTOP) > 0) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    // NOTE: Some OpenGL context attributes must be set before window creation

    // Check selection OpenGL version
    /*
    if (rlGetVersion() == RL_OPENGL_21)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    }
    else if (rlGetVersion() == RL_OPENGL_ES_20)                 // Request OpenGL ES 2.0 context
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    }
    */
    // Init window

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    platform.window = SDL_CreateWindow("", 0, 0, 960, 544,flags);
    if(platform.window == NULL){
        TRACELOG(LOG_ERROR, SDL_GetError());
    }
    // Init OpenGL context
    platform.glContext = SDL_GL_CreateContext(platform.window);
    if(platform.glContext == NULL){
        TRACELOG(LOG_ERROR, SDL_GetError());
    }
    // Check window and glContext have been initialized successfully
    if ((platform.window != NULL) && (platform.glContext != NULL))
    {
        CORE.Window.ready = true;

        CORE.Window.display.width = 960;
        CORE.Window.display.height = 544;

        CORE.Window.render.width = CORE.Window.screen.width;
        CORE.Window.render.height = CORE.Window.screen.height;
        CORE.Window.currentFbo.width = CORE.Window.render.width;
        CORE.Window.currentFbo.height = CORE.Window.render.height;

        TRACELOG(LOG_INFO, "DISPLAY: Device initialized successfully");
        TRACELOG(LOG_INFO, "    > Display size: %i x %i", CORE.Window.display.width, CORE.Window.display.height);
        TRACELOG(LOG_INFO, "    > Screen size:  %i x %i", CORE.Window.screen.width, CORE.Window.screen.height);
        TRACELOG(LOG_INFO, "    > Render size:  %i x %i", CORE.Window.render.width, CORE.Window.render.height);
        TRACELOG(LOG_INFO, "    > Viewport offsets: %i, %i", CORE.Window.renderOffset.x, CORE.Window.renderOffset.y);

        if (CORE.Window.flags & FLAG_VSYNC_HINT) SDL_GL_SetSwapInterval(1);
        else SDL_GL_SetSwapInterval(0);
    }
    else
    {
        TRACELOG(LOG_FATAL, "PLATFORM: Failed to initialize graphics device");
        return -1;
    }

    // Load OpenGL extensions
    // NOTE: GL procedures address loader is required to load extensions
    rlLoadExtensions(SDL_GL_GetProcAddress);
    //----------------------------------------------------------------------------

    // Initialize input events system
    //----------------------------------------------------------------------------
    // Initialize gamepads
    for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++)
    {
        platform.gamepad[i] = SDL_GameControllerOpen(i);

        if (platform.gamepad[i])
        {
            CORE.Input.Gamepad.ready[i] = true;
            CORE.Input.Gamepad.axisCount[i] = SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(platform.gamepad[i]));
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
            strncpy(CORE.Input.Gamepad.name[i], SDL_GameControllerNameForIndex(i), 63);
            CORE.Input.Gamepad.name[i][63] = '\0';
        }
        else TRACELOG(LOG_WARNING, "PLATFORM: Unable to open game controller [ERROR: %s]", SDL_GetError());
    }

    // Disable mouse events being interpreted as touch events
    // NOTE: This is wanted because there are SDL_FINGER* events available which provide unique data
    //       Due to the way PollInputEvents() and rgestures.h are currently implemented, setting this won't break SUPPORT_MOUSE_GESTURES
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    //----------------------------------------------------------------------------

    // Initialize timing system
    //----------------------------------------------------------------------------
    // NOTE: No need to call InitTimer(), let SDL manage it internally
    CORE.Time.previous = GetTime();     // Get time as double

    #if defined(_WIN32) && defined(SUPPORT_WINMM_HIGHRES_TIMER) && !defined(SUPPORT_BUSY_WAIT_LOOP)
    SDL_SetHint(SDL_HINT_TIMER_RESOLUTION, "1"); // SDL equivalent of timeBeginPeriod() and timeEndPeriod()
    #endif
    //----------------------------------------------------------------------------

    // Initialize storage system
    //----------------------------------------------------------------------------
    // Define base path for storage
    CORE.Storage.basePath = SDL_GetBasePath(); // Alternative: GetWorkingDirectory();
    //----------------------------------------------------------------------------


#ifdef PLATFORM_DESKTOP_SDL3
    TRACELOG(LOG_INFO, "PLATFORM: DESKTOP (SDL3): Initialized successfully");
#else
    TRACELOG(LOG_INFO, "PLATFORM: DESKTOP (SDL): Initialized successfully");
#endif

    return 0;
}

// Close platform
void ClosePlatform(void)
{
    SDL_FreeCursor(platform.cursor); // Free cursor
    SDL_GL_DeleteContext(platform.glContext); // Deinitialize OpenGL context
    SDL_DestroyWindow(platform.window);
    SDL_Quit(); // Deinitialize SDL internal global state
}

// Scancode to keycode mapping
static KeyboardKey ConvertScancodeToKey(SDL_Scancode sdlScancode)
{
    if (sdlScancode >= 0 && sdlScancode < SCANCODE_MAPPED_NUM)
    {
        return mapScancodeToKey[sdlScancode];
    }

    return KEY_NULL; // No equivalent key in Raylib
}
// EOF
