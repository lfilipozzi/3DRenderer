#include "../include/light.h"

AbstractLight::AbstractLight(QVector3D intensity) : 
    m_intensity(intensity) {
}

AbstractLight::~AbstractLight() {
}

CasterLight::CasterLight(QVector3D intensity, QVector4D direction) : 
    AbstractLight(intensity), m_direction(direction) {
}

CasterLight::~CasterLight() {
}

QMatrix4x4 CasterLight::getLightSpaceMatrix(QVector3D lightTarget) const {
    // Compute light view matrix
    QMatrix4x4 lightView;
    QVector3D lightDirection = m_direction.toVector3D();
    QVector3D lightPosition = lightTarget - 3*lightDirection;
    lightView.lookAt(lightPosition, lightTarget, QVector3D(0.0f, 0.0f, 1.0f));
    // Compute light projection matrix
    QMatrix4x4 lightProjection;
    lightProjection.ortho(-4.0f, 4.0f, -4.0f, 4.0f, 1.0f, 10.0f);
    // Compute light projection view matrix
    QMatrix4x4 lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}


// PointLight::PointLight(QVector3D intensity, QVector3D position) : 
//     AbstractLight(intensity), m_position(position) {
// }
// 
// PointLight::~PointLight() {
// }
// 
// QMatrix4x4 PointLight::getLightSpaceMatrix(QVector3D lightTarget) const {
//     // Compute light view matrix
//     QMatrix4x4 lightView;
//     lightView.lookAt(m_position, lightTarget, QVector3D(0.0f, 0.0f, 1.0f));
//     // Compute light projection matrix
//     QMatrix4x4 lightProjection;
//     lightProjection.ortho(-4.0f, 4.0f, -4.0f, 4.0f, 1.0f, 10.0f);
//     // Compute light projection view matrix
//     QMatrix4x4 lightSpaceMatrix = lightProjection * lightView;
//     return lightSpaceMatrix;
// }

