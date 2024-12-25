#include "raylib.h"

const int screenWidth=800;
const int screenHeight=600;
Camera2D camera= {-0};

typedef struct {
    Vector2 position; 
    int radius;       
    int speed;        
    Vector2 direction; 
} Player;
int main()
{
    Player player = {
        .position = {screenWidth / 2, screenHeight / 2}, 
        .radius = 20,
        .speed = 100,
        .direction = {0, 0}
    };
    
    InitWindow(screenWidth, screenHeight, "game");
    SetTargetFPS(100);
    while(!WindowShouldClose())
    {
        

        camera.offset=(Vector2){screenWidth/2,screenHeight/2};
        camera.rotation=0.0f;
        camera.zoom=1.0f;
        camera.target=player.position;
        
        //player direction
        player.direction.x = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));
        player.direction.y = (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP));

        
        float dt = GetFrameTime();
        player.position.x += player.direction.x * player.speed * dt;
        player.position.y += player.direction.y * player.speed * dt;
        

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(WHITE);
        DrawCircle(50, 100, 10, RED);  
        DrawCircle(100, 200, 100, BLUE);  
        DrawTriangle((Vector2){125,50}, (Vector2){450,50},(Vector2){90, 25}, PINK);  
        DrawCircle(200, 50, 50, ORANGE);  
        DrawCircle(540, 300, 60, YELLOW);
        DrawCircle(600, 180, 20, BLACK);
        DrawCircle(750, 550, 90, RED);
        DrawCircle(225, 750, 20, VIOLET);
        DrawCircle(550, 80, 150, MAROON);
        DrawRectangle(player.position.x, player.position.y, 50,70,BLACK);
        //DrawCircleV(player.position, player.radius, BLACK);
        EndMode2D(); 
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
