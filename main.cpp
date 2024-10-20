#include "raylib.h"
#include <cmath>
#include <map>
#include <unordered_map>
#include <iostream>
#include <vector>

const int screenWidth = 800;
const int screenHeight = 650;
const int gridSize = 80;
const int gridWidth = 800 / gridSize;
const int gridHeight = 600 / gridSize;
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
    Color c;
    float I;
    char* name;
    int selected;
} Node;


Node getEmptyNode(Vector2 from, Vector2 to, Kind c) {
    return {
        .from = from,
        .to = to,
        .type = c,
        .c = RAYWHITE,
        .I = 0
    };
}

void DrawDevice(Node, Color, int);
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
         omap visited, Vector2 cobat, vector<Node> path, Vector2 &next, vector<vector<Node>> &loops) {
    visited.insert({current.to, true});
    path.push_back(current);

    for (Node &nde : m[next]) {
        if(nde.from.x == current.from.x && nde.from.y == current.from.y&&nde.to.x == current.to.x && nde.to.y == current.to.y) {continue;}
        if(nde.from.x == next.x && nde.from.y == next.y) { 
            if(nde.to.x == cobat.x && nde.to.y == cobat.y ) {
                path.push_back(nde);
                for(auto p : path) {
                    p.c = RED;
                    DrawDevice(p, RED, 2);
                }
                loops.push_back(path);
                return false;
            }
            
            if (visited.count(nde.to)) {
                continue;
            }

            if (DFS(nde, m, visited, cobat, path, nde.to, loops)) 
                return true;
        } else {
            if(nde.from.x == cobat.x && nde.from.y == cobat.y ) {
                path.push_back(nde);
                for(auto p : path) {
                    p.c = RED;
                    DrawDevice(p, RED, 2);
                }
                loops.push_back(path);
                nde.c = RED;
                return false;
            }
            if (visited.count(nde.from)) {
                continue;
            }
            if (DFS(nde, m, visited, cobat, path, nde.from, loops)) 
                return true;
        }
    }

    visited.erase(current.from); // Backtrack
    path.pop_back(); // Remove the current node from the path
    return false; // No loop found
}

vector<vector<Node>> findLoopsFromBattery(umap& m, Node battery) {
    omap visited;
    vector<Node> path;
    vector<vector<Node>> loops;
    DFS(battery, m, visited, battery.from, path, battery.to, loops);
    // for(auto ty: loops) {
    //     cout << ty.size() << " ";
    // }
    // cout << endl;
    return loops;
    // return DFS(battery, m, visited, battery.to, path, battery.from);
}

Node* get(umap &m, Node ne) {
    for(auto &[key, value]: m) {
        for(auto &n: value) {
            Vector2 f = (n.from.x > n.from.y)?n.from:n.to;
            Vector2 t = (n.from.x > n.from.y)?n.to:n.from;
            if(f.x == ne.from.x && f.y == ne.from.y && t.x == ne.to.x && t.y == ne.to.y) {
                return &n;
            }
            if(t.x == ne.from.x && t.y == ne.from.y && f.x == ne.to.x && f.y == ne.to.y) {
                return &n;
            }
        }
    }
}

Node* set(umap &m, Node ne, float i) {
    for(auto &[key, value]: m) {
        for(auto &n: value) {
            Vector2 f = n.from;
            Vector2 t = n.to;
            if(f.x == ne.from.x && f.y == ne.from.y && t.x == ne.to.x && t.y == ne.to.y) {
                n.I += i;
            }
            if(t.x == ne.from.x && t.y == ne.from.y && f.x == ne.to.x && f.y == ne.to.y) {
                n.I += i;
            }
        }
    }
}
void setv(umap &m, Node ne, float v) {
    for(auto &[key, value]: m) {
        for(auto &n: value) {
            Vector2 f = n.from;
            Vector2 t = n.to;
            if(f.x == ne.from.x && f.y == ne.from.y && t.x == ne.to.x && t.y == ne.to.y) {
                n.value = v;
            }
            if(t.x == ne.from.x && t.y == ne.from.y && f.x == ne.to.x && f.y == ne.to.y) {
                n.value = v;
            }
        }
    }
}

void homonegise_loop() {
    
}

