#include "raylib.h"
#include <vector>
using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;


struct Brick
{
    Rectangle rect;
    bool alive;
};

struct Paddle
{
    Rectangle rect; 
    float speed; 
    void Update(float dt)
    {
        if(IsKeyDown(KEY_LEFT))
            rect.x -= speed*dt; 
        if(IsKeyDown(KEY_RIGHT))
            rect.x += speed*dt; 
        
        //keep the paddle within the screen bounds 
        if(rect.x < 0)
            rect.x = 0;
        if(rect.x + rect.width > SCREEN_WIDTH)
            rect.x = SCREEN_WIDTH - rect.width;
    }

    void Draw()
    {
        DrawRectangleRec(rect, WHITE);
    }
}; 

struct Ball 
{
    Vector2 position; 
    Vector2 velocity; 
    float radius; 

    void Update(float dt)
    {
        position.x += velocity.x * dt; 
        position.y += velocity.y * dt; 

        //left and right wall collision 
        if(position.x - radius < 0 || position.x + radius > SCREEN_WIDTH)
            velocity.x *= -1;
        
        if(position.y - radius < 0)
            velocity.y *= -1;

    }

    void Draw()
    {
        DrawCircleV(position, radius, WHITE);
    }
    void Reset()
    {
        position = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        velocity = {250.0f, -250.0f};
    }
}; 


int main()
{
    //Intialize the window 
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout");
    SetTargetFPS(60); 

    Paddle paddle; 
    paddle.rect = {380, 650, 140, 20};
    paddle.speed = 500; 

    Ball ball; 
    ball.radius = 10; 
    ball.Reset(); 
    vector<Brick> bricks; 

    int rows = 5; 
    int cols = 10; 

    float brickWidth = 75; 
    float brickHeight = 25; 

    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            Brick brick; 
            brick.rect = 
            {
                60 + col*(brickWidth + 5),
                60 + row*(brickHeight + 5), 
                brickWidth, 
                brickHeight
            };
            brick.alive = true; 
            bricks.push_back(brick); 
            
        }
    }

    int lives = 3; 
    int score = 0; 

    while(!WindowShouldClose())
    {
        float dt = GetFrameTime(); 

        //update 
        paddle.Update(dt);
        ball.Update(dt); 

        //ball paddle collision 

        if(CheckCollisionCircleRec(ball.position, ball.radius, paddle.rect))
        {
            
            ball.position.y = paddle.rect.y - ball.radius;
            ball.velocity.y *= -1;

            //add angle control 
            float paddleCenter = paddle.rect.x + paddle.rect.width / 2.0f; 

            float distance = (ball.position.x - paddleCenter) / (paddle.rect.width / 2.0f);
            ball.velocity.x = 350.0f * distance;

        }

        //ball brick collision 

        for(auto&brick : bricks)
        {
            if(brick.alive && CheckCollisionCircleRec(ball.position, ball.radius, brick.rect))
            {
                brick.alive = false; 
                ball.velocity.y *= -1; 
                score += 100; 
                break;
            }
        }

        //miss ball 
        if(ball.position.y - ball.radius > SCREEN_HEIGHT)
        {
            lives--;
            ball.Reset();
        } 

        BeginDrawing();

        ClearBackground(BLACK);

        paddle.Draw();
        ball.Draw();
        for(auto &brick: bricks)
        {
            if(brick.alive)
                DrawRectangleRec(brick.rect, WHITE); 
        }

        DrawText(
            TextFormat("Lives: %i", lives),
            20,20,30,WHITE
        );
        DrawText(
            TextFormat("Score: %i",score),
            700,20,30,WHITE
        );

        EndDrawing();

    }
    CloseWindow();


    return 0;
}