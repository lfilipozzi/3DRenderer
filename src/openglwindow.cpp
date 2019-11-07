#include "../include/openglwindow.h"
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <QDebug>
#include "../include/inputmanager.h"
#include "../include/animationplayer.h"

OpenGLWindow::OpenGLWindow(unsigned int refreshRate, QScreen * screen)
    : QWindow(screen), 
    m_wasCameraOffset(false) {
    // Request OpenGL context
    QSurfaceFormat requestedFormat;
    requestedFormat.setDepthBufferSize(24);
    requestedFormat.setVersion(3,3);

    requestedFormat.setSamples(4);
    requestedFormat.setProfile(QSurfaceFormat::CoreProfile);

    p_context = new QOpenGLContext(this);
    p_context->setFormat(requestedFormat);
    p_context->create();
    
    // Try to open an OpenGL context with the requested version
    if(p_context->format().version() != requestedFormat.version()) {
        qDebug() << "Unable to open a supported OpenGL context." <<
            "The most recent OpenGL context that can be open is" <<
            p_context->format().version().first << "." << 
            p_context->format().version().second << 
            ". Exiting the program.";
            exit(1);
    }
    else {
        p_scene = std::make_unique<Scene>(refreshRate);
    }

    setSurfaceType(OpenGLSurface);
    setFormat(p_context->format());
    resize(1200, 900);
    create();

    connect(this, SIGNAL(widthChanged(int)), this, SLOT(resizeGL()));
    connect(this, SIGNAL(heightChanged(int)), this, SLOT(resizeGL()));
    connect(p_context, SIGNAL(aboutToBeDestroyed()), this, SLOT(cleanUpGL()), Qt::DirectConnection);

    initializeGL();
    resizeGL();

    p_timer = new QTimer(this);
    p_timer->setInterval(refreshRate);
    connect(p_timer, &QTimer::timeout, this, &OpenGLWindow::renderGL);
    p_timer->start();
}


OpenGLWindow::~OpenGLWindow() {
    p_context->deleteLater();
    delete(p_context);
    delete(p_timer);
}


void OpenGLWindow::initializeGL() {
    p_context->makeCurrent(this);
    p_scene->initialize();
}


void OpenGLWindow::renderGL() {
    if(!isExposed())
        return;

    // Update the input
    InputManager::update();

    // Update the camera
    p_scene->updateCamera();

    // Draw the scene
    p_context->makeCurrent(this);
    p_scene->render();
    p_context->swapBuffers(this);

    // Update the slider of the player
    float timeBegin = p_scene->getTimestepBegin();
    float time = p_scene->getTimestep();
    float timeEnd = p_scene->getTimestepEnd();
    p_player->updateTimestepValue(time, timeBegin, timeEnd);
    
    // Emit a signal if the camera has been offset 
    bool isCameraOffset = p_scene->isCameraOffset();
    if ((m_wasCameraOffset == false) && (isCameraOffset == true)) {
        emit unlockOffsetCamera();
    }
    else if ((m_wasCameraOffset == true) && (isCameraOffset == false)) {
        emit lockOffsetCamera();
    }
    m_wasCameraOffset = isCameraOffset;
}


void OpenGLWindow::resizeGL() {
    p_context->makeCurrent(this);
    p_scene->resize(width(), height());
    renderGL();
}


void OpenGLWindow::cleanUpGL() {
    p_context->makeCurrent(this);
    p_scene->cleanUp();
}


void OpenGLWindow::setPlayer(AnimationPlayer *player) {
    p_player = player;
    QObject::connect(this, SIGNAL(unlockOffsetCamera()), 
                     p_player, SLOT(unlockCameraButton()));
    QObject::connect(this, SIGNAL(lockOffsetCamera()), 
                     p_player, SLOT(lockCameraButton()));
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
    p_scene->playPauseAnimation();
    p_player->updatePlayPauseButton(p_scene->isPaused());
}


void OpenGLWindow::restartAnimation() {
    p_scene->restartAnimation();
}


void OpenGLWindow::goEndAnimation() {
    p_scene->goEndAnimation();
    p_player->updatePlayPauseButton(p_scene->isPaused());
}


void OpenGLWindow::toggleLoopAnimation() {
    p_scene->toggleLoopAnimation();
}


void OpenGLWindow::setTimestep(int sliderPosition) {
    float slider = sliderPosition / 1000.0f;
    p_scene->setTimestepFromSlider(slider);
}


void OpenGLWindow::setTimeRate(int sliderPosition) {
    float minRate = 0.01f;
    float maxRate = 1.99f;
    float rate = minRate + sliderPosition * (maxRate - minRate) / 200;
    p_scene->setTimeRate(rate);
}

void OpenGLWindow::resetCameraOffset() {
    p_scene->resetCameraOffset();
}
