#include "terrain.h"
//#include "cube.h"
#include <stdexcept>
#include <iostream>

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), m_generatingTerrain(),
      m_generatedTerrainBuffer(), mp_context(context), chonksInit(),
      chonksData(), chonksInitmut(), vboDatamut()
{}

Terrain::~Terrain() {
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
//int64_t toKey(int x, int z) {
//    int64_t xz = 0xffffffffffffffff;
//    int64_t x64 = x;
//    int64_t z64 = z;

//    // Set all lower 32 bits to 1 so we can & with Z later
//    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

//    // Set all upper 32 bits to 1 so we can & with XZ
//    z64 = z64 | 0xffffffff00000000;

//    // Combine
//    xz = xz & z64;
//    return xz;
//}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}

bool Terrain::hasBigChungusAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 64.f));
    int zFloor = static_cast<int>(glm::floor(z / 64.f));
    bool hasBigChungus = m_generatedTerrain.find(toKey(64 * xFloor, 64 * zFloor)) != m_generatedTerrain.end();
    return hasBigChungus;
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
//    std::cout << "Chunk x1 " << x << std::endl;
//    std::cout << "Chunk z1 " << z << std::endl;

    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    uPtr<Chunk>& c = m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
//    std::cout << "Max Height " << c.get()->maxHeight << std::endl;
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    chonksInitmut.lock();
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
    chonksInitmut.unlock();
}

float mountGrassHeight(int xCoord, int zCoord) {
    float nVal = Noise::fbm3Alt(glm::vec2(xCoord, zCoord) * (1.f / 70.f));
    float n = glm::smoothstep(0.f, 1.f, 1.f - nVal + 0.5f);
    n = n - 0.23f;
    n *= 1.42f;
    n = glm::smoothstep(0.f, 1.f, n);

    float extraNoise = Noise::fbm3(glm::vec2(xCoord, zCoord) * (1.f / 15.f));
    float extraNoise2 = Noise::fbm3(glm::vec2(xCoord, zCoord));

    extraNoise = extraNoise - 1.0f;
    extraNoise = glm::abs(extraNoise);
    extraNoise *= 5.f;

    extraNoise2 = extraNoise2 - 1.0f;
    extraNoise2 = glm::abs(extraNoise2);
    extraNoise2 *= 5.f;

    float grassHeight = Noise::grassLands2(
                glm::vec2(xCoord, zCoord));
    float mountHeight = Noise::mountainSparse(glm::vec2(xCoord, zCoord));
    float height = 0.0f;

    height = glm::mix(grassHeight, mountHeight, n);
    return height;
}


Chunk* Terrain::createChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context, x, z);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = move(chunk);
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }

    for(int i = 0; i < 16; ++i) {
        for(int j = 0; j < 16; ++j) {
            float height = mountGrassHeight(x + i, z + j);
//            float nVal = Noise::fbm3Alt(glm::vec2(x + i, z + j) * (1.f / 70.f));
//            float n = glm::smoothstep(0.f, 1.f, 1.f - nVal + 0.5f);
//            n = n - 0.23f;
//            n *= 1.42f;
//            n = glm::smoothstep(0.f, 1.f, n);

            float extraNoise = Noise::fbm3(glm::vec2(x + i, z + j) * (1.f / 15.f));
            float extraNoise2 = Noise::fbm3(glm::vec2(x + i, z + j));

//            extraNoise = extraNoise - 1.0f;
//            extraNoise = glm::abs(extraNoise);
//            extraNoise *= 5.f;

//            extraNoise2 = extraNoise2 - 1.0f;
//            extraNoise2 = glm::abs(extraNoise2);
//            extraNoise2 *= 5.f;

//            float grassHeight = Noise::grassLands2(
//                        glm::vec2(x + i, z + j));
//            float mountHeight = Noise::mountainSparse(glm::vec2(x + i, z + j));
//            float height = 0.0f;

//            height = glm::mix(grassHeight, mountHeight, n);

            if (height > cPtr->maxHeight) {
                cPtr->maxHeight = height;
            } else {
                cPtr->maxHeight = 140.f;
            }

            for (int k = 0; k < height - 1; k++) {
                if (k < 128) {
                    cPtr->setBlockAt(i, k, j, STONE);
                } else if (k < 150) {
                    cPtr->setBlockAt(i, k, j, DIRT);
                } else {
                    cPtr->setBlockAt(i, k, j, STONE);
                }
            }

            if (height < 150) {
                if (extraNoise < 0.07f) {
                    cPtr->setBlockAt(i, glm::floor(height), j, DIRT); // dirt
                } else {
                    cPtr->setBlockAt(i, glm::floor(height), j, GRASS); // grass
                }
            } else if (height < 155) {
                if (extraNoise > 0.4f) {
                    cPtr->setBlockAt(i, glm::floor(height), j, GRASS);
                } else {
                    cPtr->setBlockAt(i, glm::floor(height), j, STONE);
                }
            } else if (height < 163) {
                if (extraNoise < 0.07f) {
                    cPtr->setBlockAt(i, glm::floor(height), j, DIRT);
                } else {
                    cPtr->setBlockAt(i, glm::floor(height), j, STONE);
                }
            } else {
                if (extraNoise2 > 0.4f && height > 165) {
                    cPtr->setBlockAt(i, glm::floor(height), j, ICE);
                } else if (extraNoise < 0.07f) {
                    cPtr->setBlockAt(i, glm::floor(height), j, DIRT);
                } else {
                    cPtr->setBlockAt(i, glm::floor(height), j, STONE);
                }
            }
        }
    }
    return cPtr;
}

