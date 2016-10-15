#include "glslAdapters.h"

float iGlobalTime = 0.0f;
vec2 iResolution(100.0f, 100.0f);
vec4 iMouse(0.0f);

int main(int agrc, char** argv)
{
	vec4 fragColor(0.0f, 0.0f, 0.0f, 0.0f);
	vec2 fragCoord(49.0f, 49.0f);

	mainImage(fragColor, fragCoord);
	return 0;
}
