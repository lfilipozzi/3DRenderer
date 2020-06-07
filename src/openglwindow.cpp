#include "../include/openglwindow.h"
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <QDebug>
#include "../include/inputmanager.h"
#include "../include/animationplayer.h"
#include <QSignalBlocker>
#include "../include/videorecorder.h"
#include "../include/constants.h"

OpenGLWindow::OpenGLWindow(
    unsigned int refreshRate, QString envFile, 
    std::vector<QString> vehList, QScreen * screen
) : QWindow(screen), 
    m_wasCameraOffset(false) {
    // Request OpenGL context
    QSurfaceFormat requestedFormat;
    requestedFormat.setDepthBufferSize(24);
    requestedFormat.setVersion(3,3);
//     requestedFormat.setVersion(4,5);

    requestedFormat.setSamples(4);
    requestedFormat.setProfile(QSurfaceFormat::CoreProfile);

    p_context = new QOpenGLContext(this);
    p_context->setFormat(requestedFormat);
    p_context->create();
    
    // Try to open an OpenGL context with the requested version
    if (p_context->format().version() != requestedFormat.version()) {
        qDebug() << "Unable to open a supported OpenGL context." <<
            "The most recent OpenGL context that can be open is" <<
            p_context->format().version().first << "." << 
            p_context->format().version().second << 
            ". Exiting the program.";
            exit(1);
    }
    else {
        p_scene = std::make_unique<Scene>(refreshRate, envFile, vehList);
        QString openGLAPI;
        if (p_context->isOpenGLES()) 
            openGLAPI = QString("OpenGL ES");
        else
            openGLAPI = QString("OpenGL");
        qInfo() << "Using " << openGLAPI << 
            p_context->format().version().first << "." << 
            p_context->format().version().second;
    }

    setSurfaceType(OpenGLSurface);
    setFormat(p_context->format());
    resize(1200, 900);
    create();

    connect(this, SIGNAL(widthChanged(int)), this, SLOT(resizeGL()));
    connect(this, SIGNAL(heightChanged(int)), this, SLOT(resizeGL()));
    connect(
        p_context, SIGNAL(aboutToBeDestroyed()), 
            this, SLOT(cleanUpGL()), Qt::DirectConnection
    );

    initializeGL();
    resizeGL();

    p_timer = new QTimer(this);
    p_timer->setInterval(refreshRate);
    connect(p_timer, &QTimer::timeout, this, &OpenGLWindow::updateGL);
    p_timer->start();
}


OpenGLWindow::~OpenGLWindow() {
    p_context->deleteLater();
}


void OpenGLWindow::initializeGL() {
    // Make the OpenGL context current
    p_context->makeCurrent(this);
    
    // Get pointer to OpenGL functions
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qCritical() << __FILE__ << __LINE__ <<
            "Requires a valid current OpenGL context. \n" <<
            "Unable to draw the object.";
        exit(1);
    }
    p_glFunctions = context->functions();
    if (!p_glFunctions) {
        qCritical() << __FILE__ << __LINE__ <<
            "Could not obtain required OpenGL context version";
        exit(1);
    }
    
    // Create the frame buffer and texture for shadow mapping
    p_depthMap = std::make_unique<DepthMap>(1024,1024);
    
    p_glFunctions->glEnable(GL_DEPTH_TEST);
    p_glFunctions->glEnable(GL_BLEND);
    p_glFunctions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize the scene
    p_scene->initialize();
}


