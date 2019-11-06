#ifndef SCENE_GL33_H
#define SCENE_GL33_H

// #define SHADOW_FBO_DEBUG // Display the texture from the shadow mapping FBO

#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include "abstractscene.h"
#include "vehicle_gl33.h"
#include "flatsurface_gl33.h"
#include "frame_gl33.h"
#include "skybox_gl33.h"
#include <memory>

/// Scene for OpenGL 3.3
/**
 * Scene to be used with OpenGL 3.3 context. This is the most important function
 * since this defines the animation though the initialize(), resize(), update(),
 * and cleanup() methods. It includes all the 3D objects of the scene.
 */
class Scene_GL33 : public AbstractScene, public QOpenGLFunctions_3_3_Core {
public:
    Scene_GL33(int refreshRate);

    ~Scene_GL33();

    /**
     * Initialize the scene: Load the model, create the buffers, attributes, 
     * lighting. It also enables GL_DEPTH_TEST and the background color.
     */
    void initialize();

    /**
     * Resize the animation and reset the projection matrix.
     * @param w The new width of the animation.
     * @param h The new height of the animation.
     */
    void resize(int w, int h);

    /**
     * Update the animation.
     */
    void update();

    /**
     * Cleanup the animation.
     */
    void cleanup();

private:
    /**
     * Set projection and view matrices. Set lightning.
     */
    void setupLightingAndMatrices();

    /**
     * Projection matrix: transform from the camera coordinates to the 
     * homogeneous coordinates (the 2D coordinates of the screen).
     */
    QMatrix4x4 m_projection;

    /**
     * View matrix: transform from the world (scene) coordinates to the camera 
     * coordinates, this is used to change the position of the camera.
     */
    QMatrix4x4 m_view;

    /**
     * The lighting of the scene.
     */
    CasterLight m_light;
    
    /**
     * The skybox of the scene.
     */
    Skybox_GL33 m_skybox;

    /**
     * The surface of the scene.
     */
    FlatSurface_GL33 m_surface;
    
    /**
     * The XYZ frame of the scene.
     */
    Frame_GL33 m_frame;
    
    /**
     * The animation widget width.
     */
    int m_screenWidth;
    
    /**
     * The animation widget height.
     */
    int m_screenHeight;
    
    /**
     * Width of the texture produced by the frame buffer object for shadow
     * mapping.
     */
    const unsigned int SHADOW_WIDTH;
    
    /**
     * Height of the texture produced by the frame buffer object for shadow
     * mapping.
     */
    const unsigned int SHADOW_HEIGHT;

    /**
     * ID of the frame buffer used to obtain the shadow by shadow mapping. Qt 
     * provide a class for an OpenGL frame buffer object, but this class cannot
     * be used for shadow mapping as it doesn't give access to the framebuffer's
     * depth buffer.
      */
    unsigned int m_shadowFBO;
    
    /**
     * Texture storing the framebuffer's depth buffer.
     */
    unsigned int m_shadowDepthMap;
    
    #ifdef SHADOW_FBO_DEBUG
    /**
     * Shader used to draw the texture from the shadow FBO
     */
    QOpenGLShaderProgram m_shaderProgram;
    
    /**
     * VAO used to store vertices to render the FBO texture
     */
    QOpenGLVertexArrayObject m_vao;
    
    /**
     * Buffer of vertices used to render the FBO texture
     */
    QOpenGLBuffer m_vertexBuffer;
    
    /**
     * Buffer of texture coordinates used to render the FBO texture
     */
    QOpenGLBuffer m_textureUVBuffer;
    #endif // SHADOW_FBO_DEBUG
    
};

#endif // SCENE_GL33_H










