#pragma once
#include <openglcontext.h>
#include <glm_includes.h>

//This defines a class which can be rendered by our shader program.
//Make any geometry a subclass of ShaderProgram::Drawable in order to render it with the ShaderProgram class.
class Drawable
{
protected:


    int f_count;     // The number of indices stored in bufIdx.
    GLuint f_bufIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)
    GLuint f_bufPosNorCol;   // A Vertex Buffer Object that we will use to store vertex positions (vec4s)

    bool f_idxGenerated; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool f_posNorColGenerated;// normals (vec4s), colors (vec4s), and texture coordinates (vec4s)

    int m_count;     // The number of indices stored in bufIdx.
    GLuint m_bufIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)
    GLuint m_bufPosNorCol;   // A Vertex Buffer Object that we will use to store vertex positions (vec4s)
                                // normals (vec4s), colors (vec4s), and texture coordinates (vec4s)
    GLuint m_bufPos; // A Vertex Buffer Object that we will use to store mesh vertices (vec4s)
    GLuint m_bufNor; // A Vertex Buffer Object that we will use to store mesh normals (vec4s)
    GLuint m_bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
                   // Instead, we use a uniform vec4 in the shader to set an overall color for the geometry
    GLuint m_bufUV; // Can be used to pass per-vertex color information to the shader, but is currently unused.

    bool m_idxGenerated; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool m_posNorColGenerated;
    bool m_posGenerated;
    bool m_norGenerated;
    bool m_colGenerated;
    bool m_bufUVGenerated;

    int t_count;     // The number of indices stored in bufIdx.
    GLuint t_bufIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)
    GLuint t_bufPosNorCol;   // A Vertex Buffer Object that we will use to store vertex positions (vec4s)
                                // normals (vec4s), colors (vec4s), and texture coordinates (vec4s)

    bool t_idxGenerated; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool t_posNorColGenerated;



    OpenGLContext* mp_context; // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                          // we need to pass our OpenGL context to the Drawable in order to call GL functions
                          // from within this class.


public:
    Drawable(OpenGLContext* mp_context);
    virtual ~Drawable();

    virtual void create() = 0; // To be implemented by subclasses. Populates the VBOs of the Drawable.
    void destroy(); // Frees the VBOs of the Drawable.

    // Getter functions for various GL data
    virtual GLenum drawMode();
    int elemCount();
    int elemTCount();
    int elemFCount();
    // flock renders
    void generateFIdx();
    void generateFPosNorCol();

    // Call these functions when you want to call glGenBuffers on the buffers stored in the Drawable
    // These will properly set the values of idxBound etc. which need to be checked in ShaderProgram::draw()
    void generateIdx();
    void generatePosNorCol();


    void generatePos();
    void generateNor();
    void generateCol();
    void generateUV();

    bool bindIdx();
    bool bindPosNorCol();

    bool bindPos();
    bool bindNor();
    bool bindCol();
    bool bindUV();

    // flock binding functions
    bool bindFIdx();
    bool bindFPosNorCol();

    // t functions
    void generateTIdx();
    void generateTPosNorCol();

    bool bindTIdx();
    bool bindTPosNorCol();
};
