#include "RouteNavigator.h"
#include "DateTimeUtils.h"
#include "Dsa.h"
#include <fstream>
#include <new>
#include <limits>
#include <algorithm>
#include <iostream>
#include <set>
#include <map>

RouteNavigator::RouteNavigator(const std::string& filename) : graphFilename(filename) {
    loadGraph(filename);
}
std::string RouteNavigator::normalizeLocation(const std::string& location) const {
    if (DateTimeUtils::isValidClassroom(location))
        return DateTimeUtils::normalizeRoom(location);
    return DateTimeUtils::normalizeString(location);
}
std::string RouteNavigator::findMatchingLocation(const std::string& input) const {
    const auto key = normalizeLocation(input);
    if (graph.count(key))
        return key;
    // Bare room numbers are accepted only when exactly one typed room matches.
    std::string match;
    for (const auto& suffix : {"CR", "LT", "LAB", "AUDI"}) {
        auto candidate = key + suffix;
        if (graph.count(candidate)) {
            if (!match.empty())
                return "";
            match = candidate;
        }
    }
    return match;
}
bool RouteNavigator::loadGraph(const std::string& filename) {
    const auto path = filename.empty() ? graphFilename : filename;
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open campus graph: " << path << '\n';
        return false;
    }
    decltype(graph) loaded;
    decltype(displayNames) names;
    std::map<std::string, size_t> columns;
    std::set<std::pair<std::string, std::string>> csvEdges;
    std::string line;
    size_t lineNumber = 0, invalid = 0;
    bool header = false;
    while (std::getline(file, line)) {
        ++lineNumber;
        if (DateTimeUtils::trim(line).empty())
            continue;
        try {
            auto fields = DateTimeUtils::parseCSVLine(line);
            if (!header) {
                for (size_t i = 0; i < fields.size(); ++i) {
                    auto key = DateTimeUtils::normalizeString(fields[i]);
                    if (key == "weight" || key == "time" || key == "distance")
                        key = "distance_minutes";
                    if (!columns.emplace(key, i).second)
                        throw std::invalid_argument("Duplicate graph header");
                }
                for (const auto& key : {"from", "to", "distance_minutes"})
                    if (!columns.count(key))
                        throw std::invalid_argument(std::string("Missing header: ") + key);
                header = true;
                continue;
            }
            if (fields.size() != columns.size())
                throw std::invalid_argument("Wrong number of graph fields");
            auto from = DateTimeUtils::trim(fields.at(columns.at("from")));
            auto to = DateTimeUtils::trim(fields.at(columns.at("to")));
            int distance = 0;
            if (from.empty() || to.empty())
                throw std::invalid_argument("Empty graph location");
            if (!DateTimeUtils::parseInteger(fields.at(columns.at("distance_minutes")), distance) ||
                distance <= 0)
                throw std::invalid_argument("Travel time must be a positive integer");
            auto a = normalizeLocation(from), b = normalizeLocation(to);
            if (a == b)
                throw std::invalid_argument("Self-loop is not a connection");
            auto edge = std::make_pair(std::min(a, b), std::max(a, b));
            if (!csvEdges.insert(edge).second)
                throw std::invalid_argument("Duplicate connection");
            loaded[a].push_back({b, distance});
            loaded[b].push_back({a, distance});
            names.emplace(a, from);
            names.emplace(b, to);
        } catch (const std::exception& e) {
            std::cerr << "Campus graph line " << lineNumber << ": " << e.what() << '\n';
            if (!header)
                return false;
            ++invalid;
        }
    }
    if (file.bad() || !header || csvEdges.empty()) {
        std::cerr << "No usable campus connections.\n";
        return false;
    }
    for (auto& entry : loaded)
        std::sort(entry.second.begin(), entry.second.end());
    graph.swap(loaded);
    displayNames.swap(names);
    graphFilename = path;

    locations.clear();
    for (const auto& entry : graph)
        locations.push_back(entry.first);
    std::sort(locations.begin(), locations.end());
    ids.clear();
    for (size_t i = 0; i < locations.size(); ++i)
        ids.emplace(locations[i], i);

    offsets = {0};
    edges.clear();
    for (const auto& location : locations) {
        for (const auto& [neighbor, minutes] : graph.at(location))
            edges.push_back({ids.at(neighbor), minutes});
        offsets.push_back(edges.size());
    }
    std::cout << "Campus routes: " << graph.size() << " locations, " << csvEdges.size()
              << " connections.\n";
    if (invalid)
        std::cout << "Skipped " << invalid << " invalid or duplicate connections.\n";
    return true;
}
RouteResult RouteNavigator::findShortestRoute(const std::string& source,
                                              const std::string& destination) const {
    if (graph.empty())
        return {false, 0, {}, "Campus route data is unavailable."};
    auto start = findMatchingLocation(source), finish = findMatchingLocation(destination);
    if (start.empty())
        return {false, 0, {}, "Source '" + source + "' is unknown or ambiguous."};
    if (finish.empty())
        return {false, 0, {}, "Destination '" + destination + "' is unknown or ambiguous."};
    std::vector<int64_t> distances(locations.size());
    std::vector<size_t> parents(locations.size());
    const size_t startId = ids.at(start), finishId = ids.at(finish);
    DsaStatus status = dsa_dijkstra(locations.size(), offsets.data(), edges.data(), startId,
                                    finishId, distances.data(), parents.data());
    if (status == DSA_OUT_OF_MEMORY)
        throw std::bad_alloc();
    if (status != DSA_OK)
        throw std::runtime_error("Could not calculate the campus route.");
    if (distances[finishId] == INT64_MAX)
        return {false, 0, {}, "No route connects these locations."};
    std::vector<std::string> route;
    auto current = finishId;
    while (true) {
        route.push_back(displayNames.at(locations[current]));
        if (current == startId)
            break;
        current = parents[current];
    }
    std::reverse(route.begin(), route.end());
    return {true, distances[finishId], route};
}
void RouteNavigator::displayLocations() const {
    if (graph.empty()) {
        std::cout << "No campus locations loaded.\n";
        return;
    }
    std::vector<std::string> locations;
    for (const auto& entry : displayNames)
        locations.push_back(entry.second);
    std::sort(locations.begin(), locations.end());
    std::cout << "\nCampus locations (" << locations.size() << ")\n";
    for (size_t i = 0; i < locations.size(); ++i)
        std::cout << i + 1 << ". " << locations[i] << '\n';
}
bool RouteNavigator::containsLocation(const std::string& location) const {
    return !findMatchingLocation(location).empty();
}
int RouteNavigator::getLocationCount() const {
    return static_cast<int>(graph.size());
}
int RouteNavigator::getConnectionCount() const {
    size_t count = 0;
    for (const auto& entry : graph)
        count += entry.second.size();
    return static_cast<int>(count / 2);
}
