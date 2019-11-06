#ifndef LIGHT_H
#define LIGHT_H

#include <QVector4D>
#include <QVector3D>
#include <QMatrix4x4>

/// Lighting of the scene
/**
 * Define lights in the scene. Two type of lights are supported: caster lights 
 * and point lights.
 */
class AbstractLight {
public: 
    AbstractLight(QVector3D intensity = QVector3D(1.0f, 1.0f, 1.0f));
    ~AbstractLight();
    
    enum LightType {
        CasterLight//, PointLight
    };
    
    QVector3D getIntensity() const {return m_intensity;}
    
    virtual LightType getLightType() const = 0;
    
    virtual QMatrix4x4 getLightSpaceMatrix(QVector3D lightTarget) const = 0;
    
private:
    QVector3D m_intensity;
};

/// Caster light
class CasterLight : public AbstractLight {
public:
    CasterLight(QVector3D intensity = QVector3D(1.0f, 1.0f, 1.0f), 
                QVector4D direction = QVector4D(1.0f,-1.0f, -1.0f, 0.0f));
    ~CasterLight();
    
    virtual LightType getLightType() const {return LightType::CasterLight;}
    
    virtual QMatrix4x4 getLightSpaceMatrix(QVector3D lightTarget) const;
    
    QVector4D getDirection() const {return m_direction;}
    
private:
    QVector4D m_direction;
};

/// Point light
// class PointLight : public AbstractLight {
// public:
//     PointLight(QVector3D intensity, QVector3D position);
//     ~PointLight();
//     
//     virtual LightType getLightType() const {return LightType::PointLight;}
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
