#include "turtle.h"

Turtle::Turtle(glm::vec2 p, glm::vec2 d)
{
    position = p;
    setDirection(d);
}

void Turtle::setDirection(glm::vec2 d) {
    direction = glm::normalize(d);
}
