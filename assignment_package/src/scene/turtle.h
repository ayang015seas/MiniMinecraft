#pragma once

#include <glm/glm.hpp>

class Turtle
{

public:
    Turtle(glm::vec2 p, glm::vec2 d);
    glm::vec2 position;
    glm::vec2 direction;
    int depth = 1;

    void setDirection(glm::vec2 d);
};
