#ifndef VBODATA_H
#define VBODATA_H
#include "chunk.h"
#include "smartpointerhelp.h"
#include "glm_includes.h"

class VBOData
{


public:
    std::vector<GLuint> idx;
    std::vector<glm::vec4> posNorCol;
    std::vector<GLuint> t_idx;
    std::vector<glm::vec4> t_posNorCol;
    Chunk* cPtr;
    VBOData(Chunk* cPtr);

};

#endif // VBODATA_H
