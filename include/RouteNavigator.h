#ifndef ROUTE_NAVIGATOR_H
#define ROUTE_NAVIGATOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include "Dsa.h"

struct RouteResult {
    bool found;
    long long totalMinutes;
    std::vector<std::string> path;
    std::string errorMessage;

    RouteResult() : found(false), totalMinutes(0) {}
    RouteResult(bool f, long long t, const std::vector<std::string>& p, const std::string& err = "")
        : found(f), totalMinutes(t), path(p), errorMessage(err) {}
};

class RouteNavigator {
  private:
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> graph;
    std::unordered_map<std::string, std::string> displayNames;
    std::string graphFilename;
    
    std::vector<std::string> locations;
    std::unordered_map<std::string, size_t> ids;
    std::vector<size_t> offsets;
    std::vector<DsaEdge> edges;

    std::string normalizeLocation(const std::string& location) const;
    std::string findMatchingLocation(const std::string& input) const;

  public:
    RouteNavigator(const std::string& filename = "data/campus_graph.csv");

    bool loadGraph(const std::string& filename = "");
    RouteResult findShortestRoute(const std::string& source, const std::string& destination) const;
    void displayLocations() const;
    bool containsLocation(const std::string& location) const;
    int getLocationCount() const;
    int getConnectionCount() const;
    const std::string& getGraphFilename() const { return graphFilename; }
};

#endif
