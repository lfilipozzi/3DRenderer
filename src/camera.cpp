#include "../include/camera.h"
#include "../include/inputmanager.h"
#include <math.h>

# define PI           3.14159265358979323846f

// #define CAMERA_VERBOSE // Verbose mode for camera information

#ifdef CAMERA_VERBOSE
#include <iostream>
#endif // CAMERA_VERBOSE

//TODO: update this value based on the screen
const int Camera::SCREEN_SIZE = 1920;

const float Camera::MOUSE_SENSITIVITY = 5.0f;

const float Camera::MOVEMENT_VELOCITY = 0.05f;

const float Camera::WHEEL_SENSITIVITY = 24.0f;

const float Camera::MIN_DISTANCE = 5.0f;

const float Camera::MAX_DISTANCE = 250.0f;

Camera::Camera() :
    m_pitchTrack(-35.0f),
    m_yawTrack(0.0f),
    m_targetTrack(0.0f, 0.0f, 0.0f),
    m_pitchOffset(-0.4f),
    m_yawOffset(0.0f),
    m_targetOffset(0.0f, 0.0f, 0.0f),
    m_distanceFromTarget(10.0f) {
    updateCameraAxes();
}

Camera::Camera(float pitch, float yaw, QVector3D target) :
    m_pitchTrack(pitch),
    m_yawTrack(yaw),
    m_targetTrack(target),
    m_pitchOffset(-0.4f),
    m_yawOffset(0.0f),
    m_targetOffset(0.0f, 0.0f, 0.0f),
    m_distanceFromTarget(10.0f) {
    updateCameraAxes();
}

Camera::~Camera() {

}

const QVector3D Camera::worldUpAxis = QVector3D(0.0f, 0.0f, 1.0f);

void Camera::updateCameraAxes() {
    // Define camera yaw and pitch
    float yaw = m_yawTrack + m_yawOffset;
    float pitch = m_pitchTrack + m_pitchOffset;

    // Update the axes of the camera from its yaw and pitch
    m_frontAxis = QVector3D(cos(yaw) * cos(pitch),
                           -sin(yaw) * cos(pitch),
                            sin(pitch));
    m_rightAxis = QVector3D::crossProduct(m_frontAxis, worldUpAxis);
    m_upAxis = QVector3D::crossProduct(m_rightAxis, m_frontAxis);
}

QMatrix4x4 Camera::getRotMatrixVehicleToWorld() {
    QMatrix4x4 rotationMatrix;
    rotationMatrix.setToIdentity();
    
    rotationMatrix.rotate(-m_pitchTrack*180/PI,0,1,0);
    rotationMatrix.rotate(-m_yawTrack  *180/PI,0,0,1);
    return rotationMatrix;
}

QMatrix4x4 Camera::getRotMatrixWorldToVehicle() {
    QMatrix4x4 rotationMatrix;
    rotationMatrix.setToIdentity();
    rotationMatrix.rotate(m_yawTrack  *180/PI,0,0,1);
    rotationMatrix.rotate(m_pitchTrack*180/PI,0,1,0);
    return rotationMatrix;
}

QMatrix4x4 Camera::getViewMatrix() {
    QMatrix4x4 viewMatrix;

    updateCameraAxes();

    // Define the camera position from the target and its distance to the target
    /* The target is set to m_targetTrack to follow the vehicle and an 
     * additional vector is used to position the camera as desired. To avoid 
     * rotating around this point because of the vehicle yaw motion, this vector
     * is defined in the vehicle frame (only yaw and pitch, roll is neglected) 
     * and therefore must first be converted to the world frame.
     */
    QVector3D target = m_targetTrack 
        + getRotMatrixVehicleToWorld() * m_targetOffset;
    QVector3D cameraPosition = target - m_frontAxis * m_distanceFromTarget;

    // Create the view matrix
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(cameraPosition, target, m_upAxis);
    return viewMatrix;
}

