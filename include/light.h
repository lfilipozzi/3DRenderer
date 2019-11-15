#ifndef LIGHT_H
#define LIGHT_H

#include <QVector4D>
#include <QVector3D>
#include <QMatrix4x4>
#include "camera.h"

/// Abstract light
/**
 * @brief Define a light source
 * @author Louis Filipozzi
 */
class ABCLight {
public: 
    ABCLight(QVector3D intensity = QVector3D(1.0f, 1.0f, 1.0f)) 
    : m_intensity(intensity) {};
    virtual ~ABCLight() {};
    
    /**
     * @brief Return the light direction.
     */
    QVector3D getIntensity() const {return m_intensity;}
    
    /**
     * @brief Return the lightSpace transformation matrix.
     * @param camera The camera used by the scene.
     * @param cascades Vector of float defining the different zone for cascaded
     * shadow mapping.
     */
    virtual QMatrix4x4 getLightSpaceMatrix(QVector3D lightTarget) const = 0;
//     virtual std::vector<QMatrix4x4> getLightSpaceMatrix(
//         const Camera & camera, std::vector<float> cascades
//     ) const = 0;
    
private:
    QVector3D m_intensity;
};



/// Caster light
/**
 * @brief Define a caster light source.
 * @author Louis Filipozzi
 */
class CasterLight : public ABCLight {
public:
    CasterLight(
        QVector3D intensity = QVector3D(1.0f, 1.0f, 1.0f), 
        QVector4D direction = QVector4D(1.0f,-1.0f, -1.0f, 0.0f)
    ) : ABCLight(intensity), m_direction(direction) {};
    ~CasterLight() {};
    
    
    virtual QMatrix4x4 getLightSpaceMatrix(QVector3D lightTarget) const;
    QMatrix4x4 getViewMatrix(QVector3D lightTarget) const {
        QMatrix4x4 lightView;
        QVector3D lightDirection = m_direction.toVector3D();
        QVector3D lightPosition = lightTarget - 3*lightDirection;
        lightView.lookAt(lightPosition, lightTarget, QVector3D(0.0f, 0.0f, 1.0f));
        return lightView;
    }
    QMatrix4x4 getProjectionMatrix() const {
        QMatrix4x4 lightProjection;
        lightProjection.ortho(-4.0f, 4.0f, -4.0f, 4.0f, 1.0f, 10.0f);
        return lightProjection;
    }

//     virtual std::vector<QMatrix4x4> getLightSpaceMatrix(
//         const Camera & camera, std::vector<float> cascades
//     ) const;
//     QMatrix4x4 getViewMatrix() const {
//         // Compute the light view matrix
//         QMatrix4x4 lightView;
//         lightView.lookAt(
//             QVector3D(0.0f, 0.0f, 0.0f),    // (Dummy) light position
//             m_direction.toVector3D(),       // Light direction
//             QVector3D(0.0f, 0.0f, 1.0f)     // Light up axis
//         );
//         return lightView;
//     }
//     std::vector<QMatrix4x4> getProjectionMatrix(
//         const Camera & camera, std::vector<float> cascades
//     ) const;
    
    QVector4D getDirection() const {return m_direction;};
    
private:
    QVector4D m_direction;
};



/// Point light
/**
 * @brief Define a point light source.
 * @author Louis Filipozzi
 */
// class PointLight : public ABCLight {
// public:
//     PointLight(QVector3D intensity, QVector3D position);
//     ~PointLight();
//     
//     virtual QMatrix4x4 getLightSpaceMatrix(QVector3D lightTarget) const;
//     
//     QVector3D getPosition() const {return m_position;}
//     
//     QVector3D setPosition(QVector3D position) {m_position = position;}
//     
// private:
//     QVector3D m_position;
// };




#endif // LIGHT_H
