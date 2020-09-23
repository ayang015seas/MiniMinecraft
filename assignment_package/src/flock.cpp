#include "flock.h"
#include "boid.h"
#include <glm/gtx/vector_angle.hpp>

Flock::Flock(OpenGLContext *mp_context, Terrain* world):
    Drawable(mp_context), terrainPtr(world)
{
}

void Flock::run() {
    glm::vec4 avgPosition;
    for (unsigned int i = 0; i < bois.size(); i++) {
      bois[i]->run(bois, this->terrainPtr);
      avgPosition += bois[i]->pos;
    }
    // calculate average position
    avgPosition /= bois.size();
    this->averagePos = avgPosition;
}

// add boid with a target height
void Flock::addBoidWithHeight(float x, float y, Terrain* t) {
    this->boidList.push_back(mkU<Boid>(x, y, t));
    this->bois.push_back(boidList[index].get());
    index++;
}


void Flock::setup(float x, float y) {
    float flockSizeVariance = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 4.f;

    for (unsigned int i = 0 ; i < 8 + flockSizeVariance; i++) {
        float xPos = x + float(rand() % 10);
        float yPos = y + float(rand() % 10);
        this->addBoidWithHeight(xPos, yPos, this->terrainPtr);
    }
}


void Flock::create() {
    glm::vec4 norm = glm::vec4(0, 1, 0, 1);
    glm::vec4 col = glm::vec4(0.5f, 1.f, 1.f, 1.f);

    glm::vec4 UV = glm::vec4();

    glm::vec4 p1;
    glm::vec4 p2;
    glm::vec4 p3;
    glm::vec4 p4;

    // location positions of bird vertexes
    glm::vec4 pos1Local;
    glm::vec4 pos2Local;
    glm::vec4 pos3Local;
    glm::vec4 pos4Local;
    glm::vec4 pos5Local;
    glm::vec4 pos6Local;
    glm::vec4 pos7Local;
    glm::vec4 pos8Local;
    glm::vec4 pos9Local;

    std::vector<glm::vec4> posNorCol = {};
    std::vector<GLuint> idx {};
    int i = 0;
    glm::vec4 beakColor = glm::vec4(0.5f, 1.f, 1.f, -1.0f);



    for (unsigned int j = 0; j < bois.size(); j++) {
        glm::vec4 colWithFlapIndicator = glm::vec4(0.5, 5.5, bois[j]->flapPattern, 1);

        glm::vec4 position = bois[j]->pos;
        glm::vec4 velocity = bois[j]->vel;

        glm::vec2 velLocal = glm::normalize(glm::vec2(velocity.x, velocity.y));
        glm::vec2 offSetLocal = glm::normalize(glm::vec2(-1.0, 0.15));
        float angle = glm::angle(velLocal, offSetLocal);

        // make sure rotation is 360 degrees
        if (velocity.y < 0 && velocity.x < 0) {
            angle += ((glm::radians(180.f) - angle) * 2);
        } else if (velocity.y < 0 && velocity.x > 0) {
            angle = glm::radians(360.f) - angle;
        }

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(), 1.5008f + angle, glm::vec3(0.f , 1.0f, 0.0f));
        glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(), 1.5008f + angle - 0.3f, glm::vec3(0.f , 1.0f, 0.0f));

        pos1Local = glm::vec4(0.0f, 0, 1.f, 0.f);
        pos2Local = glm::vec4(-0.35f, 0, -1.f, 0.f);
        pos3Local = glm::vec4(0.35f, 0, 1.f, 0.f);

        // centerline vertexes
        pos4Local = glm::vec4(0.0f, 0, 0.3f, 0.f);
        pos5Local = glm::vec4(0.0f, 0, -0.3f, 0.f);

        // left wing centerline
        pos6Local = glm::vec4(-1.5f, 0, 0.5f, 0.f);
        pos7Local = glm::vec4(-1.5f, 0, 0.0f, 0.f);

        // right wing centerline
        pos8Local = glm::vec4(1.5f, 0, 0.5f, 0.f);
        pos9Local = glm::vec4(1.5f, 0, 0.0f, 0.f);

        // previous and current velocities
        // mixing between 2 velocities

        pos1Local = rotationMatrix2 * pos1Local;
        pos2Local = rotationMatrix2 * pos2Local;
        pos3Local = rotationMatrix2 * pos3Local;
        pos4Local = rotationMatrix2 * pos4Local;

        float altVariation = glm::sin(this->worldTime / (bois[j]->period * 10)) * 8;
        float idealAltitude = bois[j]->targetAlt + 20 + altVariation;

        // adjust altitude
        if (bois[j]->zPos < idealAltitude) {
            bois[j]->zPos += 0.025f;
        } else if (bois[j]->zPos < idealAltitude) {
            bois[j]->zPos -= 0.025f;
        }

        float alt = bois[j]->zPos + altVariation;

        p1 = glm::vec4(position.x, alt, position.y, 1.f) + pos1Local;
        p2 = glm::vec4(position.x, alt, position.y, 1.f) + pos2Local;
        p3 = glm::vec4(position.x, alt, position.y, 1.f) + pos3Local;
        p4 = glm::vec4(position.x, alt, position.y, 1.f) + pos4Local;

        glm::vec4 v[12] = {p1, norm, col, UV + glm::vec4(0, 0, 0, 0),
                           p2, norm, beakColor, UV + glm::vec4(0, 1, 0, 0),
                           p3, norm, col, UV + glm::vec4(1, 0, 0, 0),
                           };

        posNorCol.insert(posNorCol.end(), std::begin(v), std::end(v));

        int indices[3] = {i, i + 1, i + 2};
        idx.insert(idx.end(), std::begin(indices), std::end(indices));

        i += 3;

        // left triangle
        pos5Local = rotationMatrix * pos5Local;
        pos6Local = rotationMatrix * pos6Local;
        pos7Local = rotationMatrix * pos7Local;

        p1 = glm::vec4(position.x, alt, position.y, 1.f) + pos4Local;
        p2 = glm::vec4(position.x, alt, position.y, 1.f) + pos5Local;
        p3 = glm::vec4(position.x, alt, position.y, 1.f) + pos6Local;
        p4 = glm::vec4(position.x, alt, position.y, 1.f) + pos7Local;


        glm::vec4 vLeft[12] = {p1, norm, col, UV,
                           p2, norm, col, UV,
                           p3, norm, colWithFlapIndicator, UV
                           };

        posNorCol.insert(posNorCol.end(), std::begin(vLeft), std::end(vLeft));

        int indicesLeft[3] = {i, i + 1, i + 2};
        idx.insert(idx.end(), std::begin(indicesLeft), std::end(indicesLeft));

        i += 3;

        // right triangle

        pos8Local = rotationMatrix * pos8Local;
        pos9Local = rotationMatrix * pos9Local;

        p1 = glm::vec4(position.x, alt, position.y, 1.f) + pos4Local;
        p2 = glm::vec4(position.x, alt, position.y, 1.f) + pos5Local;
        p3 = glm::vec4(position.x, alt, position.y, 1.f) + pos8Local;
        p4 = glm::vec4(position.x, alt, position.y, 1.f) + pos9Local;

        glm::vec4 vRight[12] = {p1, norm, col, UV,
                           p2, norm, col, UV,
                           p3, norm, colWithFlapIndicator, UV
                          };

        posNorCol.insert(posNorCol.end(), std::begin(vRight), std::end(vRight));

        int indicesRight[3] = {i, i + 1, i + 2};
        idx.insert(idx.end(), std::begin(indicesRight), std::end(indicesRight));

        i += 3;
    }

    f_count = idx.size();

    generateFIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, f_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generateFPosNorCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, f_bufPosNorCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, posNorCol.size() * sizeof(glm::vec4), posNorCol.data(), GL_STATIC_DRAW);

}
