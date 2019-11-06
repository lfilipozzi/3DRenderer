#include "../include/openglwindow.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <QDebug>
#include "../include/inputmanager.h"
#include "../include/animationplayer.h"

#include <iostream>

OpenGLWindow::OpenGLWindow(SceneSelector *sceneSelector, int refreshRate,
                           QPair<int, int> openGLVersion, QScreen* screen)
    : QWindow(screen), 
    m_wasCameraOffset(false) {
    // Request OpenGL context
    QSurfaceFormat requestedFormat;
    requestedFormat.setDepthBufferSize(24);
    requestedFormat.setMajorVersion(openGLVersion.first);
    requestedFormat.setMinorVersion(openGLVersion.second);

    requestedFormat.setSamples(4);
    requestedFormat.setProfile(QSurfaceFormat::CoreProfile);

    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat);
    m_context->create();

    /*
     * The next if statement checks the version of OpenGL used. If it 
     * corresponds to the requested version, sceneScelector creates the scene 
     * with the corresponding OpenGL version. If the context version does not 
     * correspond, it tries to open OpenGL2.1. Finally, if OpenGL2.1 fails, the
     * program is aborted.
     */
    // Try to open an OpenGL context with the requested vesrion
    if(m_context->format().version() == requestedFormat.version()) {
        m_scene = sceneSelector->getScene(requestedFormat.version(), 
                                          false, refreshRate);
    }
    else {
        // If impossible to open the requested format, use OpenGL 2.1
        m_context->deleteLater();
        requestedFormat.setMajorVersion(2);
        requestedFormat.setMinorVersion(1);
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat);
        m_context->create();
        
        // If the current context is less than OpenGL 2.0, abort
        if (m_context->format().version().first < 2) {
            qDebug() << "Unable to get a valid version of OpenGL, aborting";
            exit(1);
        }
        
        // Try to open an OpenGL 2.1 context
        qDebug() << "Unable to get an OpenGL " << openGLVersion.first << "." << 
            openGLVersion.second << " context."; 
        qDebug() << "Version of the current OpenGL context is " << 
            m_context->format().version().first <<  "." << 
            m_context->format().version().second << ".";
        qDebug() << "Try using OpenGL 2.1/OpenGL ES context instead.";
        m_scene = sceneSelector->getScene(m_context->format().version(),
            m_context->format().renderableType() == QSurfaceFormat::OpenGLES,
            refreshRate);
    }

    setSurfaceType(OpenGLSurface);
    setFormat(m_context->format());
    resize(1200, 900);
    create();

    connect(this, SIGNAL(widthChanged(int)), this, SLOT(resizeGL()));
    connect(this, SIGNAL(heightChanged(int)), this, SLOT(resizeGL()));
    connect(m_context, SIGNAL(aboutToBeDestroyed()), this, SLOT(cleanup()), Qt::DirectConnection);

    initializeGL();
    resizeGL();

    m_timer = new QTimer(this);
    m_timer->setInterval(refreshRate);
    connect(m_timer, &QTimer::timeout, this, &OpenGLWindow::updateGL);
    m_timer->start();
}

OpenGLWindow::~OpenGLWindow() {
    m_context->deleteLater();
    delete(m_context);
    delete(m_scene);
    delete(m_timer);
}

void OpenGLWindow::setPlayer(AnimationPlayer *player) {
    m_player = player;
    QObject::connect(this, SIGNAL(unlockOffsetCamera()), 
                     m_player, SLOT(unlockCameraButton()));
    QObject::connect(this, SIGNAL(lockOffsetCamera()), 
                     m_player, SLOT(lockCameraButton()));
}

void OpenGLWindow::initializeGL() {
    m_context->makeCurrent(this);

    m_scene->initialize();
}

void OpenGLWindow::updateGL() {
    if(!isExposed())
        return;

    // Update the input
    InputManager::update();

    // Update the camera
    m_scene->updateCamera();

    // Draw the scene
    m_context->makeCurrent(this);
    m_scene->update();
    m_context->swapBuffers(this);

    // Update the slider of the player
    float timeBegin = m_scene->getTimestepBegin();
    float time = m_scene->getTimestep();
    float timeEnd = m_scene->getTimestepEnd();
    m_player->updateTimestepValue(time, timeBegin, timeEnd);
    
    // Emit a signal if the camera has been offset 
    bool isCameraOffset = m_scene->isCameraOffset();
    if ((m_wasCameraOffset == false) && (isCameraOffset == true)) {
        emit unlockOffsetCamera();
    }
    else if ((m_wasCameraOffset == true) && (isCameraOffset == false)) {
        emit lockOffsetCamera();
    }
    m_wasCameraOffset = isCameraOffset;
}

void OpenGLWindow::resizeGL() {
    m_context->makeCurrent(this);

    m_scene->resize(width(), height());
    updateGL();
}

void OpenGLWindow::cleanup() {
    m_context->makeCurrent(this);

    m_scene->cleanup();
}

void OpenGLWindow::keyPressEvent(QKeyEvent *event) {
  if (event->isAutoRepeat()) {
    event->ignore();
  }
  else {
    InputManager::registerKeyPress(event->key());
  }
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent *event) {
  if (event->isAutoRepeat()) {
    event->ignore();
  }
  else {
    InputManager::registerKeyRelease(event->key());
  }
}

void OpenGLWindow::mousePressEvent(QMouseEvent *event) {
  InputManager::registerMousePress(event->button());
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent *event) {
  InputManager::registerMouseRelease(event->button());
}

void OpenGLWindow::wheelEvent(QWheelEvent *event)
{
    InputManager::registerWheelScroll(event->delta());
}

// Slots
void OpenGLWindow::playPauseAnimation() {
    m_scene->playPauseAnimation();
    m_player->updatePlayPauseButton(m_scene->isPaused());
}

void OpenGLWindow::restartAnimation() {
    m_scene->restartAnimation();
}

void OpenGLWindow::goEndAnimation() {
    m_scene->goEndAnimation();
    m_player->updatePlayPauseButton(m_scene->isPaused());
}

void OpenGLWindow::toggleLoopAnimation() {
    m_scene->toggleLoopAnimation();
}

void OpenGLWindow::setTimestep(int sliderPosition) {
    float slider = sliderPosition / 1000.0f;
    m_scene->setTimestepFromSlider(slider);
}

void OpenGLWindow::setTimeRate(int sliderPosition) {
    float minRate = 0.01f;
    float maxRate = 1.99f;
    float rate = minRate + sliderPosition * (maxRate - minRate) / 200;
    m_scene->setTimeRate(rate);
}

void OpenGLWindow::resetCameraOffset() {
    m_scene->resetCameraOffset();
}
