#include "raylib.h"
#include <cmath>
#include <map>

const int screenWidth = 800;
const int screenHeight = 600;
const int gridSize = 80;
const int gridWidth = screenWidth / gridSize;
const int gridHeight = screenHeight / gridSize;
const float highlightThreshold = 5.0f; 

using namespace std;

typedef struct {
    Vector2 from; 
    Vector2 to;
    Kind type;
    int value;
} Node;

int main() {
    // Initialize the window
    InitWindow(screenWidth, screenHeight, "Circuit Solver");
    map<Vector2, Node> m;
     // m["hello"] = 23;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mousePos = GetMousePosition();

        // for (int x = 1; x < gridWidth; x++) {
        //     for (int y = 1; y < gridHeight; y++) {
        //         float posX = x * gridSize;
        //         float posY = y * gridSize;

        //         if (x < gridWidth) {
        //             DrawLine(posX, 0, posX, screenHeight, LIGHTGRAY);
        //         }
        //         if (y < gridHeight) {
        //             DrawLine(0, posY, screenWidth, posY, LIGHTGRAY);
        //         }
        //     }
        // }

        int axisydist = (int)mousePos.y % gridSize;
        int axisyclosee = round(mousePos.y / gridSize);
        int axisyclose = (int)mousePos.y / gridSize;

        int axisxdist = (int)mousePos.x % gridSize;
        int axisxclose = (int)mousePos.x / gridSize;
        int axisxclosee = round(mousePos.x / gridSize);

        if( axisydist < 10 || axisydist > 70) {
             DrawLineEx((Vector2){axisxclose*gridSize, axisyclosee*gridSize}, (Vector2){(axisxclose+1)*gridSize, axisyclosee*gridSize}, 3, DARKGRAY);
        }
        else if( axisxdist < 10 || axisxdist > 70) {
             DrawLineEx((Vector2){axisxclosee*gridSize, axisyclose*gridSize}, (Vector2){axisxclosee*gridSize, (axisyclose+1)*gridSize}, 3, DARKGRAY);
        }

        for (int x = 1; x < gridWidth; x++) {
            for (int y = 1; y < gridHeight; y++) {
                float posX = x * gridSize;
                float posY = y * gridSize;
                DrawCircle(posX, posY, 3, BLACK);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
