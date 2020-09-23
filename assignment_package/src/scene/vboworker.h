#ifndef VBOWORKER_H
#define VBOWORKER_H
#include <QRunnable>
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "chunk.h"
#include "vbodata.h"
#include <QMutex>

class VBOWorker : public QRunnable {
private:
    QMutex* mut;
    std::vector<uPtr<VBOData>>* vboData;
    uPtr<VBOData> vbo;
    Chunk *c;

public:
    VBOWorker(QMutex* mut, std::vector<uPtr<VBOData>>* vboData, Chunk *c);
    void run() override;
};

#endif // VBOWORKER_H
