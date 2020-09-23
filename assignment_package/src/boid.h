#ifndef BOID_H
#define BOID_H

#include "openglcontext.h"
#include "shaderprogram.h"
#include "scene/worldaxes.h"
#include "scene/camera.h"
#include "scene/terrain.h"
#include "scene/player.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <smartpointerhelp.h>
#include <QDateTime>

class Boid {
    public:
        glm::vec4 pos;
        glm::vec4 vel;
        glm::vec4 accel;

        float r;
        float maxForce;
        float maxSpeed;

        float zPos;
        float period;
        float flapPattern;

        Chunk* currentChunk;
        int targetAlt;

        Boid(float x, float y, Terrain* t);

        void applyForce(glm::vec4 force);
        void run(std::vector<Boid*> allBoids, Terrain* tPtr);
        void flock(std::vector<Boid*> allBoids);
        void update(Terrain* currentTerrain);
        glm::vec4 separate(std::vector<Boid*> allBoids);
        glm::vec4 align(std::vector<Boid*> allBoids);
        glm::vec4 cohesion(std::vector<Boid*> allBoids);
        glm::vec4 seek(glm::vec4 target);

};

#endif // BOID_H
