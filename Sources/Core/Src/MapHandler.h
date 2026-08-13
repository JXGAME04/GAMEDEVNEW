#ifndef MAPHANDLER_H
#define MAPHANDLER_H

#include <vector>

struct Point {
    double x, y;
};

class MapHandler {
public:
    MapHandler(double spacing, double overshoot);

    void setMapBoundary(const std::vector<Point>& coords);
    Point generateRandomPointInside();
    void generateTrapLayers(Point center);

    int getLayerCount() const;
    int getPointCountInLayer(int layer) const;
    void setCurrentLayer(int layer);
    int getRadius(int layer);
    int getCurrentRadius();
    int getDistanceToCenter(int x, int y);
    Point getPoint(int layer, int index) const;
    const std::vector<Point>& getLayer(int layer) const;

private:
    std::vector<Point> map_coords;
    std::vector<std::vector<Point>> trap_layers;
    double min_x, max_x, min_y, max_y;
    double layer_spacing;
    double inner_spaceing;
    double overshoot;
    int currentLayer = 0;
    Point centerPoint;

    void updateBounds();
    static bool pointInPolygon(const Point& pt, const std::vector<Point>& poly);
    static double distance(const Point& a, const Point& b);
    static double randInRange(double min, double max);
};

extern CORE_API MapHandler g_MapHandler;

#endif // MAPHANDLER_H
