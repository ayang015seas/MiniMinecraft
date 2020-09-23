#include "player.h"
#include <QString>
#include <iostream>
using namespace glm;

Player::Player(glm::vec3 pos, Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
      mcr_camera(m_camera), flightMode(true), jumped(false), preFlight(false),
      preCreate(false), preDelete(false)
{}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(input);
    computePhysics(dT, mcr_terrain);
}

void Player::processInputs(InputBundle &inputs) {
    if (inputs.fPressed) {
        preFlight = true;
    }
    if (!inputs.fPressed && preFlight) {
        preFlight = false;
        flightMode = !flightMode;
        m_velocity = glm::vec3();
    }
    if (inputs.wPressed) {
        m_acceleration[2] = flightMode ? 30.f : 20.f;
    } else if (inputs.sPressed) {
        m_acceleration[2] = flightMode ? -30.f : -20.f;
    } else {
        m_acceleration[2] = 0.f;
    }
    if (inputs.dPressed) {
        m_acceleration[0] = flightMode ? -30.f : -20.f;
    } else if (inputs.aPressed) {
        m_acceleration[0] = flightMode ? 30.f : 20.f;
    } else {
        m_acceleration[0] = 0.f;
    }
    if (inputs.qPressed && flightMode) {
        m_acceleration[1] = 30.f;
    } else if (inputs.ePressed && flightMode) {
        m_acceleration[1] = -30.f;
    } else {
        m_acceleration[1] = 0.f;
    }
    if (inputs.spacePressed && !flightMode) {
        jumped = true;
    }
    if (inputs.rightClick) {
        preCreate = true;
    }
    if (inputs.leftClick) {
        preDelete = true;
    }
    if (!inputs.rightClick && preCreate) {
        preCreate = false;
        createBlock = true;
    }
    if (!inputs.leftClick && preDelete) {
        preDelete = false;
        deleteBlock = true;
    }
    m_camera.setThetaPhi(inputs.mouseX, inputs.mouseY);
}

