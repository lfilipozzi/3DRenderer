#ifndef MATERIALINFO_H
#define MATERIALINFO_H

#include <QString>
#include <QVector3D>
#include "opengltexture.h"

/// Material
/**
 * Contains information about how the material will look. This will be used by 
 * the Phong shading model.
 */
struct MaterialInfo {
    QString Name;

    /**
     * Ambient - used to model light that is reflecting around the scene. Light 
     * that has been reflected so many times that it appears to be constantly
     * emanating from all  directions.
     */
    QVector3D Ambient;
    
    /**
     * Diffuse - used to model light that is scattering in all directions 
     * equally off of a surface.
     */
    QVector3D Diffuse;
    
    /**
     * Specular - used to model the shininess of a surface and represents a 
     * mirror like reflection in a specific direction.
     */
    QVector3D Specular;
    
    /**
     * Shininess of the material.
     */
    float Shininess;
    
    /**
     * Non-transparency of the material (alpha value).
     */
    float Alpha;
    
    /**
     * Texture used by the material (only one texture is supported by material.
     */
    OpenGLTexture *Texture;
};

#endif // MATERIALINFO_H
