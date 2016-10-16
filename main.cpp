#include "glslAdapters.h"


/*
    By Alan Wolfe
    http://blog.demofox.org

    A remake and extension of sebbi's shader to understand the ideas better. (https://twitter.com/SebAaltonen)
    https://www.shadertoy.com/view/lly3Rc

    This shader ray traces infinite layers of infinite grids of shapes.  It's raytracing not ray marching
    which means it is a constant time operation and analytically solves for the intersection point.

    The shapes are:
    1) L
    2) square
    3) circle
    4) checkerboard
    5) thin square

    The idea is that you can reduce the problem from 3d to 2d by seeing where the ray hits the first two planes
    and then using the uv coordinates of those intersections as two points defining a 2d ray.  You then do a
    2d raytrace of that ray versus a shape, but valid intersection distances are a multiple of the vector's length
    instead of a continuous calculation.

    For this to work, you need to be able to analystically solve 2d ray vs 2d infinitely repeating shape.

    An way to solve this is to make sure the shape is constrained to a single tile, and when laying the shape on
    a grid, that no border between "open spaces" is less than 0.5 units.

    The reason for that is because the vector can at most move by 0.5 units on any single axis (since we modulus the vector).
    
    If we have a border that is less than 0.5, we will think we got a hit, when in fact it might be a miss, if the sample point
    is not actually inside the solid point!

    The last two shapes are fail cases.

    The checkerboard is a problem because if the ray is moving up and to the right or down and to the left, it can encounter
    border thickness less than 0.5.

    The thin square is a problem because it has a very thin border, which is less than 0.5 in all directions!

    The ultimate goal of this stuff is to try and figure out how to make infinitely repeating shapes in ray tracing, like
    can be done in ray marching via modulus!
    http://computergraphics.stackexchange.com/questions/4094/is-there-a-method-to-do-ray-marching-style-modulus-repeat-with-raytracing

    Blog post coming before too long.
*/

#define DARKEN_OVER_DISTANCE 1  // This makes it easier to see the different layers in a static image, not to hide a max distance.
#define SHOW_2d_SHAPE        1  // This makes the 2d shape be shown in upper left corner

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
// adapted from http://mathworld.wolfram.com/Circle-LineIntersection.html
// Could be done more efficiently, but it works :P
float RayIntersectCircle (vec2 rayPos, vec2 rayDir, vec3 circle)
{
    rayPos -= circle.xy;
    
    float x1 = rayPos.x;
    float y1 = rayPos.y;
    float x2 = rayPos.x + rayDir.x;
    float y2 = rayPos.y + rayDir.y;
    
    float dx = x2 - x1;
    float dy = y2 - y1;
    
    float d_r = length(vec2(dx, dy));
    
    float D = x1*y2 - x2*y1;
    
    float discr = circle.z * circle.z * d_r * d_r - D * D;
    
    if (discr < 0.0)
        return 2.0;
    
    float x = (D*dy - binarySign(dy)*dx*sqrt(discr)) / (d_r*d_r);
    float y = (-D*dx - abs(dy)*sqrt(discr)) / (d_r*d_r);
    
    float t;
    if (abs(rayDir.x) > 0.1)
        t = (vec2(x,y) - rayPos).x / rayDir.x;
    else
        t = (vec2(x,y) - rayPos).y / rayDir.y;
    
    if (t < 0.0)
    {
        x = (D*dy + binarySign(dy)*dx*sqrt(discr)) / (d_r*d_r);
        y = (-D*dx + abs(dy)*sqrt(discr)) / (d_r*d_r);

        if (abs(rayDir.x) > 0.1)
            t = (vec2(x,y) - rayPos).x / rayDir.x;
        else
            t = (vec2(x,y) - rayPos).y / rayDir.y;        
    }
    
    return t;
}

//============================================================
float NumberStepsFunction_L_OneAxis (float current, float stepValue)
{
    float steps;
    if (stepValue < 0.0)
        steps = (current - 0.5) / -stepValue;
    else
        steps = (1.0 - current) / stepValue;
    return steps;
}

