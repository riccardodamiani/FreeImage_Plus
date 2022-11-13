
#include <iostream>
#include <vector>
#include <math.h>
#include "Freeimage_plus.h"
#include "Freeimage_plus_graph.h"

void example1();
void example2();
void example3();
void example4();
void example5();
void example6();

int main(){

    //init the freeimage library
    FreeImage_Initialise();

    example1();     //Line and circle
    example2();     //circle and rectangle
    example3();     //text
    example4();     //blur filter
    example5();     //scaling
    example6();     //graph

    return 0;

}

//Drawing basic shapes
void example1(){
    //create a bitmap
    FIBITMAP *bmp = FreeImage_Allocate(500, 400, 24);

    //fill the bitmap with color white
    RGBQUAD white = {255, 255, 255, 0};
    FreeImage_FillBackground(bmp, &white);

    //draw two red circle filled
    RGBQUAD red = {0, 0, 255, 0};
    FreeImage_Plus::DrawCircle(bmp, 180, 250, 10, 1, true, &red);
    FreeImage_Plus::DrawCircle(bmp, 320, 250, 10, 1, true, &red);

    //draw three red lines, with a width of 4 pixels
    FreeImage_Plus::DrawLine(bmp, 150, 190, 190, 150, 4, &red);
    FreeImage_Plus::DrawLine(bmp, 190, 150, 310, 150, 4, &red);
    FreeImage_Plus::DrawLine(bmp, 310, 150, 350, 190, 4, &red);

    //save the bitmap as a png file
    FreeImage_Save(FIF_PNG, bmp, "smile.png");

    //free the bitmap memory
    FreeImage_Unload(bmp);

}

//Drawing basic shapes
void example2(){
    //create a bitmap
    FIBITMAP *bmp = FreeImage_Allocate(600, 400, 24);

    //fill the bitmap with color white
    RGBQUAD white = {255, 255, 255, 0};
    FreeImage_FillBackground(bmp, &white);

    //define some colors
    RGBQUAD blue = {255, 0, 0, 0};
    RGBQUAD green = {0, 255, 0, 0};

    //draw a empty circle with a border of 10 pixels
    FreeImage_Plus::DrawCircle(bmp, 200, 200, 50, 10, false, &blue);

    //draw a empty square with a border of 10 pixels. The x and y value are referred to the bottom-left point
    FreeImage_Plus::DrawRect(bmp, 300, 150, 100, 100, 10, false, &green);

    //save the bitmap as a png file
    FreeImage_Save(FIF_PNG, bmp, "rect_circle.png");

    //free the bitmap memory
    FreeImage_Unload(bmp);

}

//Drawing text
void example3(){

    //create a bitmap
    FIBITMAP *bmp = FreeImage_Allocate(500, 500, 24);

    //define some colors
    RGBQUAD white = {255, 255, 255, 0};
    RGBQUAD black = {0, 0, 0, 0};

    //fill the bitmap with color white
    FreeImage_FillBackground(bmp, &white);

    //draw 4 texts of different sizes, all aligned in the center
    FreeImage_Plus::DrawText(bmp, 250, 400, "Small text", 10, FreeImage_Plus::Alignment::ALIGN_CENTER, FreeImage_Plus::Alignment::ALIGN_CENTER, &black, &white);
    FreeImage_Plus::DrawText(bmp, 250, 300, "A bit larger text", 16, FreeImage_Plus::Alignment::ALIGN_CENTER, FreeImage_Plus::Alignment::ALIGN_CENTER, &black, &white);
    FreeImage_Plus::DrawText(bmp, 250, 200, "Medium text", 24, FreeImage_Plus::Alignment::ALIGN_CENTER, FreeImage_Plus::Alignment::ALIGN_CENTER, &black, &white);
    FreeImage_Plus::DrawText(bmp, 250, 100, "Large text", 40, FreeImage_Plus::Alignment::ALIGN_CENTER, FreeImage_Plus::Alignment::ALIGN_CENTER, &black, &white);

    //save the bitmap as a png file
    FreeImage_Save(FIF_PNG, bmp, "text.png");

    //free the bitmap memory
    FreeImage_Unload(bmp);

}

