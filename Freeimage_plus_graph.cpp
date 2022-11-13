
#include "Freeimage_plus_graph.h"

#include <iostream>

FIP_Graph::FIP_Graph(GraphOptions &options){
    this->options = options;
}

/*
Save the graph to a file
Args:
    The filename
    The file format
*/
void FIP_Graph::SaveToFile(char *filename, FREE_IMAGE_FORMAT image_format){
    unsigned long width, height;
    width = options.width;
    height = options.height;
    int bpp = 24;

    FreeImage_Initialise();
    FIBITMAP *img = FreeImage_Allocate(width, height, bpp);

    if(width == 0 || height == 0){
        std::cout << "Error in SaveToFile(): width and height cannot be zero" << std::endl;
        return;
    }

    if (!img){
        std::cout << "Error in SaveToFile(): Unable to create FIBITMAP";
        return;
    }

    vector2 maxPoint, minPoint;
    findExtremePoints(maxPoint, minPoint);
    graphScale = {(width/16*13) / (maxPoint.x - minPoint.x),
                    (height/16*13) / (maxPoint.y - minPoint.y)};

    if(options.graphType == GraphType::LINE_GRAPH){
        plotLineGraph(img, maxPoint, minPoint);
    }else if(options.graphType == GraphType::BAR_CHART){
        plotBarChart(img, maxPoint, minPoint);
    }

    FreeImage_Save(image_format, img, filename, 0);
    FreeImage_Unload(img);
}

FIBITMAP* FIP_Graph::SaveToBitmap(){

    unsigned long width, height;
    width = options.width;
    height = options.height;
    int bpp = 24;

    FreeImage_Initialise();
    FIBITMAP *img = FreeImage_Allocate(width, height, bpp);

    if(width == 0 || height == 0){
        std::cout << "Error in SaveToFile(): width and height cannot be zero" << std::endl;
        return nullptr;
    }

    if (!img){
        std::cout << "Error in SaveToFile(): Unable to create FIBITMAP";
        return nullptr;
    }

    vector2 maxPoint, minPoint;
    findExtremePoints(maxPoint, minPoint);
    graphScale = {(width/16*13) / (maxPoint.x - minPoint.x),
                    (height/16*13) / (maxPoint.y - minPoint.y)};

    if(options.graphType == GraphType::LINE_GRAPH){
        plotLineGraph(img, maxPoint, minPoint);
    }else if(options.graphType == GraphType::BAR_CHART){
        plotBarChart(img, maxPoint, minPoint);
    }
    return img;
}


/*
Plot a graph
Args:
    A vector of points that rapresents the graph
    The width of the line in pixels
    Whether to draw the points or not
    The color of the line
    The type of interpolation to use to draw the line connecting the points
*/
void FIP_Graph::PlotGraph(std::vector <vector2> &linePoints, unsigned int lineWidth, bool drawPoints, RGBQUAD color, enum Interpolation interpolation_type){

    if(linePoints.size() < 2){
        std::cout << "Error in PlotGraph(): linePoints must have at least 2 points" << std::endl;
        return;
    }
    if(interpolation_type != NO_INTERPOLATION){
        interpolation_type = NO_INTERPOLATION;
        std::cout << "Warning in PlotGraph(): only Interpolation::NO_INTERPOLATION is currently supported." << std::endl;
    }

    lines.push_back({interpolation_type, color, drawPoints, lineWidth, linePoints});
}


void FIP_Graph::plotLineGraph(FIBITMAP* img, vector2 maxPoint, vector2 minPoint){
    drawGraphContainer(img, maxPoint, minPoint);
    for(int i = 0; i < lines.size(); i++){
        drawGraphLine(img, i);
    }
}

void FIP_Graph::plotBarChart(FIBITMAP* img, vector2 maxPoint, vector2 minPoint){
    drawGraphContainer(img, maxPoint, minPoint);
    for(int i = 0; i < lines.size(); i++){
        drawBarChartLine(img, i);
    }
}


void FIP_Graph::findExtremePoints(vector2 &maxPoint, vector2 &minPoint){

    float max_x = -INFINITY, min_x = INFINITY, max_y = -INFINITY, min_y = INFINITY;
    float temp_max_x, temp_min_x, temp_max_y, temp_min_y;
    for(int i = 0; i < lines.size(); i++){
        temp_max_y = -INFINITY;
        temp_min_y = INFINITY;

        temp_max_x = lines[i].linePoints.back().x;
        temp_min_x = lines[i].linePoints[0].x;

        for(int j = 0; j < lines[i].linePoints.size(); j++){
            float temp_y = lines[i].linePoints[j].y;
            if(temp_y > temp_max_y){
                temp_max_y = temp_y;
            }
            if(temp_y < temp_min_y){
                temp_min_y = temp_y;
            }
        }
        if(temp_max_x > max_x) max_x = temp_max_x;
        if(temp_min_x < min_x) min_x = temp_min_x;
        if(temp_max_y > max_y) max_y = temp_max_y;
        if(temp_min_y < min_y) min_y = temp_min_y;
    }

    maxPoint = {max_x, max_y};
    minPoint = {min_x, min_y};

    this->minPoint.x = min_x;
    this->minPoint.y = min_y;
}

