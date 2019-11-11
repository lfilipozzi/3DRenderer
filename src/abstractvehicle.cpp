#include "../include/abstractvehicle.h"

#define NUMBER_FIELD_TO_LOAD 39
#define PI 3.14159265358979323846f
#define FORCE_SCALE 3000

//#define VEHICLE_VERBOSE // Set up verbose mode to check files loading

#ifdef VEHICLE_VERBOSE
#include <iostream>
#include<iomanip>
#endif // VEHICLE_VERBOSE

AbstractVehicle::AbstractVehicle(QString filePath) : 
    m_verticalOffset(0.05f),
    m_showTireForce(true)  {
    // Load the simulation data
    setVehicleTrajectory(filePath);
}

AbstractVehicle::~AbstractVehicle() {

}

Position AbstractVehicle::getChassisPosition(const float timestep) {
    // Find the first element which occur before timestep
    std::vector<float>::iterator timeIt = std::upper_bound(m_timeTrajectory.begin(),
                                                           m_timeTrajectory.end(),
                                                           timestep);
    unsigned long index = static_cast<unsigned long>(timeIt - m_timeTrajectory.begin() - 1);

    #ifdef VEHICLE_VERBOSE
    // Display header of the information to load
    std::cout << std::setw(10) << timestep << " | " <<
        std::setw(10) << m_chassisTrajectory.at(index).linearPosition.x() << " | " <<
        std::setw(10) << m_chassisTrajectory.at(index).linearPosition.y() << " | " <<
        std::setw(10) << m_chassisTrajectory.at(index).linearPosition.z() << " | " <<
        std::setw(13) << m_chassisTrajectory.at(index).yaw << " | " <<
        std::setw(13) << m_chassisTrajectory.at(index).pitch << " | " <<
        std::setw(13) << m_chassisTrajectory.at(index).roll << std::endl;
    #endif //VEHICLE_VERBOSE

    // No interpolation (only find the element hapeening just before timestep)
    return m_chassisTrajectory.at(index);
}

