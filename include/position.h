#ifndef POSITION_H
#define POSITION_H

#include <QVector3D>
#include <QMatrix4x4>

#define PI 3.14159265358979323846f

/// Position of an object
/**
 * @brief Define a position (x, y, and z coordinates and yaw, pitch and roll
 * angles).
 * @author Louis Filipozzi
 */
struct Position {
    float x;
    float y;
    float z;
    float yaw;
    float pitch;
    float roll;

    Position(
        float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f, 
        float yawFoo = 0.0f, float pitchFoo = 0.0f, float rollFoo = 0.0f
    ) :
        x(posX), y(posY), z(posZ),
        yaw(yawFoo), pitch(pitchFoo), roll(rollFoo) {};
    
    /**
     * @brief Get the x, y, and z coordinates.
     */
    QVector3D getPoint() const {return QVector3D(x, y, z);};
    
    /**
     * @brief Get the yaw, pitch and roll angle.
     */
    QVector3D getAngle() const {return QVector3D(yaw, pitch, roll);};
    
    /**
     * @brief Compute the model matrix for a given position.
     * @param position The position of the model.
     * @return The model matrix.
     */
    static QMatrix4x4 toMatrix(Position const pos) {
        QMatrix4x4 modelMatrix;
        modelMatrix.setToIdentity();
        modelMatrix.translate(pos.x, pos.y, pos.z);
        modelMatrix.rotate(pos.yaw  *180/PI, 0, 0, 1);
        modelMatrix.rotate(pos.pitch*180/PI, 0, 1, 0);
        modelMatrix.rotate(pos.roll *180/PI, 1, 0, 0);
        return modelMatrix;
    }
    
    Position operator+(const Position & a) {
        x += a.x;
        y += a.y;
        z += a.z;
        yaw   += a.yaw;
        pitch += a.pitch;
        roll  += a.roll;
        return *this;
    }
};

// Position operator+(const Position & a, const Position & b) {
//     Position pos;
//     pos.x = a.x + b.x;
//     pos.y = a.y + b.y;
//     pos.z = a.z + b.z;
//     pos.yaw   = a.yaw   + b.yaw;
//     pos.pitch = a.pitch + b.pitch;
//     pos.roll  = a.roll  + b.roll;
//     return pos;
// }

#endif // POSITION_H
