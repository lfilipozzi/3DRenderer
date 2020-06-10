#include "../include/vehicle.h"

#define NUMBER_FIELD_TO_LOAD 39
#define FORCE_SCALE 3000


// Overload operator*() of the VehiclePosition structure
VehiclePosition operator*(const VehiclePosition & lhs, const float & rhs) {
    return VehiclePosition(
        lhs.chassis * rhs,
        lhs.wheelFL * rhs, 
        lhs.wheelFR * rhs, 
        lhs.wheelRL * rhs, 
        lhs.wheelRR * rhs, 
        lhs.forceFL * rhs, 
        lhs.forceFR * rhs, 
        lhs.forceRL * rhs, 
        lhs.forceRR * rhs
    );
}

VehiclePosition operator*(const float & lhs, const VehiclePosition & rhs) {
    return rhs * lhs;
}



/***
 *         __      __  _     _      _                 
 *         \ \    / / | |   (_)    | |                
 *          \ \  / /__| |__  _  ___| | ___            
 *           \ \/ / _ \ '_ \| |/ __| |/ _ \           
 *            \  /  __/ | | | | (__| |  __/           
 *       _____ \/ \___|_| |_|_|\___|_|\___|           
 *      / ____|          | |           | | |          
 *     | |     ___  _ __ | |_ _ __ ___ | | | ___ _ __ 
 *     | |    / _ \| '_ \| __| '__/ _ \| | |/ _ \ '__|
 *     | |___| (_) | | | | |_| | | (_) | | |  __/ |   
 *      \_____\___/|_| |_|\__|_|  \___/|_|_|\___|_|   
 *                                                    
 *                                                    
 */

VehicleController::VehicleController(QString trajectory) {
        bool isFirstLine = true;
        QTextStream stream(&trajectory);
        while (!stream.atEnd()) {
            if (isFirstLine) {
                // Ignore first line
                stream.readLine();
                isFirstLine = false;
                continue;
            }
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
                
                VehiclePosition position;
                position.chassis = Position(
                    chaX, chaY, chaZ, chaYaw, chaPitch, chaRoll
                );
                position.wheelFL = Position(
                    wFLX, wFLY, wFLZ, wFLSteer+chaYaw, wFLRot, chaRoll-PI
                );
                position.wheelFR = Position(
                    wFRX, wFRY, wFRZ, wFRSteer+chaYaw, wFRRot, chaRoll
                );
                position.wheelRL = Position(
                    wRLX, wRLY, wRLZ, wRLSteer+chaYaw, wRLRot, chaRoll-PI
                );
                position.wheelRR = Position(
                    wRRX, wRRY, wRRZ, wRRSteer+chaYaw, wRRRot, chaRoll
                );
                position.forceFL = QVector3D(
                    wFLfx, wFLfy, wFLfz
                );
                position.forceFR = QVector3D(
                    wFRfx, wFRfy, wFRfz
                );
                position.forceRL = QVector3D(
                    wRLfx, wRLfy, wRLfz
                );
                position.forceRR = QVector3D(
                    wRRfx, wRRfy, wRRfz
                );
                
                // Insert the position to the trajectory
                m_trajectory[timestep] = position;
            }
            else {
                qCritical() << "Not enough field to load the vehicle."
                    << fields.size() << "fields present," 
                    << NUMBER_FIELD_TO_LOAD << "are required";
                break;
            }
        }
}


VehiclePosition VehicleController::getVehiclePosition(const float time) {
    // If before first timestep
    if (time <= m_trajectory.begin()->first) {
        return m_trajectory.begin()->second;
    }
    
    // Find the first element which occur before time-step
    Trajectory::iterator it = m_trajectory.lower_bound(time);
    
    // Linear interpolation
    // FIXME: Some glitches when interpolating with big sampling time.
    if (it != m_trajectory.end()) {
        // Can find the position in the trajectory
        float time0 = it->first;
        VehiclePosition position0 = it->second;
        if (++it != m_trajectory.end() && time0 != time) {
            // Can find a position at the next time-step
            float time1 = it->first;
            VehiclePosition position1 = it->second;
            float alpha = (time - time0) / (time1 - time0);
            return (position1 - position0) * alpha + position0;
        }
        else {
            // Cannot find a position at the next time-step
            return position0;
        }
    }
    else {
        return VehiclePosition();
//         // Return position at last timestep
//         return m_trajectory.end()->second;
    }
}



/***
 *         __      __  _     _      _           
 *         \ \    / / | |   (_)    | |          
 *          \ \  / /__| |__  _  ___| | ___      
 *           \ \/ / _ \ '_ \| |/ __| |/ _ \     
 *            \  /  __/ | | | | (__| |  __/     
 *       _____ \/ \___|_| |_|_|\___|_|\___|     
 *      / ____|               | |   (_)         
 *     | |  __ _ __ __ _ _ __ | |__  _  ___ ___ 
 *     | | |_ | '__/ _` | '_ \| '_ \| |/ __/ __|
 *     | |__| | | | (_| | |_) | | | | | (__\__ \
 *      \_____|_|  \__,_| .__/|_| |_|_|\___|___/
 *                      | |                     
 *                      |_|                     
 */