void OpenGLWindow::renderGL() {
    // Draw the scene
    p_context->makeCurrent(this);
    p_scene->update();
    // Generate the shadow map
    for (unsigned int i = 0; i < NUM_CASCADES; i++) {
        if (p_depthMap != nullptr)
            p_depthMap->bind(i);
        p_scene->renderShadow(i);
    }
    // Render the scene
    if (p_depthMap != nullptr) {
        p_depthMap->release();
        p_depthMap->bindTexture(SHADOW_TEXTURE_UNITS);
    }
    p_glFunctions->glViewport(0, 0, width(), height());
    p_scene->render();
    p_scene->updateTimestep();
    p_context->swapBuffers(this);
    
    // Print OpenGL errors (if any)
    printOpenGLError();

    // Update the slider of the player
    float timeMin = p_scene->getFirstTimestep();
    float timeMax = p_scene->getFinalTimestep();
    float time = p_scene->getTimestep();
    p_player->updateTimestepValue(time, timeMin, timeMax);
}


void OpenGLWindow::updateGL() {
    if(!isExposed())
        return;

    // Update the input
    InputManager::update();

    // Update and render the scene
    renderGL();
    
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


void OpenGLWindow::record(
    const int fps, const int width, const int height, const QString fileName
) {
    const int initWidth = this->width();
    const int initHeight = this->height();
    
    // Setup record mode (disable 
    VideoRecorder recorder = VideoRecorder(fps, width, height, fileName);
    
    // Disconnect signals to prevent from resizing the window and to 
    // disconnect the timer
    QSignalBlocker windowBlocker = QSignalBlocker(this);
    QSignalBlocker timerBlocker = QSignalBlocker(p_timer);
    
    // Resize
    setWidth(width);
    setHeight(height);
    resizeGL();
    
    // Update and render the scene
    float timeMin = p_scene->getFirstTimestep();
    float timeMax = p_scene->getFinalTimestep();
    float time = timeMin;
    while (time <= timeMax) {
        p_scene->setTimestep(time);
        renderGL();
        recorder.recordFrame();
        time += 1/static_cast<float>(fps);
    }
    
    // Resize to the original size
    setWidth(initWidth);
    setHeight(initHeight);
    resizeGL();
}


void OpenGLWindow::resizeGL() {
    p_context->makeCurrent(this);
    p_glFunctions->glViewport(0, 0, width(), height());
    p_scene->resize(width(), height());
}


void OpenGLWindow::cleanUpGL() {
    p_context->makeCurrent(this);
    p_scene->cleanUp();
}


void OpenGLWindow::printOpenGLError() {
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qCritical() << __FILE__ << __LINE__ <<
            "Requires a valid current OpenGL context. \n" <<
            "Unable to draw the object.";
        exit(1);
    }
    QOpenGLFunctions * glFunctions;
    glFunctions = context->functions();
    if (!glFunctions) {
        qCritical() << __FILE__ << __LINE__ <<
            "Could not obtain required OpenGL context version";
        exit(1);
    }
    
    GLenum errorCode;
    QString error;
    while ((errorCode = glFunctions->glGetError()) != GL_NO_ERROR)
    {
        switch (errorCode)
        {
            case GL_INVALID_ENUM:
                error.append("INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
                error.append("INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
                error.append("INVALID_OPERATION");
                break;
            case GL_STACK_OVERFLOW:
                error.append("STACK_OVERFLOW");
                break;
            case GL_STACK_UNDERFLOW:
                error.append("STACK_UNDERFLOW");
                break;
            case GL_OUT_OF_MEMORY:
                error.append("OUT_OF_MEMORY");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error.append("INVALID_FRAMEBUFFER_OPERATION"); 
                break;
        }
    }
    if (!error.isEmpty())
        qWarning() << __FILE__ << __LINE__ << "OpenGL error:" << error;
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


void OpenGLWindow::playPauseAnimation() {
    if (p_scene != nullptr && p_player != nullptr) {
        p_scene->playPauseAnimation();
        p_player->updatePlayPauseButton(p_scene->isPaused());
    }
};


void OpenGLWindow::goEndAnimation() {
    if (p_scene != nullptr && p_player != nullptr) {
        p_scene->goEndAnimation();
        p_player->updatePlayPauseButton(p_scene->isPaused());
    }
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
