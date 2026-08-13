#include "KCore.h"
#include "MapHandler.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>

MapHandler g_MapHandler(500, 2000);

MapHandler::MapHandler(double spacing, double overshoot)
    : layer_spacing(spacing), overshoot(overshoot),
    min_x(0), max_x(0), min_y(0), max_y(0) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    inner_spaceing = 1000;
}

void MapHandler::setMapBoundary(const std::vector<Point>& coords) {
    map_coords = coords;
    updateBounds();
}

Point MapHandler::generateRandomPointInside() {
    while (true) {
        Point p{ randInRange(min_x, max_x), randInRange(min_y, max_y) };
        if (pointInPolygon(p, map_coords))
            return p;
    }
}

void MapHandler::generateTrapLayers(Point center) {
    trap_layers.clear();
	double M_PI = 3.14159265358979323846; // Define M_PI if not available
    double maxD = 0.0;
    for (const auto& pt : map_coords)
        maxD = max(maxD, distance(center, pt));
    double finalRadius = maxD + overshoot;
    for (double r = inner_spaceing; r <= finalRadius; r += layer_spacing) {
        int N = max(8, static_cast<int>(2 * M_PI * r / layer_spacing));
        std::vector<Point> ring;
        for (int i = 0; i < N; ++i) {
            double theta = 2 * M_PI * i / N;
            Point p{ center.x + r * std::cos(theta), center.y + r * std::sin(theta) };
            if (pointInPolygon(p, map_coords)) {
                ring.push_back(p);
            }
        }
        if (!ring.empty())
            trap_layers.push_back(ring);
    }
    centerPoint = center;
	currentLayer = 0;
}

int MapHandler::getLayerCount() const {
    return static_cast<int>(trap_layers.size());
}

int MapHandler::getPointCountInLayer(int layer) const {
    if (layer < 0 || layer >= getLayerCount()) return 0;
    return static_cast<int>(trap_layers[layer].size());
}

void MapHandler::setCurrentLayer(int layer) {
	currentLayer = layer;
}
int MapHandler::getRadius(int layer) {
    if (layer < 0 || layer >= getLayerCount()) return 0;
	return static_cast<int>(inner_spaceing + layer * layer_spacing);
}

int MapHandler::getCurrentRadius() {
    if (currentLayer == 0) return 0;
    return static_cast<int>(inner_spaceing + currentLayer * layer_spacing);
}

int MapHandler::getDistanceToCenter(int x, int y) {
    Point p{ static_cast<double>(x), static_cast<double>(y) };
    return static_cast<int>(distance(centerPoint, p));
}

Point MapHandler::getPoint(int layer, int index) const {
    if (layer < 0 || layer >= getLayerCount()) return { 0, 0 };
    const auto& l = trap_layers[layer];
    if (index < 0 || index >= static_cast<int>(l.size())) return { 0, 0 };
    return l[index];
}

const std::vector<Point>& MapHandler::getLayer(int layer) const {
    static std::vector<Point> empty;
    if (layer < 0 || layer >= getLayerCount()) return empty;
    return trap_layers[layer];
}

void MapHandler::updateBounds() {
    min_x = min_y = -std::numeric_limits<double>::lowest();
    max_x = max_y = std::numeric_limits<double>::lowest();
    for (const auto& p : map_coords) {
        min_x = min(min_x, p.x);
        max_x = max(max_x, p.x);
        min_y = min(min_y, p.y);
        max_y = max(max_y, p.y);
    }
}

bool MapHandler::pointInPolygon(const Point& pt, const std::vector<Point>& poly) {
    bool inside = false;
    int n = static_cast<int>(poly.size());
    for (int i = 0, j = n - 1; i < n; j = i++) {
        const Point& pi = poly[i];
        const Point& pj = poly[j];
        if (((pi.y > pt.y) != (pj.y > pt.y)) &&
            (pt.x < (pj.x - pi.x) * (pt.y - pi.y) / (pj.y - pi.y) + pi.x)) {
            inside = !inside;
        }
    }
    return inside;
}

double MapHandler::distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

double MapHandler::randInRange(double min, double max) {
    return min + (max - min) * ((double)std::rand() / RAND_MAX);
}
