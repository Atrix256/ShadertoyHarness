#pragma once

#include <vector>
#include <stdint.h>

typedef uint8_t uint8;

struct SImageData
{
    SImageData()
        : m_width(0)
        , m_height(0)
    { }
 
    long m_width;
    long m_height;
    long m_pitch;
    std::vector<uint8> m_pixels;
};

bool LoadImage (const char *fileName, SImageData& imageData);

bool SaveImage (const char *fileName, const SImageData &image);