void solve(vector<vector<Node>> *loops, umap &m) {
    
    for(auto &[key, value]: m) {
        for(auto &n: value) {
            Node* ne = get(m, n);
            ne->I=0;
        }
    }

    
    for(auto loop: *loops) {
        float numres = 0.0001;
        for(auto n: loop) {
            numres += n.value;
        }
        float i = loop[0].value/numres;
        for(auto n: loop) {
            // Node* ne = get(m, n);
            // ne->I+=i;
            set(m, n, i);
        }
    }
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

void DrawResistor(Vector2 from, Vector2 start, Color c, int thickness) {
    Vector2 starto = start;
    start.x = start.x- from.x;
    start.y = start.y- from.y;
    float foc = 0.17;
    
    DrawLineEx((Vector2) {from.x, from.y}, (Vector2) {start.x*foc+from.x, start.y*foc+from.y}, thickness, c);
    if(from.y == starto.y) {
        DrawLineEx((Vector2){start.x*foc+from.x, start.y*foc+from.y}, (Vector2){start.x*foc+from.x + 5, start.y*foc+from.y+5}, thickness, c);
        DrawLineEx((Vector2){start.x*foc+from.x + 5, start.y*foc+from.y+5}, (Vector2){start.x*foc+from.x + 15, start.y*foc+from.y-5}, thickness, c);
        DrawLineEx((Vector2){start.x*foc+from.x + 15, start.y*foc+from.y-5}, (Vector2){start.x*foc+from.x + 25, start.y*foc+from.y+5}, thickness, c);
        DrawLineEx((Vector2){start.x*foc+from.x + 25, start.y*foc+from.y+5}, (Vector2){start.x*foc+from.x + 35, start.y*foc+from.y-5}, thickness, c);
        DrawLineEx((Vector2){start.x*foc+from.x + 35, start.y*foc+from.y-5}, (Vector2){start.x*foc+from.x + 45, start.y*foc+from.y+5}, thickness, c);
        DrawLineEx((Vector2){start.x*foc+from.x + 45, start.y*foc+from.y+5}, (Vector2){start.x*(1-foc)+from.x, start.y*(1-foc)+from.y}, thickness, c);
    } else {
        DrawLine( start.x*foc+from.x, start.y*foc+from.y,  start.x*foc+from.x +5, start.y*foc+from.y+5, c);
        for (int i = 0; i < 4; ++i) {
            DrawLineEx((Vector2){start.x * foc + from.x + (i % 2 == 0 ? 5 : -5), start.y * foc + from.y + (i * 10)+5},
                     (Vector2) {start.x * foc + from.x + ((i + 1) % 2 == 0 ? 5 : -5), start.y * foc + from.y + (i + 1) * 10 + 5},
                     thickness,
                     c);
        }
        DrawLineEx((Vector2){start.x * foc + from.x + 5, start.y * foc + from.y + 50 -5}, 
                 (Vector2){start.x * (1 - foc) + from.x, start.y * (1 - foc) + from.y}, thickness, c);

    }
    DrawLineEx((Vector2) {start.x*(1-foc)+from.x, start.y*(1-foc)+from.y}, (Vector2) {starto.x, starto.y}, thickness, c);
}

void DrawDevice(Node device, Color c, int t) {
    c = device.c;
    if(device.type == RESISTOR) {
        if(device.value == 0) {
            DrawLineEx(device.from, device.to, t, device.c);
        } else 
        DrawResistor(device.from, device.to, c, t);
    } else {
        DrawBattery(device.from, device.to, c);
    }
}

bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();

    if ((key >= 32) && (key <= 126)) keyPressed = true;

    return keyPressed;
}

