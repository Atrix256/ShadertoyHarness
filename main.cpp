#include "glslAdapters.h"

#define in 
/*
    By Alan Wolfe
    http://blog.demofox.org

	Analytical solve for ray vs infinite pillars of shapes defined by support vectors.

	Based on an idea sebbi shared on twitter (https://twitter.com/SebAaltonen) for O(1)
	constant time ray vs infinite grid of pillars intersection.
*/

#define SHOW_2D_SHAPE        1  // This makes the 2d shape be shown in upper left corner

const float c_pillarHeight = 2.0;

const float c_cameraDistance	= 6.0;
const float c_cameraViewWidth	= 24.0;

const float c_pi = 3.14159265359;
const float c_twoPi = c_pi * 2.0;

//============================================================
float SupportVector_Circle (vec2 normalizedDir, float timePercent)
{
    return 0.3 + sin(timePercent*2.0*c_twoPi) * 0.1;
}

//============================================================
float IntersectPillarsSilhouette(vec2 pos, vec2 dir, float mode)
{
	//DAMNIT - need to diff this vs the shadertoy to make sure and get all changes

    // get a perpendicular 2d vector by swapping x and y, and negating one.
    vec2 dirRight = normalize(vec2(dir.y, -dir.x));
    
    // TODO: handle other shapes, depending on the mode
    float leftDist = -SupportVector_Circle(-dirRight, fract(mode));
    float rightDist = SupportVector_Circle(dirRight, fract(mode));
    
	// TODO: does this need to be between -0.5 and +0.5?
    float currentDist = dot(dirRight, pos);
	currentDist = fract(currentDist);
	currentDist = fract(currentDist + 0.5) - 0.5;

	// if we are already in the silhouette, we are done
	if (currentDist >= leftDist && currentDist <= rightDist)
		return 0.0;

	// else if 

	// TODO: find out how long til the ray  is in the silhouette of the object
	// TODO: THEN not done yet.  Still need to find actual intersection distance. May not be easy / possible with all support vector shapes.

	if (currentDist != 0.0)
	{
		int ijkl = 0;
	}
    
    return 0.0;
}

//============================================================
void mainImage( vec4& fragColor, in vec2 fragCoord )
{
    float mode = mod(iGlobalTime / 3.0f, 5.0f);
    
    #if SHOW_2D_SHAPE
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
            
            float dist = length(percent - 0.5);
            vec2 dir = normalize(percent - 0.5);
            
            // TODO: handle other shapes, depending on the mode
            color = SupportVector_Circle(dir, fract(mode)) < dist ? 0.0 : 1.0;
            
            /*
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
			*/
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

        float angleX = c_pi;
        float angleY = 0.0;

        if (iMouse.z > 0.0) {
            vec2 mouse = iMouse.xy / iResolution.xy;
            angleX = 3.14 + 6.28 * mouse.x;
            angleY = (mouse.y - 0.5) * 3.14;//(mouse.y * 3.90) - 0.4;
        }

        vec3 cameraFwd	= (vec3(sin(angleX)*cos(angleY), sin(angleY), cos(angleX)*cos(angleY)));           
        vec3 cameraRight = normalize(cross(cameraFwd, vec3(0.0, 1.0, 0.0)));
        vec3 cameraUp = normalize(cross(cameraRight, cameraFwd));

        cameraPos = vec3(0.0, 0.0, 6.0 - iGlobalTime);

        float cameraViewHeight	= c_cameraViewWidth * iResolution.y / iResolution.x;
        vec3 rayTarget = cameraPos +  cameraFwd * c_cameraDistance + cameraRight * c_cameraViewWidth * percent.x + cameraUp * cameraViewHeight * percent.y;
        rayDir = normalize(rayTarget - cameraPos);
    }    
    
    // Find how long it takes the ray to hit the top plane. aka for camera z to hit c_pillarHeight
    // TODO: handle looking in the wrong direction
    float topHitDist = (c_pillarHeight - cameraPos.z) / rayDir.z;
    vec3 topHit = cameraPos + rayDir * topHitDist;
    
    // Find how long it takes the ray to hit the floor. aka for camera z to hit 0.0
    // TODO: handle looking in the wrong direction
    float bottomHitDist = -cameraPos.z / rayDir.z;
    vec3 bottomHit = cameraPos + rayDir * bottomHitDist;
    
    // Calculate the uv space ray.
    // Convert it from [0,1] to [-0.5, 0.5].
    // TODO: this will fall apart when we are between the planes! In that case we need to start at the camera position, instead of top hit.
    float topHitDist2 = ((c_pillarHeight - 1.0) - cameraPos.z) / rayDir.z;  
    vec3 topHit2 = cameraPos + rayDir * topHitDist2;
    vec2 uvRay = topHit2.xy - topHit.xy;
    uvRay = fract(uvRay + 0.5) - 0.5;

    // calculate how long it takes for the ray to hit the pillars
    // TODO: this will fall apart when we are between the planes. Should be +0 in that case, instead of +topHitDist!
    float dist = IntersectPillarsSilhouette(topHit.xy, uvRay, mode) + topHitDist;
    
    
	// TODO: temp
    float canSeeTop = step(0.0, topHitDist);
    float canSeeBottom = step(0.0, bottomHitDist);
    vec3 pixelColor = vec3(topHit.xy, 0.0) * canSeeTop;   
    pixelColor = pixelColor * (1.0 - canSeeBottom) + mix(pixelColor, vec3(bottomHit.xy, 0.0), 0.5) * canSeeBottom;
    
    // output final gamma corrected color
	fragColor = vec4(pow(pixelColor, 1.0/2.2),1.0);
}

/*

TODO:

? do support vector shapes need to have direction normalized?

! could do reflections and shadows i think! Maybe refraction too.  All fixed costs.

* make camera look work

* we could animate the height of the pillars

* finish the todo's

* make a red/blue version

? can you offset the pillars every other row?

----- ORIGINAL PLAN -----

* ray vs infinite field of pillars, constrained between two planes.
* 1st plane (looking down from top) is the top of the pillars
* 2nd plane is the ground. texture the ground.

* three stages:
	1) looking down from sky for a second or two
	2) go down to the surface
    3) run through pillars

* Do several pillars, defined by support vector shapes
 * animate size over time?

* could try lerping between intersections of two shapes to morph from one shape to the next
 * seems like it could be problematic when the intersection is from two different "steps", but try it out.

* support vectors work because you just need to find width of shape when viewed from raydir.
 * IE you find max distance on +/- relative x axis.
 ! they don't work for final intersection though. you still need a ray vs shape function too.

* show shape of pillar in upper left, like you do with the other shader (https://www.shadertoy.com/view/MlK3zt)

* show george when done?
 * not sure how he did his, might also be worth looking into.

 ----- NOTES -----

 * could animate position over time of the grid, but far easier to animate camera for same effect so no need.

*/