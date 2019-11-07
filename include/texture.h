#ifndef TEXTURE_H
#define TEXTURE_H

#include <QString>
#include <QOpenGLTexture>
#include <memory>

/// Texture class
/**
 * @brief Texture class inherited from QOpenGLTexture.
 * @author Louis Filipozzi
 */
class Texture : public QOpenGLTexture {
public:
    Texture(
        QString name, QString path, 
        QOpenGLTexture::MipMapGeneration genMipMaps = GenerateMipMaps
    ) : QOpenGLTexture(QImage(path).mirrored(), genMipMaps), m_name(name) {}
    // TODO Need to change the constructor to use QImage instead of QString so that we can check if the image is valid first
    
    Texture(
        QString name, QOpenGLTexture::Target target
    ) : QOpenGLTexture(target), m_name(name) {}

    QString getName() const {return m_name;}
    
private:
    /**
     * Name of the texture
     */
    QString m_name;
};



/// Texture manager
/**
 * @brief This class is used to manage the different textures used in the 
 * program and avoid to load twice the same texture.
 * @author Louis Filipozzi
 * @remark This class implements the singleton pattern to prevent using several
 * texture managers.
 * @remark The textures are stored using std::unique_ptr as the manager is 
 * assumed to be the only owner of all textures.
 */
class TextureManager {
public:
    /**
     * @brief Load and return the texture.
     * @param name The name of the texture.
     * @param path The path to the texture file.
     */
    static Texture * loadTexture(QString name, QString path);
    
    /**
     * @brief Get the texture.
     * @remark Return a null pointer if the texture has not been loaded yet.
     * @param path The path to the texture file.
     */
    static Texture * getTexture(QString name);
    
    /**
     * @brief Properly deallocate all textures.
     */
    static void cleanUp();
    
private:
    TextureManager();
    
    typedef std::map<QString, std::unique_ptr<Texture>> TexturesMap;
    /**
     * List of loaded textures.
     */
    static TexturesMap m_textures;
};

#endif // TEXTURE_H
