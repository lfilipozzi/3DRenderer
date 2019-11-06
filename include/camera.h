#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>
#include "position.h"

/// Camera to view the scene
/**
 * @brief Represent a camera to view a scene.
 *
 * The camera is defined by its position, target, and up vector. The camera 
 * usual behavior is to follow the vehicle. Therefore, it is more convenient 
 * to define the camera by its target, forward direction, distance from the 
 * target (adjustable by scrolling), and up axis. Moreover, WASDQE keys can be 
 * used to move the camera. Thus, the property m_targetOffset is used to define
 * an offset to the target and can move the camera. Finally, the mouse movement
 * will change the orientation of the camera, the properties m_yawOffset and 
 * m_pitchOffset are used to distinct between the angle offset defined from the 
 * user's input and the angle used to follow the vehicle.
 */
class Camera {
public:
    Camera();
    ~Camera();

    /**
     * Overloaded constructor of Camera
     * @param pitch The pitch angle of the camera.
     * @param yaw The yaw angle of the camera.
     * @param target The point at the center of the camera.
     */
    Camera(float pitch, float yaw, QVector3D target);

    /**
     * @brief Update the position of the camera to track an object: The camera 
     * is locked to the object (except for the roll).
     * @param position The position of the object to track.
     */
    void trackObject(Position const& position);

    /**
     * Define the view matrix.
     * @return The view matrix which corresponds to the camera settings.
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
     * Refocus the camera on the vehicle by eliminating the offset.
     */
    void resetTargetOffset() {m_targetOffset = QVector3D(0.0f, 0.0f, 0.0f);};
    
    /**
     * Indicate if the camera is offset.
     */
    bool isCameraOffset() {return m_targetOffset != QVector3D(0, 0, 0);};

private:
    /**
     * Return the rotation matrix used to go from the vehicle to the world
     * frame.
     */
    QMatrix4x4 getRotMatrixVehicleToWorld();
    
    /**
     * Return the rotation matrix used to go from the world to the vehicle.
     */
    QMatrix4x4 getRotMatrixWorldToVehicle();
    /**
     * Used to tune the sensitivity of the mouse for camera rotation.
     */
    static const float MOUSE_SENSITIVITY;

    /**
     * Used to tune the velocity of camera translations.
     */
    static const float MOVEMENT_VELOCITY;

    /**
     * Maximum screen size (used to normalize the mouse sensitivity).
     */
    static const int SCREEN_SIZE;

    /**
     * Used to tune the sensitivity of the wheel mouse.
     */
    static const float WHEEL_SENSITIVITY;

    /**
     * Saturate the distance of the camera to the target.
     */
    static const float MIN_DISTANCE;

    /**
     * Saturate the distance of the camera to the target.
     */
    static const float MAX_DISTANCE;

    /**
     * Up axis of the world.
     */
    static const QVector3D worldUpAxis;

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
     * Target offset of the camera according to the user input. This is defined 
     * in the vehicle frame and must be rotated, that's why it is using a
     * QVector4D instead of a QVector3D.
     */
    QVector3D m_targetOffset;

    /**
     * Variable used to define the distance between the camera and its target 
     * along the camera forward axis.
     * @brief The distance between the camera and its target.
     */
    float m_distanceFromTarget;

    /**
     * Front axis of the camera. This is computed from the camera pitch and yaw.
     */
    QVector3D m_frontAxis;

    /**
     * Right axis of the camera. This is computed from the camera pitch and yaw.
     */
    QVector3D m_rightAxis;

    /**
     * Up axis of the camera. This is computed from the camera pitch and yaw.
     */
    QVector3D m_upAxis;

    /**
     * Update the local axes of the camera from its yaw and pitch.
     */
    void updateCameraAxes();
};

#endif // CAMERA_H
