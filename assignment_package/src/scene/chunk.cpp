#include "chunk.h"
#include <iostream>

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

// functions to return UV coordinates
glm::vec4 rightUp() {
    return glm::vec4(1.f/ 16.f, 1.f/16.f, 0.f, 0.f);
}

glm::vec4 leftUp() {
    return glm::vec4(0.f/ 16.f, 1.f/16.f, 0.f, 0.f);
}

glm::vec4 leftDown() {
    return glm::vec4(0.f/ 16.f, 0.f/16.f, 0.f, 0.f);
}

glm::vec4 rightDown() {
    return glm::vec4(1.f/ 16.f, 0.f/16.f, 0.f, 0.f);
}

int Chunk::getworldX() {
    return static_cast<int>(glm::round(worldX));
}
int Chunk::getworldZ() {
    return static_cast<int>(glm::round(worldZ));
}

glm::vec4 getCol(BlockType b) {
    switch(b)
    {
    case EMPTY: return glm::vec4(0.f, 0.f, 0.f, 0.f);
    case GRASS: return glm::vec4(0.223, 0.6, 0.149, 1.f);
    case DIRT:  return glm::vec4(0.5294, 0.341, 0.153, 1.f);
    case STONE: return glm::vec4(0.561, 0.561, 0.561, 1.f);
    case REDWOOL: return glm::vec4(0.f, 0.f, 0.f, 1.f);
    case PINKWOOL: return glm::vec4(0.f, 0.f, 0.0f, 1.f);
    case GREENWOOL: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case YELLOWWOOL: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case BLUEWOOL: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case PURPLEWOOL: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case ORANGEWOOL: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case WOOD: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case ENDSAND: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case OBSIDIAN: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    }
}

glm::vec4 getUV(BlockType b) {
    switch(b)
    {
    case EMPTY: return glm::vec4(0.f, 0.f, 0.0f, 0.f);
    case GRASS: return glm::vec4(8.f / 16.f, 13.f / 16.f, 0.0f, 0.f);
    case GRASSSIDE: return glm::vec4(3.f / 16.f, 15.f / 16.f, 0.0f, 0.f);
    case DIRT:  return glm::vec4(2.f / 16.f, 15.f / 16.f, 0.0f, 0.f);
    case STONE: return glm::vec4(1.f / 16.f, 15.f / 16.f, 0.0f, 0.f);
    case ICE: return glm::vec4(2.f/ 16.f, 11.f /16.f, 0.0f, 0.f);
    case REDWOOL: return glm::vec4(1.f/ 16.f, 7.f/ 16.f, 0.f, -0.1f);
    case PINKWOOL: return glm::vec4(2.f/ 16.f, 7.f/ 16.f, 0.0f, -0.1f);
    case GREENWOOL: return glm::vec4(2.f/ 16.f, 6.f/ 16.f, 0.0f, -0.1f);
    case YELLOWWOOL: return glm::vec4(2.f/ 16.f, 5.f/ 16.f, 0.0f, -0.1f);
    case BLUEWOOL: return glm::vec4(2.f/ 16.f, 4.f/ 16.f, 0.0f, -0.1f);
    case PURPLEWOOL: return glm::vec4(2.f/ 16.f, 3.f/ 16.f, 0.0f, -0.1f);
    case ORANGEWOOL: return glm::vec4(2.f/ 16.f, 2.f/ 16.f, 0.0f, -0.1f);
    case WOOD: return glm::vec4(6.f/ 16.f, 2.f/ 16.f, 0.0f, -0.1f);
    case WATER: return glm::vec4(14.f / 16.f, 2.f / 16.f, 1.0f, 1.f);
    case ENDSAND: return glm::vec4(15.f / 16.f, 5.f / 16.f, 0.0f, -0.1f);
    case OBSIDIAN: return glm::vec4(7.f / 16.f, 4.f / 16.f, 0.0f, -0.1f);
    }
}

Chunk::Chunk(OpenGLContext *mp_context, float x, float z)
    : Drawable(mp_context), m_blocks(), worldX(x), worldZ(z),
      m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    this->maxHeight = 140.f;
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}


