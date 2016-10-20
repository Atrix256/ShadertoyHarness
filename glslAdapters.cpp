#include "glslAdapters.h"
#include "Settings.h"
#include "SImageData.h"

float iGlobalTime = c_timeSeconds;
vec3 iResolution(float(c_imageResolution[0]), float(c_imageResolution[1]), 1.0f);
vec4 iMouse(0.0f);

int main(int agrc, char** argv)
{
	SImageData outImage;
	outImage.m_width = (long)c_imageResolution[0];
	outImage.m_height = (long)c_imageResolution[1];
	outImage.m_pitch = outImage.m_width * 3;
	if (outImage.m_pitch & 3)
	{
		outImage.m_pitch &= ~3;
		outImage.m_pitch += 4;
	}
	outImage.m_pixels.resize(outImage.m_pitch*outImage.m_height);

	vec4 fragColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	for (size_t y = 0; y < c_imageResolution[1]; ++y)
	{
		uint8* pixelRow = &outImage.m_pixels[y * outImage.m_pitch];
		for (size_t x = 0; x < c_imageResolution[0]; ++x)
		{
			mainImage(fragColor, vec2(x,y));

			// write the output color.  Note that the color channels are reversed!
			pixelRow[0] = uint8(clamp(fragColor[2], 0.0f, 1.0f) * 255.0f);
			pixelRow[1] = uint8(clamp(fragColor[1], 0.0f, 1.0f) * 255.0f);
			pixelRow[2] = uint8(clamp(fragColor[0], 0.0f, 1.0f) * 255.0f);
			pixelRow += 3;
		}
	}

	SaveImage(s_outImageFileName, outImage);
	return 0;
}
