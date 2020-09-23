#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "chunk.h"
#include <QRunnable>
#include <QMutex>
#include <unordered_map>
#include "noise.h"
#include <iostream>
#include "lsystem.h"

class BlockTypeWorker : public QRunnable {

private:
    std::vector<Chunk*>* chonks;
    QMutex* mut;
    std::vector<Chunk*> toAdd;
    int x, z, seed;

public:
    BlockTypeWorker(std::vector<Chunk*>* chonks, QMutex* mut,
                    std::vector<Chunk*> toAdd, int x, int z, int seed);
    void run() override;
    Chunk* createBlockTypeData(Chunk *cPtr);
};


