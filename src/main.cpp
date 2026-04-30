#include "raylib.h"
#include <vector>
#include <algorithm>
using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;


struct Brick
{
    Rectangle rect;
    bool alive;
};
//adding particles 

struct Particle
{
    Vector2 pos; 
    Vector2 velocity; 
    float life; 
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

// Game states enum
enum GameState
{
    MENU,
    PLAYING,
    GAMEOVER,
    WIN
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
    vector<Particle> particles; 

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
    GameState gameState = MENU;

    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();
        
        // Handle menu input
        if(gameState == MENU)
        {
            if(IsKeyPressed(KEY_ENTER))
            {
                gameState = PLAYING;
            }
        }

        // Update game objects
        if(gameState == PLAYING)
        {
            paddle.Update(dt);
            ball.Update(dt);

            // Update particles
            for(auto&p : particles)
            {
                p.pos.x += p.velocity.x * dt; 
                p.pos.y += p.velocity.y * dt; 
                p.life -= dt; 
            }
            // Remove dead particles
            particles.erase(
            remove_if(particles.begin(), particles.end(),
            [](Particle &p){ return p.life <= 0; }),
            particles.end()
            );
            // Ball-paddle collision
            if(CheckCollisionCircleRec(ball.position, ball.radius, paddle.rect))
            {
                ball.position.y = paddle.rect.y - ball.radius;
                ball.velocity.y *= -1;

                // Add angle control based on paddle position
                float paddleCenter = paddle.rect.x + paddle.rect.width / 2.0f; 
                float distance = (ball.position.x - paddleCenter) / (paddle.rect.width / 2.0f);
                ball.velocity.x = 350.0f * distance;
            }

            // Ball-brick collision
            for(auto& brick : bricks)
            {
                if(brick.alive && CheckCollisionCircleRec(ball.position, ball.radius, brick.rect))
                {
                    brick.alive = false; 
                    ball.velocity.y *= -1; 
                    score += 100; 

                    //spawn particles
                    for(int i = 0; i < 10; i++)
                    {
                        Particle p; 
                        p.pos = ball.position; 
                        p.velocity = {
                            (float)(GetRandomValue(-200, 200)),
                            (float)(GetRandomValue(-200, 200))
                        }; 
                        p.life = 1.0f; 
                        particles.push_back(p);

                    }
                    break;
                }
            }

            // Check win condition
            bool allDestroyed = true; 
            for(auto& brick : bricks)
            {
                if(brick.alive)
                {
                    allDestroyed = false; 
                    break;
                }
            }
            if(allDestroyed)
            {
                gameState = WIN; 
            }

            // Check if ball is missed
            if(ball.position.y - ball.radius > SCREEN_HEIGHT)
            {
                lives--;
                if(lives <= 0)
                {
                    gameState = GAMEOVER;
                }
                ball.Reset();
            }
        }

        // Reset game
        if((gameState == WIN || gameState == GAMEOVER) && IsKeyPressed(KEY_R))
        {
            lives = 3;
            score = 0;
            for(auto& brick : bricks)
            {
                brick.alive = true;
            }
            ball.Reset();
            gameState = MENU;
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw game objects
        paddle.Draw();
        ball.Draw();
        for(auto& brick : bricks)
        {
            if(brick.alive)
            {
                DrawRectangleRec(brick.rect, WHITE); 
            }
        }
        //Draw particles
        for(auto&p : particles)
        {
            DrawCircleV(p.pos,3,WHITE);
        }
        // Draw UI
        DrawText(TextFormat("Lives: %i", lives), 20, 20, 30, WHITE);
        DrawText(TextFormat("Score: %i", score), 700, 20, 30, WHITE);

        // Draw game state messages
        if(gameState == MENU)
        {
            DrawText("PRESS ENTER TO START", 220, 300, 40, WHITE);
        }
        if(gameState == GAMEOVER)
        {
            DrawText("GAME OVER", 320, 300, 50, WHITE);
        }
        if(gameState == WIN)
        {
            DrawText("YOU WIN!", 330, 300, 50, WHITE);
        }

        EndDrawing();
    }
    CloseWindow();


    return 0;
}