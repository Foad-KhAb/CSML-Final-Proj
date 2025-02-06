#include <iostream>
#include "raylib.h"
#include <cmath>

using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

const int screen_width = 1200;
const int screen_height = 800;

int cpu_point = 0;
int player_point = 0;

Texture2D soccer_ball_texture; // Global texture to manage loading/unloading

// Game modes
enum GameMode {
    REGULAR,
    CURVE,
    SINUS_COSINUS
};

// BALL CLASS
class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    float speed = 7; // Constant speed for Sinus/Cosinus mode
    int radius;
    float rotation = 0; // Rotation angle
    float rotation_speed = 0.03; // Rotation speed
    float curve_intensity = 0.02; // Intensity of the curve
    float angle = 0.0f; // Angle for curved movement
    GameMode mode = REGULAR; // Default mode

    void Draw() {
        DrawTexturePro(
            soccer_ball_texture,
            (Rectangle){0, 0, (float)soccer_ball_texture.width, (float)soccer_ball_texture.height},
            (Rectangle){x - radius, y - radius, radius * 2, radius * 2},
            (Vector2){radius, radius},
            rotation,
            WHITE
        );
    }

    void Update() {
        switch (mode) {
            case REGULAR:
                // Regular movement
                x += speed_x;
                y += speed_y;

                // Collision with screen borders
                if (y + radius >= GetScreenHeight() || y - radius <= 0) {
                    speed_y *= -1;
                }

                if (x + radius >= GetScreenWidth()) { // CPU scores
                    cpu_point++;
                    ResetBall();
                }
                if (x - radius <= 0) { // Player scores
                    player_point++;
                    ResetBall();
                }
                break;

            case CURVE:
                // Apply curved movement
                angle += curve_intensity;
                x += speed_x * cos(angle);
                y += speed_y * sin(angle);

                // Spinning effect
                rotation += rotation_speed;
                if (rotation >= 360) rotation -= 360;

                // Collision with screen borders
                if (y + radius >= GetScreenHeight() || y - radius <= 0) {
                    speed_y *= -1;
                    curve_intensity *= -1; // Reverse curve direction on vertical bounce
                }

                if (x + radius >= GetScreenWidth()) { // CPU scores
                    cpu_point++;
                    ResetBall();
                }
                if (x - radius <= 0) { // Player scores
                    player_point++;
                    ResetBall();
                }
                break;

            case SINUS_COSINUS:
                // Apply curved movement using sine and cosine
                angle += curve_intensity;
                x += speed * cos(angle);
                y += speed * sin(angle);

                // Spinning effect
                rotation += rotation_speed;
                if (rotation >= 360) rotation -= 360;

                // Collision with screen borders
                if (y + radius >= GetScreenHeight() || y - radius <= 0) {
                    curve_intensity *= -1; // Reverse curve direction on vertical bounce
                }

                if (x + radius >= GetScreenWidth()) { // CPU scores
                    cpu_point++;
                    ResetBall();
                }
                if (x - radius <= 0) { // Player scores
                    player_point++;
                    ResetBall();
                }
                break;
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        if (mode == REGULAR || mode == CURVE) {
            int speed_choice[2] = {-1, 1};
            speed_x = speed_choice[GetRandomValue(0, 1)] * 7;
            speed_y = speed_choice[GetRandomValue(0, 1)] * 7;
        }

        rotation_speed = GetRandomValue(3, 8); // Random spin speed
        curve_intensity = (GetRandomValue(-5, 5) / 100.0f); // Random curve intensity

        if (mode == SINUS_COSINUS) {
            angle = GetRandomValue(0, 360) * (PI / 180.0f); // Random starting angle in radians
        } else {
            angle = 0.0f; // Reset angle for other modes
        }
    }

    void SetMode(GameMode newMode) {
        mode = newMode;
        ResetBall(); // Reset ball when mode changes
    }
};

// PADDLE CLASS
class Paddle {
protected:
    void LimitMovement() {
        if (y <= 0) y = 0;
        if (y + height >= GetScreenHeight()) y = GetScreenHeight() - height;
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) y -= speed;
        if (IsKeyDown(KEY_DOWN)) y += speed;

        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
public:
    void Update(int ball_y) {
        if (y + height / 2 > ball_y) y -= speed;
        if (y + height / 2 < ball_y) y += speed;

        LimitMovement();
    }
};

// Instances
Ball ball;
Paddle player;
CpuPaddle cpu;

int main() {
    cout << "Starting The Game" << endl;

    InitWindow(screen_width, screen_height, "My Pong Game");
    SetTargetFPS(60);

    soccer_ball_texture = LoadTexture("soccer-ball.png"); // Load texture

    // Initialize ball
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    // Initialize player
    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    // Initialize CPU
    cpu.width = 25;
    cpu.height = 120;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    // Game mode selection
    GameMode currentMode = REGULAR;
    bool modeChanged = false;

    while (!WindowShouldClose()) {
        // Handle mode selection
        if (IsKeyPressed(KEY_ONE)) {
            currentMode = REGULAR;
            modeChanged = true;
        } else if (IsKeyPressed(KEY_TWO)) {
            currentMode = CURVE;
            modeChanged = true;
        } else if (IsKeyPressed(KEY_THREE)) {
            currentMode = SINUS_COSINUS;
            modeChanged = true;
        }

        if (modeChanged) {
            ball.SetMode(currentMode);
            modeChanged = false;
        }

        BeginDrawing();

        // Update game logic
        ball.Update();
        player.Update();
        cpu.Update(ball.y);

        // Check collisions
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height})) {
            ball.speed_x *= -1;
            ball.curve_intensity *= -1; // Reverse curve on paddle hit
        }
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height})) {
            ball.speed_x *= -1;
            ball.curve_intensity *= -1; // Reverse curve on paddle hit
        }

        // Drawing
        ClearBackground(Dark_Green);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        ball.Draw();
        player.Draw();
        cpu.Draw();
        DrawText(TextFormat("%i", cpu_point), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_point), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        // Display current mode
        const char* modeText;
        switch (currentMode) {
            case REGULAR:
                modeText = "Mode: Regular (Press 1, 2, or 3 to change)";
                break;
            case CURVE:
                modeText = "Mode: Curve (Press 1, 2, or 3 to change)";
                break;
            case SINUS_COSINUS:
                modeText = "Mode: Sinus/Cosinus (Press 1, 2, or 3 to change)";
                break;
        }
        DrawText(modeText, 10, screen_height - 30, 20, WHITE);

        EndDrawing();
    }

    UnloadTexture(soccer_ball_texture); // Unload texture
    CloseWindow();

    return 0;
}