//============================================================
bool BooleanFunction_L (vec2 current)
{
    return (current.x < 0.5 || current.y < 0.5);
}

//============================================================
float NumberStepsFunction_L (vec2 current, vec2 stepValue)
{
    if (BooleanFunction_L(current))
        return 0.0;
    
    float stepsX = NumberStepsFunction_L_OneAxis(current.x, stepValue.x);
    float stepsY = NumberStepsFunction_L_OneAxis(current.y, stepValue.y);
    return ceil(min(stepsX, stepsY));
}

//============================================================
bool BooleanFunction_Checker (vec2 current)
{
    if (current.x >= 0.5)
    {
        return current.y < 0.5;
    }
    else
    {
        return current.y >= 0.5;
    }
}

//============================================================
float NumberStepsFunction_Checker (vec2 current, vec2 stepValue)
{
    // if it's already in the shape, no steps to take
    if (BooleanFunction_Checker(current))
        return 0.0;

	// there are different values to reach based on where we are in the pattern.
    float lower = step(0.5, current.x) * 0.5;
    float upper = lower + 0.5;

    // see how long to escape the box on each axis, take sooner event
    float stepsX;
    if (stepValue.x < 0.0)
        stepsX = (current.x - lower) / -stepValue.x;
    else
        stepsX = (upper - current.x) / stepValue.x;

    float stepsY;
    if (stepValue.y < 0.0)
        stepsY = (current.y - lower) / -stepValue.y;
    else
        stepsY = (upper - current.y) / stepValue.y;     
    
	return ceil(min(stepsX, stepsY));    
}

//============================================================
bool BooleanFunction_Square (vec2 current)
{
    return (current.x < 0.25 || current.x > 0.75 || current.y < 0.25 || current.y > 0.75);
}

//============================================================
float NumberStepsFunction_Square (vec2 current, vec2 stepValue)
{
    // if it's already in the shape, no steps to take
    if (BooleanFunction_Square(current))
        return 0.0;
    
    float stepsX;
    if (stepValue.x < 0.0)
        stepsX = (current.x - 0.25) / -stepValue.x;
    else
        stepsX = (0.75 - current.x) / stepValue.x;
    
    float stepsY;
    if (stepValue.y < 0.0)
        stepsY = (current.y - 0.25) / -stepValue.y;
    else
        stepsY = (0.75 - current.y) / stepValue.y;    
    
    return ceil(min(stepsX, stepsY));
}

//============================================================
bool BooleanFunction_Circle (vec2 current)
{
    const float radius = 0.25;
    const float radiusSq = radius * radius;
    vec2 rel = current - vec2(0.5, 0.5);
    return rel.x*rel.x + rel.y*rel.y > radiusSq;
}

//============================================================
float NumberStepsFunction_Circle (vec2 current, vec2 stepValue)
{
    // if it's already in the shape, no steps to take
    if (BooleanFunction_Circle(current))
        return 0.0;
    
    float steps = RayIntersectCircle(current, stepValue, vec3(0.5, 0.5, 0.25));
    return ceil(steps);
}

//============================================================
bool BooleanFunction_ThinSquare (vec2 current)
{
    return (current.x < 0.05 || current.x > 0.95 || current.y < 0.05 || current.y > 0.95);
}

//============================================================
float NumberStepsFunction_ThinSquare (vec2 current, vec2 stepValue)
{
    // if it's already in the shape, no steps to take
    if (BooleanFunction_ThinSquare(current))
        return 0.0;
    
    float stepsX;
    if (stepValue.x < 0.0)
        stepsX = (current.x - 0.05) / -stepValue.x;
    else
        stepsX = (0.95 - current.x) / stepValue.x;
    
    float stepsY;
    if (stepValue.y < 0.0)
        stepsY = (current.y - 0.05) / -stepValue.y;
    else
        stepsY = (0.95 - current.y) / stepValue.y;    
    
    return ceil(min(stepsX, stepsY));
}
    
