#ifndef LIGHT_H
#define LIGHT_H

#include <QVector4D>
#include <QVector3D>
#include <QMatrix4x4>
#include <array>
#include "camera.h"

#include "constants.h"

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
    virtual std::array<QMatrix4x4,NUM_CASCADES> getLightSpaceMatrix(
        const Camera & camera, std::array<float,NUM_CASCADES+1> cascades
    ) const = 0;
    
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
    
    QMatrix4x4 getViewMatrix() const {
        // Compute the light view matrix
        QMatrix4x4 lightView;
        lightView.lookAt(
            QVector3D(0.0f, 0.0f, 0.0f),    // (Dummy) light position
            m_direction.toVector3D(),       // Light direction
            QVector3D(0.0f, 0.0f, 1.0f)     // Light up axis
        );
        return lightView;
    }
    
    std::array<QMatrix4x4,NUM_CASCADES> getProjectionMatrix(
        const Camera & camera, std::array<float,NUM_CASCADES+1> cascades
    ) const;

    virtual std::array<QMatrix4x4,NUM_CASCADES> getLightSpaceMatrix(
        const Camera & camera, std::array<float,NUM_CASCADES+1> cascades
    ) const;
    
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
