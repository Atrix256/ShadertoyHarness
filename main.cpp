#include "glslAdapters.h"

#define in 
/*
    By Alan Wolfe
    http://blog.demofox.org

	Analytical solve for ray vs infinite layers of infinite concentric circles.

	Continuation of:
    https://www.shadertoy.com/view/MlK3zt
    and sebbi's:
    https://www.shadertoy.com/view/lly3Rc

*/

#define DARKEN_OVER_DISTANCE 1  // This makes it easier to see the different layers in a static image, not to hide a max distance.

const float c_cameraDistance	= 6.0;
const float c_cameraViewWidth	= 24.0;

//============================================================
float binarySign (float v)
{
    return step(0.0, v) * 2.0 - 1.0;
}

//============================================================
// returns t
// circle xy = position, z = radius
// Adapted from "real time collision detection" IntersectRaySphere()
float RayIntersectCircle (in vec2 rayPos, in vec2 rayDir, in vec3 circle)
{
	vec2 m = rayPos - circle.xy;
	float b = dot(m, rayDir);
	float c = dot(m, m) - circle.z*circle.z;
	
	// Exit if the ray is outside the circle and pointing away from the circle
	if (c > 0.0 && b > 0.0)
		return -1.0;

	float discr = b*b - c;

	// A negative discriminant means it missed the sphere
	if (discr < 0.0)
		return -1.0;

	float t = -b - sqrt(discr);
	if (t < 0.0)
		t = -b + sqrt(discr);

	return t;
}

//============================================================
float NumberStepsFunction_Circle (vec2 current, vec2 stepValue)
{
   
    const float c_circleRadiusStep = 2.5;
    const float c_circleWidth = 2.0;
    const float c_circleHalfWidth = c_circleWidth * 0.5;
   
    // find out how far we are from the center of the circles
	float currentDist = length(current);
    
    // find the distance of the circle more inward than where we are, and more outward.
    float innerDistance = floor(currentDist / c_circleRadiusStep) * c_circleRadiusStep + c_circleHalfWidth;
    float outerDistance = ceil(currentDist / c_circleRadiusStep) * c_circleRadiusStep - c_circleHalfWidth;
    
    if (currentDist < c_circleRadiusStep)
        innerDistance = 0.0;
    
    // if we are already inside the shape, no steps need to be taken
    if (currentDist < innerDistance || currentDist > outerDistance)
        return 0.0;
    
    // Test our ray against both inner and outer circles to see which we hit first.
    // If the ray is going outwards (dot(current, stepValue) >= 0), we technically only need to test the outer circle.
    // But, if the ray is going inwards, it could hit either.
    // No harm in testing against both though.
    float innerSteps = RayIntersectCircle(current, stepValue, vec3(0.0, 0.0, innerDistance));
    float outerSteps = RayIntersectCircle(current, stepValue, vec3(0.0, 0.0, outerDistance));
    
   	// return the first valid intersection if there is one
    if (innerSteps < 0.0 && outerSteps < 0.0)
        return 0.0;
    
    if (innerSteps < 0.0)
        return ceil(outerSteps);
    
    if (outerSteps < 0.0)
        return ceil(innerSteps);
    
    return ceil(min(innerSteps, outerSteps));
}

//============================================================
void mainImage( vec4& fragColor, in vec2 fragCoord )
{
    // set up the camera
    vec3 cameraPos;
    vec3 rayDir;
    {
        vec2 percent = (fragCoord / iResolution.xy) - vec2(0.5,0.5);  

        vec3 offset = vec3(iGlobalTime, 0.1,0.01);

        float angleX = 0.0;
        float angleY = 0.0;

        if (iMouse.z > 0.0) {
            vec2 mouse = iMouse.xy / iResolution.xy;
            angleX = 3.14 + 6.28 * mouse.x;
            angleY = (mouse.y - 0.5) * 3.14;//(mouse.y * 3.90) - 0.4;
        }

        vec3 cameraFwd	= (vec3(sin(angleX)*cos(angleY), sin(angleY), cos(angleX)*cos(angleY)));           
        vec3 cameraRight = normalize(cross(vec3(0.0,1.0,0.0),cameraFwd));
        vec3 cameraUp = normalize(cross(cameraFwd, cameraRight));

        cameraPos = vec3(0.0, 0.0, -1.0) + offset;
        vec3 cameraTarget = vec3(0.0, 0.0, 0.0) + offset;

        float cameraViewHeight	= c_cameraViewWidth * iResolution.y / iResolution.x;
        vec3 rayTarget = cameraPos +  cameraFwd * c_cameraDistance + cameraRight * c_cameraViewWidth * percent.x + cameraUp * cameraViewHeight * percent.y;
        rayDir = normalize(rayTarget - cameraPos);
    }

    // modulus camera z to simplify math, since it just repeats endlessly on z axis anyways
	cameraPos.z = fract(cameraPos.z);   
    
    // If ray facing negative on z axis, just flip direction and invert where we are in the cube on the z axis.
    // Now we only have to deal with positive z directions.
    if (rayDir.z < 0.0) {
        rayDir *= -1.0;
        cameraPos.z = 1.0 - cameraPos.z;
    }
        
    // calculate the 3d position of the first ray hit, as a place to start our uv raytrace.
    float intersection1Distance = (1.0 - cameraPos.z) / rayDir.z;
    vec3 intersection1 = (cameraPos + rayDir * intersection1Distance);
     
    // Calculate how much the uv changes when stepping along the z axis one unit.
    // We need to know this to know if the uvs are going positive or negative and by how much, on each axis.
    vec2 uvStep = rayDir.xy / rayDir.z;
          
    // calculate how many steps it takes to hit something on the X and Y axis and take whichever hits first.
    float steps = NumberStepsFunction_Circle(intersection1.xy, uvStep);    
        
    // calculate how far it is to the intersection we found
    float dist = (1.0 - cameraPos.z) / rayDir.z + steps / rayDir.z;
    
    #if DARKEN_OVER_DISTANCE
	float tint = clamp(1.0 - dist / 10.0, 0.0, 1.0);
    #else
    float tint = 1.0;
    #endif
    
    // calculate the hit point
    vec3 hitPoint = cameraPos + rayDir * dist;
    vec2 uv = hitPoint.xy;
    
    // sample the texture
	//fragColor = vec4(texture2D(iChannel0, uv).rgb * tint, 1.0);    
	fragColor = vec4(uv*tint, 0.0, 1.0);
}

/*

TODO:

* concentric circles

* How thick does the circle need to be? ray could be moving up to 1.0 on each axis i think!
 * it may actually be unbound?
 * use the harness to find out maximum value in practice? may be based on resolution / aspect ratio ):

* make the camera move around on x,y axis too?

* maybe we can make the radius of the circles grow and shink over time
 * per layer? dunno

* is the darken over distance correct? maybe try non linear like distance fog?

--------------------
Note: maximum uv step is based on camera stuff.

uvStep = rayDir.xy / rayDir.z

! formalize how rayDir is created so you can explain maximum uv step size on an axis.



*/
