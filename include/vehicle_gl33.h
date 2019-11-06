#ifndef VEHICLE_GL33_H
#define VEHICLE_GL33_H

#include "abstractvehicle.h"
#include "model3d_gl33.h"
#include "line_gl33.h"

/// Vehicle class used to represent the vehicle and its state with OpenGL 3.3
/**
 * @brief The Vehicle_GL33 class
 */
class Vehicle_GL33 : public AbstractVehicle {
public:
    /**
     * Constructor of the vehicle
     * @param simDataPath The path to the file containing the simulation data.
     */
    Vehicle_GL33(QString simDataPath);
    
    ~Vehicle_GL33();

    /**
     * Initialize the object: e.g. load the model, create the buffers, 
     * attributes, lighting.
     */
    void initialize();

    /**
     * Update the animation. Model matrices must be updated before using this 
     * function.
     */
    void update(const CasterLight &light, const QMatrix4x4 view,
                const QMatrix4x4 projection, const QMatrix4x4 lightSpaceMatrix);
    
    /**
     * This function draws the scene, similarly to update(), but uses the shader
     * for shadow mapping and transform to the light space. This is used to 
     * obtain the shadow map.
     */
    void renderShadow(const QMatrix4x4 lightSpaceMatrix);

    /**
     * Cleanup the animation.
     */
    void cleanup();

private:
    /**
     * @brief The 3D model of the chassis.
     */
    Model3D_GL33 *m_chassis;

    /**
     * @brief The 3D model of the wheel.
     */
    Model3D_GL33 *m_wheel;
    
    /**
     * @brief Draw tire forces using GL_LINES.
     */
    Line_GL33 m_forceLine;
};

#endif // VEHICLE_GL33_H