void AbstractVehicle::setVehicleTrajectory(const QString filePath) {
    QFile file(filePath);

    if (file.open(QFile::ReadOnly)) {
        #ifdef VEHICLE_VERBOSE
        // Display header of the information to load
        std::cout << "Simulation Data" << std::endl;
        std::cout << std::setw(10) << "Time" << " | " <<
                     std::setw(10) << "Chassis X" << " | " <<
                     std::setw(10) << "Chassis Y" << " | " <<
                     std::setw(10) << "Chassis Z" << " | " <<
                     std::setw(13) << " Chassis Yaw " << " | " <<
                     std::setw(13) << "Chassis Pitch" << " | " <<
                     std::setw(13) << "Chassis  Roll" << std::endl;
        #endif //VEHICLE_VERBOSE

        bool isFirstLine = true;
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            if (isFirstLine) {
                // Ignore first line
                stream.readLine();
                isFirstLine = false;
            }
            else {
                QString line = stream.readLine();
                QStringList fields = line.split(",");

                if (fields.size() == NUMBER_FIELD_TO_LOAD) {
                    // Save the trajectory
                    float timestep = fields.at(0).toFloat();
                    
                    float chaX     = fields.at(1).toFloat();
                    float chaY     = fields.at(2).toFloat();
                    float chaZ     = fields.at(3).toFloat();
                    float chaYaw   = fields.at(4).toFloat();
                    float chaPitch = fields.at(5).toFloat();
                    float chaRoll  = fields.at(6).toFloat();
                    
                    float wFLX     = fields.at(7).toFloat();
                    float wFLY     = fields.at(8).toFloat();
                    float wFLZ     = fields.at(9).toFloat();
                    float wFLRot   = fields.at(10).toFloat();
                    float wFLSteer = fields.at(11).toFloat();
                    float wFLfx    = fields.at(12).toFloat();
                    float wFLfy    = fields.at(13).toFloat();
                    float wFLfz    = fields.at(14).toFloat();
                    
                    float wFRX     = fields.at(15).toFloat();
                    float wFRY     = fields.at(16).toFloat();
                    float wFRZ     = fields.at(17).toFloat();
                    float wFRRot   = fields.at(18).toFloat();
                    float wFRSteer = fields.at(19).toFloat();
                    float wFRfx    = fields.at(20).toFloat();
                    float wFRfy    = fields.at(21).toFloat();
                    float wFRfz    = fields.at(22).toFloat();
                    
                    float wRLX     = fields.at(23).toFloat();
                    float wRLY     = fields.at(24).toFloat();
                    float wRLZ     = fields.at(25).toFloat();
                    float wRLRot   = fields.at(26).toFloat();
                    float wRLSteer = fields.at(27).toFloat();
                    float wRLfx    = fields.at(28).toFloat();
                    float wRLfy    = fields.at(29).toFloat();
                    float wRLfz    = fields.at(30).toFloat();
                    
                    float wRRX     = fields.at(31).toFloat();
                    float wRRY     = fields.at(32).toFloat();
                    float wRRZ     = fields.at(33).toFloat();
                    float wRRRot   = fields.at(34).toFloat();
                    float wRRSteer = fields.at(35).toFloat();
                    float wRRfx    = fields.at(36).toFloat();
                    float wRRfy    = fields.at(37).toFloat();
                    float wRRfz    = fields.at(38).toFloat();

                    m_timeTrajectory.push_back(timestep);
                    m_chassisTrajectory.emplace_back(chaX, chaY, chaZ, chaYaw,
                                                     chaPitch, chaRoll);
                    m_wFLTrajectory.emplace_back(wFLX, wFLY, wFLZ, 
                                                 wFLSteer+chaYaw, wFLRot, chaRoll-PI);
                    m_wFRTrajectory.emplace_back(wFRX, wFRY, wFRZ, 
                                                 wFRSteer+chaYaw, wFRRot, chaRoll);
                    m_wRLTrajectory.emplace_back(wRLX, wRLY, wRLZ, 
                                                 wRLSteer+chaYaw, wRLRot, chaRoll-PI);
                    m_wRRTrajectory.emplace_back(wRRX, wRRY, wRRZ, 
                                                 wRRSteer+chaYaw, wRRRot, chaRoll);
                    
                    m_wFLForce.emplace_back(wFLfx, wFLfy, wFLfz);
                    m_wFRForce.emplace_back(wFRfx, wFRfy, wFRfz);
                    m_wRLForce.emplace_back(wRLfx, wRLfy, wRLfz);
                    m_wRRForce.emplace_back(wRRfx, wRRfy, wRRfz);

                    #ifdef VEHICLE_VERBOSE
                    // Display the loaded information
                    std::cout << std::fixed <<
                        std::setprecision(5) << std::setw(10) << timestep << std::right << "|" <<
                        std::setprecision(5) << std::setw(10) << chaX << std::right << "|" <<
                        std::setprecision(5) << std::setw(10) << chaY << std::right << "|" <<
                        std::setprecision(5) << std::setw(10) << chaZ << std::right << "|" <<
                        std::setprecision(10) << std::setw(13) << chaYaw << std::right << "|" <<
                        std::setprecision(10) << std::setw(13) << chaPitch << std::right << "|" <<
                        std::setprecision(10) << std::setw(13) << chaRoll << std::right <<
                        std::endl;
                    #endif //VEHICLE_VERBOSE
                }
                else {
                    qCritical() << "Not enought field to load in file '" 
                        << file.fileName() << "'." << fields.size() 
                        << "fields present," << NUMBER_FIELD_TO_LOAD 
                        << "are required";
                    break;
                }
            }
        }
        m_initialTime = m_timeTrajectory.front();
        m_finalTime = m_timeTrajectory.back();
        file.close();
    }
    else {
        qCritical() << "Unable to load file '" << file.fileName() << "'.";
    }
}

