#include "boid.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */

Boid::Boid(float x, float y, Terrain* t) {
    this->accel = glm::vec4();
    float random1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float random2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float random3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    this->period = glm::clamp(random3 * 100, 50.f, 100.f);
    float random4 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    this->flapPattern =  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    this->vel = glm::vec4(random1 * 2.f - 1.f, random2 * 2.f - 1.f, 0.f, 0.f);
    this->pos = glm::vec4(x, y, 0.f, 0.f);
    this->r = 3.0f;
    this->maxSpeed = 8.f;    // Maximum speed
    this->maxForce = 0.05f;

    if (t->hasChunkAt(glm::floor(this->pos.x), glm::floor(this->pos.y))) {
        this->currentChunk = t->getChunkAt(glm::floor(x), glm::floor(y)).get();
        this->targetAlt = currentChunk->maxHeight;
    } else {
        this->targetAlt = 150;
    }
    this->zPos = glm::max(targetAlt + 20 + (random4 * 6), 140.f);
}

// assumes that mass is one
void Boid::applyForce(glm::vec4 force) {
    this->accel += force;
}

void Boid::run(std::vector<Boid*> allBoids, Terrain* tPtr) {
    this->flock(allBoids);
    this->update(tPtr);
};

void Boid::flock(std::vector<Boid*> allBoids) {
    glm::vec4 sep = this->separate(allBoids);
    glm::vec4 ali = this->align(allBoids);
    glm::vec4 coh = this->cohesion(allBoids);

    float target = 0.01f;

    // weight each factor
    sep *= 1.5f;
    ali *= 0.7f;
    coh *= 0.05f;

    sep *= target;
    ali *= target;
    coh *= target;

    this->applyForce(sep);
    this->applyForce(ali);
    this->applyForce(coh);
};

void Boid::update(Terrain* currentTerrain) {
    int xTest = (int) this->pos.x;
    int yTest = (int) this->pos.y;

    if (currentTerrain->hasBigChungusAt(xTest, yTest)) {
        Chunk* currentArea = currentTerrain->getChunkAt(xTest, yTest).get();
        if (currentArea != this->currentChunk) {
            currentChunk = currentArea;
            targetAlt = currentChunk->maxHeight;
        }
    } else {
        targetAlt = 160.f;
    }


    this->vel += this->accel;
    this->vel = glm::clamp(this->vel, -1.f * glm::vec4(this->maxSpeed), glm::vec4(this->maxSpeed));

    this->pos += (this->vel * 0.12f);
    this->accel *= 0.f;
};

glm::vec4 Boid::separate(std::vector<Boid*> allBoids) {
    float desiredSep = 5.f;
    glm::vec4 steer = glm::vec4();
    unsigned int count = 0;

    for (unsigned int i = 0; i < allBoids.size(); i++) {
        float dist = glm::distance(this->pos, allBoids[i]->pos);

        if (dist > 0 && dist < desiredSep) {
            glm::vec4 toNeighbor = this->pos - allBoids[i]->pos;
            toNeighbor = glm::normalize(toNeighbor);
            steer += toNeighbor;
            count++;
        }
    }

    if (count > 0) {
        steer /= count;
    }

    if (glm::length(steer) > 0) {
        steer = glm::normalize(steer);
        steer *= this->maxSpeed;
        steer -= this->vel;
        steer = glm::clamp(steer, glm::vec4(-maxForce), glm::vec4(maxForce));
    }
    return steer;
};

glm::vec4 Boid::align(std::vector<Boid*> allBoids) {
    float neighborDist = 10.f;
    glm::vec4 sum = glm::vec4();
    unsigned int count = 0;
    for (unsigned int i = 0; i < allBoids.size(); i++) {
        float dist = glm::distance(this->pos, allBoids[i]->pos);
        if (dist > 0 && dist < neighborDist) {
            sum += allBoids[i]->vel;
            count++;
        }
    }

    if (count > 0) {
        sum /= count;
        sum = glm::normalize(sum);
        sum *= this->maxSpeed;
        glm::vec4 steer = sum - this->vel;
        steer = glm::clamp(steer, glm::vec4(-maxForce), glm::vec4(maxForce));
        return steer;
    } else {
        return glm::vec4();
    }
};

glm::vec4 Boid::cohesion(std::vector<Boid*> allBoids) {
    float neighborDist = 10.f;
    glm::vec4 sum = glm::vec4();
    unsigned int count = 0;
    for (unsigned int i = 0; i < allBoids.size(); i++) {
        float dist = glm::distance(this->pos, allBoids[i]->pos);
        if (dist > 0 && dist < neighborDist) {
            sum += allBoids[i]->pos;
            count++;
        }
    }

    if (count > 0) {
        sum /= count;
        return seek(sum);
    } else {
        return glm::vec4();
    }
};

// guide a vector toward a target
glm::vec4 Boid::seek(glm::vec4 target) {
    glm::vec4 ideal = target - this->pos;
    ideal = glm::normalize(ideal);
    ideal *= this->maxSpeed;
    glm::vec4 steer = ideal - this->vel;
    return steer;
};
