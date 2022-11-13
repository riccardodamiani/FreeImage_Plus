
#ifndef GRAPH_H
#define GRAPH_H

#include "Freeimage_plus.h"

#include <string>

enum Interpolation{
    NO_INTERPOLATION,
    LINEAR_INTERPOLATION,
    POLYNOMIAL_INTERPOLATION
};

enum AxisScale{
    LINEAR,
    LOGARITHMIC
};

enum GraphType{
    LINE_GRAPH,
    BAR_CHART
};

struct GraphOptions{
    FreeImage_Plus::FontSize fontSize;
    FreeImage_Plus::Alignment alignment;
    AxisScale xAxisScale, yAxisScale;
    GraphType graphType;
    unsigned int axisTextSize;
    unsigned int titleSize;
    unsigned int width, height;
    std::string xAxisName;
    std::string yAxisName;
    std::string graphTitle;
};

struct Line{
    enum Interpolation lineInterpolation;
    RGBQUAD lineColor;
    bool drawPoints;
    unsigned int lineWidth;
    std::vector <vector2> linePoints;
};

class FIP_Graph{

public:
    FIP_Graph(GraphOptions &);
    void PlotGraph(std::vector <vector2> graphPoints, unsigned int lineWidth, bool drawPoints, RGBQUAD color, enum Interpolation interpolation_type );
    void SaveToFile(char *filename, FREE_IMAGE_FORMAT image_format);
private:
    std::vector <Line> lines;
    GraphOptions options;
    vector2 graphScale;
    vector2 minPoint;

    void plotLineGraph(FIBITMAP* img, vector2 maxPoint, vector2 minPoint);
    void plotBarChart(FIBITMAP* img, vector2 maxPoint, vector2 minPoint);

    void drawGraphContainer(FIBITMAP* img, vector2 maxPoint, vector2 minPoint);
    void drawGraphLine(FIBITMAP* img, int index);
    void drawBarChartLine(FIBITMAP* img, int index);
    void findExtremePoints(vector2 &maxPoint, vector2 &minPoint);
};


#endif // GRAPH_H
