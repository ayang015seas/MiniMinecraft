#include "lsystem.h"
#include <iostream>
#include <math.h>
#include <glm/gtx/string_cast.hpp>

LSystem::LSystem(glm::vec2 startPos, glm::vec2 startDir, unsigned int maxIterations)
    : turtle(Turtle(startPos, startDir)),
      turtleHistory(),
      maxIterations(maxIterations) {}

void LSystem::fRule() {
    glm::vec2 start = turtle.position;
    float length = LENGTH * pow(2.f/3.f, turtle.depth);
    glm::vec2 end = start + length*turtle.direction;

    river.push_back(lineSegment{start, end, turtle.depth});
    turtle.position = end;
}

void LSystem::plusRule() {
    float x = turtle.direction.x;
    float y = turtle.direction.y;

    float angle = ANGLE - (turtle.depth-1)*5;
    float sinAngle = glm::sin(glm::radians(angle));
    float cosAngle = glm::cos(glm::radians(angle));

    glm::vec2 newDir(cosAngle*x - sinAngle*y, sinAngle*x + cosAngle*y);
    turtle.setDirection(newDir);
}

void LSystem::minusRule() {
    float x = turtle.direction.x;
    float y = turtle.direction.y;

    float angle = ANGLE - (turtle.depth-1)*5;
    float sinAngle = glm::sin(glm::radians(angle));
    float cosAngle = glm::cos(glm::radians(angle));

    glm::vec2 newDir(cosAngle*x + sinAngle*y, -1*sinAngle*x + cosAngle*y);
    turtle.setDirection(newDir);
}

void LSystem::lBracketRule() {
    turtle.depth = 1 + turtleHistory.size();
    turtleHistory.push(turtle);
    turtle.depth++;
}

void LSystem::rBracketRule() {
    turtle = turtleHistory.top();
    turtleHistory.pop();
}

void LSystem::create(unsigned int random) {

    std::string newString = "";
    std::string curString = AXIOM;
    std::string::iterator s;

    srand(random);

    for (unsigned int i = 0; i < maxIterations; i++) {
        for (s = curString.begin(); s < curString.end(); s++) {
            switch(*s) {
            case 'X': {
                float r = (float)rand() / (RAND_MAX);
                if (r < 0.7f) {
                    newString.append(std::string("[+FX]-FX"));
                } else {
                    newString.append(std::string("[+FX][-FX]"));
                }
                break;
            }
            default:
                newString += *s;
            }
        }
        curString = newString;
        newString = "";
    }

    ANGLE = (float)rand() / (RAND_MAX) * 20 + 30;

    for (s = curString.begin(); s < curString.end(); s++) {
        switch(*s) {
        case 'F':
            fRule();
            break;
        case '+':
            plusRule();
            break;
        case '-':
            minusRule();
            break;
        case '[':
            lBracketRule();
            break;
        case ']':
            rBracketRule();
            break;
        case 'X':
            break;
        default:
            std::cout << std::string("Unrecognized symbol encountered:") + *s << std::endl;
            break;
        }
    }
}

void LSystem::draw(int x, int z, std::vector<Chunk*>& chunks) {
    glm::vec2 origin = glm::vec2(x, z);
    std::vector<lineSegment>::iterator it;
    for (it = river.begin(); it < river.end(); ++it) {
        float r = 4.f - it->branch*(11.f/13);
        std::vector<glm::vec2> points = getPointsOnSinCurve(it->start, it->end, 1);
        for (int i = 1; i < points.size(); i++) {
            fillWithWater(points[i-1] + origin, points[i] + origin, r, 136, chunks);
        }
    }
}

void LSystem::fillWithWater(glm::vec2 start, glm::vec2 end, float r, int height, std::vector<Chunk*>& chunks) {
    float RADIUS = glm::clamp(r * 1.7f, 3.f, 11.f);
    float elevation = std::sqrt(RADIUS*RADIUS - r*r) + height;

    int llx = floor(std::min(start.x, end.x) - RADIUS);
    int urx = ceil(std::max(start.x, end.x) + RADIUS);
    int lly = floor(std::min(start.y, end.y) - RADIUS);
    int ury = ceil(std::max(start.y, end.y) + RADIUS);

    for (int i = llx; i < urx; i++) {
        for (int j = lly; j < ury; j++) {
            for (int y = floor(elevation - RADIUS); y < elevation + 1; y++) {
                float dist = test(glm::vec3(i, y, j), glm::vec3(start.x, elevation, start.y), glm::vec3(end.x, elevation, end.y), RADIUS);
                if (dist <= 0 ) { //&& terrain->hasChunkAt(i, j)
                    int inChungusx = i - static_cast<int>(glm::floor(i / 64.f) * 64.f);
                    int inChungusz = j - static_cast<int>(glm::floor(j / 64.f) * 64.f);
                    int chunkx = static_cast<int>(glm::floor(inChungusx / 16.f));
                    int chunkz = static_cast<int>(glm::floor(inChungusz / 16.f));
                    int inChunkx = inChungusx - chunkx * 16;
                    int inChunkz = inChungusz - chunkz * 16;
                    //std::cout << chunkx << " " << chunkz << std::endl;
                    if (y <= height) {
                        chunks[4 * chunkx + chunkz]->setBlockAt(inChunkx, y, inChunkz, WATER);
                        chunks[4 * chunkx + chunkz]->setBlockAt(inChunkx, y - 1, inChunkz, WATER);
                    } else {
                        int y2 = y;
                        while(chunks[4 * chunkx + chunkz]->getBlockAt(inChunkx, y2, inChunkz) != EMPTY) {
                            chunks[4 * chunkx + chunkz]->setBlockAt(inChunkx, y2++, inChunkz, EMPTY);
                        }
                    }
                }
            }
        }
    }
}

float LSystem::test(glm::vec3 p, glm::vec3 a, glm::vec3 b, float r) {
    glm::vec3 pa = p - a;
    glm::vec3 ba = b - a;
    float h = glm::clamp(glm::dot(pa, ba)/glm::dot(ba, ba), 0.f, 1.f);
    return glm::length(pa - ba*h) - r;
}

std::vector<glm::vec2> LSystem::getPointsOnSinCurve(glm::vec2& start, glm::vec2& end, float stepSize) {
    std::vector<glm::vec2> points;
    if (start.x > 4 && start.x < 59 && start.y > 4 && start.y < 59) {
        points.push_back(start);
    }
    float t = stepSize;

    float amplitude = 0.5;
    glm::vec2 dirNorm = glm::normalize(end - start);
    glm::vec2 perpNorm = glm::vec2(dirNorm.y, -1*dirNorm.x);
    float length = glm::length(end - start);
    float height = length / (2 * M_PI);

    while (t < length) {
        float x = t * 2*M_PI / length;
        float y = glm::sin(x)*height*amplitude;

        glm::vec2 point(t*dirNorm + start + y*perpNorm);
        if (point.x > 4 && point.x < 59 && point.y > 4 && point.y < 59) {
            points.push_back(point);
        }
        t += stepSize;
    }
    if (end.x > 4 && end.x < 59 && end.y > 4 && end.y < 59) {
        points.push_back(end);
    }
    return points;
}
