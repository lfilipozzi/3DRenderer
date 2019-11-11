#ifndef FRAME_GL33_H
#define FRAME_GL33_H

#include "line.h"

// 3D frame
/**
 * This class is used to draw the XYZ frame in OpenGL 3.3.
 */
class Frame_GL33 {
public:
    Frame_GL33(QVector3D origin);
    ~Frame_GL33();
    
    /**
     * Initialize the object: e.g. create the buffers, attributes, lighting.
     */
    void initialize();

    /**
     * Update the animation.
     */
    void update(const CasterLight &light, const QMatrix4x4 view,
                const QMatrix4x4 projection, const QMatrix4x4 lightSpaceMatrix);

    /**
     * Cleanup the animation.
     */
    void cleanup();
    
private:
    /**
     * The x axis.
     */
    Line X_AXIS;
    
    /**
     * The y axis.
     */
    Line Y_AXIS;
    
    /**
     * The z axis.
     */
    Line Z_AXIS;
};

#endif // FRAME_GL33_H
