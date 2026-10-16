#include "RouteNavigator.h"
#include "DateTimeUtils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>

using namespace std;

int RouteNavigator::findLocation(const string& name) const {
    string search = DateTimeUtils::lowerCase(name);
    for (int i = 0; i < static_cast<int>(locations.size()); ++i) {
        if (DateTimeUtils::lowerCase(locations[i]) == search) {
            return i;
        }
    }
    return -1;
}

int RouteNavigator::addLocation(const string& name) {
    int index = findLocation(name);
    if (index >= 0) {
        return index;
    }
    locations.push_back(name);
    neighbors.push_back(vector<int>());
    return static_cast<int>(locations.size()) - 1;
}

void RouteNavigator::loadGraph(const string& filename) {
    ifstream file(filename);
    string line;
    if (!getline(file, line) || DateTimeUtils::trim(line) != "from,to") {
        throw runtime_error("Cannot read campus graph or its header: " + filename);
    }
    RouteNavigator loaded;
    while (getline(file, line)) {
        if (DateTimeUtils::trim(line).empty()) {
            continue;
        }
        vector<string> fields = DateTimeUtils::parseCSVLine(line);
        if (fields.size() != 2) {
            throw runtime_error("Graph rows need two locations.");
        }
        int from = loaded.addLocation(fields[0]);
        int to = loaded.addLocation(fields[1]);
        if (from == to) {
            throw runtime_error("A graph edge must connect different locations.");
        }
        bool connected = false;
        for (int neighbor : loaded.neighbors[from]) {
            if (neighbor == to) {
                connected = true;
                break;
            }
        }
        if (connected) {
            continue;
        }
        // Campus connections work in both directions.
        loaded.neighbors[from].push_back(to);
        loaded.neighbors[to].push_back(from);
    }
    if (file.bad() || loaded.locations.empty()) {
        throw runtime_error("Campus graph is empty or unreadable.");
    }
    locations = loaded.locations;
    neighbors = loaded.neighbors;
}

void RouteNavigator::displayLocations() const {
    cout << "Sample campus locations:\n";
    for (const string& location : locations) {
        cout << "  " << location << '\n';
    }
}

vector<string> RouteNavigator::findShortestRoute(const string& source,
                                                         const string& destination) const {
    int start = findLocation(source);
    int finish = findLocation(destination);
    if (start == -1 || finish == -1) {
        throw runtime_error("Unknown location. Use a name displayed in the list.");
    }

    queue<int> waiting;
    vector<bool> visited(locations.size(), false);
    vector<int> parent(locations.size(), -1);
    waiting.push(start);
    visited[start] = true;

    // BFS visits all locations one edge away, then two edges away, and so on.
    while (!waiting.empty()) {
        int current = waiting.front();
        waiting.pop();
        if (current == finish) {
            break;
        }
        for (int next : neighbors[current]) {
            if (visited[next]) {
                continue;
            }
            visited[next] = true;
            parent[next] = current; // Remember the way back to the source.
            waiting.push(next);
        }
    }
    vector<string> route;
    if (!visited[finish]) {
        return route; // Empty list means no route was found.
    }
    int current = finish;
    while (current != -1) {
        route.push_back(locations[current]);
        current = parent[current];
    }
    reverse(route.begin(), route.end());
    return route;
}
