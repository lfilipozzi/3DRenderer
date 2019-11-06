#ifndef ABSTRACTVEHICLE_H
#define ABSTRACTVEHICLE_H

#include "position.h"
#include <vector>
#include <QFile>
#include <QMatrix4x4>
#include "light.h"

/// Interface for the Vehicle
/**
 * The vehicle class should contains 3D models to animate the vehicle in the 
 * virtual scene and properties and methods to obtain its position from the 
 * simulation data. However, since the implentation of the model depends on the
 * version of OpenGL used, several classes are needed depending on the OpenGL 
 * version. This interface is designed to reduce the code repetition. It mainly 
 * deals with the simulation data, the animation with OpenGL is dealt by child 
 * classes.
 */
class AbstractVehicle {
public:
    /**
     * Constructor of the vehicle
     * @param filePath The path to the file containing the simulation data.
     */
    AbstractVehicle(QString filePath);
    
    virtual ~AbstractVehicle();
    
    /**
     * @brief Return the position of the chassis at a requested timestep. This 
     * is used by the scene to update the position of the camera.
     * @param timestep The timestep
     * @return The position.
     */
    Position getChassisPosition(const float timestep);
    
    /**
     * @brief Update the model matrices of the chassis and its wheels based on 
     * the vehicle trajectory from the simulation results. This is used to place
     * the vehicle in the scene.
     * @param timestep The timestep.
     */
    void updateModelMatrices(const float timestep);

    /**
     * Initialize the object: e.g. load the model, create the buffers, 
     * attributes, lighting.
     */
    virtual void initialize() = 0;

    /**
     * Update the animation.
     */
    virtual void update(const CasterLight &light, const QMatrix4x4 view,
                const QMatrix4x4 projection, const QMatrix4x4 lightSpaceMatrix) = 0;
                
    /**
     * This function draws the scene, similarly to update(), but uses the shader
     * for shadow mapping and transform to the light space. This is used to 
     * obtain the shadow map.
     */
    virtual void renderShadow(const QMatrix4x4 lightSpaceMatrix) = 0;

    /**
     * Cleanup the animation.
     */
    virtual void cleanup() = 0;

    /**
     * Accessor method for the initial time of the simulation.
     * @return The initial time.
     */
    float getInitialTime() const {return m_initialTime;}

    /**
     * Accessor method for the final time of the simulation.
     * @return The final time.
     */
    float getFinalTime() const {return m_finalTime;}
    
    /**
     * Toggle tire force in the animation.
     */
    void toggleTireForce() {m_showTireForce = !m_showTireForce;}

protected:
    /**
     * @brief Fill the vehicle position from the simulation result file.
     * @param filePath The path to the file containing the simulation data.
     */
    void setVehicleTrajectory(const QString filePath);
    
    /**
     * @brief Trajectory: liste of position containing the position and Euler 
     * angles.
     */
    typedef std::vector<Position> Trajectory;

    /**
     * @brief The timestep of the vehicle trakectory.
     */
    std::vector<float> m_timeTrajectory;

    /**
     * @brief The initial timestep of the simulation.
     */
    float m_initialTime;

    /**
     * @brief The last timestep of the simulation.
     */
    float m_finalTime;

    /**
     * @brief The chassis trajectory.
     */
    Trajectory m_chassisTrajectory;

    /**
     * @brief The FL wheel trajectory.
     */
    Trajectory m_wFLTrajectory;

    /**
     * @brief The FR wheel trajectory.
     */
    Trajectory m_wFRTrajectory;

    /**
     * @brief The RL wheel trajectory.
     */
    Trajectory m_wRLTrajectory;

    /**
     * @brief The RR wheel trajectory.
     */
    Trajectory m_wRRTrajectory;
    
    /**
     * @brief The force at the FL wheel over time.
     */
    std::vector<QVector3D> m_wFLForce;
    
    /**
     * @brief The force at the FR wheel over time.
     */
    std::vector<QVector3D> m_wFRForce;
    
    /**
     * @brief The force at the RL wheel over time.
     */
    std::vector<QVector3D> m_wRLForce;
    
    /**
     * @brief The force at the RR wheel over time.
     */
    std::vector<QVector3D> m_wRRForce;

    /**
     * @brief The model matrix of the chassis.
     */
    QMatrix4x4 m_chassisModelMatrix;

    /**
     * @brief The model matrix of the FL wheel.
     */
    QMatrix4x4 m_wheelFLModelMatrix;

    /**
     * @brief The model matrix of the FR wheel.
     */
    QMatrix4x4 m_wheelFRModelMatrix;

    /**
     * @brief The model matrix of the RL wheel.
     */
    QMatrix4x4 m_wheelRLModelMatrix;

    /**
     * @brief The model matrix of the RR wheel.
     */
    QMatrix4x4 m_wheelRRModelMatrix;
    
    /**
     * @brief The model matrix of to draw the FL tire forces.
     */
    QMatrix4x4 m_forcesFLModelMatrix;
    
    /**
     * @brief The model matrix of to draw the FR tire forces.
     */
    QMatrix4x4 m_forcesFRModelMatrix;
    
    /**
     * @brief The model matrix of to draw the RL tire forces.
     */
    QMatrix4x4 m_forcesRLModelMatrix;
    
    /**
     * @brief The model matrix of to draw the RR tire forces.
     */
    QMatrix4x4 m_forcesRRModelMatrix;

    /**
     * @brief Compute the model matrix from the position.
     * @param position The position of the model.
     * @return The model matrix
     */
    QMatrix4x4 Position2ModelMatrix(Position position);
    
    /**
     * @brief Compute the model matrix to draw the force.
     * @param force The force vector.
     * @param wheelPos The wheel position.
     * @param chassisPos The chassis Position.
     * @param offset The offset defining the origin of the force vector 
     * relatively to the wheel hub.
     */
    QMatrix4x4 getForceModelMatrix(const QVector3D &force,
                                   const Position &wheelPos, 
                                   const Position &chassisPos,
                                   const QVector3D &offset);
    
    /**
     * Vertical offset used to better position the vehicle (not the best way to
     * do that)
     */
    float m_verticalOffset;
    
    /**
     * Show the tire frame of the vehicle.
     */
    bool m_showTireForce;
};

#endif // ABSTRACTVEHICLE_H
