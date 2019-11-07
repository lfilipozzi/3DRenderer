#include "../include/texture.h"


// Instantiate static member variables
TextureManager::TexturesMap TextureManager::m_textures;


TextureManager::TextureManager() {}


Texture * TextureManager::loadTexture(QString name, QImage & image) {
    m_textures[name] = std::make_unique<Texture>(name, image);
    return m_textures[name].get();
}


Texture * TextureManager::getTexture(QString name) {
    TexturesMap::iterator 
        it(m_textures.find(name));
    if(it != m_textures.end())
        return it->second.get();
    else
        return nullptr;
}


void TextureManager::cleanUp() {
    // Delete all textures
    for(TexturesMap::iterator it = m_textures.begin(); 
        it != m_textures.end(); it++)
        it->second->destroy();
}
