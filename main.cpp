#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <raylib.h> // Assuming you're using raylib for Vector2

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

// Function to perform DFS and detect loops
bool DFS(const Vector2& current, const Vector2& parent, 
         umap& m, 
         omap& visited) {
    visited.insert({current, true});

    for (const Node& node : m[current]) {
        Vector2 next = node.to; // Change this if your logic differs
        if (next.x == parent.x && next.y == parent.y) continue; // Ignore the parent node
        
        // if (visited.count(next)) {
        //     return true; // A loop is found
        // }

        if (DFS(next, current, m, visited)) {
            return true; // Loop found in the recursive call
        }
    }

    visited.erase(current); // Backtrack
    return false; // No loop found
}

bool findLoopsFromBattery(umap& m, const Vector2& batteryPosition) {
    omap visited;
    return DFS(batteryPosition, { -1, -1 }, m, visited); // Start DFS with no parent
}

int main() {
    umap graph;
    Vector2 batteryPosition = {0, 0}; // Starting point (battery)
    
    // Populate the graph (example data)
    graph[batteryPosition].push_back({batteryPosition, {1, 0}});
    graph[{1, 0}].push_back({{1, 0}, {2, 0}});
    graph[{2, 0}].push_back({{2, 0}, {1, 0}}); // This creates a loop
    graph[{0, 1}].push_back({{0, 1}, {0, 0}}); // Another connection
    
    // Check for loops starting from the battery
    if (findLoopsFromBattery(graph, batteryPosition)) {
        std::cout << "Loop found!" << std::endl;
    } else {
        std::cout << "No loops found." << std::endl;
    }

    return 0;
}

