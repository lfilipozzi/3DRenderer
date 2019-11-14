#include "../include/camera.h"
#include "../include/inputmanager.h"
#include <math.h>

# define PI           3.14159265358979323846f

const float Camera::c_mouseSensitivity = 5.0f / 1920;
const float Camera::c_velocity = 0.05f;
const float Camera::c_wheelSensitivity = 24.0f;
const float Camera::c_minDistance = 5.0f;
const float Camera::c_maxDistance = 250.0f;
const float Camera::c_FOV = 60.0f;
const QVector3D Camera::c_worldUpAxis = QVector3D(0.0f, 0.0f, 1.0f);


Camera::Camera(float pitch, float yaw, QVector3D target) :
    m_pitchTrack(pitch),
    m_yawTrack(yaw),
    m_targetTrack(target),
    m_pitchOffset(-0.4f),
    m_yawOffset(0.0f),
    m_targetOffset(0.0f, 0.0f, 0.0f),
    m_distanceFromTarget(10.0f) {
    updateAxes();
}


Camera::~Camera() {}


void Camera::updateAxes() {
    // Define camera yaw and pitch
    float yaw = m_yawTrack + m_yawOffset;
    float pitch = m_pitchTrack + m_pitchOffset;

    // Update the axes of the camera from its yaw and pitch
    m_frontAxis = QVector3D(cos(yaw) * cos(pitch),
                           -sin(yaw) * cos(pitch),
                            sin(pitch));
    m_rightAxis = QVector3D::crossProduct(m_frontAxis, c_worldUpAxis);
    m_upAxis = QVector3D::crossProduct(m_rightAxis, m_frontAxis);
}


QMatrix4x4 Camera::getTargetToWorldMatrix() const {
    QMatrix4x4 rotationMatrix;
    rotationMatrix.setToIdentity();
    
    rotationMatrix.rotate(-m_pitchTrack*180/PI,0,1,0);
    rotationMatrix.rotate(-m_yawTrack  *180/PI,0,0,1);
    return rotationMatrix;
}


QMatrix4x4 Camera::getWorldToTargetMatrix() const {
    return getTargetToWorldMatrix().inverted();
}


QMatrix4x4 Camera::getViewMatrix() const {
    QMatrix4x4 viewMatrix;

    // Define the camera position from the target and its distance to the target
    /* The target is set to m_targetTrack to follow the target and an 
     * additional vector is used to position the camera as desired. To avoid 
     * rotating around this point because of the target yaw motion, this vector
     * is defined in the target frame (only yaw and pitch, roll is neglected) 
     * and therefore must first be converted to the world frame.
     */
    QVector3D target;
    QVector3D cameraPosition;
    target = m_targetTrack  + getTargetToWorldMatrix() * m_targetOffset;
    cameraPosition = target - m_frontAxis * m_distanceFromTarget;

    // Create the view matrix
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(cameraPosition, target, m_upAxis);
    return viewMatrix;
}


QMatrix4x4 Camera::getProjectionMatrix() const {
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(c_FOV, m_aspect, .3f, 1000);
    return projectionMatrix;
}



void Camera::processMouseMovement() {
    if (InputManager::isButtonPressed(Qt::RightButton)) {
        float xOffset = InputManager::mouseDelta().x() * c_mouseSensitivity;
        float yOffset = InputManager::mouseDelta().y() * c_mouseSensitivity;

        m_yawOffset += xOffset;
        m_pitchOffset -= yOffset;

        // Constraint the pitch to prevent from flipping the screen
        if (m_pitchOffset > -0.1f) // 1.55 rad is 89 deg
            m_pitchOffset = -0.1f;
        if (m_pitchOffset < -1.55f)
            m_pitchOffset = -1.55f;

        // Make sure yaw is between 0 and 360
        m_yawOffset = fmod(m_yawOffset, 360.0f);
    }

    // Process the scroll wheel
    m_distanceFromTarget -= InputManager::getWheelScroll() / c_wheelSensitivity;
    m_distanceFromTarget = std::max(
        c_minDistance, std::min(m_distanceFromTarget, c_maxDistance)
    );
}


void Camera::processKeyboard()
{
    /* Need to compute rotation matrix so that pressing this key will result in 
     * the same effect no matter the target's yaw angle.
     */
    QMatrix4x4 rotation = getWorldToTargetMatrix();
    if (InputManager::isKeyPressed(Qt::Key_W)) {
        m_targetOffset += rotation * m_frontAxis * c_velocity;
    }
    if (InputManager::isKeyPressed(Qt::Key_S)) {
        m_targetOffset -= rotation * m_frontAxis * c_velocity;
    }
    if (InputManager::isKeyPressed(Qt::Key_A)) {
        m_targetOffset -= rotation * m_rightAxis * c_velocity;
    }
    if (InputManager::isKeyPressed(Qt::Key_D)) {
        m_targetOffset += rotation * m_rightAxis * c_velocity;
    }
    if (InputManager::isKeyPressed(Qt::Key_Q)) {
        m_targetOffset -= rotation * m_upAxis * c_velocity;
    }
    if (InputManager::isKeyPressed(Qt::Key_E)) {
        m_targetOffset += rotation * m_upAxis * c_velocity;
    }
}


void Camera::trackObject(const Position & position) {
    m_targetTrack = QVector3D(position.x, position.y, position.z);
    m_yawTrack = -position.yaw;
    m_pitchTrack = -position.pitch;
}