//============================================================
void mainImage( vec4& fragColor, vec2 fragCoord )
{
    
    float mode = mod(iGlobalTime / 3.0f, 5.0f);
    
    #if SHOW_2d_SHAPE
    {
        vec2 percent = (fragCoord / iResolution.xy);
        percent.x *= iResolution.x / iResolution.y;
        if (percent.x < 0.2 && percent.y > 0.8)
        {
            percent.x *= 5.0;
            percent.y = (percent.y - 0.8) * 5.0;
            float color = 1.0;
            if (abs(percent.x - 0.5) > 0.48 || abs(percent.y - 0.5) > 0.48)
            {
                fragColor = vec4(1.0, 0.5, 0.0, 1.0);
                return;
            }   
            
            if (mode < 1.0)
            	color = float(BooleanFunction_L(percent));                        
            else if (mode < 2.0)
                color = float(BooleanFunction_Square(percent));               
            else if (mode < 3.0)
                color = float(BooleanFunction_Circle(percent));   
            else if (mode < 4.0)
                color = float(BooleanFunction_Checker(percent));                  
            else if (mode < 5.0)
                color = float(BooleanFunction_ThinSquare(percent));                  
			fragColor = vec4(vec3(color), 1.0);
            return;                              
        }
    }
    #endif
    
    // set up the camera
    vec3 cameraPos;
    vec3 rayDir;
    {
        vec2 percent = (fragCoord / iResolution.xy) - vec2(0.5,0.5);  

        vec3 offset = vec3(0.5, 0.5, iGlobalTime+0.01);

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

    // keep the camera in a unit cube
	cameraPos = fract(cameraPos);   
    
    // If ray facing negative on z axis, just flip direction and invert where we are in the cube on the z axis.
    // Now we only have to deal with positive z directions.
    if (rayDir.z < 0.0) {
        rayDir *= -1.0;
        cameraPos.z = 1.0 - cameraPos.z;
    }
        
    // calculate the 3d position of the next two plane intersections
    float intersection1Distance = (1.0 - cameraPos.z) / rayDir.z;
    float intersection2Distance = (2.0 - cameraPos.z) / rayDir.z;
    vec3 intersection1 = fract(cameraPos + rayDir * intersection1Distance);
    vec3 intersection2 = fract(cameraPos + rayDir * intersection2Distance);
     
    // Calculate how much the uv changes from intersection1 to intersection2.
    // Convert it from [0,1] to [-0.5, 0.5].
    // We need to know this to know if the uvs are going positive or negative and by how much, on each axis.
    vec2 uvStep = intersection2.xy - intersection1.xy;
    uvStep = fract(uvStep + 0.5) - 0.5;
          
    // calculate how many steps it takes to hit something on the X and Y axis and take whichever hits first.
    float steps = 0.0;
    if (mode < 1.0)
    	steps = NumberStepsFunction_L(intersection1.xy, uvStep);
    else if (mode < 2.0)
        steps = NumberStepsFunction_Square(intersection1.xy, uvStep);
    else if (mode < 3.0)
        steps = NumberStepsFunction_Circle(intersection1.xy, uvStep);    
    else if (mode < 4.0)
        steps = NumberStepsFunction_Checker(intersection1.xy, uvStep);            
    else if (mode < 5.0)
        steps = NumberStepsFunction_ThinSquare(intersection1.xy, uvStep);        
        
    // calculate how far it is to the intersection we found
    float dist = (1.0 - cameraPos.z) / rayDir.z + steps / rayDir.z;
    
    #if DARKEN_OVER_DISTANCE
	float tint = clamp(1.0 - dist / 5.0, 0.0, 1.0);
    #else
    float tint = 1.0;
    #endif
    
    // calculate the hit point
    vec3 hitPoint = cameraPos + rayDir * dist;
    vec2 uv = hitPoint.xy;
    
    // sample the texture
	//fragColor = vec4(texture2D(iChannel0, uv).rgb * tint, 1.0);    

	fragColor = vec4(uv * tint, 0.0f, 1.0f);
}
