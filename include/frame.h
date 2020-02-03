#ifndef FRAME_H
#define FRAME_H

#include "line.h"

// 3D frame
/**
 * @brief This class is used to draw a Cartesian frame.
 * @author Louis Filipozzi
 */
class Frame {
public:
    Frame(QVector3D origin);
    ~Frame() {};
    
    /**
     * @brief Initialize the object.
     */
    void initialize();

    /**
     * @brief Update the animation.
     */
    void render(
        const CasterLight & light, const QMatrix4x4 & view,
        const QMatrix4x4 & projection, 
        const std::array<QMatrix4x4,NUM_CASCADES> & lightSpace,
        const std::array<float,NUM_CASCADES+1> & cascades
    );

    /**
     * @brief Cleanup the animation.
     */
    void cleanup();
    
    /**
     * @brief Set the model matrix.
     */
    void setModelMatrix(QMatrix4x4 const model);
    
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

#endif // FRAME_H