void VehicleGraphics::updateMatrices(const VehiclePosition vehiclePosition) {
    Position chassis = vehiclePosition.chassis;
    Position wheelFL = vehiclePosition.wheelFL;
    Position wheelFR = vehiclePosition.wheelFR;
    Position wheelRL = vehiclePosition.wheelRL;
    Position wheelRR = vehiclePosition.wheelRR;
    const QVector3D & forceFL = vehiclePosition.forceFL;
    const QVector3D & forceFR = vehiclePosition.forceFR;
    const QVector3D & forceRL = vehiclePosition.forceRL;
    const QVector3D & forceRR = vehiclePosition.forceRR;
    
    /* The wheel spin does not correspond to the pitch of the wheel as defined 
     * in Cardan angles. Instead, it must be the first transformation applied to
     * the wheel.
     */
    float wheelFLSpin = wheelFL.pitch; wheelFL.pitch = chassis.pitch;
    float wheelFRSpin = wheelFR.pitch; wheelFR.pitch = chassis.pitch;
    float wheelRLSpin = wheelRL.pitch; wheelRL.pitch = chassis.pitch;
    float wheelRRSpin = wheelRR.pitch; wheelRR.pitch = chassis.pitch;
    
    // Compute the model matrices
    /* We first want to apply rotation (R) to the model and then translation 
     * (T). Therefore, we must compute T * R and apply first translation, then 
     * rotation.
     */
    m_chassisMatrix = Position::toMatrix(chassis + m_offset);
    m_wheelFLMatrix = Position::toMatrix(wheelFL + m_offset);
    m_wheelFRMatrix = Position::toMatrix(wheelFR + m_offset);
    m_wheelRLMatrix = Position::toMatrix(wheelRL + m_offset);
    m_wheelRRMatrix = Position::toMatrix(wheelRR + m_offset);
    // Apply rotation for wheel spin
    m_wheelFLMatrix.rotate(wheelFLSpin*180/PI, 0,-1, 0);
    m_wheelFRMatrix.rotate(wheelFRSpin*180/PI, 0, 1, 0);
    m_wheelRLMatrix.rotate(wheelRLSpin*180/PI, 0,-1, 0);
    m_wheelRRMatrix.rotate(wheelRRSpin*180/PI, 0, 1, 0);
    
    // Compute model matrices to draw the force arrows
    m_forceFLMatrix = getForceModelMatrix(
        forceFL, wheelFL, chassis, QVector3D( 1, 0.5,0)
    );
    m_forceFRMatrix = getForceModelMatrix(
        forceFR, wheelFR, chassis, QVector3D( 1,-0.5,0)
    );
    m_forceRLMatrix = getForceModelMatrix(
        forceRL, wheelRL, chassis, QVector3D(-1.2, 0.5,0)
                       );
    m_forceRRMatrix = getForceModelMatrix(
        forceRR, wheelRR, chassis, QVector3D(-1.2,-0.5,0)
    );
}


QMatrix4x4 VehicleGraphics::getForceModelMatrix(const QVector3D & force,
                                                const Position & wheelPos,
                                                const Position & chassisPos,
                                                const QVector3D & offset) {
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(wheelPos.x, wheelPos.y, wheelPos.z);
    modelMatrix.rotate(chassisPos.yaw *180/PI,0,0,1);
    modelMatrix.translate(offset);
    modelMatrix.scale(force / FORCE_SCALE);
    return modelMatrix;
}


void VehicleGraphics::render(
    const CasterLight & light, const QMatrix4x4 & view, 
    const QMatrix4x4 & projection, 
    const std::array<QMatrix4x4,NUM_CASCADES> & lightSpace,
    const std::array<float,NUM_CASCADES+1> & cascades
) {
    if (p_wheelModel != nullptr) {
        p_wheelModel->setModelMatrix(m_wheelFLMatrix);
        p_wheelModel->render(light, view, projection, lightSpace, cascades);
        p_wheelModel->setModelMatrix(m_wheelFRMatrix);
        p_wheelModel->render(light, view, projection, lightSpace, cascades);
        p_wheelModel->setModelMatrix(m_wheelRLMatrix);
        p_wheelModel->render(light, view, projection, lightSpace, cascades);
        p_wheelModel->setModelMatrix(m_wheelRRMatrix);
        p_wheelModel->render(light, view, projection, lightSpace, cascades);
    }
    if (p_chassisModel != nullptr) {
        p_chassisModel->setModelMatrix(m_chassisMatrix);
        p_chassisModel->render(light, view, projection, lightSpace, cascades);
    }
    if (p_forceLine != nullptr && m_showTireForce) {
        p_forceLine->setModelMatrix(m_forceFLMatrix);
        p_forceLine->render(light, view, projection, lightSpace, cascades);
        p_forceLine->setModelMatrix(m_forceFRMatrix);
        p_forceLine->render(light, view, projection, lightSpace, cascades);
        p_forceLine->setModelMatrix(m_forceRLMatrix);
        p_forceLine->render(light, view, projection, lightSpace, cascades);
        p_forceLine->setModelMatrix(m_forceRRMatrix);
        p_forceLine->render(light, view, projection, lightSpace, cascades);
    }
}


