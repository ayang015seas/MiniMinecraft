#version 150

uniform mat4 u_ViewProj;
uniform vec3 u_Eye;
uniform ivec2 u_Dimensions;
uniform int u_Time;

out vec4 out_Col;

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

// THREE SETTINGS FOR THIS SHADER:
// COMP_HEAVY adds clouds to the sky during sunset and sunrise by applying FBM noise with a worley basis.
// COMP_HEAVY really slows down the GPU so I like COMP_GRAINY, which applies a small random noise to every fragment.
// COMP_LITE does not add any noise to the skybox.

//#define COMP_HEAVY
//#define COMP_LITE
#define COMP_GRAINY


// Sunset palette
const vec3 sunset[5] = vec3[](vec3(255.0, 229.0, 119.0) / 255.0,
                               vec3(254.0, 192.0, 81.0) / 255.0,
                               vec3(255.0, 137.0, 103.0) / 255.0,
                               vec3(253.0, 96.0, 81.0) / 255.0,
                               vec3(57.0, 32.0, 51.0) / 255.0);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0,
                            vec3(96, 72, 120) / 255.0,
                            vec3(72, 48, 120) / 255.0,
                            vec3(48, 24, 96) / 255.0,
                            vec3(0, 24, 72) / 255.0);

const vec3 sunColor = vec3(255.0, 255.0, 190.0) / 255.0;
const vec3 moonColor = vec3(196.0, 194.0, 165.0) / 255.0;
const float sunSize = 3.5;
const float moonSize = 1.7;
const vec3 cloudColor = sunset[3];

// Function to transform cartesian representation of a vector to a polar coordinates with constant radius.
vec2 sphereToUV(vec3 p) {
    float phi = atan(p.z, p.x);
    if(phi < 0) {
        phi += TWO_PI;
    }
    float theta = acos(clamp(-0.999999, 0.999999, p.y));
    return vec2(1.0 - phi / TWO_PI, 1.0 - theta / PI);
}

// Function to convert polar theta angle to fragment sky color during sunset.
// Theta ranges between 0 and 1, where 0.5 represents the horizon, and 1 represents high noon position.
// We linearly interpolate between discrete sunset shades as we increase theta to create a nice
// blend of shades that transitions from one shade to the next smoothly.
vec3 uvToSunset(float y) {
    if(y < 0.5) {
        return sunset[0];
    }
    else if(y < 0.55) {
        return mix(sunset[0], sunset[1], (y - 0.5) / 0.05);
    }
    else if(y < 0.6) {
        return mix(sunset[1], sunset[2], (y - 0.55) / 0.05);
    }
    else if(y < 0.65) {
        return mix(sunset[2], sunset[3], (y - 0.6) / 0.05);
    }
    else if(y < 0.75) {
        return mix(sunset[3], sunset[4], (y - 0.65) / 0.1);
    }
    return sunset[4];
}

// Same thing as uvToSunset but he handpick different shades to represent a dusk palette.
vec3 uvToDusk(float y) {
    if(y < 0.5) {
        return dusk[0];
    }
    else if(y < 0.55) {
        return mix(dusk[0], dusk[1], (y - 0.5) / 0.05);
    }
    else if(y < 0.6) {
        return mix(dusk[1], dusk[2], (y - 0.55) / 0.05);
    }
    else if(y < 0.65) {
        return mix(dusk[2], dusk[3], (y - 0.6) / 0.05);
    }
    else if(y < 0.75) {
        return mix(dusk[3], dusk[4], (y - 0.65) / 0.1);
    }
    return dusk[4];
}

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 *43758.5453);
}

float WorleyNoise3D(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }
    return minDist;
}

float WorleyNoise(vec2 uv)
{
    // Tile the space
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));

            // Random point inside current neighboring cell
            vec2 point = random2(uvInt + neighbor);

            // Animate the point
            point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float worleyFBM(vec3 uv) {
    float sum = 0;
    float freq = 4;
    float amp = 0.5;
    for(int i = 0; i < 4; i++) {
        sum += WorleyNoise3D(uv * freq) * amp;
        freq *= 2;
        amp *= 0.5;
    }
    return sum;
}

// We want the sun position to rotate about the X axis so that the sun appears to rise and set.
// If we want to simulate different regions of our world, we could change the axis of rotation for our sun,
// but this would require a more complicated rotation matrix.
vec3 rotateX(vec3 p, float a) {
    float s = sin(-a);
    float c = cos(-a);
    return vec3(p.x, c * p.y - s * p.z, s * p.y + c * p.z);
}

