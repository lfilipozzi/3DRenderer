#ifndef TEXTURE_H
#define TEXTURE_H

#include <QString>
#include <QOpenGLTexture>

/// Personalized texture class
/**
 * Personalized class inherited forom QOpenGLTexture.
 */
class OpenGLTexture : public QOpenGLTexture {
public:
    enum TextureType {
        Diffuse//, Specular
    };
    
    OpenGLTexture(QString path, TextureType type = TextureType::Diffuse) :
    QOpenGLTexture(QImage(path).mirrored()),
    m_type(type),
    m_path(path) {
}

    QString path() const {return m_path;}
    
private:
    /**
     * Type of texture.
     */
    TextureType m_type;
    
    /**
     * Path to the image file of the texture
     */
    QString m_path;
};

#endif // TEXTURE_H
