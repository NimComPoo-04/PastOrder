#include "raylib.h"
#include "raymath.h"

#define tileH 9
#define tileW 9
void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);
const int screenWidth=800;
const int screenHeight=600;

#define Max_texture 1
typedef enum
{
    texture_tilemap = 0
}texture_asset;

Texture2D textures[Max_texture];

#define World_Width 20 //20*Tile_width
#define World_Height 20

typedef struct 
{
    int x,y;
}sTile;

sTile world[World_Width][World_Height];

Camera2D camera= {-0};

typedef struct
{
  int x,y; 
}sEntity;

sEntity player;   

void GStart()
{
    InitAudioDevice();

    //tilemap- 143*89 each tile 8*8 so 17 tiles each row and 11 each col
    Image image=LoadImage("blah.png");
    textures[texture_tilemap]= LoadTextureFromImage(image);
    UnloadImage(image);

    for(int i=0; i<World_Width;i++)
    {
        for(int j=0;j<World_Height;j++)
        {
            world[i][j]=(sTile)
            {
                .x=i,
                .y=j
            };
        }
    }

    player= (sEntity)
    {
         .x=tileW*3,
         .y=tileH*3
    };    

    camera.target= (Vector2){player.x,player.y};
    camera.offset=(Vector2){(float) screenWidth/2, (float) screenHeight/2};
    camera.rotation=0.0f;
    camera.zoom=5.0f;

}
void GUpdate()
{
    float x=player.x;
    float y=player.y;
    if(IsKeyPressed(KEY_LEFT))
        x-=1*tileW; 
    else if(IsKeyPressed(KEY_RIGHT)) 
        x+=1*tileW;
    else if(IsKeyPressed(KEY_UP))
        y-=1*tileH;
    else if(IsKeyPressed(KEY_DOWN))
        y+=1*tileH;
    float wheel= GetMouseWheelMove();
    if(wheel !=0)
    {
        const float zoomIncrement=0.125f;
        camera.zoom+=(wheel*zoomIncrement);
        //if(camera.zoom<3.0f) camera.zoom=3.0f;
        //if(camera.zoom>8.0f) camera.zoom=8.0f;
    } 
    
    player.x=x;
    player.y=y;
    camera.target= (Vector2){player.x,player.y};
}
void GRender()
{
    BeginMode2D(camera);

    sTile tile;
    int texture_index_x=0, texture_index_y=0;
    for(int i=0; i<World_Width;i++)
    {
        for(int j=0;j<World_Height;j++)
        {
            tile=world[i][j];
            texture_index_x=12; texture_index_y=8;
            DrawTile(tile.x *tileW, tile.y * tileH, texture_index_x, texture_index_y);
            /*Rectangle source = { (float)texture_index_x*tileW, (float)texture_index_y*tileH, (float)tileW, (float)tileH};
            Rectangle dest= {(float)(tile.x*tileW), (float)(tile.y*tileH), (float)tileW, (float)tileH};
            Vector2 origin= {0,0}; 
            DrawTexturePro(textures[texture_tilemap], source, dest, origin, 0.0f, WHITE);*/
        }
    }
    

            
    DrawTile(camera.target.x, camera.target.y, 4,0);
    
    EndMode2D();    
}
void GClose()
{
    for(int i=0;i<Max_texture;i++)
        UnloadTexture(textures[i]);
    CloseAudioDevice();
    
      
}
void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y)
 {
         Rectangle source = {(float)(tileW * texture_index_x), (float)(tileH * texture_index_y), (float)tileH, (float)tileW};
         Rectangle dest= { (float)(pos_x), (float)(pos_y),(float)tileW,(float)tileH};
         Vector2 origin= {0,0};
         DrawTexturePro(textures[texture_tilemap], source, dest, origin, 0.0f, WHITE);
 }  
int main()
{
    InitWindow(screenWidth, screenHeight, "game");
    SetTargetFPS(100);

    GStart();
    while(!WindowShouldClose())
    {
        GUpdate();
        BeginDrawing();

        ClearBackground(WHITE);
        GRender();
         
        EndDrawing();
    }
    GClose();
    CloseWindow();
    return 0;
}
