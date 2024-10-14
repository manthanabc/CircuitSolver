#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <raylib.h> // Assuming you're using raylib for Vector2

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

bool DFS(const Vector2& current, const Vector2& parent, 
         umap& m, 
         omap& visited, Vector2 cobat, vector<Vector2> path) {
    visited.insert({current, true});
    path.push_back(current);

    for (const Node& node : m[current]) {
        if(m[node.to].empty()) continue;
        for (auto nodee : m.at(node.to)) {
            Vector2 next = nodee.from;
        
            if(current.x == cobat.x && current.y == cobat.y) {
                for(auto p : path) {
                    cout << p.x<< " " << p.y << endl;
                }
                cout << "\n" << endl;
            }

            if (visited.count(next)) {
                continue;
            }

  
            if (DFS(next, current, m, visited, cobat, path)) {
                return true; // Loop found in the recursive call
            }
        }
    }

    visited.erase(current); // Backtrack
    path.pop_back(); // Remove the current node from the path
    return false; // No loop found
}

bool findLoopsFromBattery(umap& m, Node battery) {
    omap visited;
    vector<Vector2> path;
    return DFS(battery.to, { -1, -1 }, m, visited, battery.from, path); // Start DFS with no parent
}

int main() {
    umap graph;
    Node battery = { {0, 0}, {1, 0} }; // Starting point (battery)
    
    // Populate the graph (example data)
    graph[{0, 0}].push_back(battery);
    graph[{1, 0}].push_back({.from={1, 0}, .to={2, 2}});
    graph[{2, 2}].push_back({.from={2, 2}, .to={0, 0}}); // This creates a loop
    graph[{1, 0}].push_back({.from={1, 0}, .to={0, 0}}); // Another connection
    
    if (findLoopsFromBattery(graph, battery)) {
        std::cout << "Loop found!" << std::endl;
    } else {
        std::cout << "No loops found." << std::endl;
    }

    return 0;
}