void main()
{
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0; // Grab normalized device coordinates
    vec4 p = vec4(ndc.xy, 1.0, 1.0);
    p *= 1000.0;
    p = u_ViewProj * p;                                            // ray cast by projecting our screen pixel into world space

    vec3 rayDir = normalize(p.xyz - u_Eye);                        // the direction of the ray relative our player position.
    vec2 uv = sphereToUV(rayDir);                                  // convert our ray into polar representation.

    float time = float(u_Time);
    vec3 sunDir = normalize(rotateX(vec3(0.0, 0.1, 1.0), time * 0.005));  // sun direction depends on how much time has elapsed since start of program.
    float angle = acos(clamp(-0.99999, 0.99999, dot(rayDir, sunDir)));    // grab angle between our cast ray and the suns direction
    float angleSun = angle * 180.0 / PI;                                  // convert our angle from the sun into radians.
    float angleMoon = (PI-angle) * 180.0 / PI;                            // angle from the moon is PI different from sun angle.

    vec3 skyCol;
    vec2 uvSun = sphereToUV(sunDir);                                      // convert our sun direction to polar representation.
    vec3 day = vec3(113.0, 190.0, 245.0) / 255.0;                         // day sky color (light blue)
    vec3 night = vec3(20.0, 24.0, 82.0) / 255.0;                          // night sky color (dark blue)

    if (angleMoon < moonSize) {                                           // Moon has crisp borders.
        skyCol = moonColor;
    } else if (uvSun.y < 0.4 || sunDir.y < -0.9) {                        // Night Mode
        skyCol = night;
    } else if (uvSun.y > 0.6) {                                           // Day Mode
        skyCol = day;
    } else {                                                              // Otherwise its a sunset or sunrise.
        float t = abs(uvSun.x - uv.x);                                    // t represents our cast rays x direction from the suns.
        if (t > 0.5) {                                                    // t ranges from 0 to 1 but we need to cap it at 0.5
            t = 1.0 - t;                                                  // so that is t is continuous and there is no vertical line the sky
        }                                                                 // that partitions the color.
        t = smoothstep(0.0, 1.0, t / 0.5);                                // Plug and chug that shit into smoothstep to make it even smooother.
        float offset = 0.0;
#ifdef COMP_HEAVY
        offset = worleyFBM(rayDir);                                       // Add some cloud like noise if we're in COMP_HEAVY mode.
        offset *= 2.0;
        offset -= 1.0;
        offset *= 0.1;
#endif
        skyCol = mix(uvToSunset(uv.y + offset), uvToDusk(uv.y + offset), t); // Interpolate between sunrise and sunset based on t.
        if (uvSun.y < 0.5) {                                                 // Interpolate again if we are some phase between night and complete sunrise.
            float t2 = smoothstep(0.0, 1.0, (uvSun.y - 0.4) / 0.1);          // so we again mix it real good based on the angle from the suns center.
            skyCol = mix(night, skyCol, t2);                                 // We add the sun at the end so we don't have to recalculate sky color.
        } else {                                                             // Interpolate again if we are in some phase between day and complete sunset.
            float t2 = smoothstep(0.0, 1.0, (uvSun.y - 0.5) / 0.1);
            skyCol = mix(skyCol, day, t2);
        }
    }

    if (angleSun < sunSize) {                                                // Add the sun. We want the sun to blend with the background
        if (angleSun < sunSize / 2.0) {                                      // so we again mix it real good based on the angle from the suns center.
            skyCol = sunColor;                                               // We add the sun at the end so we don't have to recalculate sky color.
        } else {
            float t = smoothstep(0.0, 1.0, 2.0 * angleSun / sunSize - 1.0);
            skyCol = mix(sunColor, skyCol, t);
        }
    }


#ifdef COMP_LITE
    out_Col = vec4(skyCol, 1.0);                                          // If COMP_LITE, just directly spit out the skycolor.
#endif

#ifdef COMP_HEAVY
    out_Col = vec4(skyCol, 1.0);                                          // Same thing, COMP_HEAVY noise is handled above.
#endif

#ifdef COMP_GRAINY
    vec3 offset = random3(rayDir);                                        // COMP_GRAINY we disturb the output color a tiny bit with random noise.
    offset *= 2.0;
    offset -= vec3(1.0);
    out_Col = vec4(skyCol + offset*0.1, 1);
#endif
}
