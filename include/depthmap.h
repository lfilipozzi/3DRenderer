#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include <QOpenGLFunctions_3_0>
#include "constants.h"

/// Depth map
/**
 * @brief Create a framebuffer object (FBO) for cascades shadow mapping.
 * @details Only one FBO is created and NUM_CASCADES textures are generated from
 * this unique FBO.
 * @author Louis Filipozzi
 */
class DepthMap {
public:
    /**
     * @brief Create a DepthMap. You must call create() with a valid OpenGL 
     * context before using. The width and height of the underlying framebuffer 
     * object has the supplied format.
     */
    DepthMap(unsigned int width = 1024, unsigned int height = 1024);
    
    /**
     * @brief Destroy the framebuffer object and free any allocated resources.
     */
    ~DepthMap();
    
    /**
     * @brief Switches rendering from the default, windowing system provided 
     * framebuffer to this framebuffer object. Returns true upon success, 
     * false otherwise.
     * @param cascadeIdx The cascade index.
     */
    void bind(unsigned int cacadeIdx);
    
    /**
     * @brief Switches rendering back to the default windowing system. This also
     * clear the color and depth buffer.
     */
    void release();
    
    /**
     * @brief Bind the shadow map to supplied texture unit.
     * @param unit Array texture unit to bind each shadow map texture.
     */
    void bindTexture(unsigned int unit[NUM_CASCADES]);
    
    /**
     * @brief Returns the id of the underlying OpenGL framebuffer objects.
     */
    unsigned int objectId() const {return m_FBOId;};
    
    /**
     * @brief Returns the texture ID of the attached texture of each cascade.
     */
    std::array<unsigned int,NUM_CASCADES> texture() const {
        std::array<unsigned int,NUM_CASCADES> id;
        for (unsigned int i = 0; i < NUM_CASCADES; i++)
            id[i] = m_textureId[i];
        return id;
    };
    
    /**
     * @brief Returns the size of the underlying framebuffer object in a pair
     */
    std::pair<unsigned int, unsigned int> size() const {
        return std::make_pair(c_width, c_height);
    };
    
private:
    /**
     * Store the OpenGL functions.
     */
    QOpenGLFunctions_3_0 * p_glFunctions;
    
    /**
     * Width of the framebuffer.
     */
    const unsigned int c_width;
    
    /**
     * Height of the framebuffer.
     */
    const unsigned int c_height;

    /**
     * OpenGL ID of the framebuffer.
      */
    unsigned int m_FBOId;
    
    /**
     * Texture ID storing the framebuffer depth buffer.
     */
    unsigned int m_textureId[NUM_CASCADES];
};




#endif // DEPTHMAP_H
