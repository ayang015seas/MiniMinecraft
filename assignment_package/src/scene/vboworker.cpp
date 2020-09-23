#include "vboworker.h"

VBOWorker::VBOWorker(QMutex* mut, std::vector<uPtr<VBOData>>* vboData, Chunk *c)
    : mut(mut), vboData(vboData), vbo(mkU<VBOData>(c)), c(c) { }

void VBOWorker::run() {
    int i = 0;
    int t_i = 0;

    // y axis
    vbo->cPtr->createNSFaces(vbo->idx, vbo->posNorCol, i, vbo->t_idx, vbo->t_posNorCol, t_i);

    // x axis
    vbo->cPtr->createEWFaces(vbo->idx, vbo->posNorCol, i, vbo->t_idx, vbo->t_posNorCol, t_i);

    // y axis
    vbo->cPtr->createUDFaces(vbo->idx, vbo->posNorCol, i, vbo->t_idx, vbo->t_posNorCol, t_i);

    vbo->cPtr->setm_count(vbo->idx.size());
    vbo->cPtr->sett_count(vbo->t_idx.size());
    mut->lock();
    vboData->push_back(std::move(vbo));
    mut->unlock();
}
