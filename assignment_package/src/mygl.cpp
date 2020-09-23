#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this), m_progSky(this),
      waterOverlay(this),
      skyBox(this),
      blockFrame(this),
      m_terrain(this),
      m_flock(this, &m_terrain),
      m_player(glm::vec3(48.f, 150.f, 48.f), m_terrain),
      lastFrame(QDateTime::currentMSecsSinceEpoch())
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible

    srand(time(NULL));
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}


void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // add texture function


    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);
    //    glClearColor(0.9f, 0.9f, 0.9f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    // Create and set up the sky shader
    m_progSky.create(":/glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_progLambert.initTexture();

    for (unsigned int i = 0; i < 4; i++) {
        float xDirection = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.f) - 1.f;
        float yDirection = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.f) - 1.f;
        xDirection = 48 + (xDirection * 40);
        yDirection = 48 + (yDirection * 40);
        this->flockList.push_back(mkU<Flock>(this, &m_terrain));
        this->flockList[i].get()->setup(xDirection, yDirection);
        this->flockList[i].get()->create();
    }

    waterOverlay.create();
    blockFrame.create();
    skyBox.create();
}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    m_terrain.tick(m_player.mcr_position);
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    long long currframe = QDateTime::currentMSecsSinceEpoch();
    this->w_time++;
    w_time = w_time % 31000;

    m_progLambert.setTime(w_time);

    m_player.tick(currframe - lastFrame, m_inputs);
    lastFrame = currframe;
    sendPlayerDataToGUI();
    // Updates the info in the secondary window displaying player data
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = m_player.mcr_camera.getView();
    glm::mat4 proj = m_player.mcr_camera.getProj();
    glm::mat4 viewProj = proj * view;
    m_progLambert.setViewProjMatrix(viewProj);
    m_progLambert.setViewMatrix(view);
    m_progFlat.setViewProjMatrix(viewProj);

    // Terrain
    renderTerrain();
    drawBlockFrame();

    // skybox
    m_progSky.setViewProjMatrix(glm::inverse(viewProj));
    m_progSky.setEye(m_player.mcr_camera.mcr_position);
    m_progSky.setTime(w_time);
    m_progSky.setDimensions(width() * devicePixelRatio(), height() * devicePixelRatio());
    m_progSky.draw(skyBox);

    // World axes
    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);

    // bird stuff
    for (unsigned int i = 0; i < flockList.size(); i++) {
        flockList[i].get()->create();
        flockList[i].get()->worldTime = w_time;
        flockList[i].get()->run();
        m_progLambert.drawFlock(*(flockList[i].get()));
        float dist = glm::distance(glm::vec2(m_player.mcr_position.x, m_player.mcr_position.z), glm::vec2(flockList[i]->averagePos));

        if (dist >= 120.f) {
            float xDirection = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.f - 1.f;
            float yDirection = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.f - 1.f;
            float spawnDistance = 60.f;
            xDirection = m_player.mcr_position.x + (xDirection * spawnDistance);
            yDirection = m_player.mcr_position.z + (yDirection * spawnDistance);
//            std::cout << m_player.mcr_position.x << " XPOS" << std::endl;
//            std::cout << m_player.mcr_position.y << " YPOS" << std::endl;

            flockList.erase(flockList.begin() + i);

            uPtr<Flock> newFlock = mkU<Flock>(this, &m_terrain);
            newFlock.get()->setup(xDirection, yDirection);
            newFlock.get()->create();
            this->flockList.push_back(std::move(newFlock));
//            std::cout << "NEW FLOCK " << flockList.size() << std::endl;
        }
    }

}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_progLambert.textureHandle);
    glUseProgram(m_progLambert.prog);
    glUniform1i(m_progLambert.unifSampler, 0);
    m_progLambert.setTime(w_time);
    m_progLambert.setEye(m_player.mcr_camera.mcr_position);
    m_progLambert.setDimensions(width() * devicePixelRatio(), height() * devicePixelRatio());
    m_terrain.draw(m_player.mcr_position, &m_progLambert);
}


void MyGL::keyPressEvent(QKeyEvent *e) {
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = true;
    } else if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = true;
    } else if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = true;
    } else if (e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = true;
    } else if (e->key() == Qt::Key_F) {
        m_inputs.fPressed = true;
    }
}

void MyGL::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = false;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = false;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = false;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = false;
    } else if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = false;
    } else if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = false;
    } else if (e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = false;
    } else if (e->key() == Qt::Key_F) {
        m_inputs.fPressed = false;
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    m_inputs.mouseX = e->x();
    m_inputs.mouseY = e->y();
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton) {
        m_inputs.rightClick = true;
    } else if (e->button() == Qt::LeftButton) {
        m_inputs.leftClick = true;
    }
}

void MyGL::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton) {
        m_inputs.rightClick = false;
    } else if (e->button() == Qt::LeftButton) {
        m_inputs.leftClick = false;
    }
}

void MyGL::leaveEvent(QEvent *e) {
    QPoint p = QCursor::pos();
    if (p.x() >= width()) {
        QCursor::setPos(0, p.y());
    } else if (p.x() <= 0) {
        QCursor::setPos(width() - 1, p.y());
    }
}

void MyGL::drawBlockFrame()
{
    glm::ivec3 blockHit = glm::ivec3();
    float outDist = 0.f;
    int inter = -1;
    if (m_player.gridMarch(m_player.mcr_position + glm::vec3(0.f, 1.5f, 0.f),
                           m_player.mcr_camera.getForward() * 3.f,
                           m_terrain, &outDist, &blockHit, &inter)) {
        glm::vec3 pos = glm::vec3(blockHit[0], blockHit[1], blockHit[2]);
        glm::mat4 blockFrameTranslation = glm::translate(glm::mat4(), pos);
        m_progFlat.setModelMatrix(blockFrameTranslation);
        m_progFlat.draw(blockFrame);
    }
}
