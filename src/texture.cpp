#include "../include/texture.h"


/***
 *             _______           _                     
 *            |__   __|         | |                    
 *               | |  ___ __  __| |_  _   _  _ __  ___ 
 *               | | / _ \\ \/ /| __|| | | || '__|/ _ \
 *      __  __   | ||  __/ >  < | |_ | |_| || |  |  __/
 *     |  \/  |  |_| \___|/_/\_\ \__| \__,_||_|   \___|
 *     | \  / |  __ _  _ __    __ _   __ _   ___  _ __ 
 *     | |\/| | / _` || '_ \  / _` | / _` | / _ \| '__|
 *     | |  | || (_| || | | || (_| || (_| ||  __/| |   
 *     |_|  |_| \__,_||_| |_| \__,_| \__, | \___||_|   
 *                                    __/ |            
 *                                   |___/             
 */

// Instantiate static member variables
TextureManager::TexturesMapsContainer TextureManager::m_textures;


Texture * TextureManager::loadTexture(
    QString name, Texture::Type type, QImage & image
) {
    // Check if a texture with the same type has already been loaded
    TexturesMapsContainer::iterator searchType = m_textures.find(type);
    if (searchType != m_textures.end()) {
        // Check if a texture with the same name and type has been loaded
        TexturesMap::iterator searchTexture = m_textures[type].find(name);
        if (searchTexture != m_textures.at(type).end()) {
            // The texture has already been loaded
            return searchTexture->second.get();
        }
    }
    // The texture has not been loaded yet
    m_textures[type][name] = std::make_unique<Texture>(type, image);
    return m_textures[type][name].get();
}


Texture * TextureManager::getTexture(QString name, Texture::Type type) {
    TexturesMap::iterator it(m_textures[type].find(name));
    if (it != m_textures.at(type).end())
        return it->second.get();
    else
        return nullptr;
}


void TextureManager::cleanUp() {
    // Delete all textures
    // FIXME error message: Texture has not been destroyed. QOpenGLTexturePrivate::destroy() called without a current context.
    for (
        TexturesMapsContainer::iterator itType = m_textures.begin();
        itType != m_textures.end(); itType++
    ) {
        Texture::Type type = itType->first;
        for (
            TexturesMap::iterator itTextures = m_textures.at(type).begin(); 
            itTextures != m_textures.at(type).end(); itTextures++
        ) {
            itTextures->second->destroy();
        }
    }
}
