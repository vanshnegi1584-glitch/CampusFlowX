#ifndef ROUTE_NAVIGATOR_H
#define ROUTE_NAVIGATOR_H
#include <string>
#include <vector>

using namespace std;


class RouteNavigator {
private:
    vector<string> locations;
    vector<vector<int>> neighbors;
    int findLocation(const string& name) const;
    int addLocation(const string& name);
public:
    void loadGraph(const string& filename);
    void displayLocations() const;
    vector<string> findShortestRoute(const string& source,
                                             const string& destination) const;
};
#endif
