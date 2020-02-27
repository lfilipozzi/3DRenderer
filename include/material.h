#ifndef MATERIAL_H
#define MATERIAL_H

#include <QString>
#include <QVector3D>
#include "texture.h"

/// Material class
/**
 * @brief This class defines a material applied to an object.
 * @details A material is defined by several properties such as the ambient, 
 * diffuse, and specular color; the shininess, and the alpha factors; the 
 * diffuse, normal, and bump textures.
 * @remark The bump texture is really a depth texture. Black pixels correspond 
 * to zero depth, while white pixels correspond to a depth of `heightScale'. The
 * value of `heightScale' can be modified with the method 
 * Material::setHeightScale.
 * @author Louis Filipozzi
 */
class Material {
public:
    /**
     * @brief Constructor of the material.
     * @remark If the texture is not provided, a default texture is set.
     * @param name The name of the material.
     * @param diffuse The diffuse texture.
     * @param normal The normal texture.
     * @param bump The bump (or displacement) texture.
     */
    Material(
        QString name, Texture * diffuse = nullptr, Texture * normal = nullptr,
        Texture * bump = nullptr
    );
    ~Material() {};
    
    void setAmbientColor(QVector3D color) {m_ambient = color;};
    void setDiffuseColor(QVector3D color) {m_diffuse = color;};
    void setSpecularColor(QVector3D color) {m_specular = color;};
    void setShininess(float shininess) {m_shininess = shininess;};
    void setAlpha(float alpha) {m_alpha = alpha;};
    void setHeightScale(float height) {m_heightScale = height;};
    void setDiffuseTexture(Texture * diffuse);
    void setNormalTexture(Texture * normal);
    void setBumpTexture(Texture * bump);
    
    QString getName() const {return m_name;};
    QVector3D getAmbientColor() const {return m_ambient;};
    QVector3D getDiffuseColor() const {return m_diffuse;};
    QVector3D getSpecularColor() const {return m_specular;};
    float getShininess() const {return m_shininess;};
    float getAlpha() const {return m_alpha;};
    float getHeightScale() const {return m_heightScale;};
    Texture * getDiffuseTexture() const {return m_diffuseTexture;};
    Texture * getNormalTexture() const {return m_normalTexture;};
    Texture * getBumpTexture() const {return m_bumpTexture;};
    
private:
    /**
     * @brief set the default texture of the material.
     */
    void setDefaultTexture();
    
private:
    QString m_name;
    QVector3D m_ambient;
    QVector3D m_diffuse;
    QVector3D m_specular;
    float m_shininess;
    float m_alpha;
    float m_heightScale;
    Texture * m_diffuseTexture;
    Texture * m_normalTexture;
    Texture * m_bumpTexture;
};

#endif // MATERIAL_H
