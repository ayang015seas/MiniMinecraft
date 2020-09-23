#pragma once

#ifndef FLOCK_H
#define FLOCK_H


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
#include "boid.h"
#include "scene/terrain.h"

class Flock: public Drawable {
    public:
        std::vector<uPtr<Boid>> boidList;
        std::vector<Boid*> bois;
        int index = 0;
        glm::vec4 averagePos;
        Terrain* terrainPtr;


        Flock(OpenGLContext *mp_context, Terrain* world);
        void run();
        void setup(float x, float y);
        float worldTime;
        void addBoidWithHeight(float x, float y, Terrain* t);

        glm::vec4 seek(glm::vec4 target);
        virtual void create();
};

#endif // FLOCK_H
