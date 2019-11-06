#ifndef POSITION_H
#define POSITION_H

#include <QVector3D>

/// Structure defining the position of an object in the world frame.
/**
 * The position of an object is defined by its position in the cartesian frame 
 * of the world and by its Euler angle.
 */
struct Position {
    QVector3D linearPosition;
    float yaw;
    float pitch;
    float roll;

    Position(float posX, float posY, float posZ, 
             float yawFoo, float pitchFoo, float rollFoo) :
        linearPosition(QVector3D(posX, posY, posZ)),
        yaw(yawFoo),
        pitch(pitchFoo),
        roll(rollFoo) {
    }
};

#endif // POSITION_H
