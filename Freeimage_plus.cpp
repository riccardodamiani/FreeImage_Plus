#include "Freeimage_plus.h"
#include <string.h>
#include <stdio.h>
#include <iostream>


//Init the FreeImage library
void FreeImage_Plus::Init(){
    FreeImage_Initialise();
}

/*
Draw a line in a bitmap
Args:
    The starting and ending points of the line
    The width of the line in pixels
    The color
*/
void FreeImage_Plus::DrawLine(FIBITMAP* img, int x0, int y0, int x1, int y1, int line_width, RGBQUAD *color){
    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
    int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

    for (line_width = (line_width+1)/2; ; ) {                                   /* pixel loop */
        FreeImage_SetPixelColor(img, x0, y0, color);
        e2 = err; x2 = x0;
        if (2*e2 >= -dx) {                                           /* x step */
            for (e2 += dy, y2 = y0; e2 < ed*line_width && (y1 != y2 || dx > dy); e2 += dx)
                FreeImage_SetPixelColor(img, x0, y2 += sy, color);
            if (x0 == x1) break;
            e2 = err; err -= dy; x0 += sx;
        }
        if (2*e2 <= dy) {                                            /* y step */
            for (e2 = dx-e2; e2 < ed*line_width && (x1 != x2 || dx < dy); e2 += dy)
                FreeImage_SetPixelColor(img, x2 += sx, y0, color);
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}

/*
Draw a rectangle in a bitmap
Args:
    The starting point of the rectangle (left, bottom)
    The width and the height
    The line width in pixels
    Whether the rectangle should be filled or not
    The color
*/
void FreeImage_Plus::DrawRect(FIBITMAP* img, int x, int y, int width, int height, int line_width, bool fill, RGBQUAD *color){

    if(fill){
        for(int i = x; i <= x + width; i++){
            for(int j = y; j <= y + height; j++){
                FreeImage_SetPixelColor(img, i, j, color);
            }
        }
        return;
    }

    if(line_width > 1){
        DrawRect(img, x + 1, y + 1, width - 2, height - 2, line_width - 1, false, color);
    }
    DrawLine(img, x, y, x + width, y, 1, color);
    DrawLine(img, x + width, y, x + width, y + height, 1, color);
    DrawLine(img, x + width, y + height, x, y + height, 1, color);
    DrawLine(img, x, y + height, x, y, 1, color);


}

/*
Draw a circle in a bitmap
Args:
    The center point of the circle
    The radius
    The line width in pixels
    Whether the circle should be filled or not
    The color
*/
void FreeImage_Plus::DrawCircle(FIBITMAP* img, int center_x, int center_y, int radius, int line_width, bool fill, RGBQUAD *color){
	int r, xc, yc, pk, x, y;

	xc = center_x;
	yc = center_y;
	r = radius;

	pk = 3 - 2*r;
	x=0; y = r;
	draw_points_for_circle(img, x, y, xc, yc, color);
	while(x < y)
	{
		if(pk <= 0)
		{
			pk = pk + (4*x) + 6;
			draw_points_for_circle(img, ++x, y, xc, yc, color);
		}
		else
		{
			pk = pk + (4*(x-y)) + 10;
			draw_points_for_circle(img, ++x, --y, xc, yc, color);
		}
	}

    if(fill){
        for(int i = xc + radius; i > xc - radius; i--){
            for(int j = 0; j <= radius; j++){
                if(sqrt((float)(xc -  i) * (xc - i) + (yc - (j + yc)) * (yc - (j + yc))) <= (float)radius){
                    FreeImage_SetPixelColor(img, i, yc + j, color);
                    FreeImage_SetPixelColor(img, i, yc - j, color);
                }
            }
        }
    }else if(line_width > 1){
        DrawCircle(img, center_x, center_y, radius - 1, line_width - 1, fill, color);
	}
}

void FreeImage_Plus::draw_points_for_circle(FIBITMAP* img, int x, int y, int xc, int yc, RGBQUAD *color)
{
	FreeImage_SetPixelColor(img, x+xc,y+yc, color);
	FreeImage_SetPixelColor(img, -x+xc,y+yc, color);
	FreeImage_SetPixelColor(img, x+xc, -y+yc, color);
	FreeImage_SetPixelColor(img, -x+xc, -y+yc, color);
	FreeImage_SetPixelColor(img, y+xc, x+yc, color);
	FreeImage_SetPixelColor(img, y+xc, -x+yc, color);
	FreeImage_SetPixelColor(img, -y+xc, x+yc, color);
	FreeImage_SetPixelColor(img, -y+xc, -x+yc, color);
}

/*Draw a text on a bitmap
Args:
    Text position
    A pointer to the text
    The size of the text corrisponding to the width in pixels of each character
    The alignment on the x axis and the y axis
    The text color
    The background color
*/
void FreeImage_Plus::DrawText(FIBITMAP* img, int x, int y, const char *text, int text_size, Alignment x_alignment, Alignment y_alignment, RGBQUAD *color, RGBQUAD *bgColor){
    int textWidth = TextWidth(text, text_size);
    int textHeight = TextHeight(text, text_size);

    if(x_alignment == Alignment::ALIGN_CENTER){
        x -= textWidth / 2;
    }else if(x_alignment == Alignment::ALIGN_RIGHT){
        x -= textWidth;
    }

    if(y_alignment == Alignment::ALIGN_CENTER){
        y -= textHeight / 2;
    }else if(y_alignment == Alignment::ALIGN_TOP){
        y -= textHeight;
    }

    FIBITMAP *base_text = _drawTextInternal(text, color, bgColor);
    Rect dst_rect = {x, y, textWidth, textHeight};
    CopyResize(base_text, img, nullptr, &dst_rect, Algorithm::NEAREST);

    FreeImage_Unload(base_text);
}

FIBITMAP* FreeImage_Plus::_drawTextInternal(const char *text, RGBQUAD *color, RGBQUAD *bgColor){
    int textWidth = TextWidth(text, 12);
    int textHeight = TextHeight(text, 12);

    FIBITMAP *img = FreeImage_Allocate(textWidth, textHeight, 24);
    int x = 0, y = textHeight-1;
    for(int c = 0; c < strlen(text); c++){
        int c_int = text[c];
        if(c_int < 32 || c_int > 128)
            continue;
        for(int j = 0; j < 16; j++){
            for(int i = 0; i < 12; i++){
                unsigned char mask = (0x80 >> ((j * 12 + i) % 8));
                int b = (j * 12 + i) / 8;
                unsigned char al = fontMap[c_int - 32][b];
                if(al & mask){
                    FreeImage_SetPixelColor(img, x + c*12 + i, y - j + 1, color);
                }else{
                    FreeImage_SetPixelColor(img, x + c*12 + i, y - j + 1, bgColor);
                }
            }
        }
    }
    return img;
}

/*
Copy a rectangle of the src_img to a rectangle of a dst_img
Args:
    A pointer to the source bitmap
    A pointer to the destination bitmap
    A pointer to the rectangle you want to copy from the source bitmap. If it is nullptr the entire bitmap will be copied
    A pointer to the rectangle of the destination bitmap you want to copy to. If is nullptr the entire bitmap is filled
    The interpolation algorithm to use
*/
void FreeImage_Plus::CopyResize(FIBITMAP* src_img, FIBITMAP* dst_img, Rect *src_rect_ptr, Rect *dst_rect_ptr, Algorithm algorithm){

    if(src_img == nullptr || dst_img == nullptr){
        return;
    }

    Rect src_rect, dst_rect;
    if(src_rect_ptr == nullptr){
        int width = FreeImage_GetWidth(src_img);
        int height = FreeImage_GetHeight(src_img);
        src_rect = {0, 0, width, height};
    }else{
        src_rect = *src_rect_ptr;
    }
    if(dst_rect_ptr == nullptr){
        int width = FreeImage_GetWidth(dst_img);
        int height = FreeImage_GetHeight(dst_img);
        dst_rect = {0, 0, width, height};
    }else{
        dst_rect = *dst_rect_ptr;
    }

    vector2 dst_px_coords, src_px_coords;
    vector2 scale = {dst_rect.w / src_rect.w, dst_rect.h / src_rect.h };

    if(algorithm == Algorithm::NEAREST){
        for(int x = 0; x < dst_rect.w; x++){
            for(int y = 0; y < dst_rect.h; y++){
                dst_px_coords = {0.5 + x, 0.5 + y};
                src_px_coords = {src_rect.x + dst_px_coords.x / scale.x, src_rect.y + dst_px_coords.y / scale.y};
                RGBQUAD dst_color;
                FreeImage_GetPixelColor(src_img, src_px_coords.x, src_px_coords.y, &dst_color);
                FreeImage_SetPixelColor(dst_img, x + dst_rect.x, y + dst_rect.y, &dst_color);
            }
        }

    }else if(algorithm == Algorithm::BILINEAR){
        for(int x = 0; x < dst_rect.w; x++){
            for(int y = 0; y < dst_rect.h; y++){
                dst_px_coords = {0.5 + x, 0.5 + y};
                src_px_coords = {src_rect.x + dst_px_coords.x / scale.x, src_rect.y + dst_px_coords.y / scale.y};

                vector2 temp_coords = {src_px_coords.x + 0.5, src_px_coords.y + 0.5};
                int src_x_coord = temp_coords.x;
                int src_y_coord = temp_coords.y;

                //calculate the coords of the pixels to interpolate
                RGBQUAD p1, p2, p3, p4;
                vector2 p1_coords = {src_x_coord-1 + 0.5, src_y_coord -1 + 0.5};
                vector2 p2_coords = {src_x_coord + 0.5, src_y_coord -1 + 0.5};
                vector2 p3_coords = {src_x_coord-1 + 0.5, src_y_coord + 0.5};
                vector2 p4_coords = {src_x_coord + 0.5, src_y_coord + 0.5 };

                //if the pixel is not inside the image use black as a color
                if(!FreeImage_GetPixelColor(src_img, src_x_coord - 1, src_y_coord - 1, &p1))
                    p1 = {0, 0, 0, 0};
                if(!FreeImage_GetPixelColor(src_img, src_x_coord, src_y_coord - 1, &p2))
                    p2 = {0, 0, 0, 0};
                if(!FreeImage_GetPixelColor(src_img, src_x_coord - 1, src_y_coord, &p3))
                    p3 = {0, 0, 0, 0};
                if(!FreeImage_GetPixelColor(src_img, src_x_coord, src_y_coord, &p4))
                    p4 = {0, 0, 0, 0};

                //do the linear interpolation
                RGBQUAD r2 = {p1.rgbBlue * (p2_coords.x - src_px_coords.x) + p2.rgbBlue * (src_px_coords.x - p1_coords.x),
                        p1.rgbGreen * (p2_coords.x - src_px_coords.x) + p2.rgbGreen * (src_px_coords.x - p1_coords.x),
                        p1.rgbRed * (p2_coords.x - src_px_coords.x) + p2.rgbRed * (src_px_coords.x - p1_coords.x), 0};
                RGBQUAD r1 = {p3.rgbBlue * (p4_coords.x - src_px_coords.x) + p4.rgbBlue * (src_px_coords.x - p3_coords.x),
                        p3.rgbGreen * (p4_coords.x - src_px_coords.x) + p4.rgbGreen * (src_px_coords.x - p3_coords.x),
                        p3.rgbRed * (p4_coords.x - src_px_coords.x) + p4.rgbRed * (src_px_coords.x - p3_coords.x), 0};

                RGBQUAD dst_color = {r1.rgbBlue * (src_px_coords.y - p2_coords.y) + r2.rgbBlue * (p4_coords.y - src_px_coords.y),
                        r1.rgbGreen * (src_px_coords.y - p2_coords.y) + r2.rgbGreen * (p4_coords.y - src_px_coords.y ),
                        r1.rgbRed * (src_px_coords.y - p2_coords.y) + r2.rgbRed * (p4_coords.y - src_px_coords.y), 0};

                FreeImage_SetPixelColor(dst_img, x + dst_rect.x, y + dst_rect.y, &dst_color);
            }
        }
    }
}

//A filter that invert the bitmap color inside a rectangle. If the rect_ptr is nullptr the entire image filtered
void FreeImage_Plus::InvertColor(FIBITMAP* img, Rect *rect_ptr){

    if(img == nullptr){
        return;
    }

    Rect rect;
    if(rect_ptr == nullptr){
        int width = FreeImage_GetWidth(img);
        int height = FreeImage_GetHeight(img);
        rect = {0, 0, width, height};
    }else{
        rect = *rect_ptr;
    }

    for(int x = 0; x < rect.w; x++){
        for(int y = 0; y < rect.h; y++){
            RGBQUAD c;
            if(FreeImage_GetPixelColor(img, rect.x + x, rect.y + y, &c)){
                c = {255 - c.rgbBlue, 255 - c.rgbGreen, 255 - c.rgbRed};
                FreeImage_SetPixelColor(img, rect.x + x, rect.y + y, &c);
            }
        }
    }
}

//weird filter that emphasize borders and invert the colors
//the filter only affects non-sharp edges so it's recommended to pass the image in the blur filter first
void FreeImage_Plus::NeonFilter(FIBITMAP* img, Rect *rect_ptr){

    if(img == nullptr){
        return;
    }

    Rect rect;
    if(rect_ptr == nullptr){
        int width = FreeImage_GetWidth(img);
        int height = FreeImage_GetHeight(img);
        rect = {0, 0, width, height};
    }else{
        rect = *rect_ptr;
    }

    for(int x = 0; x < rect.w; x++){
        for(int y = 0; y < rect.h; y++){
            RGBQUAD c;
            if(FreeImage_GetPixelColor(img, rect.x + x, rect.y + y, &c)){
                c = {- c.rgbBlue,- c.rgbGreen, - c.rgbRed}; //here is the magic
                FreeImage_SetPixelColor(img, rect.x + x, rect.y + y, &c);
            }
        }
    }
}


/*
Perform a blur filtering in the bitmap
Args:
    The bitmap to filter
    The clearness: The higher the value the less blurry it gets
    The size of the kernel: Bigger values corrisponds to greater blur but also more time to process the image
Returns:
    A pointer to the filtered bitmap
*/
FIBITMAP* FreeImage_Plus::BlurFilter(FIBITMAP* img, float clearness, int kernelSize){

    if(img == nullptr){
        return nullptr;
    }
    if(kernelSize%2 == 0 || kernelSize == 1){
        std::cout << "! Error in BlurFilter(): Kernel size must be an odd number greater that 1" << std::endl;
        return nullptr;
    }
    int width = FreeImage_GetWidth(img);
    int height = FreeImage_GetHeight(img);

    float** kernel = new float*[kernelSize];
    for(int i = 0; i < kernelSize; i++){
        kernel[i] = new float[kernelSize];
        for(int j = 0; j < kernelSize; j++){
            kernel[i][j] = 1;
        }
    }
    kernel[kernelSize/2][kernelSize/2] = clearness;
    float div = kernelSize*kernelSize - 1 + clearness;

    FIBITMAP *newImg = FreeImage_Allocate(width, height, 24);

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            RGBQUAD c;
            calculate_blur(img, i, j, kernel, kernelSize, div, &c);
            FreeImage_SetPixelColor(newImg, i, j, &c);
        }
    }
    for(int i = 0; i < kernelSize; i++){
        delete kernel[i];
    }
    delete kernel;
    return newImg;
}

void FreeImage_Plus::calculate_blur(FIBITMAP* img, int x, int y, float **kernel, int kernelSize, float div, RGBQUAD *out_color){
    float r = 0, g = 0, b = 0;
    int offset = kernelSize / 2;

    for(int i = 0; i < kernelSize; i++){
        for(int j = 0; j < kernelSize; j++){
            RGBQUAD c;
            if(FreeImage_GetPixelColor(img, x + i - offset, y + j - offset, &c)){
                r += c.rgbRed * kernel[i][j];
                g += c.rgbGreen * kernel[i][j];
                b += c.rgbBlue * kernel[i][j];
            }
        }
    }
    out_color->rgbBlue = b / div;
    out_color->rgbGreen = g / div;
    out_color->rgbRed = r / div;
}

//Calculates the width in pixels of the text
int FreeImage_Plus::TextWidth(const char *text, int font_size){
    return strlen(text) * font_size;
}

//Calculates the height in pixels of the text
int FreeImage_Plus::TextHeight(const char *text, int font_size){
    return 1.33 * font_size;
}
