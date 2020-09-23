#include "blocktypeworker.h"

int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

BlockTypeWorker::BlockTypeWorker(std::vector<Chunk*>* chonks,
                                 QMutex* mut,
                                 std::vector<Chunk*> toAdd,
                                 int x, int z, int seed)
    : chonks(chonks), mut(mut), toAdd(toAdd), x(x), z(z), seed(seed) {}

void BlockTypeWorker::run() {
    for (Chunk* c : toAdd) {
        createBlockTypeData(c);
    }
    if ((float)seed/RAND_MAX  < 0.3) {
        float r = (float)rand() / RAND_MAX;
        if (r < 0.25) {
            LSystem lsystem = LSystem(glm::vec2(0,0), glm::vec2(1,1), 3);
            lsystem.create(rand());
            lsystem.draw(x, z, toAdd);
        } else if (r < 0.5) {
            LSystem lsystem = LSystem(glm::vec2(63,0), glm::vec2(-1,1), 3);
            lsystem.create(rand());
            lsystem.draw(x, z, toAdd);
        } else if (r < 0.75) {
            LSystem lsystem = LSystem(glm::vec2(0,63), glm::vec2(1,-1), 3);
            lsystem.create(rand());
            lsystem.draw(x, z, toAdd);
        } else {
            LSystem lsystem = LSystem(glm::vec2(63,1), glm::vec2(-1,-1), 3);
            lsystem.create(rand());
            lsystem.draw(x, z, toAdd);
        }
    }
    mut->lock();
    for (Chunk* c : toAdd) {
        chonks->push_back(c);
    }
    mut->unlock();
}

