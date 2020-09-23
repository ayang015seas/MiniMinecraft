#version 150
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform vec4 u_Color; // The color with which to render this instance of geometry.
uniform sampler2D textureSampler;
uniform int u_Time;

// EXTRA SHIT FOR GOOD FOG
uniform ivec2 u_Dimensions;
uniform mat4 u_ViewProj;
uniform vec3 u_Eye;

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;
in vec4 fs_UV;
in float visibility;
in mat4 viewProjInverse;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5;
    float freq = 4.0;
    float sum = 0.0;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}

vec3 rotateX(vec3 p, float a) {
    float s = sin(-a);
    float c = cos(-a);
    return vec3(p.x, c * p.y - s * p.z, s * p.y + c * p.z);
}

#define GOD_TIER_FOG

#ifdef GOD_TIER_FOG
const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

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

vec2 sphereToUV(vec3 p) {
    float phi = atan(p.z, p.x);
    if(phi < 0) {
        phi += TWO_PI;
    }
    float theta = acos(clamp(-0.999999, 0.999999, p.y));
    return vec2(1.0 - phi / TWO_PI, 1.0 - theta / PI);
}

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
#endif

void main()
{

    float time = float(u_Time);
    vec3 sunDir = normalize(rotateX(vec3(0.0, 0.1, 1.0), time * 0.005));
    vec3 skyCol = vec3(1.0, 1.0, 1.0);

    // Material base color (before shading)
    vec2 uv = fs_UV.xy;
    vec4 col;

    if (fs_UV.w > 0) {
        float scaleVar = mod((u_Time * 0.0007), 1.f / 16.f);
        uv.x = (uv.x + scaleVar);
        float scaleY = abs(mod(sin(u_Time * 0.01), 1.f / 16.f));
        uv.y = (uv.y + scaleVar);
        col = texture(textureSampler, uv);
    } else {
        col = texture(textureSampler, fs_UV.xy);
    }

#ifdef GOD_TIER_FOG
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0;
    vec4 p = vec4(ndc.xy, 1.0, 1.0);
    p *= 1000.0;
    p = viewProjInverse * p;

    vec3 rayDir = normalize(p.xyz - u_Eye);
    vec2 uv2 = sphereToUV(rayDir);

    vec2 uvSun = sphereToUV(sunDir);
    vec3 day = vec3(113.0, 190.0, 245.0) / 255.0;
    vec3 night = vec3(20.0, 24.0, 82.0) / 255.0;

    if (uvSun.y < 0.4 || sunDir.y < -0.9) {
        skyCol = night;
    } else if (uvSun.y > 0.6) {
        skyCol = day;
    } else {
        float t = abs(uvSun.x - uv2.x);
        if (t > 0.5) {
            t = 1.0 - t;
        }
        t = smoothstep(0.0, 1.0, t / 0.5);
        skyCol = mix(uvToSunset(uv2.y), uvToDusk(uv2.y), t);
        if (uvSun.y < 0.5) {
            float t2 = smoothstep(0.0, 1.0, (uvSun.y - 0.4) / 0.1);
            skyCol = mix(night, skyCol, t2);
        } else {
            float t2 = smoothstep(0.0, 1.0, (uvSun.y - 0.5) / 0.1);
            skyCol = mix(skyCol, day, t2);
        }
    }
#endif

        vec4 diffuseColor = col;
        //diffuseColor = diffuseColor * (0.5 * fbm(fs_Pos.xyz) + 0.5);

        // Calculate the diffuse term for Lambert shading

        float diffuseTerm = 0;
        if (sunDir.y > 0.1) {
            diffuseTerm = dot(normalize(fs_Nor), normalize(vec4(sunDir, 1.0)));
        } else if (sunDir.y < -0.1) {
            diffuseTerm = dot(normalize(fs_Nor), normalize(vec4(-1.0*sunDir, 1.0)));
            diffuseTerm *= 0.1;
        } else {
            float diffuseTermSun = dot(normalize(fs_Nor), normalize(vec4(sunDir, 1.0)));
            float diffuseTermMoon = 0.1* dot(normalize(fs_Nor), normalize(vec4(-1.0*sunDir, 1.0)));
            float t = smoothstep(0.0, 1.0, (sunDir.y + 0.1) / 0.2);
            diffuseTerm = mix(diffuseTermMoon, diffuseTermSun, t);
        }

        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.2;

        float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.
        if (fs_Col.w < -0.3) {
            out_Col = vec4(1, 0.5, 0, 1);
        } else if (fs_Col.x == 0.5) {
            if (fs_Col.z > 0.66) {
                out_Col = vec4(0, 0, 0, 1);
            } else if (fs_Col.z > 0.33) {
                out_Col = vec4(0, 0, 1, 1);
            } else {
                out_Col = vec4(1, 0, 0, 1);
            }
        } else {
            vec4 lightCol = vec4(vec3(255.0, 255.0, 255.0) / 255.0, 1.0);

            vec4 out_ColTemp = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
            out_ColTemp *= lightCol;
            out_Col = mix(vec4(skyCol, 1.0), out_ColTemp, visibility);
//            out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
        }
        // Compute final shaded color
}
