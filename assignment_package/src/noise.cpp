#include "noise.h"
#include <math.h>
#include <iostream>

class Noise;


float rand1(glm::vec2 n) {
        return glm::fract(glm::sin(glm::dot(n, glm::vec2(12.9898, 4.1414))) * 43758.5453);
}

float Noise::rand2(glm::vec2 n) {
        return glm::fract(glm::sin(glm::dot(n, glm::vec2(7.3914, 19.43923))) * 89235.2389);
}

float Noise::rand3(glm::vec2 n) {
        return glm::fract(glm::sin(glm::dot(n, glm::vec2(24.2309, 12.23904))) * 120843.293);
}

float noise(glm::vec2 n) {
        const glm::vec2 d = glm::vec2(0.0, 1.0);
      glm::vec2 b = glm::floor(n), f = glm:: smoothstep(glm::vec2(0.0), glm::vec2(1.0), glm::fract(n));
            return glm::mix(glm::mix(rand1(b), rand1(b + glm::vec2(d.y, d.x)), f.x), glm::mix(rand1(b + glm::vec2(d.x, d.y)), rand1(b + glm::vec2(d.y, d.y)), f.x), f.y);
}

float interpNoise1D(float x) {
    float intX = floor(x);
    float fractX = glm::fract(x);

    float v1 = noise(glm::vec2(intX));
    float v2 = noise(glm::vec2(intX + 1.f));
    return glm::mix(v1, v2, fractX);
}


float Noise::fbm(float x) {
    float total = 0;
    // default val 0.5f
    float persistence = 0.5f;
    int octaves = 7;

    for(int i = 1; i <= octaves; i++) {
        // change amplitude+ frequency to default values
        float freq = pow(2.f, i);
        float amp = pow(persistence, i);

        total += interpNoise1D(x * freq) * amp;
    }
    return total;
}

glm::vec2 sin2(glm::vec2 in) {
    return glm::vec2(glm::sin(in.x), glm::sin(in.y));
}


glm::vec2 fract2(glm::vec2 in) {
    return glm::vec2(glm::fract(in.x), glm::fract(in.y));
}



glm::vec2 random2(glm::vec2 c){
    float xCoord = glm::fract(glm::sin(glm::dot(glm::vec2(c.x, c.y), glm::vec2(12.9898,78.233))) * 43758.5453);
    float yCoord = glm::fract(glm::cos(glm::dot(glm::vec2(c.y, c.x), glm::vec2(54.216146, 24.3146))) * 16461.3513);
    return glm::vec2(xCoord, yCoord);
}

glm::vec2 random2(glm::vec2 p, float noise) {
    float dot1 = glm::dot(p, glm::vec2(127.1, 311.7));
    float dot2 = glm::dot(p, glm::vec2(269.5,183.3));
    glm::vec2 firstSin = sin2(glm::vec2(dot1, dot2));
    glm::vec2 firstFrac = glm::vec2(fract2(firstSin).x * 43758.5453f, fract2(firstSin).y * 43758.5453f);
    glm::vec2 secondFrac = glm::vec2(fract2(firstFrac).x + noise, fract2(firstFrac).y * noise);
    return secondFrac;
//    return glm::fract(fract2(
//                          sin2(glm::vec2(glm::dot(p, glm::vec2(127.1, 311.7)),
//                 glm::dot(p, glm::vec2(269.5,183.3))))
//                 * 43758.5453) + noise);
}

glm::vec2 pow2(glm::vec2 first, float powerOf) {
    return glm::vec2(pow(first.x, powerOf), pow(first.y, powerOf));
}

float surflet(glm::vec2 p, glm::vec2 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec2 t2 = glm::abs(p - gridPoint);
        glm::vec2 t = glm::vec2(1.f) - (6.f * pow2(t2, 5.f)) + (15.f * pow2(t2, 4.f)) - (10.f * pow2(t2, 3.f));

//        std::cout << t.x << " " << t.y << std::endl;

//    vec2 t = vec2(1.f) - (6.f * pow2(t2, 5.f)) + (15.f * pow2(t2, 4.f)) - (10.f * pow2(t2, 3.f));
    // Get the random vector for the grid point (assume we wrote a function random2)
    glm::vec2 gradient = random2(gridPoint);
    // Get the vector from the grid point to P
    glm::vec2 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
//    std::cout << "Height " << height << std::endl;

    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y;
}

float Noise::perlinNoise(glm::vec2 uv) {
    // this matters a lot in terms of scaling
    uv *= 3.f;
    uv /= 16.f;
    float surfletSum = 0.f;
        // Iterate over the four integer corners surrounding uv
        for (int dx = 0; dx <= 1; ++dx) {
                for(int dy = 0; dy <= 1; ++dy) {
//                    vec2 intermed = floor(uv) + vec2(dx, dy);
//                    std::cout << surflet(uv, glm::floor(uv) + glm::vec2(dx, dy)) << std::endl;
                    surfletSum += surflet(uv, glm::floor(uv) + glm::vec2(dx, dy));
                }
        }
        return surfletSum;
}