void Chunk::create()
{
    std::vector<GLuint> idx {};
    std::vector<glm::vec4> posNorCol = {};
    int i = 0;

    std::vector<GLuint> t_idx {};
    std::vector<glm::vec4> t_posNorCol = {};
    int t_i = 0;


    // y axis
    createNSFaces(idx, posNorCol, i, t_idx, t_posNorCol, t_i);

    // x axis
    createEWFaces(idx, posNorCol, i, t_idx, t_posNorCol, t_i);

    // y axis
    createUDFaces(idx, posNorCol, i, t_idx, t_posNorCol, t_i);


    m_count = idx.size();

    t_count = t_idx.size();

    populateVBOs(idx, posNorCol);
    populateTVBOs(t_idx, t_posNorCol);
}

void Chunk::setm_count(int m_count) {
    this->m_count = m_count;
}
void Chunk::sett_count(int t_count) {
    this->t_count = t_count;
}

int Chunk::getm_count() {
    return m_count;
}
int Chunk::gett_count() {
    return t_count;
}

void Chunk::populateVBOs(const std::vector<GLuint> &idx, const std::vector<glm::vec4> &posNorCol)
{
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePosNorCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosNorCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, posNorCol.size() * sizeof(glm::vec4), posNorCol.data(), GL_STATIC_DRAW);
}

void Chunk::populateTVBOs(const std::vector<GLuint> &idx, const std::vector<glm::vec4> &posNorCol)
{
//    std::cout << "Trans Size" << idx.size() << std::endl;
    generateTIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generateTPosNorCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, t_bufPosNorCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, posNorCol.size() * sizeof(glm::vec4), posNorCol.data(), GL_STATIC_DRAW);
}


// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

// determines if we should map uvs to a side grass texture
glm::vec4 isGrassSide(BlockType curr) {
    if (curr == GRASS) {
        return getUV(GRASSSIDE);
    } else {
        return getUV(curr);
    }
}

bool isTransparent(BlockType curr) {
    return (getUV(curr).z == 1.f);
}

// pushes all indices and buffer elements to a std::vector
void pushAll(std::vector<GLuint>& idx, std::vector<glm::vec4>& posNorCol, int& i,
             glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 p4, glm::vec4 norm, glm::vec4 col, glm::vec4 UV) {
    glm::vec4 v[16] = {p1, norm, col, UV + rightDown(),
                       p2, norm, col, UV + leftDown(),
                       p3, norm, col, UV + leftUp(),
                       p4, norm, col, UV + rightUp()};

    posNorCol.insert(posNorCol.end(), std::begin(v), std::end(v));

    int indices[6] = {i, i + 1, i + 2, i, i + 2, i + 3};
    idx.insert(idx.end(), std::begin(indices), std::end(indices));

    i += 4;
}

