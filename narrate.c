#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define maxD 100 
#define maxLine 256 
const int screenWidth = 800;
const int screenHeight = 600;
typedef struct speech 
{
       char *name;
       int name_len;
       char  *speech;
       int speech_len;
       Rectangle bubble;
       enum { CRY, LAUGH, SCREAM } action;
}speech;

speech dialogues[maxD];
int count = 0;
int current = 0;
bool status = true;

void LoadDialoguesFromFile(char *filename) 
{
    FILE *file = fopen(filename, "r");

    char line[maxLine];
    int lineNumber = 1;
    count = 0;

    while (fgets(line, sizeof(line), file)) 
    {

        dialogues[count].name = (lineNumber%2)!=0? "robot":"oldie"; // Odd lines mane Robot bolche, Even mane ol man
        dialogues[count].speech = line;
        dialogues[count].bubble  = (Rectangle)
        {
            .x = (lineNumber % 2 != 0) ? 100 : 400, 
            .y = 385,
            .width = 300,
            .height = 50
        };
        

        count++;
        lineNumber++;
        
    }

    fclose(file);
}

int main() {


    InitWindow(screenWidth, screenHeight, "Dialogue");
    SetTargetFPS(60);

    LoadDialoguesFromFile("dial.txt");

    while (!WindowShouldClose()) 
    {
        
        if (IsKeyPressed(KEY_SPACE) && status) 
        {
            current++;
            if (current >= count) 
                status = false;
        }


        BeginDrawing();
        ClearBackground(WHITE);

        if (status) 
        {
            speech d = dialogues[current];


            DrawRectangleRounded(d.bubble, 0.2, 10, (strcmp(d.name, "robot")) ? SKYBLUE : LIGHTGRAY);
            DrawRectangleRoundedLines(d.bubble, 0.2, 10, 2, DARKGRAY);

            DrawText(d.name, d.bubble.x, d.bubble.y - 20, 20, BLACK);
            DrawText(d.speech, d.bubble.x + 10, d.bubble.y + 10, 20, DARKGRAY);
        } 
        else 
            DrawText("Dialogue Complete.", 200, 300, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
