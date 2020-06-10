#ifndef VEHICLE_H
#define VEHICLE_H

#include "abstractobject.h"
#include "position.h"
#include <QFile>
#include <QMatrix4x4>

/**
 * @brief Contains the position of the vehicle (chassis, wheels, tire forces).
 * @author Louis Filipozzi
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
    
    VehiclePosition () :
    chassis(Position()),
    wheelFL(Position()),
    wheelFR(Position()), 
    wheelRL(Position()),
    wheelRR(Position()),
    forceFL(QVector3D()),
    forceFR(QVector3D()),
    forceRL(QVector3D()),
    forceRR(QVector3D()) {};
    
    VehiclePosition(
        Position cha, Position wFL, Position wFR, Position wRL, Position wRR, 
        QVector3D fFL, QVector3D fFR, QVector3D fRL, QVector3D fRR
    ) : 
    chassis(cha),
    wheelFL(wFL),
    wheelFR(wFR), 
    wheelRL(wRL),
    wheelRR(wRR),
    forceFL(fFL),
    forceFR(fFR),
    forceRL(fRL),
    forceRR(fRR) {};
    
    void operator+=(const VehiclePosition & a) {
        chassis += a.chassis;
        wheelFL += a.wheelFL;
        wheelFR += a.wheelFR;
        wheelRL += a.wheelRL;
        wheelRR += a.wheelRR;
        forceFL += a.forceFL;
        forceFR += a.forceFR;
        forceRL += a.forceRL;
        forceRR += a.forceRR;
    };
    
    void operator-=(const VehiclePosition & a) {
        chassis -= a.chassis;
        wheelFL -= a.wheelFL;
        wheelFR -= a.wheelFR;
        wheelRL -= a.wheelRL;
        wheelRR -= a.wheelRR;
        forceFL -= a.forceFL;
        forceFR -= a.forceFR;
        forceRL -= a.forceRL;
        forceRR -= a.forceRR;
    };
    
    VehiclePosition operator+(const VehiclePosition & a) {
        VehiclePosition tmp(*this);
        tmp += a;
        return tmp;
    };
    
    VehiclePosition operator-(const VehiclePosition & a) {
        VehiclePosition tmp(*this);
        tmp -= a;
        return tmp;
    };
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
     * @param trajectory The data describing the trajectory.
     */
    VehicleController(const QString trajectory);
    
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
        ABCObject * chassisModel, ABCObject * wheelModel, ABCObject * line
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
     * @param lightSpace The view and projection matrices of the light (used for 
     * shadow mapping).
     * @param cascades Array containing the distance for cascade shadow mapping.
     */
    void render(
        const CasterLight & light, const QMatrix4x4 & view, 
        const QMatrix4x4 & projection, 
        const std::array<QMatrix4x4,NUM_CASCADES> & lightSpace,
        const std::array<float,NUM_CASCADES+1> & cascades
    );
    
    /**
     * @brief Draw the object when computing the framebuffer for shadow mapping.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    void renderShadow(const QMatrix4x4 & lightSpace);
    
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
    ABCObject * p_forceLine;
    
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
        ABCObject * chassisModel, ABCObject * wheelModel, ABCObject * line, 
        const QString trajectory
    ) :
    m_graphics(chassisModel, wheelModel, line),
    m_controller(trajectory) {};
    
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
        const QMatrix4x4 & projection, 
        const std::array<QMatrix4x4,NUM_CASCADES> & lightSpace,
        const std::array<float,NUM_CASCADES+1> & cascades
    ) {
        m_graphics.render(light, view, projection, lightSpace, cascades);
    };
    
    /**
     * @brief Draw the vehicle when computing the framebuffer for shadow 
     * mapping.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    void renderShadow(const QMatrix4x4 & lightSpace) {
        m_graphics.renderShadow(lightSpace);
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



/// Vehicle builder
/**
 * @brief Load a vehicle.
 * @author Louis Filipozzi
 */
class VehicleBuilder {
public:
    VehicleBuilder(QString file) : m_file(file), p_vehicle(nullptr) {};
    
    virtual bool build();
    virtual std::unique_ptr<Vehicle>  getVehicle();
    
private:
    
private:
    /**
     * The path to the XML file.
     */
    QString m_file;
    
    /**
     * Pointer to the vehicle.
     */
    std::unique_ptr<Vehicle> p_vehicle;
};

#endif // VEHICLE_H