void Chunk::createEWFaces(std::vector<GLuint>& idx, std::vector<glm::vec4>& posNorCol, int& i,
                          std::vector<GLuint>& t_idx, std::vector<glm::vec4>& t_posNorCol, int& t_i)
{
    // i is an incrementer
    // each chunk is 16 by 16 by 256

    glm::vec4 norm;
    glm::vec4 col;
    glm::vec4 UV;

    glm::vec4 p1;
    glm::vec4 p2;
    glm::vec4 p3;
    glm::vec4 p4;

    // populate EAST and WEST faces of the chunk
    Chunk* chunkLeft = m_neighbors[XNEG];
    Chunk* chunkRight = m_neighbors[XPOS];
    if (chunkLeft) {
        // creating all along the x axis
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 256; y++) {
                BlockType blockCurrent = getBlockAt(0, y, z);
                BlockType blockLeft = chunkLeft->getBlockAt(15, y, z);

                if ((blockCurrent != EMPTY) && (blockLeft != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(-1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockLeft);
                    } else if (isTransparent(blockLeft)) {
                        norm = glm::vec4(1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }
                    if (isTransparent(blockCurrent) ^ isTransparent(blockLeft)) {
                        p1 = glm::vec4(worldX, y + 0.f, worldZ + z, 1.f);
                        p2 = glm::vec4(worldX, y + 0.f, worldZ + z + 1, 1.f);
                        p3 = glm::vec4(worldX, y + 1.f, worldZ + z + 1, 1.f);
                        p4 = glm::vec4(worldX, y + 1.f, worldZ + z, 1.f);
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);
                    }

                } else if ((blockCurrent == EMPTY) != (blockLeft == EMPTY)) {

                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(-1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    } else {
                        norm = glm::vec4(1, 0, 0, 0);
                        col = getCol(blockLeft);
                        UV = isGrassSide(blockLeft);
                    }
                    p1 = glm::vec4(worldX, y + 0.f, worldZ + z, 1.f);
                    p2 = glm::vec4(worldX, y + 0.f, worldZ + z + 1, 1.f);
                    p3 = glm::vec4(worldX, y + 1.f, worldZ + z + 1, 1.f);
                    p4 = glm::vec4(worldX, y + 1.f, worldZ + z, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockLeft)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);
                    }
                }
            }
        }
    }
    if (chunkRight) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 256; y++) {
                BlockType blockCurrent = getBlockAt(15, y, z);
                BlockType blockRight = chunkRight->getBlockAt(0, y, z);

                if ((blockCurrent != EMPTY) && (blockRight != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockRight);
                    } else if (isTransparent(blockRight)) {
                        norm = glm::vec4(-1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }

                    if (isTransparent(blockCurrent) ^ isTransparent(blockRight)) {
                        p1 = glm::vec4(worldX + 16.f, y + 0.f, worldZ + z, 1.f);
                        p2 = glm::vec4(worldX + 16.f, y + 0.f, worldZ + z + 1, 1.f);
                        p3 = glm::vec4(worldX + 16.f, y + 1.f, worldZ + z + 1, 1.f);
                        p4 = glm::vec4(worldX + 16.f, y + 1.f, worldZ + z, 1.f);

                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);
                    }

                } else if ((blockCurrent == EMPTY) != (blockRight == EMPTY)) {

                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    } else {
                        norm = glm::vec4(-1, 0, 0, 0);
                        col = getCol(blockRight);
                        UV = isGrassSide(blockRight);
                    }
                    p1 = glm::vec4(worldX + 16.f, y + 0.f, worldZ + z, 1.f);
                    p2 = glm::vec4(worldX + 16.f, y + 0.f, worldZ + z + 1, 1.f);
                    p3 = glm::vec4(worldX + 16.f, y + 1.f, worldZ + z + 1, 1.f);
                    p4 = glm::vec4(worldX + 16.f, y + 1.f, worldZ + z, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockRight)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);

                    }
                }
            }
        }
    }

    // populate blocks between EAST-WEST faces of the chunk
    for (int x = 0; x < 15; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 256; y++) {
                BlockType blockCurrent = getBlockAt(x, y, z);
                BlockType blockRight   = getBlockAt(x + 1, y, z);

                if ((blockCurrent != EMPTY) && (blockRight != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockRight);
                    } else if (isTransparent(blockRight)) {
                        norm = glm::vec4(-1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }

                    if (isTransparent(blockCurrent) ^ isTransparent(blockRight)) {
                        p1 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z, 1.f);
                        p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z + 1, 1.f);
                        p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                        p4 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z, 1.f);

                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);


                    }

                } else if ((blockCurrent == EMPTY) != (blockRight == EMPTY)) {
                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(1, 0, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    } else {
                        norm = glm::vec4(-1, 0, 0, 0);
                        col = getCol(blockRight);
                        UV = isGrassSide(blockRight);
                    }
                    p1 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z, 1.f);
                    p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z + 1, 1.f);
                    p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                    p4 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockRight)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);
                    }
                }
            }
        }
    }


    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 15; z++) {
            for (int y = 0; y < 256; y++) {
                BlockType blockCurrent = getBlockAt(x, y, z);
                BlockType blockRight   = getBlockAt(x, y, z + 1);

                if ((blockCurrent != EMPTY) && (blockRight != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockRight);
                    } else if (isTransparent(blockRight)) {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }

                    if (isTransparent(blockCurrent) ^ isTransparent(blockRight)) {
                        p1 = glm::vec4(worldX + x, y + 0.f, worldZ + z + 1, 1.f);
                        p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z + 1, 1.f);
                        p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                        p4 = glm::vec4(worldX + x, y + 1.f, worldZ + z + 1, 1.f);

                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);

                    }

                } else if ((blockCurrent == EMPTY) != (blockRight == EMPTY)) {
                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    } else {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockRight);
                        UV = isGrassSide(blockRight);
                    }
                    p1 = glm::vec4(worldX + x, y + 0.f, worldZ + z + 1, 1.f);
                    p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z + 1, 1.f);
                    p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                    p4 = glm::vec4(worldX + x, y + 1.f, worldZ + z + 1, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockRight)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);
                    }
                }
            }
        }
    }

}