void FIP_Graph::drawGraphContainer(FIBITMAP* img, vector2 maxPoint, vector2 minPoint){

    int width = FreeImage_GetWidth(img);
    int height = FreeImage_GetHeight(img);

    //some colors
    RGBQUAD white = {255,255,255,0};
    FreeImage_FillBackground(img, &white, 0);
    RGBQUAD black = {0, 0, 0, 0};
    RGBQUAD grey = {128, 128, 128};

    long y_axis = width/16;
    long x_axis = height/16;

    //axis
    FreeImage_Plus::DrawLine(img, y_axis, x_axis, y_axis*15, x_axis, 3, &black);
    FreeImage_Plus::DrawLine(img, y_axis, x_axis, y_axis, x_axis*15, 3, &black);

    //grid
    int grid_size_y = (x_axis * 13) / 10;
    int grid_size_x = (y_axis * 13) / 10;
    for(int i = 1; i <= 10; i++){
        FreeImage_Plus::DrawLine(img, y_axis, x_axis + grid_size_y * i, y_axis*15, x_axis + grid_size_y * i, 1, &grey);
        FreeImage_Plus::DrawLine(img, y_axis + grid_size_x * i, x_axis, y_axis + grid_size_x * i, x_axis*15, 1, &grey);
    }

    //draw axis name and title
    if(options.xAxisName.length() != 0) FreeImage_Plus::DrawText(img, y_axis * 16 - 5, x_axis - 15, (char *)options.xAxisName.c_str(), options.axisTextSize,
                                                       FreeImage_Plus::Alignment::ALIGN_RIGHT, FreeImage_Plus::Alignment::ALIGN_TOP, &black, &white);

    if(options.yAxisName.length() != 0) FreeImage_Plus::DrawText(img, y_axis - 5, x_axis*15, (char *)options.yAxisName.c_str(), options.axisTextSize,
                                                       FreeImage_Plus::Alignment::ALIGN_RIGHT, FreeImage_Plus::Alignment::ALIGN_BOTTOM, &black, &white);

    if(options.graphTitle.length() != 0) FreeImage_Plus::DrawText(img, y_axis * 8, x_axis * 15 + x_axis/2, (char *)options.graphTitle.c_str(), options.titleSize,
                                                 FreeImage_Plus::Alignment::ALIGN_CENTER, FreeImage_Plus::Alignment::ALIGN_CENTER, &black, &white);

    //draw y axis text
    char x_value_text[30];
    float delta = (maxPoint.x - minPoint.x)/10.0;

    //draws x axis value every 2-tenth of the total axis
    for(int i = 0; i <= 10; i+=2){
        if((minPoint.x + delta*i) > 1){
            sprintf(x_value_text, "%.2f", minPoint.x + delta*i);
        }else{
            sprintf(x_value_text, "%.4f", minPoint.x + delta*i);
        }
        FreeImage_Plus::DrawText(img, y_axis + y_axis*1.3*i, x_axis -10, x_value_text, options.axisTextSize, FreeImage_Plus::Alignment::ALIGN_CENTER,
                                FreeImage_Plus::Alignment::ALIGN_TOP, &black, &white);
    }

    //draw y values
    char y_value_text[30];
    delta = (maxPoint.y - minPoint.y);
    delta /= 10;

    //draws y axis value every 2-tenth of the total axis
    for(int i = 0; i <= 10; i+=2){
        if(( minPoint.y + delta*i) > 1){
            sprintf(y_value_text, "%.2f", minPoint.y + delta*i);
        }else{
            sprintf(y_value_text, "%.4f", minPoint.y + delta*i);
        }
        FreeImage_Plus::DrawText(img, y_axis - 5, x_axis + x_axis*1.3*i, y_value_text, options.axisTextSize,
                    FreeImage_Plus::Alignment::ALIGN_RIGHT, FreeImage_Plus::Alignment::ALIGN_BOTTOM, &black, &white);
    }
}

void FIP_Graph::drawBarChartLine(FIBITMAP* img, int index){
    if(index >= lines.size() || img == nullptr)
        return;

    Line &line = lines[index];

    int width = FreeImage_GetWidth(img);
    int height = FreeImage_GetHeight(img);
    float baseX = width / 16;
    float baseY = height / 16;

    for(int i = 0; i < line.linePoints.size(); i++){
        vector2 point = {baseX + (line.linePoints[i].x - minPoint.x) * graphScale.x,
                            baseY + (line.linePoints[i].y - minPoint.y) * graphScale.y};
        FreeImage_Plus::DrawLine(img, point.x, baseY, point.x, point.y, line.lineWidth, &line.lineColor);
    }
}

void FIP_Graph::drawGraphLine(FIBITMAP* img, int index){

    if(index >= lines.size() || img == nullptr)
        return;

    Line &line = lines[index];

    if(line.linePoints.size() < 2)
        return;

    int width = FreeImage_GetWidth(img);
    int height = FreeImage_GetHeight(img);
    float baseX = width / 16;
    float baseY = height / 16;

    if(line.lineInterpolation == Interpolation::NO_INTERPOLATION){
        for(int i = 0; i < line.linePoints.size()-1; i++){
            vector2 point1 = {baseX + (line.linePoints[i].x - minPoint.x) * graphScale.x,
                                baseY + (line.linePoints[i].y - minPoint.y) * graphScale.y};
            vector2 point2 = {baseX + (line.linePoints[i+1].x - minPoint.x) * graphScale.x,
                                baseY + (line.linePoints[i+1].y - minPoint.y) * graphScale.y};
            FreeImage_Plus::DrawLine(img, point1.x, point1.y, point2.x, point2.y, line.lineWidth, &line.lineColor);
        }
    }

    if(line.drawPoints){
        for(int i = 0; i < line.linePoints.size(); i++){
            vector2 point = {baseX + (line.linePoints[i].x - minPoint.x) * graphScale.x,
                                baseY + (line.linePoints[i].y - minPoint.y) * graphScale.y};
            FreeImage_Plus::DrawCircle(img, point.x, point.y,  line.lineWidth + 1, 1, true, &line.lineColor);
        }
    }

}