Chunk* BlockTypeWorker::createBlockTypeData(Chunk *cPtr) {
//    float chonknVal = Noise::fbm3Alt(glm::vec2(cPtr->getworldX(), cPtr->getworldZ()) * (1.f / 70.f));
//    float chonkn = glm::smoothstep(0.f, 1.f, 1.f - chonknVal + 0.5f);
//    chonkn = chonkn - 0.23f;
//    chonkn *= 1.42f;
//    chonkn = glm::smoothstep(0.f, 1.f, chonkn);
//    float chonkgrassHeight = Noise::grassLands2(
//                glm::vec2(cPtr->getworldX(), cPtr->getworldZ()));
//    float chonkmountHeight = Noise::mountainSparse(glm::vec2(cPtr->getworldX(), cPtr->getworldZ()));
//    float chonkheight = 0.0f;
//    chonkheight = glm::mix(chonkgrassHeight, chonkmountHeight, chonkn);
    float chunkx = cPtr->getworldX();
    float chunkz = cPtr->getworldZ();
    for(int i = 0; i < 16; ++i) {
        for(int j = 0; j < 16; ++j) {
            float biomeVal = Noise::fbm5(glm::vec2(chunkx + i, chunkz + j) * (1.f / 10.f));
            biomeVal = glm::smoothstep(0.f, 1.f, biomeVal / 2.f);
            float nVal = Noise::fbm3Alt(glm::vec2(chunkx + i, chunkz + j) * (1.f / 70.f));
            float n = glm::smoothstep(0.f, 1.f, 1.f - nVal + 0.5f);
            n = n - 0.23f;
            n *= 1.42f;
            n = glm::smoothstep(0.f, 1.f, n);

            float extraNoise = Noise::fbm3(glm::vec2(chunkx + i, chunkz + j) * (1.f / 15.f));
            float extraNoise2 = Noise::fbm3(glm::vec2(chunkx+ i, chunkz + j));

            extraNoise = extraNoise - 1.0f;
            extraNoise = glm::abs(extraNoise);
            extraNoise *= 5.f;

            extraNoise2 = extraNoise2 - 1.0f;
            extraNoise2 = glm::abs(extraNoise2);
            extraNoise2 *= 5.f;

            float grassHeight = Noise::grassLands2(glm::vec2(chunkx + i, chunkz + j));
            float mountHeight = Noise::mountainSparse(glm::vec2(chunkx + i, chunkz + j));
            float height = 0.0f;

            height = glm::mix(grassHeight, mountHeight, n);

            if (biomeVal < 0.3f) {
                float mixingVal = biomeVal * 2.f;
                height = glm::floor(glm::mix(160.f, height, mixingVal));
                for (int k = 0; k <= 127; k++) {
                    cPtr->setBlockAt(i, k, j, STONE);
                }
                if (biomeVal > 0.27f) {
                    for (int k = 128; k < height - 1; k++) {
                        cPtr->setBlockAt(i, k, j, ENDSAND);
                    }
                    float limit = glm::min(255.f, height + 12.f * Noise::rand3(glm::vec2(chunkx + i, chunkz + j)) + 5.f);
                    for (int k = static_cast<int>(glm::floor(height)); k < limit; k++) {
                        cPtr->setBlockAt(i, k, j, OBSIDIAN);
                    }
                    if (height > cPtr->maxHeight) {
                        cPtr->maxHeight = height;
                    }
                } else {
                    height = glm::max(height - 20.f * glm::pow(Noise::WorleyNoise0(glm::vec2(chunkx + i, chunkz + j) / 20.f), 4.f), 129.f);
                    for (int k = 128; k < height - 1; k++) {
                        cPtr->setBlockAt(i, k, j, ENDSAND);
                    }
                    if (height > cPtr->maxHeight) {
                        cPtr->maxHeight = height;
                    }
                }
            } else if (biomeVal < 0.7f) {
                for (int k = 0; k < height - 1; k++) {
                    if (k < 128) {
                        cPtr->setBlockAt(i, k, j, STONE);
                    } else if (k < 150) {
                        cPtr->setBlockAt(i, k, j, DIRT);
                    } else {
                        cPtr->setBlockAt(i, k, j, STONE);
                    }
                }

                if (height > cPtr->maxHeight) {
                    cPtr->maxHeight = height;
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
            } else {
                glm::vec2 closestPoint = Noise::worleyClouds(glm::vec2(cPtr->getworldX() + i, cPtr->getworldZ() + j));
                float cloudheight = 40.f * Noise::rand2(closestPoint);


                float centernVal = Noise::fbm3Alt(closestPoint * (1.f / 70.f));
                float centern = glm::smoothstep(0.f, 1.f, 1.f - centernVal + 0.5f);
                centern = centern - 0.23f;
                centern *= 1.42f;
                centern = glm::smoothstep(0.f, 1.f, centern);
                float grassCenterHeight = Noise::grassLands2(closestPoint);
                float mountCenterHeight = Noise::mountainSparse(closestPoint);
                float centerHeight = 0.0f;
                centerHeight = glm::mix(grassCenterHeight, mountCenterHeight, centern);

                if (centerHeight > cPtr->maxHeight) {
                    cPtr->maxHeight = centerHeight;
                }

                for (int k = 0; k < 128; k++) {
                    cPtr->setBlockAt(i, k, j, STONE);
                }
                for (int k = 128; k <= height; k++) {
                    cPtr->setBlockAt(i, k, j, WOOD);
                }
                int col = static_cast<int>(7 * Noise::rand3(closestPoint));
                switch (col) {
                case 0 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, REDWOOL); break;
                case 1 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, PINKWOOL); break;
                case 2 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, GREENWOOL); break;
                case 3 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, YELLOWWOOL); break;
                case 4 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, BLUEWOOL); break;
                case 5 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, PURPLEWOOL); break;
                case 6 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, ORANGEWOOL); break;
                case 7 : cPtr->setBlockAt(i, glm::min(cloudheight + centerHeight + 5.f, 255.f), j, ORANGEWOOL); break;
                }

            }
        }
    }
    return cPtr;
}