// this is the problem function (creates all faces along the z axis)

void Chunk::createNSFaces(std::vector<GLuint> &idx, std::vector<glm::vec4> &posNorCol, int &i,
                          std::vector<GLuint>& t_idx, std::vector<glm::vec4>& t_posNorCol, int& t_i)
{
    glm::vec4 UV;
    glm::vec4 norm;
    glm::vec4 col;
    glm::vec4 p1;
    glm::vec4 p2;
    glm::vec4 p3;
    glm::vec4 p4;


    // populate EAST and WEST faces of the chunk
    Chunk* chunkForward = m_neighbors[ZPOS];
    Chunk* chunkBackward = m_neighbors[ZNEG];

    // under this if statement, we are basically checking the
    // absolute sides of the chunk (part that borders neighboring chunk only)
    if (chunkBackward) {
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 256; y++) {
                BlockType blockCurrent = getBlockAt(x, y, 0);
                BlockType blockBackward = chunkBackward->getBlockAt(x, y, 15);


                if ((blockCurrent != EMPTY) && (blockBackward != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockBackward);
                    } else if (isTransparent(blockBackward)) {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }

                    if (isTransparent(blockCurrent) ^ isTransparent(blockBackward)) {
                        p1 = glm::vec4(worldX + x, y + 0.f, worldZ, 1.f);
                        p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ, 1.f);
                        p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ, 1.f);
                        p4 = glm::vec4(worldX + x, y + 1.f, worldZ, 1.f);

                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);
                    }

                } else if ((blockCurrent == EMPTY) != (blockBackward == EMPTY)) {
                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);

                    } else {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockBackward);
                        UV = isGrassSide(blockBackward);
                    }
                    p1 = glm::vec4(worldX + x, y + 0.f, worldZ, 1.f);
                    p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ, 1.f);
                    p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ, 1.f);
                    p4 = glm::vec4(worldX + x, y + 1.f, worldZ, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockBackward)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);
                    }
                }
            }
        }
    }

    // under this if statement, we are basically checking the
    // absolute other side of the chunk (part that borders neighboring chunk only)
    if (chunkForward) {
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 256; y++) {
                BlockType blockCurrent = getBlockAt(x, y, 15);
                BlockType blockForward = chunkForward->getBlockAt(x, y, 0);

                if ((blockCurrent != EMPTY) && (blockForward != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockForward);
                    } else if (isTransparent(blockForward)) {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }

                    if (isTransparent(blockCurrent) ^ isTransparent(blockForward)) {
                        p1 = glm::vec4(worldX + x, y + 0.f, worldZ + 16.f, 1.f);
                        p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + 16.f, 1.f);
                        p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + 16.f, 1.f);
                        p4 = glm::vec4(worldX + x, y + 1.f, worldZ + 16.f, 1.f);

                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);
                    }

                } else if ((blockCurrent == EMPTY) != (blockForward == EMPTY)) {
                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    } else {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockForward);
                        UV = isGrassSide(blockForward);
                    }
                    p1 = glm::vec4(worldX + x, y + 0.f, worldZ + 16.f, 1.f);
                    p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + 16.f, 1.f);
                    p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + 16.f, 1.f);
                    p4 = glm::vec4(worldX + x, y + 1.f, worldZ + 16.f, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockForward)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);
                    }
                }
            }
        }
    }

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 15; z++) {
            for (int y = 0; y < 256; y++) {
                BlockType blockCurrent = getBlockAt(x, y, z);
                BlockType blockRight   = getBlockAt(x, y, z + 1);

                if ((blockCurrent != EMPTY) && (blockRight != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockRight);
                    } else if (isTransparent(blockRight)) {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }

                    if (isTransparent(blockCurrent) ^ isTransparent(blockRight)) {
                        p1 = glm::vec4(worldX + x, y + 0.f, worldZ + z + 1, 1.f);
                        p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z + 1, 1.f);
                        p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                        p4 = glm::vec4(worldX + x, y + 1.f, worldZ + z + 1, 1.f);

                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);
                    }

                } else if ((blockCurrent == EMPTY) != (blockRight == EMPTY)) {
                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(0, 0, 1, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    } else {
                        norm = glm::vec4(0, 0, -1, 0);
                        col = getCol(blockRight);
                        UV = isGrassSide(blockRight);
                    }
                    p1 = glm::vec4(worldX + x, y + 0.f, worldZ + z + 1, 1.f);
                    p2 = glm::vec4(worldX + x + 1, y + 0.f, worldZ + z + 1, 1.f);
                    p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                    p4 = glm::vec4(worldX + x, y + 1.f, worldZ + z + 1, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockRight)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);
                    }
                }
            }
        }
    }

}