void Player::computePhysics(float dT, Terrain &terrain) {
    if (deleteBlock) {
        float outDist = 0.f;
        glm::ivec3 blockHit = glm::ivec3();
        int inter = -1;
        if (gridMarch(m_position + glm::vec3(0.f, 1.5f, 0.f), m_camera.getForward() * 3.f,
                      terrain, &outDist, &blockHit, &inter)) {
            terrain.setBlockAt(blockHit[0], blockHit[1], blockHit[2], EMPTY);
            terrain.recreateChunk(blockHit[0], blockHit[2]);
        }
        deleteBlock = false;
    }
    if (createBlock) {
        float outDist = 0.f;
        glm::ivec3 blockHit = glm::ivec3();
        int inter = -1;
        if (gridMarch(m_position + glm::vec3(0.f, 1.5f, 0.f), m_camera.getForward() * 3.f,
                      terrain, &outDist, &blockHit, &inter)) {
            BlockType t = terrain.getBlockAt(blockHit[0], blockHit[1], blockHit[2]);
            if (blockHit[inter] < m_position[inter]) {
                blockHit[inter]++;
                terrain.setBlockAt(blockHit[0], blockHit[1], blockHit[2], t);
                terrain.recreateChunk(blockHit[0], blockHit[2]);
            } else {
                blockHit[inter]--;
                terrain.setBlockAt(blockHit[0], blockHit[1], blockHit[2], t);
                terrain.recreateChunk(blockHit[0], blockHit[2]);
            }
        }
        createBlock = false;
    }

    if (flightMode) {
        m_velocity *= 0.95f;
        m_velocity += m_acceleration * dT * 0.001f;
        glm::vec3 movevec = glm::vec3(glm::rotate(glm::mat4(), glm::radians(m_camera.theta), glm::vec3(0.f, 1.f, 0.f))
                                        * glm::rotate(glm::mat4(), glm::radians(m_camera.phi), glm::vec3(1.f, 0.f, 0.f))
                                        * glm::vec4((m_velocity * dT * 0.001f), 1.f));
        this->moveAlongVector(movevec);
    } else {

        glm::vec3 currPos = glm::floor(m_position);
        if (terrain.getBlockAt(currPos[0], currPos[1] - 1, currPos[2]) == EMPTY
                || m_position[1] - currPos[1] >= 0.01f) {
            m_velocity *= 0.95f;
            m_acceleration[1] = -50.f;
        } else if (terrain.getBlockAt(currPos[0], currPos[1], currPos[2]) == WATER) {
            m_velocity *= 0.67f;
            if (terrain.getBlockAt(currPos[0], currPos[1] - 1, currPos[2]) == WATER) {
                    m_acceleration [1] = -33.3f;
            }
        } else {
            m_velocity *= 0.9f;
        }
        m_velocity += m_acceleration * dT * 0.001f;
        for (int i = 0; i < 3; i++) {
            if (glm::abs(m_velocity[i]) <= 0.01f) {
                m_velocity[i] = 0.f;
            }
        }
        if (jumped && terrain.getBlockAt(currPos[0], currPos[1] - 1, currPos[2]) != EMPTY
                && m_position[1] - currPos[1] < 0.01f
                && terrain.getBlockAt(currPos[0], currPos[1], currPos[2]) != WATER) {
            m_velocity[1] += 13.5f;
        }
        if (jumped && terrain.getBlockAt(currPos[0], currPos[1], currPos[2]) == WATER
                && terrain.getBlockAt(currPos[0], currPos[1] + 0.5f, currPos[2] == WATER) ) {
            m_velocity[1] = 4.f;
        } else if (jumped && terrain.getBlockAt(currPos[0], currPos[1], currPos[2]) == WATER) {
            m_velocity[1] = 0.f;
        }
        jumped = false;
        glm::vec3 movevec = glm::vec3(glm::rotate(glm::mat4(), glm::radians(m_camera.theta), glm::vec3(0.f, 1.f, 0.f))
                                      * glm::vec4((m_velocity * dT * 0.001f), 1.f));
        glm::vec3 minVec = movevec;
        float outDist = 0.f;
        glm::ivec3 blockHit = glm::ivec3();
        int inter = -1;
        for (float i = 0.f; i <= 2.f; i++) {
            for (int j = 0; j <= 2; j++) {
                glm::vec3 raydir = glm::vec3();
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(0.5f, i, 0.5f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(0.5f, i, -0.5f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(-0.5f, i, 0.5f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(-0.5f, i, -0.5f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(0.5f, i, 0.f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(0.f, i, 0.5f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(-0.5f, i, 0.f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
                raydir[j] = minVec[j];
                if (gridMarch(m_position + glm::vec3(0.f, i, -0.5f), raydir, terrain, &outDist, &blockHit, &inter)) {
                    if (outDist > 0.001f) {
                        minVec[j] = glm::sign(minVec[j]) * (glm::min(glm::abs(minVec[j]), outDist) - 0.0001f);
                    } else {
                        minVec[j] = 0.f;
                    }
                }
            }
        }
        movevec = minVec;
        this->moveAlongVector(movevec);

    }

}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}

bool Player::gridMarch(glm::vec3 rayOrigin, int axis, float length,
               const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit) {
    if (length == 0) {
        return false;
    }
    float maxLen = glm::abs(length);
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = 1.f;
        float offset = glm::max(0.f, glm::sign(length)); // See slide 5
        // If the player is *exactly* on an interface then
        // they'll never move if they're looking in a negative direction
        if(currCell[axis] == rayOrigin[axis] && offset == 0.f) {
            offset = -1.f;
        }
        int nextIntercept = currCell[axis] + offset;
        float axis_t = (nextIntercept - rayOrigin[axis]) / length;
        axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
        if(axis_t < min_t) {
            min_t = axis_t;
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin[axis] += min_t;
        glm::ivec3 offsetV = glm::ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offsetV[axis] = glm::min(0.f, glm::sign(length));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offsetV;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        if(cellType != EMPTY && cellType != WATER) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

bool Player::gridMarch(vec3 rayOrigin, vec3 rayDirection, const Terrain &terrain, float *out_dist, ivec3 *out_blockHit,
               int *interfaceAxis) {
    float maxLen = length(rayDirection); // Farthest we search
    ivec3 currCell = ivec3(floor(rayOrigin));
    rayDirection = normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        *interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    *interfaceAxis = i;
                }
            }
        }
        if(*interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        ivec3 offset = ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[*interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[*interfaceAxis]));
        currCell = ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        if (terrain.hasChunkAt(currCell.x, currCell.z)) {
            BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
            if(cellType != EMPTY && cellType != WATER) {
                *out_blockHit = currCell;
                *out_dist = glm::min(maxLen, curr_t);
                return true;
            }
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

//bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
//               const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit, int *interfaceAxis) {
//    float maxLen = glm::length(rayDirection); // Farthest we search
//    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
//    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

//    float curr_t = 0.f;
//    while(curr_t < maxLen) {
//        float min_t = glm::sqrt(3.f);
//        *interfaceAxis = -1; // Track axis for which t is smallest
//        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
//            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
//                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
//                // If the player is *exactly* on an interface then
//                // they'll never move if they're looking in a negative direction
//                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
//                    offset = -1.f;
//                }
//                int nextIntercept = currCell[i] + offset;
//                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
//                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
//                if(axis_t < min_t) {
//                    min_t = axis_t;
//                    *interfaceAxis = i;
//                }
//            }
//        }
//        if(*interfaceAxis == -1) {
//            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
//        }
//        curr_t += min_t; // min_t is declared in slide 7 algorithm
//        rayOrigin += rayDirection * min_t;
//        glm::ivec3 offset = glm::ivec3(0,0,0);
//        // Sets it to 0 if sign is +, -1 if sign is -
//        offset[*interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[*interfaceAxis]));
//        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
//        // If currCell contains something other than EMPTY, return
//        // curr_t
//        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
//        if(cellType != EMPTY) {
//            *out_blockHit = currCell;
//            *out_dist = glm::min(maxLen, curr_t);
//            return true;
//        }
//    }
//    *out_dist = glm::min(maxLen, curr_t);
//    return false;
//}