void Terrain::createBigChungusAt(int x, int z) {

    Chunk* newchunks[16];
    int increment = 0;
    if (!hasBigChungusAt(x, z)) {
        for (int i = 0; i < 64; i += 16) {
            for (int j = 0; j < 64; j += 16) {
                newchunks[increment++] = createChunkAt(x + i, z + j);
            }
        }
        m_generatedTerrain.insert(toKey(x, z));
        for (int i = 0; i < 16; i++) {
            newchunks[i]->create();
        }
    }
}

void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if (hasChunkAt(x, z)) {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(0, 0, 0)));
                shaderProgram->drawChunk(*chunk);
            }
        }
    }   
}

void Terrain::draw(const glm::vec3 &position, ShaderProgram *shaderProgram) {
    int x = static_cast<int>(position.x);
    int z = static_cast<int>(position.z);

    for (int i = -2; i < 3; i++) {
        for (int j = -2; j < 3; j++) {
            int xFloor = 64 * (static_cast<int>(glm::floor(x / 64.f)) + i);
            int zFloor = 64 * (static_cast<int>(glm::floor(z / 64.f)) + j);
            if (hasBigChungusAt(xFloor, zFloor)) {
                for (int x = 0; x < 64; x += 16) {
                    for (int z = 0; z < 64; z += 16) {
                        const uPtr<Chunk> &chunk = getChunkAt(xFloor + x, zFloor + z);
                        if (chunk->getm_count() != -1) {
                            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(0, 0, 0)));
                            shaderProgram->drawChunk(*chunk);
                        }
                    }
                }
            }
        }
    }
    for (int i = -2; i < 3; i++) {
        for (int j = -2; j < 3; j++) {
            int xFloor = 64 * (static_cast<int>(glm::floor(x / 64.f)) + i);
            int zFloor = 64 * (static_cast<int>(glm::floor(z / 64.f)) + j);
            if (hasBigChungusAt(xFloor, zFloor)) {
                for (int x = 0; x < 64; x += 16) {
                    for (int z = 0; z < 64; z += 16) {
                        const uPtr<Chunk> &chunk = getChunkAt(xFloor + x, zFloor + z);
                        if (chunk->getm_count() != -1) {
                            shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(0, 0, 0)));
                            shaderProgram->drawChunkT(*chunk);
                        }
                    }
                }
            }
        }
    }
}


bool Terrain::hasProcessedChungusAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 64.f));
    int zFloor = static_cast<int>(glm::floor(z / 64.f));
    int64_t key = toKey(64 * xFloor, 64 * zFloor);
    bool hasProcessedChungus = (m_generatedTerrain.find(key) != m_generatedTerrain.end() ||
            m_generatingTerrain.find(key) != m_generatingTerrain.end());
    return hasProcessedChungus;
}