float Noise::fbm2(float x) {
    int NUM_OCTAVES = 3;
        float v = 0.0;
        float a = 0.4;
        float shift = float(100);
        for (int i = 0; i < NUM_OCTAVES; ++i) {
                v += a * noise(glm::vec2(x));
                x = x * 2.0 + shift;
                a *= 0.5;
        }
        return v;
}

float Noise::fbm3(glm::vec2 perlin) {
    float total = 0;
    // default val 0.5f
    float persistence = 0.54f;
    int octaves = 3;

    for (int i = 1; i <= octaves; i++) {
        // change amplitude+ frequency to default values
        float freq = pow(2.f, i - 0.5);
        float amp = pow(persistence, i);
        perlin = perlin * freq;
        total += amp * (1 - perlinNoise(perlin));
    }
    return total;
}


float Noise::WorleyNoise0(glm::vec2 uv) {
    uv *= 15.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            minDist = glm::min(minDist, dist);
        }
    }
    return minDist;
}

float Noise::WorleyNoise1(glm::vec2 uv) {
    uv *= 1.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    float secondMin = 1.0;
    glm::vec2 closestPoint;

    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            if (dist < minDist) {
                secondMin = minDist;
                minDist = dist;
                closestPoint = neighbor + point;
            } else if (dist < secondMin) {
                secondMin = dist;
            }
            // minDist = min(minDist, dist);
        }
    }
    float height = -1 * minDist + 1 * secondMin;
    float scalar = 1;
    return height * scalar;
}



float Noise::fbm4(glm::vec2 perlin) {
    float total = 0;
    // default val 0.5f
    float persistence = 0.25f;
    int octaves = 3;

    for (int i = 1; i <= octaves; i++) {
        // change amplitude+ frequency to default values
        float freq = pow(2.f, i);
        float amp = pow(persistence, i);

        total += amp * (1 - perlinNoise(perlin * freq));
        // total += interpNoise1D(x * freq) * amp;
    }
    return total;
}

float Noise::worleyNoise(glm::vec2 uv) {
    uv /= 16.0;
    uv *= 15.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    float secondMin = 1.0;
    glm::vec2 closestPoint;

    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);

            if (dist < minDist) {
                secondMin = minDist;
                minDist = dist;
                closestPoint = neighbor + point;
            } else if (dist < secondMin) {
                secondMin = dist;
            }

            // minDist = glm::min(minDist, dist);
        }
    }
    float height = -1 * minDist + 1 * secondMin;
    return height;
    // return minDist;
}

float Noise::blendedNoise(float grass, float mountain) {
    return (grass + mountain) / 2;
}

