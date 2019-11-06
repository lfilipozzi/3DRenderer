#include "../include/vehicle_gl33.h"

Vehicle_GL33::Vehicle_GL33(QString simDataPath) : AbstractVehicle(simDataPath), 
    m_forceLine(QVector<QVector3D>({
        QVector3D(0, 0, 0), QVector3D(1, 0, 0), 
        QVector3D(0, 1, 0), QVector3D(0, 0, 1)}),
        QVector<unsigned int>({0, 1, 0, 2, 0, 3}), 
        QVector3D(0.0f, 0.0f, 1.0f)) {
    // Set the 3D model for the rendering
    QString OpenGLChassisModel("asset/3DModels/Cars/MustangGT/mustangChassis.obj");
    QString OpenGLWheelModel("asset/3DModels/Cars/MustangGT/mustangWheel.obj");
    QString textureDir("asset/3DModels/Cars/MustangGT/");
    
    m_chassis = new Model3D_GL33(OpenGLChassisModel, textureDir);
    m_wheel = new Model3D_GL33(OpenGLWheelModel, textureDir);
}

Vehicle_GL33::~Vehicle_GL33() {
    delete(m_chassis);
    delete(m_wheel);
}

void Vehicle_GL33::initialize() {
    m_chassis->initialize();
    m_wheel->initialize();
    m_forceLine.initialize();
}

void Vehicle_GL33::update(const CasterLight &light, 
                          const QMatrix4x4 view, 
                          const QMatrix4x4 projection, 
                          const QMatrix4x4 lightSpaceMatrix) {
    m_chassis->setModelMatrix(m_chassisModelMatrix);
    m_chassis->update(light, view, projection, lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelFLModelMatrix);
    m_wheel->update(light, view, projection, lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelFRModelMatrix);
    m_wheel->update(light, view, projection, lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelRLModelMatrix);
    m_wheel->update(light, view, projection, lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelRRModelMatrix);
    m_wheel->update(light, view, projection, lightSpaceMatrix);
    if (m_showTireForce) {
        m_forceLine.setModelMatrix(m_forcesFLModelMatrix);
        m_forceLine.update(light, view, projection, lightSpaceMatrix);
        m_forceLine.setModelMatrix(m_forcesFRModelMatrix);
        m_forceLine.update(light, view, projection, lightSpaceMatrix);
        m_forceLine.setModelMatrix(m_forcesRLModelMatrix);
        m_forceLine.update(light, view, projection, lightSpaceMatrix);
        m_forceLine.setModelMatrix(m_forcesRRModelMatrix);
        m_forceLine.update(light, view, projection, lightSpaceMatrix);
    }
}

void Vehicle_GL33::renderShadow(const QMatrix4x4 lightSpaceMatrix) {
    m_chassis->setModelMatrix(m_chassisModelMatrix);
    m_chassis->renderShadow(lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelFLModelMatrix);
    m_wheel->renderShadow(lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelFRModelMatrix);
    m_wheel->renderShadow(lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelRLModelMatrix);
    m_wheel->renderShadow(lightSpaceMatrix);
    m_wheel->setModelMatrix(m_wheelRRModelMatrix);
    m_wheel->renderShadow(lightSpaceMatrix);
}

void Vehicle_GL33::cleanup() {
    m_chassis->cleanup();
    m_wheel->cleanup();
    m_forceLine.cleanup();
}