void Terrain::tick(const glm::vec3& position) {

    int x = static_cast<int>(glm::floor(position.x));
    int z = static_cast<int>(glm::floor(position.z));

    for (int i = -2; i < 3; i++) {
        for (int j = -2; j < 3; j++) {
            int chunkx = (static_cast<int>(glm::floor(x / 64.f)) + i) * 64;
            int chunkz = (static_cast<int>(glm::floor(z / 64.f)) + j) * 64;
            if (!hasProcessedChungusAt(chunkx, chunkz)) {
                std::vector<Chunk*> toAdd;
                toAdd.clear();
                for (int k = 0; k < 64; k += 16) {
                    for (int l = 0; l < 64; l += 16) {
                        uPtr<Chunk> chunk = mkU<Chunk>(mp_context, chunkx + k, chunkz + l);
                        Chunk *cPtr = chunk.get();
//                      float biomeVal = Noise::fbm5(glm::vec2(cPtr->getworldX() + i, cPtr->getworldZ() + j) * (1.f / 10.f));
//                      std::cout << "Noise:" << biomeVal << std::endl;
                        toAdd.push_back(cPtr);
                        m_chunks[toKey(chunkx + k, chunkz + l)] = move(chunk);
                        if(hasChunkAt(chunkx + k, chunkz + l + 16)) {
                            auto &chunkNorth = m_chunks[toKey(chunkx + k, chunkz + l + 16)];
                            cPtr->linkNeighbor(chunkNorth, ZPOS);
                        }
                        if(hasChunkAt(chunkx + k, chunkz + l - 16)) {
                            auto &chunkSouth = m_chunks[toKey(chunkx + k, chunkz + l - 16)];
                            cPtr->linkNeighbor(chunkSouth, ZNEG);
                        }
                        if(hasChunkAt(chunkx + k + 16, chunkz + l)) {
                            auto &chunkEast = m_chunks[toKey(chunkx + k + 16, chunkz + l)];
                            cPtr->linkNeighbor(chunkEast, XPOS);
                        }
                        if(hasChunkAt(chunkx + k - 16, chunkz + l)) {
                            auto &chunkWest = m_chunks[toKey(chunkx + k - 16, chunkz + l)];
                            cPtr->linkNeighbor(chunkWest, XNEG);
                        }
                    }
                }
                m_generatingTerrain.insert(toKey(chunkx, chunkz));
                int r = rand();
                BlockTypeWorker *bw = new BlockTypeWorker(&chonksInit, &chonksInitmut, toAdd, chunkx, chunkz, r);
                QThreadPool::globalInstance()->start(bw);
            }

        }
    }
    chonksInitmut.lock();
    while (!chonksInit.empty()) {
        Chunk *c = chonksInit.front();
        VBOWorker *vw = new VBOWorker(&vboDatamut, &chonksData, c);
        QThreadPool::globalInstance()->start(vw);
        chonksInit.erase(chonksInit.begin());
    }
    chonksInitmut.unlock();

    vboDatamut.lock();
    while (!chonksData.empty()) {
        uPtr<VBOData>& vb = chonksData.front();
        vb->cPtr->populateVBOs(vb->idx, vb->posNorCol);
        vb->cPtr->populateTVBOs(vb->t_idx, vb->t_posNorCol);

        // Here we check if m_genertedTerrain should be updated based on the number of chunks with
        // VBOs set up in each 64 x 64 region, which is stored in m_generatedTerrainBuffer.
        int x = static_cast<int>(glm::floor(vb->cPtr->worldX / 64.f)) * 64;
        int z = static_cast<int>(glm::floor(vb->cPtr->worldZ / 64.f)) * 64;
        int64_t key = toKey(x, z);
        m_generatedTerrainBuffer[key]++;
        if (m_generatedTerrainBuffer[key] >= 16) {
            m_generatingTerrain.erase(key);
            m_generatedTerrain.insert(key);
        }
        chonksData.erase(chonksData.begin());
    }
    vboDatamut.unlock();
}

void Terrain::recreateChunk(int x, int z) {
    vboDatamut.lock();
    Chunk* c = getChunkAt(x, z).get();
    c->create();
    vboDatamut.unlock();

}