float surfletAlt(glm::vec2 p, glm::vec2 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec2 t2 = glm::abs(p - gridPoint);
        glm::vec2 t = glm::vec2(1.f) - (6.f * pow2(t2, 5.f)) + (15.f * pow2(t2, 4.f)) - (10.f * pow2(t2, 3.f));

//        std::cout << t.x << " " << t.y << std::endl;

//    vec2 t = vec2(1.f) - (6.f * pow2(t2, 5.f)) + (15.f * pow2(t2, 4.f)) - (10.f * pow2(t2, 3.f));
    // Get the random vector for the grid point (assume we wrote a function random2)
    glm::vec2 gradient = random2(gridPoint) * 2.f - glm::vec2(1.f, 1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
//    std::cout << "Height " << height << std::endl;

    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y;
}

float Noise::perlinNoiseAlt(glm::vec2 uv) {
    // this matters a lot in terms of scaling
    uv *= 3.f;
    uv /= 16.f;
    float surfletSum = 0.f;
        // Iterate over the four integer corners surrounding uv
        for (int dx = 0; dx <= 1; ++dx) {
                for(int dy = 0; dy <= 1; ++dy) {
//                    vec2 intermed = floor(uv) + vec2(dx, dy);
//                    std::cout << surflet(uv, glm::floor(uv) + glm::vec2(dx, dy)) << std::endl;
                    surfletSum += surfletAlt(uv, glm::floor(uv) + glm::vec2(dx, dy));
                }
        }
        return surfletSum;
}

float Noise::fbm3Alt(glm::vec2 perlin) {
    float total = 0;
    // default val 0.5f
    float persistence = 0.49f;
    int octaves = 3;

    for (int i = 1; i <= octaves; i++) {
        // change amplitude+ frequency to default values
        float freq = pow(2.f, i - 0.5);
        float amp = pow(persistence, i);
        perlin = perlin * freq;
        total += amp * (1 - perlinNoiseAlt(perlin));
    }
    return total;
}

float Noise::mountainSparse(glm::vec2 in) {

    float test = glm::abs(Noise::fbm3Alt(in * (1.f / 64.f)));
    test = test - 0.8f;
    test = Noise::fbm2(test);

    test *= 10.f;

    glm::vec2 scaledIn = in * (1.f / 48.f);
    float worleyHeight = Noise::worleyNoise(scaledIn);
    float worleyHeightMod = 1 - glm::sin(worleyHeight);
    worleyHeightMod *= 15;

    test *= 30.f;


    test += worleyHeightMod;
    glm::clamp(0.f, 255.f, test);
    test += 110;
    return test;

}

/*
 * Returns a height between 10 and 80
 *
 */

float Noise::mountain(glm::vec2 in) {
    glm::vec2 scaledIn = in * (1.f / 58.f);
    float mHeight = glm::abs(Noise::fbm3(scaledIn));
    mHeight = mHeight - 0.9f;
    mHeight *= 9;
    mHeight *= 20;
    return glm::clamp(0.f, 80.f, mHeight);


    // std::cout << "Mount" << w4 << std::endl;
}

/*
 * Returns a height between 0 and 30
 *
 */

float Noise::grassLands(glm::vec2 in) {
    glm::vec2 scaledIn = in * (1.f / 30.f);
    float worleyHeight = Noise::worleyNoise(scaledIn);
    float worleyHeightMod = glm::sin(worleyHeight);
    worleyHeightMod = 1 - worleyHeightMod;
    float fbm = Noise::WorleyNoise0(glm::vec2(Noise::fbm4(scaledIn)));

    worleyHeightMod *= 20;
    fbm *= 20;
    fbm = glm::mix(fbm, worleyHeightMod, 0.5);
    return glm::clamp(0.f, 255.f, fbm);
}

float Noise::grassLands2(glm::vec2 in) {
    in = glm::abs(in);
    glm::vec2 scaledIn = in * (1.f / 40.f);
    float worleyHeight = Noise::WorleyNoise1(scaledIn);
    float worleyHeightMod = glm::sin(worleyHeight);
    worleyHeightMod = worleyHeightMod;
    float fbm = Noise::WorleyNoise0(glm::vec2(Noise::fbm4(scaledIn)));
//    return worleyHeight *= 20;
    worleyHeightMod *= 20;
    fbm *= 16;
    fbm = glm::mix(fbm, worleyHeightMod, 0.5);
    fbm += 128;
    fbm += 5;
    return glm::clamp(0.f, 255.f, fbm);
}

/*
 * Returns a height between 0 and 80
 *
 */
float Noise::blended(glm::vec2 in) {
    return glm::mix(Noise::grassLands2(in), Noise::mountainSparse(in), 0.5f);
}

float Noise::fbm5(glm::vec2 perlin) {
    float total = 0;
    // default val 0.5f
    float persistence = 0.25f;
    int octaves = 8;
    float amp = 0.75f;

    for (int i = 1; i <= octaves; i++) {
        // change amplitude+ frequency to default values
        float freq = pow(1.5f, i);


        total += amp * (1 - perlinNoise5(perlin * freq));
        // total += interpNoise1D(x * freq) * amp;
        amp *= persistence;
    }
    return total;
}

float Noise::surflet5(glm::vec2 p, glm::vec2 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec2 t2 = glm::abs(p - gridPoint);
        glm::vec2 t = glm::vec2(1.f) - (6.f * pow2(t2, 5.f)) + (15.f * pow2(t2, 4.f)) - (10.f * pow2(t2, 3.f));

//        std::cout << t.x << " " << t.y << std::endl;

//    vec2 t = vec2(1.f) - (6.f * pow2(t2, 5.f)) + (15.f * pow2(t2, 4.f)) - (10.f * pow2(t2, 3.f));
    // Get the random vector for the grid point (assume we wrote a function random2)
    glm::vec2 gradient = random2(gridPoint) * 2.f - glm::vec2(1.f, 1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
//    std::cout << "Height " << height << std::endl;

    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y;
}

float Noise::perlinNoise5(glm::vec2 uv) {
    // this matters a lot in terms of scaling
//    uv *= 3.f;
    uv /= 33.f;
    float surfletSum = 0.f;
        // Iterate over the four integer corners surrounding uv
        for (int dx = 0; dx <= 1; ++dx) {
                for(int dy = 0; dy <= 1; ++dy) {
//                    vec2 intermed = floor(uv) + vec2(dx, dy);
//                    std::cout << surflet(uv, glm::floor(uv) + glm::vec2(dx, dy)) << std::endl;
                    surfletSum += surflet5(uv, glm::floor(uv) + glm::vec2(dx, dy));
                }
        }
        return surfletSum;
}

glm::vec2 Noise::worleyClouds(glm::vec2 uv) {
    uv /= 10.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    glm::vec2 minPoint = glm::vec2();
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            if (dist < minDist) {
                minDist = dist;
                minPoint = uvInt + neighbor + point;
            }
        }
    }
    return minPoint;
}

