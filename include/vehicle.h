#ifndef VEHICLE_H
#define VEHICLE_H

#include "abstractobject.h"
#include "position.h"
#include <QFile>
#include <QMatrix4x4>

#include "line_gl33.h" // TODO remove

/**
 * @brief Contains the position of the vehicle (chassis, wheels, tire forces).
 */
struct VehiclePosition {
    Position chassis;
    Position wheelFL;
    Position wheelFR;
    Position wheelRL;
    Position wheelRR;
    QVector3D forceFL;
    QVector3D forceFR;
    QVector3D forceRL;
    QVector3D forceRR;
};



/// Vehicle controller
/**
 * @brief Control a vehicle. Define its position.
 * @author Louis Filipozzi
 */
class VehicleController {
public:
    /**
     * @brief Constructor of the vehicle
     * @param filePath The path to the file containing the simulation data.
     */
    VehicleController(const QString filePath);
    
    /**
     * @brief Return the position of the vehicle at the requested time-step.
     * @param timestep The time-step
     * @return The position of the chassis
     */
    VehiclePosition getVehiclePosition(const float timestep);

    /**
     * @brief Return the first time-step for which a vehicle position is 
     * defined.
     */
    float getFirstTimeStep() const {
        if (m_trajectory.begin() != m_trajectory.end()) 
            return m_trajectory.begin()->first;
        return 0.0f;
    }

    /**
     * @brief Return the final time-step for which a vehicle position is 
     * defined.
     */
    float getFinalTimeStep() const {
        if (m_trajectory.rbegin() != m_trajectory.rend())
            return m_trajectory.rbegin()->first;
        return 0.0f;
    }

private:
    /**
     * @brief Fill the vehicle position from the simulation result file.
     * @param filePath The path to the file containing the simulation data.
     */
    void setVehicleTrajectory(const QString filePath);
    
private:
    /**
     * @typedef A trajectory is a map of time-step to vehicle position.
     */
    typedef std::map<float, VehiclePosition> Trajectory;

    /**
     * @brief The time-step of the vehicle trajectory.
     */
    Trajectory m_trajectory;
};



/// Vehicle graphics
/**
 * @brief This class allow to easily render a vehicle.
 * @author Louis Filipozzi
 */
class VehicleGraphics {
public:
    VehicleGraphics(
        ABCObject * chassisModel, ABCObject * wheelModel, Line_GL33 * line
    ) : 
    p_chassisModel(chassisModel),
    p_wheelModel(wheelModel), 
    p_forceLine(line), 
    m_offset(Position(0.0f, 0.0f, 0.05f, 0.0f, 0.0f, 0.0f)),
    m_showTireForce(true) {};
    
    /**
     * @brief Update the model matrices.
     * @param vehiclePosition The vehicle position.
     */
    void updateMatrices(const VehiclePosition vehiclePosition);
    
    /**
     * @brief Draw the object.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    void render(const CasterLight & light, const QMatrix4x4 & view, 
                const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace);
    
    /**
     * @brief Draw the object when computing the framebuffer for shadow mapping.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    void renderShadow(const CasterLight & light, const QMatrix4x4 & view, 
                const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace);
    
    /**
     * @brief Render/hide tire forces.
     */
    void toggleTireForce() {m_showTireForce = !m_showTireForce;};
    
private:
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
    
private:
    /**
     * The 3D model of the chassis.
     */
    ABCObject * p_chassisModel;

    /**
     * The 3D model of the wheel.
     */
    ABCObject * p_wheelModel;
    
    /**
     * Object to draw 3D lines in the scene.
     */
    Line_GL33 * p_forceLine;
    
    /**
     * Offset used to better position the vehicle.
     */
    Position m_offset;
    
    /**
     * Show the tire frame of the vehicle.
     */
    bool m_showTireForce;
    
    QMatrix4x4 m_chassisMatrix;
    QMatrix4x4 m_wheelFLMatrix;
    QMatrix4x4 m_wheelFRMatrix;
    QMatrix4x4 m_wheelRLMatrix;
    QMatrix4x4 m_wheelRRMatrix;
    QMatrix4x4 m_forceFLMatrix;
    QMatrix4x4 m_forceFRMatrix;
    QMatrix4x4 m_forceRLMatrix;
    QMatrix4x4 m_forceRRMatrix;
};



/// Vehicle
/**
 * @brief Define a vehicle.
 * @author Louis Filipozzi
 * @details This class uses the component pattern to avoid strong coupling 
 * between the vehicle model and its rendering.
 */
class Vehicle {
public:
    Vehicle(
        ABCObject * chassisModel, ABCObject * wheelModel, Line_GL33 * line, 
        const QString filePath
    ) :
    m_graphics(chassisModel, wheelModel, line),
    m_controller(filePath) {};
    
    /**
     * @brief Return the position of the vehicle at the requested time-step.
     * @param timestep The time-step
     * @return The position of the chassis
     */
    Position getPosition(const float timestep) {
        return m_controller.getVehiclePosition(timestep).chassis;
    }

    /**
     * @brief Return the first time-step for which a vehicle position is 
     * defined.
     */
    float getFirstTimeStep() const {
        return m_controller.getFirstTimeStep();
    }

    /**
     * @brief Return the final time-step for which a vehicle position is 
     * defined.
     */
    float getFinalTimeStep() const {
        return m_controller.getFinalTimeStep();
    }
    
    /**
     * @brief Update the position of the vehicle.
     * @param timestep The time-step.
     */
    void updatePosition(float timestep) {
        VehiclePosition vehiclePosition = 
            m_controller.getVehiclePosition(timestep);
        m_graphics.updateMatrices(vehiclePosition);
    }
    
    /**
     * @brief Draw the vehicle.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    void render(
        const CasterLight & light, const QMatrix4x4 & view, 
        const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace
    ) {
        m_graphics.render(light, view, projection, lightSpace);
    };
    
    /**
     * @brief Draw the vehicle when computing the framebuffer for shadow 
     * mapping.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    void renderShadow(
        const CasterLight & light, const QMatrix4x4 & view, 
        const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace
    ) {
        m_graphics.renderShadow(light, view, projection, lightSpace);
    };
    
    /**
     * @brief Render/hide tire forces.
     */
    void toggleTireForce() {m_graphics.toggleTireForce();};
    
private:
    /**
     * Component used to draw the vehicle.
     */
    VehicleGraphics m_graphics;
    
    /**
     * Component used to control the vehicle.
     */
    VehicleController m_controller;
};

#endif // VEHICLE_H
