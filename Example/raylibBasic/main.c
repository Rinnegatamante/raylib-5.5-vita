#include <stdio.h>
#include <psp2/kernel/clib.h>
#include "raylib.h"

int _newlib_heap_size_user = 12 * 1024 * 1024;

#define MAX_TOUCH_POINTS 10

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 960;
    const int screenHeight = 544;

    SetConfigFlags(FLAG_MSAA_4X_HINT); // Set MSAA 4X hint before windows creation

    InitWindow(screenWidth, screenHeight, "raylib [core] example - input multitouch");

    Vector2 touchPositions[MAX_TOUCH_POINTS] = {0};

    // Set axis deadzones
    const float leftStickDeadzoneX = 0.1f;
    const float leftStickDeadzoneY = 0.1f;
    const float rightStickDeadzoneX = 0.1f;
    const float rightStickDeadzoneY = 0.1f;
    const float leftTriggerDeadzone = -0.9f;
    const float rightTriggerDeadzone = -0.9f;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------
    int gamepad = 0; // which gamepad to display
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Get the touch point count ( how many fingers are touching the screen )
        int tCount = GetTouchPointCount();
        // Clamp touch points available ( set the maximum touch points allowed )
        if (tCount > MAX_TOUCH_POINTS)
            tCount = MAX_TOUCH_POINTS;
        // Get touch points positions
        for (int i = 0; i < tCount; ++i)
            touchPositions[i] = GetTouchPosition(i);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_LEFT) && gamepad > 0)
            gamepad--;
        if (IsKeyPressed(KEY_RIGHT))
            gamepad++;

        if (IsGamepadAvailable(gamepad))
        {
            DrawText(TextFormat("GP%d: %s", gamepad, GetGamepadName(gamepad)), 10, 10, 20, BLACK);

            // Get axis values
            float leftStickX = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_X);
            float leftStickY = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_Y);
            float rightStickX = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_X);
            float rightStickY = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_Y);
            float leftTrigger = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_TRIGGER);
            float rightTrigger = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_TRIGGER);

            // Calculate deadzones
            if (leftStickX > -leftStickDeadzoneX && leftStickX < leftStickDeadzoneX)
                leftStickX = 0.0f;
            if (leftStickY > -leftStickDeadzoneY && leftStickY < leftStickDeadzoneY)
                leftStickY = 0.0f;
            if (rightStickX > -rightStickDeadzoneX && rightStickX < rightStickDeadzoneX)
                rightStickX = 0.0f;
            if (rightStickY > -rightStickDeadzoneY && rightStickY < rightStickDeadzoneY)
                rightStickY = 0.0f;
            if (leftTrigger < leftTriggerDeadzone)
                leftTrigger = -1.0f;
            if (rightTrigger < rightTriggerDeadzone)
                rightTrigger = -1.0f;
            DrawRectangleRounded((Rectangle){175, 110, 460, 220}, 0.3f, 16, DARKGRAY);

            // Draw buttons: basic
            DrawCircle(365, 170, 12, RAYWHITE);
            DrawCircle(405, 170, 12, RAYWHITE);
            DrawCircle(445, 170, 12, RAYWHITE);
            DrawCircle(516, 191, 17, RAYWHITE);
            DrawCircle(551, 227, 17, RAYWHITE);
            DrawCircle(587, 191, 17, RAYWHITE);
            DrawCircle(551, 155, 17, RAYWHITE);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_MIDDLE_LEFT))
                DrawCircle(365, 170, 10, RED);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_MIDDLE))
                DrawCircle(405, 170, 10, GREEN);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_MIDDLE_RIGHT))
                DrawCircle(445, 170, 10, BLUE);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
                DrawCircle(516, 191, 15, GOLD);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                DrawCircle(551, 227, 15, BLUE);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
                DrawCircle(587, 191, 15, GREEN);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_UP))
                DrawCircle(551, 155, 15, RED);

            // Draw buttons: d-pad
            DrawRectangle(245, 145, 28, 88, RAYWHITE);
            DrawRectangle(215, 174, 88, 29, RAYWHITE);
            DrawRectangle(247, 147, 24, 84, BLACK);
            DrawRectangle(217, 176, 84, 25, BLACK);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP))
                DrawRectangle(247, 147, 24, 29, RED);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
                DrawRectangle(247, 147 + 54, 24, 30, RED);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
                DrawRectangle(217, 176, 30, 25, RED);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
                DrawRectangle(217 + 54, 176, 30, 25, RED);

            // Draw buttons: left-right back
            DrawRectangleRounded((Rectangle){215, 98, 100, 10}, 0.5f, 16, DARKGRAY);
            DrawRectangleRounded((Rectangle){495, 98, 100, 10}, 0.5f, 16, DARKGRAY);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
                DrawRectangleRounded((Rectangle){215, 98, 100, 10}, 0.5f, 16, RED);
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))
                DrawRectangleRounded((Rectangle){495, 98, 100, 10}, 0.5f, 16, RED);

            // Draw axis: left joystick
            Color leftGamepadColor = BLACK;
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_THUMB))
                leftGamepadColor = RED;
            DrawCircle(345, 260, 40, BLACK);
            DrawCircle(345, 260, 35, LIGHTGRAY);
            DrawCircle(345 + (int)(leftStickX * 20),
                       260 + (int)(leftStickY * 20), 25, leftGamepadColor);

            // Draw axis: right joystick
            Color rightGamepadColor = BLACK;
            if (IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_THUMB))
                rightGamepadColor = RED;
            DrawCircle(465, 260, 40, BLACK);
            DrawCircle(465, 260, 35, LIGHTGRAY);
            DrawCircle(465 + (int)(rightStickX * 20),
                       260 + (int)(rightStickY * 20), 25, rightGamepadColor);

            // Draw axis: left-right triggers
            DrawRectangle(151, 110, 15, 70, GRAY);
            DrawRectangle(644, 110, 15, 70, GRAY);
            DrawRectangle(151, 110, 15, (int)(((1 + leftTrigger) / 2) * 70), RED);
            DrawRectangle(644, 110, 15, (int)(((1 + rightTrigger) / 2) * 70), RED);
            DrawText(TextFormat("DETECTED AXIS [%i]:", GetGamepadAxisCount(0)), 10, 50, 20, MAROON);

            for (int i = 0; i < GetGamepadAxisCount(0); i++)
            {
                DrawText(TextFormat("AXIS %i: %.02f", i, GetGamepadAxisMovement(0, i)), 15, 70 + 30 * i, 20, DARKGRAY);
            }

            if (GetGamepadButtonPressed() != GAMEPAD_BUTTON_UNKNOWN)
                DrawText(TextFormat("DETECTED BUTTON: %i", GetGamepadButtonPressed()), 10, 520, 20, RED);
            else
                DrawText("DETECTED BUTTON: NONE", 10, 520, 20, GRAY);
        }
        else
        {
            DrawText(TextFormat("GP%d: NOT DETECTED", gamepad), 10, 10, 20, GRAY);
        }

        for (int i = 0; i < tCount; ++i)
        {
            // Make sure point is not (0, 0) as this means there is no touch for it
            if ((touchPositions[i].x > 0) && (touchPositions[i].y > 0))
            {
                // Draw circle and touch index number
                DrawCircleV(touchPositions[i], 34, ORANGE);
                DrawText(TextFormat("%d", i), (int)touchPositions[i].x - 10, (int)touchPositions[i].y - 70, 40, BLACK);
            }
        }

        DrawText("touch the screen at multiple locations to get multiple balls", 10, 30, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}