//Apply a blur filter to a image
void example4(){
    //create a bitmap
    FIBITMAP *bmp = FreeImage_Allocate(800, 200, 24);

    //define some colors
    RGBQUAD white = {255, 255, 255, 0};
    RGBQUAD black = {0, 0, 0, 0};

    //fill the bitmap with color white
    FreeImage_FillBackground(bmp, &white);

    //draw a text in the middle of the bitmap
    FreeImage_Plus::DrawText(bmp, 400, 100, "This is a text ~", 40, FreeImage_Plus::Alignment::ALIGN_CENTER, FreeImage_Plus::Alignment::ALIGN_CENTER, &black, &white);

    FIBITMAP *blurred_bmp = FreeImage_Plus::BlurFilter(bmp, 1, 7);

    //save the two bitmaps as png files
    FreeImage_Save(FIF_PNG, bmp, "clear_text.png");
    FreeImage_Save(FIF_PNG, blurred_bmp, "blurred_text.png");

    //free the bitmap memory
    FreeImage_Unload(bmp);
    FreeImage_Unload(blurred_bmp);
}

//Scaling a image
void example5(){

    //load the image of a blurred text created in example4()
    FIBITMAP *src_bmp = FreeImage_Load(FIF_PNG, "blurred_text.png");
    if(src_bmp == nullptr){
        std::cout << "Error: File not found";
        return;
    }

    //Create a new bitmap
    FIBITMAP* dest_bmp = FreeImage_Allocate(400, 400, 24);

    //copy the src_bmp inside a rectangle of dest_bmp
    Rect dest_rect = {100, 200, 200, 50};
    FreeImage_Plus::CopyResize(src_bmp, dest_bmp, nullptr, &dest_rect, FreeImage_Plus::Algorithm::BILINEAR);

    //draw a green rectangle int the area the src_bmp was copied to
    RGBQUAD green = {0, 255, 0, 0};
    FreeImage_Plus::DrawRect(dest_bmp, dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h, 1, false, &green);

    //save the bitmap as a png file
    FreeImage_Save(FIF_PNG, dest_bmp, "downscaled.png");

    //free the bitmap memory
    FreeImage_Unload(src_bmp);
    FreeImage_Unload(dest_bmp);

}

//graph
void example6(){
    std::vector <vector2> sin_points, cos_points;

    double sampleFrequency = 44100;
    double signalFrequency = 440;
    int samplesForPeriod = static_cast<int>(sampleFrequency / signalFrequency);

    //sample a sinusoidal and a cosinusoidal signal for two periods, and puts the samples in the corrisponding vectors
    for(int i = 0; i < samplesForPeriod * 2; i++){
        double w = 2 * M_PI * signalFrequency;
        double t = static_cast<double>(i / sampleFrequency);
        sin_points.push_back({t,     //time in seconds
                             sin(t*w)});        //value
        cos_points.push_back({t,     //time in seconds
                             cos(t*w)});        //value
    }

    //define some colors
    RGBQUAD blue = {255, 0, 0, 0};
    RGBQUAD red = {0, 0, 255, 0};

    //set the options for the graph
    GraphOptions options;
    options.graphTitle = "Example graph";
    options.titleSize = 16;
    options.xAxisName = "Time";
    options.yAxisName = "Signal";
    options.axisTextSize = 7;
    options.width = 1000;       //graph width in pixels
    options.height = 600;       //graph height in pixels
    options.graphType = GraphType::LINE_GRAPH;

    //create a graph object
    FIP_Graph graph(options);

    //Plot the sin signal in red
    graph.PlotGraph(sin_points, 3, false, red);

    //Plot the cos signal in blue
    graph.PlotGraph(cos_points, 3, false, blue);

    //save the graph as a png
    graph.SaveToFile("graph.png", FIF_PNG);

}