void AbstractVehicle::updateModelMatrices(const float timestep) {
    // Find the first element which occur before timestep
    std::vector<float>::iterator timeIt = std::upper_bound(m_timeTrajectory.begin(),
                                                           m_timeTrajectory.end(),
                                                           timestep);
    unsigned long index = static_cast<unsigned long>(timeIt - m_timeTrajectory.begin() - 1);

    // No interpolation (only find the element happening just before timestep)
    Position chaPos = m_chassisTrajectory.at(index);
    Position wFLPos = m_wFLTrajectory.at(index);
    Position wFRPos = m_wFRTrajectory.at(index);
    Position wRLPos = m_wRLTrajectory.at(index);
    Position wRRPos = m_wRRTrajectory.at(index);
    QVector3D forceFL = m_wFLForce.at(index);
    QVector3D forceFR = m_wFRForce.at(index);
    QVector3D forceRL = m_wRLForce.at(index);
    QVector3D forceRR = m_wRRForce.at(index);
    
    /* The wheel spin does not correspond to the pitch of the wheel as defined 
     * in Cardan angles. Instead, it must be the first transformaton applied to
     * the wheel.
     */
    float wFLSpin = wFLPos.pitch; wFLPos.pitch = chaPos.pitch;
    float wFRSpin = wFRPos.pitch; wFRPos.pitch = chaPos.pitch;
    float wRLSpin = wRLPos.pitch; wRLPos.pitch = chaPos.pitch;
    float wRRSpin = wRRPos.pitch; wRRPos.pitch = chaPos.pitch;
    
    // Compute the model matrices
    /* We first want to apply rotation (R) to the model and then translation 
     * (T). Therefore, we must compute T * R and apply first translation, then 
     * rotation.
     */
    m_chassisModelMatrix = Position2ModelMatrix(chaPos);
    m_wheelFLModelMatrix = Position2ModelMatrix(wFLPos);
    m_wheelFRModelMatrix = Position2ModelMatrix(wFRPos);
    m_wheelRLModelMatrix = Position2ModelMatrix(wRLPos);
    m_wheelRRModelMatrix = Position2ModelMatrix(wRRPos);
    // Apply rotation for wheel spin
    m_wheelFLModelMatrix.rotate(wFLSpin*180/PI, 0,-1, 0);
    m_wheelFRModelMatrix.rotate(wFRSpin*180/PI, 0, 1, 0);
    m_wheelRLModelMatrix.rotate(wRLSpin*180/PI, 0,-1, 0);
    m_wheelRRModelMatrix.rotate(wRRSpin*180/PI, 0, 1, 0);
    
    // Compute model matrices to draw the force arrows
    m_forcesFLModelMatrix = getForceModelMatrix(forceFL, wFLPos, chaPos, 
                                                QVector3D( 1, 0.5,0));
    m_forcesFRModelMatrix = getForceModelMatrix(forceFR, wFRPos, chaPos, 
                                                QVector3D( 1,-0.5,0));
    m_forcesRLModelMatrix = getForceModelMatrix(forceRL, wRLPos, chaPos, 
                                                QVector3D(-1.2, 0.5,0));
    m_forcesRRModelMatrix = getForceModelMatrix(forceRR, wRRPos, chaPos, 
                                                QVector3D(-1.2,-0.5,0));
}

QMatrix4x4 AbstractVehicle::Position2ModelMatrix(Position position) {
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(position.x, position.y, position.z);
    modelMatrix.translate(0.0f, 0.0f, m_verticalOffset);
    modelMatrix.rotate(position.yaw  *180/PI,0,0,1);
    modelMatrix.rotate(position.pitch*180/PI,0,1,0);
    modelMatrix.rotate(position.roll *180/PI,1,0,0);
    return modelMatrix;
}

QMatrix4x4 AbstractVehicle::getForceModelMatrix(const QVector3D &force,
                                                const Position &wheelPos,
                                                const Position &chassisPos,
                                                const QVector3D &offset) {
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(wheelPos.x, wheelPos.y, wheelPos.z);
    modelMatrix.rotate(chassisPos.yaw *180/PI,0,0,1);
    modelMatrix.translate(offset);
    modelMatrix.scale(force / FORCE_SCALE);
    return modelMatrix;
}







