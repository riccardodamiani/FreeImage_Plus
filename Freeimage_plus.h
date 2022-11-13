#ifndef FREEIMAGE_PLUS_H
#define FREEIMAGE_PLUS_H

#include <math.h>
#include <algorithm>
#include <string.h>
#include <vector>

#include "Freeimage.h"

struct vector2{
    double x, y;
};

struct Rect{
    double x, y;
    double w, h;
};

namespace{
    //The font contains only printable ASCII characters from 32 to 128
    //Each chatacter is 16x12 pixels (raws x columns). Each pixel is rapresented by a bit,
    //so each character has a total of 16*12 bits = 192 bits = 24 bytes
    //If the bit is 1 the text color is printed, otherwise the background color is printed
    const static unsigned char fontMap[96][24]{
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},      //space
        {0x06,0x00,0x60,0x0F,0x00,0xF0,0x0F,0x00,0xF0,0x0F,0x00,0x60,0x06,0x00,0x60,0x00,0x00,0x00,0x06,0x00,0x60,0x00,0x00,0x00},       //!
        {0x00,0x00,0x00,0x19,0x81,0x98,0x19,0x81,0x98,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //"
        {0x00,0x00,0x66,0x06,0x60,0x66,0x3F,0xF0,0xCC,0x0C,0xC1,0x98,0x19,0x87,0xFC,0x33,0x03,0x30,0x33,0x00,0x00,0x00,0x00,0x00},      //#
        {0x06,0x00,0x60,0x1F,0x83,0xFC,0x36,0x03,0x60,0x3F,0x81,0xFC,0x06,0xC0,0x6C,0x3F,0xC1,0xF8,0x06,0x00,0x60,0x00,0x00,0x00},      //...
        {0x00,0x00,0x00,0x00,0x13,0x83,0x38,0x73,0x8E,0x01,0xC0,0x38,0x07,0x00,0xE0,0x1C,0x03,0x8E,0x70,0xE6,0x0E,0x00,0x00,0x00},
        {0x00,0x00,0x70,0x0D,0x81,0x98,0x19,0x81,0xB0,0x0E,0x01,0xE0,0x3E,0x03,0x36,0x33,0xC3,0x18,0x3B,0xC1,0xE6,0x00,0x00,0x00},
        {0x0E,0x00,0xE0,0x0E,0x00,0x60,0x06,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x03,0x80,0x60,0x0E,0x00,0xC0,0x1C,0x01,0xC0,0x1C,0x01,0xC0,0x1C,0x01,0xC0,0x0C,0x00,0xE0,0x06,0x00,0x38,0x00,0x00,0x00},
        {0x1C,0x00,0x60,0x07,0x00,0x30,0x03,0x80,0x38,0x03,0x80,0x38,0x03,0x80,0x38,0x03,0x00,0x70,0x06,0x01,0xC0,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x03,0x6C,0x36,0xC1,0xF8,0x0F,0x03,0xFC,0x0F,0x01,0xF8,0x36,0xC3,0x6C,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x60,0x06,0x03,0xFC,0x3F,0xC0,0x60,0x06,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x0E,0x00,0xE0,0x06,0x00,0xC0},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFC,0x3F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x0E,0x00,0xE0,0x00,0x00,0x00},
        {0x00,0x00,0x01,0x00,0x30,0x07,0x00,0xE0,0x1C,0x03,0x80,0x70,0x0E,0x01,0xC0,0x38,0x07,0x00,0x60,0x00,0x00,0x00,0x00,0x00},
        {0x0F,0x83,0xFE,0x30,0x66,0x07,0x60,0xF6,0x1B,0x63,0x36,0x63,0x6C,0x37,0x83,0x70,0x33,0x06,0x3F,0xE0,0xF8,0x00,0x00,0x00},
        {0x03,0x00,0x70,0x1F,0x01,0xF0,0x03,0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x30,0x1F,0xE1,0xFE,0x00,0x00,0x00},
        {0x1F,0xC3,0xFE,0x70,0x76,0x03,0x60,0x70,0x0E,0x01,0xC0,0x38,0x07,0x00,0xE0,0x1C,0x03,0x80,0x7F,0xF7,0xFF,0x00,0x00,0x00},
        {0x1F,0xC3,0xFE,0x70,0x76,0x03,0x00,0x30,0x07,0x0F,0xE0,0xFC,0x00,0x60,0x03,0x60,0x37,0x07,0x3F,0xE1,0xFC,0x00,0x00,0x00},
        {0x01,0xC0,0x3C,0x07,0xC0,0xEC,0x1C,0xC3,0x8C,0x70,0xC6,0x0C,0x7F,0xF7,0xFF,0x00,0xC0,0x0C,0x00,0xC0,0x0C,0x00,0x00,0x00},
        {0x7F,0xF7,0xFF,0x60,0x06,0x00,0x60,0x07,0xFC,0x3F,0xE0,0x07,0x00,0x30,0x03,0x60,0x37,0x07,0x3F,0xE1,0xFC,0x00,0x00,0x00},
        {0x03,0xC0,0x7C,0x0E,0x01,0xC0,0x38,0x03,0x00,0x7F,0xC7,0xFE,0x70,0x76,0x03,0x60,0x37,0x07,0x3F,0xE1,0xFC,0x00,0x00,0x00},
        {0x7F,0xF7,0xFF,0x00,0x60,0x06,0x00,0xC0,0x0C,0x01,0x80,0x18,0x03,0x00,0x30,0x06,0x00,0x60,0x0C,0x00,0xC0,0x00,0x00,0x00},
        {0x0F,0x81,0xFC,0x38,0xE3,0x06,0x30,0x63,0x8E,0x1F,0xC3,0xFE,0x70,0x76,0x03,0x60,0x37,0x07,0x3F,0xE1,0xFC,0x00,0x00,0x00},
        {0x1F,0xC3,0xFE,0x70,0x76,0x03,0x60,0x37,0x07,0x3F,0xF1,0xFF,0x00,0x60,0x0E,0x01,0xC0,0x38,0x1F,0x01,0xE0,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0xE0,0x0E,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0xE0,0x0E,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0xE0,0x0E,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0xE0,0x0E,0x00,0x60,0x06,0x00,0xC0},
        {0x00,0xC0,0x1C,0x03,0x80,0x70,0x0E,0x01,0xC0,0x38,0x03,0x80,0x1C,0x00,0xE0,0x07,0x00,0x38,0x01,0xC0,0x0C,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFE,0x3F,0xE0,0x00,0x00,0x03,0xFE,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x30,0x03,0x80,0x1C,0x00,0xE0,0x07,0x00,0x38,0x01,0xC0,0x1C,0x03,0x80,0x70,0x0E,0x01,0xC0,0x38,0x03,0x00,0x00,0x00,0x00},
        {0x1F,0x83,0xFC,0x70,0xE6,0x06,0x60,0xE0,0x1C,0x03,0x80,0x70,0x06,0x00,0x60,0x06,0x00,0x00,0x06,0x00,0x60,0x00,0x00,0x00},
        {0x1F,0xC3,0xFE,0x30,0x66,0x7B,0x6F,0xB6,0xDB,0x6D,0xB6,0xDB,0x6D,0xB6,0xFE,0x67,0xC7,0x00,0x3F,0xC0,0xFC,0x00,0x00,0x00},
        {0x06,0x00,0x60,0x0F,0x00,0xF0,0x0F,0x01,0x98,0x19,0x81,0x98,0x30,0xC3,0xFC,0x3F,0xC6,0x06,0x60,0x66,0x06,0x00,0x00,0x00},
        {0x7F,0x07,0xF8,0x61,0xC6,0x0C,0x60,0xC6,0x1C,0x7F,0x87,0xFC,0x60,0xE6,0x06,0x60,0x66,0x0E,0x7F,0xC7,0xF8,0x00,0x00,0x00},
        {0x0F,0x81,0xFC,0x38,0xE3,0x06,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x30,0x63,0x8E,0x1F,0xC0,0xF8,0x00,0x00,0x00},
        {0x7F,0x07,0xF8,0x61,0xC6,0x0C,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0xC6,0x1C,0x7F,0x87,0xF0,0x00,0x00,0x00},
        {0x7F,0xE7,0xFE,0x60,0x06,0x00,0x60,0x06,0x00,0x7F,0x87,0xF8,0x60,0x06,0x00,0x60,0x06,0x00,0x7F,0xE7,0xFE,0x00,0x00,0x00},
        {0x7F,0xE7,0xFE,0x60,0x06,0x00,0x60,0x06,0x00,0x7F,0x87,0xF8,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x00,0x00,0x00},
        {0x0F,0xC1,0xFE,0x38,0x63,0x00,0x60,0x06,0x00,0x63,0xE6,0x3E,0x60,0x66,0x06,0x30,0x63,0x86,0x1F,0xE0,0xFE,0x00,0x00,0x00},
        {0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x7F,0xE7,0xFE,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x00,0x00,0x00},
        {0x1F,0x81,0xF8,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x1F,0x81,0xF8,0x00,0x00,0x00},
        {0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x66,0x06,0x60,0x67,0x0C,0x3F,0xC1,0xF8,0x00,0x00,0x00},
        {0x60,0x66,0x0E,0x61,0xC6,0x38,0x67,0x06,0xE0,0x7C,0x07,0xC0,0x6E,0x06,0x70,0x63,0x86,0x1C,0x60,0xE6,0x06,0x00,0x00,0x00},
        {0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x7F,0xE7,0xFE,0x00,0x00,0x00},
        {0x60,0x67,0x0E,0x70,0xE7,0x9E,0x79,0xE6,0xF6,0x6F,0x66,0x66,0x66,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x00,0x00,0x00},
        {0x60,0x67,0x06,0x70,0x67,0x86,0x6C,0x66,0xC6,0x66,0x66,0x66,0x63,0x66,0x36,0x61,0xE6,0x0E,0x60,0xE6,0x06,0x00,0x00,0x00},
        {0x0F,0x01,0xF8,0x39,0xC3,0x0C,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x30,0xC3,0x9C,0x1F,0x80,0xF0,0x00,0x00,0x00},
        {0x7F,0x87,0xFC,0x60,0xE6,0x06,0x60,0x66,0x06,0x60,0xE7,0xFC,0x7F,0x86,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x00,0x00,0x00},
        {0x0F,0x01,0xF8,0x39,0xC3,0x0C,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x36,0x33,0xC3,0x9C,0x1F,0xE0,0xF6,0x00,0x00,0x00},
        {0x7F,0x87,0xFC,0x60,0xE6,0x06,0x60,0x66,0x06,0x60,0xE7,0xFC,0x7F,0x86,0x70,0x63,0x86,0x1C,0x60,0xE6,0x06,0x00,0x00,0x00},
        {0x1F,0x83,0xFC,0x70,0xE6,0x06,0x60,0x07,0x00,0x3F,0x81,0xFC,0x00,0xE0,0x06,0x60,0x67,0x0E,0x3F,0xC1,0xF8,0x00,0x00,0x00},
        {0x3F,0xC3,0xFC,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x00,0x00,0x00},
        {0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x63,0x0C,0x3F,0xC1,0xF8,0x00,0x00,0x00},
        {0x60,0x66,0x06,0x60,0x63,0x0C,0x30,0xC3,0x0C,0x19,0x81,0x98,0x19,0x80,0xF0,0x0F,0x00,0xF0,0x06,0x00,0x60,0x00,0x00,0x00},
        {0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x66,0x66,0x66,0x66,0xF6,0x79,0xE7,0x0E,0x70,0xE6,0x06,0x00,0x00,0x00},
        {0x60,0x66,0x06,0x30,0xC3,0x0C,0x19,0x80,0xF0,0x06,0x00,0x60,0x0F,0x01,0x98,0x30,0xC3,0x0C,0x60,0x66,0x06,0x00,0x00,0x00},
        {0x60,0x66,0x06,0x30,0xC3,0x0C,0x19,0x81,0x98,0x0F,0x00,0xF0,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x00,0x00,0x00},
        {0x7F,0xE7,0xFE,0x00,0xC0,0x0C,0x01,0x80,0x30,0x06,0x00,0x60,0x0C,0x01,0x80,0x30,0x03,0x00,0x7F,0xE7,0xFE,0x00,0x00,0x00},
        {0x1F,0x81,0xF8,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x1F,0x81,0xF8,0x00,0x00,0x00},
        {0x00,0x04,0x00,0x60,0x07,0x00,0x38,0x01,0xC0,0x0E,0x00,0x70,0x03,0x80,0x1C,0x00,0xE0,0x07,0x00,0x30,0x00,0x00,0x00,0x00},
        {0x1F,0x81,0xF8,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x1F,0x81,0xF8,0x00,0x00,0x00},
        {0x02,0x00,0x70,0x0F,0x81,0xDC,0x38,0xE7,0x07,0x60,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xF7,0xFF},
        {0x00,0x00,0x70,0x07,0x00,0x70,0x06,0x00,0x60,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFC,0x3F,0xE0,0x06,0x1F,0xE3,0xFE,0x60,0x66,0x06,0x7F,0xE3,0xFE,0x00,0x00,0x00},
        {0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0xF8,0x7F,0xC7,0x0E,0x60,0x66,0x06,0x60,0x66,0x0E,0x7F,0xC7,0xF8,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0x3F,0xC7,0x06,0x60,0x06,0x00,0x60,0x07,0x06,0x3F,0xC1,0xF8,0x00,0x00,0x00},
        {0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x61,0xF6,0x3F,0xE7,0x1E,0x60,0x66,0x06,0x60,0x67,0x06,0x3F,0xE1,0xFE,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0x3F,0xC7,0x06,0x7F,0xE7,0xFC,0x60,0x07,0x00,0x3F,0xC1,0xF8,0x00,0x00,0x00},
        {0x07,0x80,0xF8,0x1C,0x01,0x80,0x18,0x01,0x80,0x7F,0x07,0xF0,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFE,0x3F,0xE7,0x06,0x60,0x67,0x0E,0x3F,0xE1,0xF6,0x00,0x60,0x0E,0x3F,0xC3,0xF8},
        {0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0xF0,0x7F,0x87,0x1C,0x60,0xC6,0x0C,0x60,0xC6,0x0C,0x60,0xC6,0x0C,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x06,0x00,0x60,0x00,0x00,0xE0,0x0E,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x1F,0x81,0xF8,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x01,0x80,0x18,0x00,0x00,0x38,0x03,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x81,0x98,0x1F,0x80,0xF0},
        {0x30,0x03,0x00,0x30,0x03,0x00,0x30,0x03,0x18,0x33,0x83,0x70,0x3E,0x03,0xE0,0x37,0x03,0x38,0x31,0xC3,0x0C,0x00,0x00,0x00},
        {0x0E,0x00,0xE0,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x1F,0x81,0xF8,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x98,0x7F,0xC7,0xFE,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF8,0x3F,0xC3,0x0E,0x30,0x63,0x06,0x30,0x63,0x06,0x30,0x63,0x06,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0x3F,0xC7,0x0E,0x60,0x66,0x06,0x60,0x67,0x0E,0x3F,0xC1,0xF8,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF8,0x7F,0xC6,0x0E,0x60,0x66,0x06,0x70,0xE7,0xFC,0x6F,0x86,0x00,0x60,0x06,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFE,0x3F,0xE7,0x06,0x60,0x66,0x06,0x70,0xE3,0xFE,0x1F,0x60,0x06,0x00,0x60,0x06},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x7C,0x3F,0xE3,0x86,0x30,0x03,0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x7F,0x86,0x00,0x7F,0x03,0xF8,0x01,0x80,0x18,0x7F,0x83,0xF0,0x00,0x00,0x00},
        {0x00,0x01,0x80,0x18,0x01,0x80,0x18,0x07,0xF0,0x7F,0x01,0x80,0x18,0x01,0x80,0x18,0x01,0x80,0x1F,0x80,0xF8,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x60,0x66,0x06,0x60,0x66,0x06,0x60,0x67,0x0E,0x3F,0xE1,0xF6,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x60,0x63,0x0C,0x30,0xC1,0x98,0x19,0x80,0xF0,0x0F,0x00,0x60,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x6F,0x63,0xFC,0x39,0xC1,0x08,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0C,0x71,0xC3,0xB8,0x1F,0x00,0xE0,0x1F,0x03,0xB8,0x71,0xC6,0x0C,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0C,0x30,0xC1,0x98,0x19,0x80,0xF0,0x0F,0x00,0x60,0x06,0x00,0xC0,0x0C,0x01,0x80},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x7F,0x80,0x30,0x06,0x00,0xC0,0x18,0x03,0x00,0x7F,0xC7,0xFC,0x00,0x00,0x00},
        {0x03,0xC0,0x7C,0x0E,0x00,0xC0,0x0C,0x00,0xC0,0x1C,0x03,0x80,0x1C,0x00,0xC0,0x0C,0x00,0xC0,0x0E,0x00,0x7C,0x03,0xC0,0x00},
        {0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x00,0x00,0x00},
        {0x3C,0x03,0xE0,0x07,0x00,0x30,0x03,0x00,0x30,0x03,0x80,0x1C,0x03,0x80,0x30,0x03,0x00,0x30,0x07,0x03,0xE0,0x3C,0x00,0x00},
        {0x00,0x00,0x00,0x1C,0x63,0x6C,0x63,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0xF0,0x19,0x83,0x0C,0x60,0x66,0x06,0x7F,0xE7,0xFE,0x00,0x00,0x00,0x00,0x00,0x00}
    };
}

