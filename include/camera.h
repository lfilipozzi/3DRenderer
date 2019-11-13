#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>
#include "position.h"

/// Camera to view the scene
/**
 * @brief Represent a camera to view a scene.
 * @author Louis Filipozzi
 * @details The camera has two behavior, one is to follow an object in the scene,
 * the other one is being controlled by the user.
 */
class Camera {
public:
    /**
     * @brief Overloaded constructor of Camera
     * @param pitch The pitch angle of the camera.
     * @param yaw The yaw angle of the camera.
     * @param target The point at the center of the camera.
     */
    Camera(
        float pitch = -35.0f, float yaw = 0.0f, 
        QVector3D target = QVector3D(0.0f, 0.0f, 0.0f)
    );
    ~Camera();

    /**
     * @brief Update the position of the camera to track an object.
     * @details The camera is locked to the object: if the yaw or pitch angle of
     * the object change, it also changes the yaw and pitch angle of the camera 
     * (not the roll angle).
     * @param position The position of the object to track.
     */
    void trackObject(Position const & position);

    /**
     * @brief Return the view matrix of the camera.
     */
    QMatrix4x4 getViewMatrix();

    /**
     * @brief Process the displacement of the mouse and convert it into camera 
     * movement.
     */
    void processMouseMovement();

    /**
     * @brief Process the keyboard inputs and convert it into camera movement.
     */
    void processKeyboard();
    
    /**
     * @brief Reset the offset to the target to zero.
     */
    void resetTargetOffset() {m_targetOffset = QVector3D(0.0f, 0.0f, 0.0f);};
    
    /**
     * @brief Indicate if the camera is offset.
     */
    bool isCameraOffset() {return m_targetOffset != QVector3D(0, 0, 0);};

private:
    /**
     * @brief Return the rotation matrix used to go from the target to the 
     * world frame.
     */
    QMatrix4x4 getTargetToWorldMatrix();
    
    /**
     * @brief Return the rotation matrix used to go from the world to the 
     * target.
     */
    QMatrix4x4 getWorldToTargetMatrix();
    
    /**
     * @brief Update the local axes of the camera from its yaw and pitch.
     */
    void updateCameraAxes();
    
private:
    /**
     * Used to tune the sensitivity of the mouse for camera rotation.
     */
    static const float c_mouseSensitivity;

    /**
     * Used to tune the velocity of camera translations.
     */
    static const float c_velocity;

    /**
     * Used to tune the sensitivity of the wheel mouse.
     */
    static const float c_wheelSensitivity;

    /**
     * Minimum authorised distance of the camera to the target.
     */
    static const float c_minDistance;

    /**
     * Maximum authorized distance of the camera to the target.
     */
    static const float c_maxDistance;

    /**
     * Up axis of the world.
     */
    static const QVector3D c_worldUpAxis;

    /**
     * Camera pitch angle used to track an object in the scene.
     */
    float m_pitchTrack;

    /**
     * Camera yaw angl used to track an objet in the scene.
     */
    float m_yawTrack;

    /**
     * Target point of the camera to track an object in the scene.
     */
    QVector3D m_targetTrack;

    /**
     * Camera pitch angle offset according to the user mouse input.
     */
    float m_pitchOffset;

    /**
     * Camera yaw angle offset according to the user mouse input.
     */
    float m_yawOffset;

    /**
     * Target offset of the camera according to the user input defined in the
     * target reference frame and must be rotated.
     */
    QVector3D m_targetOffset;

    /**
     * The distance between the camera and its target.
     */
    float m_distanceFromTarget;

    /**
     * Front axis of the camera.
     */
    QVector3D m_frontAxis;

    /**
     * Right axis of the camera.
     */
    QVector3D m_rightAxis;

    /**
     * Up axis of the camera.
     */
    QVector3D m_upAxis;
};

#endif // CAMERA_H
