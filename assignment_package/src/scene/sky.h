#pragma once

#include "drawable.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Sky : public Drawable
{
public:
    Sky(OpenGLContext* context);
    virtual void create();
};
