#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include <QOpenGLFunctions_3_0>

/// Depth map
/**
 * @brief Create a framebuffer object for shadow mapping.
 * @author Louis Filipozzi
 */
class DepthMap {
public:
    /**
     * @brief Create a DepthMap. You must call create() with a valid OpenGL 
     * context before using. The width and height of the underlying framebuffer 
     * object has the supplied format.
     */
    DepthMap(unsigned int width, unsigned int height);
    
    /**
     * @brief Destroys the framebuffer object and frees any allocated resources.
     */
    ~DepthMap();
    
    /**
     * @brief Switches rendering from the default, windowing system provided 
     * framebuffer to this framebuffer object. Returns true upon success, 
     * false otherwise.
     */
    void bind();
    
    /**
     * @brief Switches rendering back to the default, windowing system provided
     * framebuffer. This also restore the viewport and clear the color and depth
     * buffer.
     */
    void release();
    
    /**
     * @brief Bind the shadow map to supplied texture unit.
     * @param GL_TEXTUREi The texture unit to bind the shadow map texture.
     */
    void bindTexture(GLenum GL_TEXTUREi);
    
    /**
     * @brief Returns the id of the underlying OpenGL framebuffer object.
     */
    unsigned int objectId() const {return m_FBOId;};
    
    /**
     * @brief Returns the texture ID of the attached texture.
     */
    unsigned int texture() const {return m_textureId;};
    
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
     * Dimension of the viewport before binding the underlying framebuffer.
     */
    GLint m_previousViewport[4];

    /**
     * OpenGL ID of the framebuffer.
      */
    unsigned int m_FBOId;
    
    /**
     * Texture ID storing the framebuffer depth buffer.
     */
    unsigned int m_textureId;
};




#endif // DEPTHMAP_H
