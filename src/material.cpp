#include "../include/material.h"
#include "../include/texture.h"
#include <QDebug>


Material::Material(QString name, Texture * diffuse, Texture * normal) :
    m_name(name), 
    m_ambient(QVector3D(0.0f,0.0f,0.0f)), 
    m_diffuse(QVector3D(0.0f,0.0f,0.0f)), 
    m_specular(QVector3D(0.0f,0.0f,0.0f)), 
    m_shininess(0.0f), 
    m_alpha(1.0f),
    m_diffuseTexture(diffuse),
    m_normalTexture(normal) {
    setDefaultTexture();
}

             
void Material::setDiffuseTexture(Texture * diffuse) {
    if (diffuse == nullptr)
        setDefaultTexture();
    else
        m_diffuseTexture = diffuse;
}


void Material::setNormalTexture(Texture * normal) {
    if (normal == nullptr)
        setDefaultTexture();
    else
        m_normalTexture = normal;
}


void Material::setDefaultTexture() { 
    if (m_diffuseTexture == nullptr) {
        QImage image("asset/Texture/Default/diffuse.png");
        if (image.isNull())
            qCritical() << __FILE__ << __LINE__ << 
                "The image file to the default diffuse texture does not exist.";
        m_diffuseTexture = TextureManager::loadTexture(
            QString("DefaultDiffuseTexture"), Texture::Type::Diffuse, image
        );
    }
    if (m_normalTexture == nullptr) {
        QImage image("asset/Texture/Default/normal.png");
        if (image.isNull())
            qCritical() << __FILE__ << __LINE__ << 
                "The image file to the default normal texture does not exist.";
        m_normalTexture = TextureManager::loadTexture(
            QString("DefaultNormalTexture"), Texture::Type::Normal, image
        );
    }
}






