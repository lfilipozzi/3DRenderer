#ifndef LIGHT_H
#define LIGHT_H

#include <QVector4D>
#include <QVector3D>
#include <QMatrix4x4>

/// Abstract light
/**
 * @brief Define a light source
 * @author Louis Filipozzi
 */
class ABCLight {
public: 
    ABCLight(QVector3D intensity = QVector3D(1.0f, 1.0f, 1.0f));
    virtual ~ABCLight() {};
    
    QVector3D getIntensity() const {return m_intensity;}
    
    virtual QMatrix4x4 getLightSpaceMatrix(QVector3D lightTarget) const = 0;
    
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
    CasterLight(QVector3D intensity = QVector3D(1.0f, 1.0f, 1.0f), 
                QVector4D direction = QVector4D(1.0f,-1.0f, -1.0f, 0.0f));
    ~CasterLight();
    
    virtual QMatrix4x4 getLightSpaceMatrix(QVector3D lightTarget) const;
    
    QVector4D getDirection() const {return m_direction;}
    
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