void Chunk::createUDFaces(std::vector<GLuint> &idx, std::vector<glm::vec4> &posNorCol, int &i,
                          std::vector<GLuint>& t_idx, std::vector<glm::vec4>& t_posNorCol, int& t_i)
{
    glm::vec4 UV;
    glm::vec4 norm;
    glm::vec4 col;
    glm::vec4 p1;
    glm::vec4 p2;
    glm::vec4 p3;
    glm::vec4 p4;

    // populate blocks between UP-DOWN faces of the chunk
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 255; y++) {
                BlockType blockCurrent = getBlockAt(x, y, z);
                BlockType blockAbove   = getBlockAt(x, y + 1, z);
                if ((blockCurrent != EMPTY) && (blockAbove != EMPTY)) {
                    if (isTransparent(blockCurrent)) {
                        norm = glm::vec4(0, 1, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockAbove);
                    } else if (isTransparent(blockAbove)) {
                        norm = glm::vec4(0, -1, 0, 0);
                        col = getCol(blockCurrent);
                        UV = isGrassSide(blockCurrent);
                    }

                    if (isTransparent(blockCurrent) ^ isTransparent(blockAbove)) {
                        p1 = glm::vec4(worldX + x, y + 1.f, worldZ + z, 1.f);
                        p2 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z, 1.f);
                        p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                        p4 = glm::vec4(worldX + x, y + 1.f, worldZ + z + 1, 1.f);

                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, -norm, col, UV);
                    }

                } else if ((blockCurrent == EMPTY) != (blockAbove == EMPTY)) {
                    if (blockCurrent != EMPTY) {
                        norm = glm::vec4(0, 1, 0, 0);
                        col = getCol(blockCurrent);
                        UV = getUV(blockCurrent);
                    } else {
                        norm = glm::vec4(0, -1, 0, 0);
                        col = getCol(blockAbove);
                        UV = getUV(blockAbove);
                    }

                    p1 = glm::vec4(worldX + x, y + 1.f, worldZ + z, 1.f);
                    p2 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z, 1.f);
                    p3 = glm::vec4(worldX + x + 1, y + 1.f, worldZ + z + 1, 1.f);
                    p4 = glm::vec4(worldX + x, y + 1.f, worldZ + z + 1, 1.f);

                    if (isTransparent(blockCurrent) || isTransparent(blockAbove)) {
                        pushAll(t_idx, t_posNorCol, t_i, p1, p2, p3, p4, norm, col, UV);
                    } else {
                        pushAll(idx, posNorCol, i, p1, p2, p3, p4, norm, col, UV);
                    }
                }
            }
        }
    }
}
