#include<iostream>
#include"raylib.h"

using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};


const int screen_width = 1200;
const int screen_height = 800;

int cpu_point = 0;
int player_point = 0;

// BALL CLASS
class Ball{
    public:
    float x,y;
    int speed_x, speed_y;
    int radius;

    void Draw(){
        DrawCircle(x,y, radius, Yellow);
    }

    // Update position
    void Update(){
        x += speed_x;
        y += speed_y;

        // if it collide the bottom or top of the screen
        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
        }
        // if it collids the right or left of the screen
        if (x + radius >= GetScreenWidth()) //Cpu Wins
        {
            cpu_point++;
            ResetBall();
        }
        if (x - radius <= 0)
        {
            player_point++;
            ResetBall();
        }    
    }

    void ResetBall(){
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choice[2] = {-1,1};
        speed_x *= speed_choice[GetRandomValue(0,1)];
        speed_y *= speed_choice[GetRandomValue(0,1)];
    }
};
// PADDLE CLASS
class Paddle{
    protected:
    void LimitMovement(){
        if (y <= 0)
        {
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height;
        }    
    }
public:
    float x,y;
    float width, height;
    int speed;
    
    void Draw(){
        DrawRectangleRounded(Rectangle{x , y , width, height},0.8, 0, WHITE);
    }

    // Update position
    void Update(){
        if (IsKeyDown(KEY_UP))
        {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            y = y + speed;
        }
        
        // if it goes out of the window
        LimitMovement();
    }
};

class CpuPaddle: public Paddle{

    public:

    void Update(int ball_y){
        if (y + height/2 > ball_y)
        {
            y = y - speed;
        }
        if (y + height/2 < ball_y)
        {
            y = y + speed;
        }
        LimitMovement();
    }
};


// instace of ball
Ball ball;
Paddle player;
CpuPaddle cpu;

int main(){
    cout << "Starting The Game"<<endl;


    InitWindow(screen_width , screen_height , "My Pong Game");
    SetTargetFPS(60); //set target fps
    //if returns true the game continues

    //set Ball
    ball.radius = 20;
    ball.x = screen_width/2;
    ball.y = screen_height/2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    //set player
    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    //set cpu
    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height/2 - cpu.height /2;
    cpu.speed = 6;


    while (WindowShouldClose() == false){
        BeginDrawing();

        // Updating
        ball.Update();
        player.Update();
        cpu.Update(ball.y);

        //Checking for collision
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
        {
            ball.speed_x *= -1;
        }
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
        {
            ball.speed_x *= -1;
        }


        // Drawing
        ClearBackground(Dark_Green);
        DrawLine(screen_width/2, 0, screen_width/2, screen_height, WHITE);
        ball.Draw();
        player.Draw();
        cpu.Draw();
        DrawText(TextFormat("%i" , cpu_point) , screen_width/4 - 20, 20 ,80, WHITE);
        DrawText(TextFormat("%i" , player_point) , 3 * screen_width/4 - 20, 20 ,80, WHITE);
        
        EndDrawing();
    }
    


    CloseWindow();
    return 0;
}