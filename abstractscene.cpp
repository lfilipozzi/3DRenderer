#include "abstractscene.h"

AbstractScene::AbstractScene(int refreshRate) :
    m_timestepBegin(0.0f),
    m_timestepEnd(std::numeric_limits<float>::max()),
    m_refreshRate(refreshRate),
    m_timeRate(1.0f),
    m_frameRate(1 / static_cast<float>(m_refreshRate)),
    m_enableLoop(true),
    m_showGlobalFrame(false) {
    m_camera = Camera(0.0f, 0.0f,QVector3D(0.0f, 0.0f, 0.0f));
    m_timestep = 0.0f;
}

AbstractScene::~AbstractScene() {

}

void AbstractScene::updateTimestep() {
    // Update the timestep
    m_timestep += m_frameRate * m_timeRate;

    // Restart the animation if necessary
    if (m_enableLoop && m_timestep > m_timestepEnd)
        m_timestep = m_timestepBegin;

    // Make sure the timestep is between the min and max value
    m_timestep = std::max(m_timestepBegin, std::min(m_timestep, m_timestepEnd));
}

void AbstractScene::updateCamera() {
    // Update camera based of user inputs (this is done here rather than
    // in the scene class to prevent writing this code for every scene).

    // Handle translations
    m_camera.processKeyboard();

    // Handle rotations
    m_camera.processMouseMovement();
}

void AbstractScene::playPauseAnimation() {
    if (m_frameRate == 0.0f)
        m_frameRate = 1 / static_cast<float>(m_refreshRate);
    else
        m_frameRate = 0.0f;
}

void AbstractScene::restartAnimation() {
    m_timestep = m_timestepBegin;
}

void AbstractScene::goEndAnimation() {
    m_frameRate = 0.0f;
    m_timestep = m_timestepEnd;
}








