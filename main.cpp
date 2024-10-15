#include "raylib.h"
#include <cmath>
#include <map>
#include <unordered_map>
#include <iostream>
#include <vector>

const int screenWidth = 800;
const int screenHeight = 600;
const int gridSize = 80;
const int gridWidth = screenWidth / gridSize;
const int gridHeight = screenHeight / gridSize;
const float highlightThreshold = 5.0f; 

using namespace std;

enum Kind {
    RESISTOR,
    BATTERY
};

typedef struct {
    Vector2 from; 
    Vector2 to;
    Kind type;
    int value;
} Node;

void DrawDevice(Node, Color);
struct hashFunc{
    size_t operator()(const Vector2 &k) const{
    size_t h1 = std::hash<double>()(k.x);
    size_t h2 = std::hash<double>()(k.y);
    return (h1 ^ (h2 << 1));
    }
};

struct equalsFunc{
  bool operator()( const Vector2& lhs, const Vector2& rhs ) const{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
  }
};

typedef unordered_map<Vector2, vector<Node>, hashFunc, equalsFunc> umap;
typedef unordered_map<Vector2, bool, hashFunc, equalsFunc> omap;

bool DFS(Node& current, 
         umap& m, 
         omap& visited, Vector2 cobat, vector<Node> path, Vector2 &next) {
    visited.insert({current.to, true});
    path.push_back(current);

    for (Node nde : m[next]) {
        if(nde.from.x == next.x && nde.from.y == next.y) { 
            if(nde.to.x == cobat.x && nde.to.y == cobat.y ) {
                cout <<"SOLVUTINO" <<endl;
                for(auto p : path) {
                    cout << p.from.x<< " " << p.from.y << endl;
                    DrawDevice(p, RED);
                }
                DrawDevice(nde, RED);
                cout << "\n" << endl;
            }
            
            if (visited.count(nde.to)) {
                continue;
            }
            cout << "EDPLORINGcusr" <<  nde.to.x << " " << nde.to.y << endl;

            if (DFS(nde, m, visited, cobat, path, nde.to)) 
                return true;
        } else {
            if(nde.from.x == cobat.x && nde.from.y == cobat.y ) {
                cout <<"SOLVUTINO" <<endl;
                for(auto p : path) {
                    cout << p.from.x<< " " << p.from.y << endl;
                    DrawDevice(p, RED);
                }
                DrawDevice(nde, RED);
                cout << "\n" << endl;
            }
            
            if (visited.count(nde.from)) {
                continue;
            }
            cout << "EDPLORINGcusr" <<  nde.from.x << " " << nde.from.y << endl;

            if (DFS(nde, m, visited, cobat, path, nde.from)) 
                return true;
        }
    }

    visited.erase(current.from); // Backtrack
    path.pop_back(); // Remove the current node from the path
    return false; // No loop found
}

// bool DFS(Node current, 
//          umap& m, 
//          omap& visited, Vector2 cobat, vector<Node> path) {

//     visited.insert({current.from, true});
//     path.push_back(current);

//     for (const Node& node : m[current.from]) {
//         if(m[node.to].empty()) continue;
//         for (auto nodee : m.at(node.to)) {
//             Vector2 next = nodee.from;
//             if(current.from.x == cobat.x && current.from.y == cobat.y) {
//                 for(auto p : path) {
//                     cout << p.from.x << " " << p.from.y << endl;
//                     DrawDevice(p, RED);
//                 }
//                 cout << "\n" << endl;
//             }
        
//             if (visited.count(next)) {
//                 continue;
//             }

//             if (DFS(nodee, m, visited, cobat, path)) {
//                 return true;
//             }
//         }
//     }

//     visited.erase(current.from); // Backtrack
//     path.pop_back(); 
//     return false;
// }


bool findLoopsFromBattery(umap& m, Node battery) {
    omap visited;
    vector<Node> path;
    return DFS(battery, m, visited, battery.from, path, battery.to);
}


void DrawBattery(Vector2 from, Vector2 start, Color c) {
    Vector2 starto = start;
    start.x = start.x- from.x;
    start.y = start.y- from.y;
    float foc = 0.4;
    DrawLine(from.x, from.y, start.x*foc+from.x, start.y*foc+from.y, c);
    if(from.y == starto.y) {
        DrawLine( start.x*foc+from.x, start.y*foc+from.y-10,  start.x*foc+from.x, start.y*foc+from.y+10, c);
        DrawLine(start.x*(1-foc)+from.x, start.y*(1-foc)+from.y-5, start.x*(1-foc)+from.x, start.y*(1-foc)+from.y +5, c);
    } else {
        DrawLine( start.x*foc+from.x-10, start.y*foc+from.y,  start.x*foc+from.x+10, start.y*foc+from.y, c);
        DrawLine(start.x*(1-foc)+from.x-5, start.y*(1-foc)+from.y, start.x*(1-foc)+from.x+5, start.y*(1-foc)+from.y, c);
    }
    DrawLine(start.x*(1-foc)+from.x, start.y*(1-foc)+from.y, starto.x, starto.y, c);
}

