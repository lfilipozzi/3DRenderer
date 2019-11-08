#ifndef TEXTURE_H
#define TEXTURE_H

#include <QString>
#include <QImage>
#include <QOpenGLTexture>
#include <memory>

/// Texture class
/**
 * @brief Texture class inherited from QOpenGLTexture.
 * @author Louis Filipozzi
 */
class Texture : public QOpenGLTexture {
public:
    enum Type {
        Diffuse, Reflection, Normal, Displacement, Cubemap
    };
    
    Texture(Type type, QImage & image, 
            QOpenGLTexture::MipMapGeneration genMipMaps = GenerateMipMaps) 
    : QOpenGLTexture(image, genMipMaps), m_type(type) {}
    
    Texture(Type type, QOpenGLTexture::Target target) 
    : QOpenGLTexture(target), m_type(type) {}

    Type getType() const {return m_type;}
    
private:
    /**
     * Texture type.
     */
    Type m_type;
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
     * @param type The texture type.
     * @param path The path to the texture file.
     */
    static Texture * loadTexture(QString name, Texture::Type type,
                                 QImage & image);
    
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