class FreeImage_Plus{
    public:

    enum Alignment{
        ALIGN_RIGHT,
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_TOP,
        ALIGN_BOTTOM
    };

    enum FontSize{
        FONT_SMALL,
        FONT_NORMAL,
        FONT_LARGE,
        FONT_EXTRA_LARGE,
    };

    enum Algorithm{
        NEAREST,
        BILINEAR,
        BICUBIC
    };

    static void Init();
    static void DrawLine(FIBITMAP* img, int x0, int y0, int x1, int y1, int width, RGBQUAD *color);
    static void DrawRect(FIBITMAP* img, int x, int y, int width, int height, int line_width, bool fill, RGBQUAD *color);
    static void DrawCircle(FIBITMAP* img, int center_x, int center_y, int radius, int line_width, bool fill, RGBQUAD *color);
    static void DrawText(FIBITMAP* img, int x, int y, const char *text, int font_size, Alignment x_alignment, Alignment y_alignment, RGBQUAD *text_color, RGBQUAD *bgColor);
    static void CopyResize(FIBITMAP* src_img, FIBITMAP* dst_img, Rect *src_rect, Rect *dest_rect, Algorithm algorithm);
    static void InvertColor(FIBITMAP* img, Rect *rect_ptr);
    static void NeonFilter(FIBITMAP* img, Rect *rect_ptr);
    static FIBITMAP* BlurFilter(FIBITMAP* img, float clearness, int kernelSize);
    static int TextWidth(const char *text, int font_size);
    static int TextHeight(const char *text, int font_size);

    private:
    static void draw_points_for_circle(FIBITMAP* img, int x, int y, int xc, int yc, RGBQUAD *color);
    static FIBITMAP* _drawTextInternal(const char *text, RGBQUAD *color, RGBQUAD *bgColor);
    static void calculate_blur(FIBITMAP* img, int x, int y, float **kernel, int kernelSize, float div, RGBQUAD *out_color);

};

#include "Freeimage_plus_graph.h"

#endif // FREEIMAGE_PLUS_H