void VehicleGraphics::renderShadow(const QMatrix4x4 & lightSpace) {
    if (p_wheelModel != nullptr) {
        p_wheelModel->setModelMatrix(m_wheelFLMatrix);
        p_wheelModel->renderShadow(lightSpace);
        p_wheelModel->setModelMatrix(m_wheelFRMatrix);
        p_wheelModel->renderShadow(lightSpace);
        p_wheelModel->setModelMatrix(m_wheelRLMatrix);
        p_wheelModel->renderShadow(lightSpace);
        p_wheelModel->setModelMatrix(m_wheelRRMatrix);
        p_wheelModel->renderShadow(lightSpace);
    }
    if (p_chassisModel != nullptr) {
        p_chassisModel->setModelMatrix(m_chassisMatrix);
        p_chassisModel->renderShadow(lightSpace);
    }
}



/***
 *     __      __  _     _      _       
 *     \ \    / / | |   (_)    | |      
 *      \ \  / /__| |__  _  ___| | ___  
 *       \ \/ / _ \ '_ \| |/ __| |/ _ \ 
 *        \  /  __/ | | | | (__| |  __/ 
 *      ___\/ \___|_| |_|_|\___|_|\___| 
 *     |  _ \      (_) |   | |          
 *     | |_) |_   _ _| | __| | ___ _ __ 
 *     |  _ <| | | | | |/ _` |/ _ \ '__|
 *     | |_) | |_| | | | (_| |  __/ |   
 *     |____/ \__,_|_|_|\__,_|\___|_|   
 *                                      
 *                                      
 */

#include <QtXml>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include "../include/object.h"
#include "../include/line.h"

bool VehicleBuilder::build() {
    // Get data
    if (!QFile::exists(m_file)) {
        qWarning() << "The file" << m_file << "does not exist.";
        return false;
    }
    
    QDomDocument domDoc;
    // Load XML file as raw data
    QFile file(m_file);
    if (!file.open(QIODevice::ReadOnly)) {
        // Error while loading file
        qWarning() << "Error while loading file" << m_file;
    }
    
    // Retrieve the XML schema
    QXmlSchema schema;
    QUrl schemaUrl = QUrl::fromLocalFile(":/xml/vehicle.xsd");
    if (!schema.load(schemaUrl)) {
        qDebug() << "Cannot load XSD schema. The XML file will not be parsed.";
        return false;
    }
    // The XSD resource file cannot be invalid
    if (!schema.isValid()) {
        qCritical() << "The  XML schema (.xsd) is invalid. The XML file will"
            "not be parsed.";
        return false;
    }

    // Validate the vehicle XML file
    QXmlSchemaValidator validator{schema};
    if (!validator.validate(&file, QUrl::fromLocalFile(file.fileName()))) {
        qCritical() << "The file" << m_file << "does not meet the XML "
        "schema definition. The XML will not be parsed.";
        return false;
    }
    
    // Set data into the QDomDocument before processing
    file.reset();
    domDoc.setContent(&file);
    file.close();
    
    // Get vehicle element
    QDomElement root = domDoc.documentElement();
    
    // Process chassis
    QDomElement elmt = root.firstChildElement();
    QString chassisObject = elmt.attribute("model", "");
    QString chassisTex = elmt.attribute("texture", "");
    
    // Process wheel
    elmt = elmt.nextSiblingElement();
    QString wheelObject = elmt.attribute("model", "");
    QString wheelTex = elmt.attribute("texture", "");
    
    // Process trajectory
    elmt = elmt.nextSiblingElement();
    QString trajectory = elmt.text();
    
    // Load the chassis model
    ABCObject * chassis = nullptr;
    Object::Loader chassisLoader(chassisObject,chassisTex);
    if (chassisLoader.build()) {
        chassis = ObjectManager::loadObject(
            chassisObject, chassisLoader.getObject()
        );
    }
    
    // Load the wheel model
    ABCObject * wheel = nullptr;
    Object::Loader wheelLoader(wheelObject,wheelTex);
    if (wheelLoader.build()) {
        wheel = ObjectManager::loadObject(
            wheelObject, wheelLoader.getObject()
        );
    }
    
    // Load line
    ABCObject * line = nullptr;
    line = ObjectManager::loadObject(
        "line", 
        std::make_unique<Line>(
            // Vertices
            QVector<QVector3D>({
                QVector3D(0, 0, 0), QVector3D(1, 0, 0), 
                QVector3D(0, 1, 0), QVector3D(0, 0, 1)}
            ),
            // Indices
            QVector<unsigned int>({0, 1, 0, 2, 0, 3}), 
            // Color
            QVector3D(0.0f, 0.0f, 1.0f)
        )
    );
    
    // Create the vehicle
    p_vehicle = std::make_unique<Vehicle>(chassis, wheel, line, trajectory);
    
    return true;
}


std::unique_ptr<Vehicle> VehicleBuilder::getVehicle() {
    return move(p_vehicle);
}





