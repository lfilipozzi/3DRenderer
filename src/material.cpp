#include "../include/material.h"
#include "../include/texture.h"


Material::Material(QString name, Texture * texture) :
    m_name(name), 
    m_ambient(QVector3D(0.0f,0.0f,0.0f)), 
    m_diffuse(QVector3D(0.0f,0.0f,0.0f)), 
    m_specular(QVector3D(0.0f,0.0f,0.0f)), 
    m_shininess(0.0f), 
    m_alpha(1.0f),
    m_texture(texture) {
    if(m_texture == nullptr){
        setDefaultTexture();
    }
}

             
void Material::setTexture(Texture * texture) {
    if(texture == nullptr)
        setDefaultTexture();
    else
        m_texture = texture;
}


void Material::setDefaultTexture() { 
    ///@todo set default texture   
//     TODO set default texture
//     m_texture = TextureManager::loadTexture(TODO path to default texture);
}