void DrawResistor(Vector2 from, Vector2 start, Color c) {
    Vector2 starto = start;
    start.x = start.x- from.x;
    start.y = start.y- from.y;
    float foc = 0.17;
    DrawLine(from.x, from.y, start.x*foc+from.x, start.y*foc+from.y, DARKBLUE);
    if(from.y == starto.y) {
        DrawLine( start.x*foc+from.x, start.y*foc+from.y,  start.x*foc+from.x + 5, start.y*foc+from.y+5, c);
        DrawLine( start.x*foc+from.x + 05, start.y*foc+from.y+5, start.x*foc+from.x + 15, start.y*foc+from.y-5, c);
        DrawLine( start.x*foc+from.x + 15, start.y*foc+from.y-5, start.x*foc+from.x + 25, start.y*foc+from.y+5, c);
        DrawLine( start.x*foc+from.x + 25, start.y*foc+from.y+5, start.x*foc+from.x + 35, start.y*foc+from.y-5, c);
        DrawLine( start.x*foc+from.x + 35, start.y*foc+from.y-5, start.x*foc+from.x + 45, start.y*foc+from.y+5, c);
        DrawLine( start.x*foc+from.x + 35, start.y*foc+from.y-5, start.x*foc+from.x + 45, start.y*foc+from.y+5, c);
        DrawLine( start.x*foc+from.x + 45, start.y*foc+from.y+5, start.x*(1-foc)+from.x, start.y*(1-foc)+from.y,c);
    } else {
        DrawLine( start.x*foc+from.x, start.y*foc+from.y,  start.x*foc+from.x +5, start.y*foc+from.y+5, c);
        for (int i = 0; i < 4; ++i) {
            DrawLine(start.x * foc + from.x + (i % 2 == 0 ? 5 : -5), start.y * foc + from.y + (i * 10)+5,
                     start.x * foc + from.x + ((i + 1) % 2 == 0 ? 5 : -5), start.y * foc + from.y + (i + 1) * 10 + 5,
                     c);
        }
        DrawLine(start.x * foc + from.x + 5, start.y * foc + from.y + 50 -5, 
                 start.x * (1 - foc) + from.x, start.y * (1 - foc) + from.y, c);

    }
    DrawLine(start.x*(1-foc)+from.x, start.y*(1-foc)+from.y, starto.x, starto.y, c);
}

void DrawDevice(Node device, Color c) {
    if(device.type == RESISTOR) {
        DrawResistor(device.from, device.to, c);
    } else {
        DrawBattery(device.from, device.to, c);
        // findLoopsFromBattery(devices, device);
    }
}

int main() {
    // Initialize the window
    InitWindow(screenWidth, screenHeight, "Circuit Solver");
    umap devices;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        Vector2 mousePos = GetMousePosition();

        int axisydist = (int)mousePos.y % gridSize;
        int axisyclosee = round(mousePos.y / gridSize);
        int axisyclose = (int)mousePos.y / gridSize;

        int axisxdist = (int)mousePos.x % gridSize;
        int axisxclose = (int)mousePos.x / gridSize;
        int axisxclosee = round(mousePos.x / gridSize);

        pair<Vector2, Vector2> activeLine;
        int t= 0;
        if( axisydist < 10 || axisydist > 70) {
             activeLine.first = (Vector2) {axisxclose*gridSize, axisyclosee*gridSize};
             activeLine.second = (Vector2) {(axisxclose+1)*gridSize, axisyclosee*gridSize};
             t=1;
        }
        else if( axisxdist < 10 || axisxdist > 70) {
             activeLine.first = (Vector2) { axisxclosee*gridSize, axisyclose*gridSize};
             activeLine.second = (Vector2) { axisxclosee*gridSize, (axisyclose+1)*gridSize};
             t=1;
        }
        DrawLineEx(activeLine.first, activeLine.second, 3, DARKGRAY);

        for (int x = 1; x < gridWidth; x++) {
            for (int y = 1; y < gridHeight; y++) {
                float posX = x * gridSize;
                float posY = y * gridSize;
                DrawCircle(posX, posY, 3, RAYWHITE);
            }
        }

        for (const auto& [key, devicess] : devices) {
                for(const auto& device: devicess) {
                    DrawDevice(device, LIGHTGRAY);
                    if(device.type == BATTERY)
                    findLoopsFromBattery(devices, device);
                    cout << "DONE" << "\n\n\n";
                }
        }

        EndDrawing();
        if (IsKeyReleased(KEY_R)) printf("R key released!\n");
        if (IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_SHIFT)) {
            if(t) {
                Node n ;
                n.from = activeLine.first;
                n.to = activeLine.second;
                n.value = 1;
                n.type = RESISTOR;
                devices[activeLine.first].push_back(n);
                // n.from = activeLine.second;
                // n.to = activeLine.first;
                devices[activeLine.second].push_back(n);
            }
        }
        if (IsKeyPressed(KEY_B) && IsKeyDown(KEY_LEFT_SHIFT)) {
            if(t) {
                Node n ;
                n.from = activeLine.first;
                n.to = activeLine.second;
                n.value = 1;
                n.type = BATTERY;
                devices[activeLine.first].push_back(n);
            }
        }
        if (IsKeyDown(KEY_J)) cout << "BALLS" << endl;
    }

    CloseWindow();

    return 0;
}
