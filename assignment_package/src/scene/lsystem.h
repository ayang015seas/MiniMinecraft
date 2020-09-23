#pragma once

#include "turtle.h"
#include <map>
#include <stack>
#include <vector>
#include "chunk.h"

class LSystem
{
    typedef void (*Rule)(void);
    struct lineSegment {
        glm::vec2 start;
        glm::vec2 end;
        int branch;
    };

private:
    std::string AXIOM = "FX";
    float ANGLE = 45;
    float LENGTH = 40;

    unsigned int maxIterations;

    Turtle turtle;
    std::stack<Turtle> turtleHistory;
    std::vector<lineSegment> river;

    // Drawing rules
    void fRule();
    void plusRule();
    void minusRule();
    void lBracketRule();
    void rBracketRule();

    std::vector<glm::vec2> getPointsOnSinCurve(glm::vec2& start, glm::vec2& end, float stepSize);
    void fillWithWater(glm::vec2 start, glm::vec2 end, float r, int height, std::vector<Chunk*>& chunks);
    float test(glm::vec3 p, glm::vec3 a, glm::vec3 b, float r);

public:
    LSystem(glm::vec2 startPos, glm::vec2 startDir, unsigned int maxIterations);

    void create(unsigned int random);
    void draw(int x, int z, std::vector<Chunk*>& chunks);
};