void Camera::processMouseMovement() {
    if (InputManager::isButtonPressed(Qt::RightButton)) {
        float xOffset = InputManager::mouseDelta().x() * MOUSE_SENSITIVITY / SCREEN_SIZE;
        float yOffset = InputManager::mouseDelta().y() * MOUSE_SENSITIVITY / SCREEN_SIZE;

        m_yawOffset += xOffset;
        m_pitchOffset -= yOffset;

        // Constraint the pitch to prevent from flipping the screen
        if (m_pitchOffset > -0.1f) // 1.55 rad is 89 deg
            m_pitchOffset = -0.1f;
        if (m_pitchOffset < -1.55f)
            m_pitchOffset = -1.55f;

        // Make sure yaw is between 0 and 360
        m_yawOffset = fmod(m_yawOffset, 360.0f);

        #ifdef CAMERA_VERBOSE
        if (InputManager::mouseDelta().x() != 0 && InputManager::mouseDelta().y() != 0) {
            std::cout << "Delta mouse: x=" << InputManager::mouseDelta().x() <<
                      ", y=" << InputManager::mouseDelta().y() <<
                      ", yawOffset=" << m_yawOffset <<
                      ", pitchOffset" << m_pitchOffset << std::endl;
        }
        #endif //CAMERA_VERBOSE
    }

    // Process the scroll wheel
    m_distanceFromTarget -= InputManager::getWheelScroll() / WHEEL_SENSITIVITY;
    m_distanceFromTarget = std::max(MIN_DISTANCE, std::min(m_distanceFromTarget, MAX_DISTANCE));
}

void Camera::processKeyboard()
{
    /* Need to compute rotation matrix so that pressing this key will result in 
     * the same effect no matter the vehicle's yaw angle.
     */
    if (InputManager::isKeyPressed(Qt::Key_W)) {
        m_targetOffset += getRotMatrixWorldToVehicle() * m_frontAxis * MOVEMENT_VELOCITY;
        #ifdef CAMERA_VERBOSE
        std::cout << "W pressed: x=" << m_targetOffset.x() <<
                   ", y=" <<  m_targetOffset.y() <<
                   ", z=" <<  m_targetOffset.z() << std::endl;
        #endif // CAMERA_VERBOSE
    }
    if (InputManager::isKeyPressed(Qt::Key_S)) {
        m_targetOffset -= getRotMatrixWorldToVehicle() * m_frontAxis * MOVEMENT_VELOCITY;
        #ifdef CAMERA_VERBOSE
        std::cout << "S pressed: x=" << m_targetOffset.x() <<
                   ", y=" <<  m_targetOffset.y() <<
                   ", z=" <<  m_targetOffset.z() << std::endl;
        #endif // CAMERA_VERBOSE
    }
    if (InputManager::isKeyPressed(Qt::Key_A)) {
        m_targetOffset -= getRotMatrixWorldToVehicle() * m_rightAxis * MOVEMENT_VELOCITY;
        #ifdef CAMERA_VERBOSE
        std::cout << "A pressed: x=" << m_targetOffset.x() <<
                   ", y=" <<  m_targetOffset.y() <<
                   ", z=" <<  m_targetOffset.z() << std::endl;
        #endif // CAMERA_VERBOSE
    }
    if (InputManager::isKeyPressed(Qt::Key_D)) {
        m_targetOffset += getRotMatrixWorldToVehicle() * m_rightAxis * MOVEMENT_VELOCITY;
        
        #ifdef CAMERA_VERBOSE
        std::cout << "D pressed: x=" << m_targetOffset.x() <<
                   ", y=" <<  m_targetOffset.y() <<
                   ", z=" <<  m_targetOffset.z() << std::endl;
        #endif // CAMERA_VERBOSE
    }
    if (InputManager::isKeyPressed(Qt::Key_Q)) {
        m_targetOffset -= getRotMatrixWorldToVehicle() * m_upAxis * MOVEMENT_VELOCITY;
        #ifdef CAMERA_VERBOSE
        std::cout << "Q pressed: x=" << m_targetOffset.x() <<
                   ", y=" <<  m_targetOffset.y() <<
                   ", z=" <<  m_targetOffset.z() << std::endl;
        #endif // CAMERA_VERBOSE
    }
    if (InputManager::isKeyPressed(Qt::Key_E)) {
        m_targetOffset += getRotMatrixWorldToVehicle() * m_upAxis * MOVEMENT_VELOCITY;
        #ifdef CAMERA_VERBOSE
        std::cout << "E pressed: x=" << m_targetOffset.x() <<
                   ", y=" <<  m_targetOffset.y() <<
                   ", z=" <<  m_targetOffset.z() << std::endl;
        #endif // CAMERA_VERBOSE
    }
}

void Camera::trackObject(const Position & position) {
    m_targetTrack = QVector3D(position.x, position.y, position.z);
    m_yawTrack = -position.yaw;
    m_pitchTrack = -position.pitch;
}