int main() {
    // Initialize the window
    InitWindow(screenWidth, screenHeight, "Circuit Solver");
    umap devices;

    SetTargetFPS(60);
    int num=0;
    int written=1;
    int hne= 0;
    Node highlighted;

    char name[40 + 1] = "\0";      // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;

    Rectangle textBox = { 140, 615, 60, 30 };
    bool mouseOnText = false;
    
    while (!WindowShouldClose()) {
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
             activeLine.first = (Vector2) { (float)axisxclose*gridSize, (float)axisyclosee*gridSize};
             activeLine.second = (Vector2) {(float)(axisxclose+1)*gridSize, (float)axisyclosee*gridSize};
             t=1;
        }
        else if( axisxdist < 10 || axisxdist > 70) {
             activeLine.first = (Vector2) { (float)axisxclosee*gridSize, (float)axisyclose*gridSize};
             activeLine.second = (Vector2) { (float)axisxclosee*gridSize,(float) (axisyclose+1)*gridSize};
             t=1;
        }
        BeginDrawing();
        DrawLineEx(activeLine.first, activeLine.second, 3, DARKGRAY);

        for (int x = 1; x < gridWidth; x++) {
            for (int y = 1; y < gridHeight; y++) {
                float posX = x * gridSize;
                float posY = y * gridSize;
                DrawCircle(posX, posY, 3, RAYWHITE);
            }
        }

        DrawRectangle(0, 560, screenWidth, screenHeight, WHITE);        

        vector<vector<Node>> loops;
        Node batry;
        for (const auto& [key, devicess] : devices) {
                for(const auto& device: devicess) {
                    if(device.type == BATTERY)
                        batry = device;
                    DrawDevice(device, PINK, 1);
                }
        }
        loops = findLoopsFromBattery(devices, batry);

        solve(&loops, devices);

        // cin >> num;
        if(loops.size() > num && !written)
        cout << "START "<< loops.size() << endl;
        if(loops.size() > num)
            for(int j=0; j<loops[num].size(); j++) {
                loops[num][j].c = ORANGE;
                DrawDevice(loops[num][j], BLUE, 2);
                Node* n = get(devices, loops[num][j]);
                if(!written) {cout << loops[num][j].from.x<< ", " << loops[num][j].from.y<< " to " << loops[num][j].to.x << ", "<< loops[num][j].to.y << " "<< n->I<<endl;
            }
        }
        if(loops.size() > num) written = true;
        if(!written)
        cout << "END" << endl;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if(mousePos.y < 600) hne=0;
            if(t) {
                if(devices.find(activeLine.first) != devices.end()) {
                    for(auto dv : devices[activeLine.first]) {
                        for(auto dt: devices[activeLine.second]) {
                            if(dv.from.x == dt.from.x && dv.to.x == dt.to.x  && dv.from.y == dt.from.y && dv.to.y == dt.to.y) {
                                highlighted = dv;
                                hne = 1;
                                sprintf(name, "%d", get(devices, highlighted)->value);
                                letterCount = 0;
                                break;
                            }
                            if(dv.to.x == dt.from.x && dv.from.x == dt.to.x  && dv.to.y == dt.from.y && dv.from.y == dt.to.y) {
                                highlighted = dv;
                                hne = 1;
                                sprintf(name, "%d", get(devices, highlighted)->value);
                                letterCount = 0;
                                break;
                            }
                        }
                        if(dv.type == BATTERY) {
                            highlighted=dv;
                            sprintf(name, "%d", get(devices, highlighted)->value);
                            letterCount = 0;
                            hne = 1;
                        }
                    }
                }
            }
        }

        /////////////////////////////////////// MOUSE HANDLING FOR INPUT FIELD



        if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;
        else mouseOnText = false;

        if (mouseOnText)
        {
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (letterCount < 10))
                {
                    name[letterCount] = (char)key;
                    name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                    letterCount++;
                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                name[letterCount] = '\0';
            }
            if(hne ) {
                cout << stoi(name) << "HERE" << endl;;
                setv(devices, highlighted, stoi(name));
            }
        }
        
        else SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        DrawRectangleRec(textBox, LIGHTGRAY);
        if (mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
        else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

        DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 20, MAROON);

        // DrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, 10), 315, 250, 20, DARKGRAY);


        ///////////////////////////////////
        DrawRectangle(20, 570, 70, 70, DARKGRAY);
        if(hne) {
            // cout << "HIGHLGHTE" << endl;
            highlighted.from = { 20, 610};
            highlighted.to  =  { 100, 610};
            highlighted.c = WHITE;
            DrawDevice(highlighted, PINK, 3);
            DrawText(TextFormat("Type: %s", (highlighted.type)?"Battery":"Resistor"), 120, 570, 20, BLACK);
            DrawText(TextFormat("Value: %d%s", highlighted.value, (highlighted.type)?"V":"ohm"), 120, 590, 20, BLACK);
        }
        
        EndDrawing();

        if (IsKeyPressed(KEY_ZERO)){ num = 0; written = 0; }
        if (IsKeyPressed(KEY_ONE)){ num = 1; written = 0; }
        if (IsKeyPressed(KEY_TWO)){ num = 2; written = 0; }
        if (IsKeyPressed(KEY_THREE)){ num = 3; written = 0; }
        if (IsKeyPressed(KEY_FOUR)){ num = 4; written = 0; }
        if (IsKeyPressed(KEY_FIVE)){ num = 5; written = 0; }
        if (IsKeyPressed(KEY_SIX)){ num = 6; written = 0; }
        if (IsKeyPressed(KEY_SEVEN)){ num = 7; written = 0; }
        if (IsKeyPressed(KEY_EIGHT)){ num = 8; written = 0; }
        if (IsKeyPressed(KEY_NINE)){ num = 9; written = 0; }
        
        if (IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_SHIFT)) {
            if(t) {
                Node n = getEmptyNode(activeLine.first, activeLine.second, RESISTOR);
                n.value = 1;
                devices[activeLine.first].push_back(n);
                devices[activeLine.second].push_back(n);
            }
        }
        if (IsKeyPressed(KEY_W) && IsKeyDown(KEY_LEFT_SHIFT)) {
            if(t) {
                Node n = getEmptyNode(activeLine.first, activeLine.second, RESISTOR);
                n.value = 0;
                devices[activeLine.first].push_back(n);
                devices[activeLine.second].push_back(n);
            }
        }
        if (IsKeyPressed(KEY_B) && IsKeyDown(KEY_LEFT_SHIFT)) {
            if(t) {
                Node n = getEmptyNode(activeLine.first, activeLine.second, BATTERY);
                n.value = 10;
                devices[activeLine.first].push_back(n);
            }
        }
    }

    CloseWindow();

    return 0;
}
