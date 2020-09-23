#ifndef NOISE_H
#define NOISE_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <smartpointerhelp.h>
#include <glm_includes.h>

class Noise {


public:
    static float perlinNoise(glm::vec2 uv);
    static float perlinNoiseAlt(glm::vec2 uv);
    static float fbm3Alt(glm::vec2 perlin);
    static float endPerlin(glm::vec2 uv);

    static float perlinNoise5(glm::vec2 uv);
    static float surflet5(glm::vec2 p, glm::vec2 gridPoint);

    static float worleyNoise(glm::vec2 uv);
    static float blendedNoise(float grass, float mountain);
    static float WorleyNoise0(glm::vec2 uv);
    static float WorleyNoise1(glm::vec2 uv);

    static float fbm(float x);
    static float fbm2(float x);
    static float fbm3(glm::vec2 perlin);
    static float fbm4(glm::vec2 perlin);
    static float fbm5(glm::vec2 perlin);
    static float fbmsine(glm::vec3 sin);

    static glm::vec2 worleyClouds(glm::vec2 uv);
    static float rand2(glm::vec2);
    static float rand3(glm::vec2);

public:
    static float mountainSparse(glm::vec2 in); // returns num between 0 and 80
    static float mountain(glm::vec2 in); // returns num between 0 and 80
    static float grassLands(glm::vec2 in); // returns num between 0 and 30
    static float grassLands2(glm::vec2 in); // returns num between 0 and 3
    static float blended(glm::vec2 in); // returns blended mount + grass noise


};

#endif // NOISE_H
