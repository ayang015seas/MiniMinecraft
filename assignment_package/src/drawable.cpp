#include "drawable.h"
#include <glm_includes.h>

Drawable::Drawable(OpenGLContext* context)
    : f_count(-1), f_bufIdx(), f_bufPosNorCol(), f_idxGenerated(false), f_posNorColGenerated(false),
      m_count(-1), m_bufIdx(), m_bufPos(), m_bufNor(), m_bufCol(), m_bufUV(),
      m_idxGenerated(false), m_posGenerated(false), m_norGenerated(false), m_colGenerated(false),
      m_bufUVGenerated(false), t_count(-1), t_bufIdx(),
      t_idxGenerated(false), t_posNorColGenerated(false),
      mp_context(context)
{}

Drawable::~Drawable()
{}


void Drawable::destroy()
{
    mp_context->glDeleteBuffers(1, &f_bufIdx);
    mp_context->glDeleteBuffers(1, &f_bufPosNorCol);
    mp_context->glDeleteBuffers(1, &m_bufPos);
    mp_context->glDeleteBuffers(1, &m_bufNor);
    mp_context->glDeleteBuffers(1, &m_bufCol);
    mp_context->glDeleteBuffers(1, &m_bufUV);
    mp_context->glDeleteBuffers(1, &t_bufIdx);
    mp_context->glDeleteBuffers(1, &t_bufPosNorCol);


    m_idxGenerated = m_posGenerated = m_norGenerated = m_colGenerated = m_bufUVGenerated = false;

    t_idxGenerated = t_posNorColGenerated = false;
    m_count = -1;
    t_count = -1;
    f_count = -1;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return m_count;
}


int Drawable::elemFCount()
{
    return f_count;
}


void Drawable::generateFIdx()
{
    f_idxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &f_bufIdx);
}

void Drawable::generateFPosNorCol()
{
    f_posNorColGenerated = true;
    mp_context->glGenBuffers(1, &f_bufPosNorCol);
}


void Drawable::generateIdx()
{
    m_idxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufIdx);
}

void Drawable::generatePosNorCol()
{
    m_posNorColGenerated = true;
    mp_context->glGenBuffers(1, &m_bufPosNorCol);
}

void Drawable::generatePos()
{
    m_posGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mp_context->glGenBuffers(1, &m_bufPos);
}

void Drawable::generateNor()
{
    m_norGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mp_context->glGenBuffers(1, &m_bufNor);
}

void Drawable::generateCol()
{
    m_colGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufCol);
}

void Drawable::generateUV()
{
    m_bufUVGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufUV);
}


// t functions
int Drawable::elemTCount()
{
    return t_count;
}

void Drawable::generateTIdx()
{
    t_idxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &t_bufIdx);
}

void Drawable::generateTPosNorCol()
{
    t_posNorColGenerated = true;
    mp_context->glGenBuffers(1, &t_bufPosNorCol);
}

// normal bool functions

bool Drawable::bindIdx()
{
    if(m_idxGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    }
    return m_idxGenerated;
}

bool Drawable::bindPosNorCol()
{
    if(m_posNorColGenerated) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosNorCol);
    }
    return m_posNorColGenerated;
}

bool Drawable::bindPos()
{
    if(m_posGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    }
    return m_posGenerated;
}

bool Drawable::bindNor()
{
    if(m_norGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    }
    return m_norGenerated;
}

bool Drawable::bindCol()
{
    if(m_colGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    }
    return m_colGenerated;
}

bool Drawable::bindUV()
{
    if(m_bufUVGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufUV);
    }
    return m_bufUVGenerated;
}


// t bool function

bool Drawable::bindTIdx()
{
    if(t_idxGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t_bufIdx);
    }
    return t_idxGenerated;
}

bool Drawable::bindTPosNorCol()
{
    if(t_posNorColGenerated) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, t_bufPosNorCol);
    }
    return t_posNorColGenerated;
}

// f bind functions

bool Drawable::bindFIdx()
{
    if(f_idxGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, f_bufIdx);
    }
    return f_idxGenerated;
}

bool Drawable::bindFPosNorCol()
{
    if(f_posNorColGenerated) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, f_bufPosNorCol);
    }
    return f_posNorColGenerated;
}
