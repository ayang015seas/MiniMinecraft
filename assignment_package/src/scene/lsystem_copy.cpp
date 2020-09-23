#include "lsystem.h"
#include <iostream>

LSystem::LSystem(glm::vec2 startPos, glm::vec2 startDir, unsigned int maxIterations)
    : turtle(Turtle(startPos, startDir)),
      turtleHistory(),
      maxIterations(maxIterations)
{
    charToExpansion['X'] = std::string("[+FX]-FX");
}

void LSystem::fRule() {
    glm::vec2 start = turtle.position;
    glm::vec2 end = start + turtle.direction*LENGTH;

    river.push_back(lineSegment{start, end});
    turtle.position = end;
}

void LSystem::plusRule() {
    float x = turtle.direction.x;
    float y = turtle.direction.y;

    turtle.direction.x = cosAngle*x - sinAngle*y;
    turtle.direction.y = sinAngle*x + cosAngle*y;
}

void LSystem::minusRule() {
    float x = turtle.direction.x;
    float y = turtle.direction.y;

    turtle.direction.x = cosAngle*x + sinAngle*y;
    turtle.direction.y = -1*sinAngle*x + cosAngle*y;
}

void LSystem::lBracketRule() {
    turtleHistory.push(turtle);
}

void LSystem::rBracketRule() {
    turtle = turtleHistory.top();
    turtleHistory.pop();
}

void LSystem::create() {
    std::string newString = "";
    std::string curString = AXIOM;
    std::string::iterator s;

    for (unsigned int i = 0; i < maxIterations; i++) {
        for (s = curString.begin(); s < curString.end(); s++) {
            if (charToExpansion.find(*s) != charToExpansion.end()) {
                newString.append(charToExpansion[*s]);
            } else {
                newString += *s;
            }
        }
        curString = newString;
        newString = "";
    }

    std::cout << curString << std::endl;
}
