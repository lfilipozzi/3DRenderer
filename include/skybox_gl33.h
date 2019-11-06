#ifndef SKYBOX_GL33_H
#define SKYBOX_GL33_H

#include "abstractobject3d_gl33.h"
#include <QOpenGLTexture>

/// Skybox
/**
 * This class encapsulate a skybox.
 */
class Skybox_GL33 : public AbstractObject3D_GL33 {
public:
    Skybox_GL33();
    ~Skybox_GL33();
    
    /**
     * Initialize the object: e.g. create the buffers, attributes, lighting.
     */
    virtual void initialize();

    /**
     * Update the animation.
     */
    virtual void update(const CasterLight &light, const QMatrix4x4 view,
                const QMatrix4x4 projection, const QMatrix4x4 lightSpaceMatrix);

    /**
     * Cleanup the animation.
     */
    virtual void cleanup();
    
private:
    /**
     * Load the cubemap textures.
     */
    void loadCubeMapTextures();
    
    /**
     * Set attributes of the shaders program.
     */
    void createAttributes();

    /**
     * Create the vertex, normal, texture, and index buffers.
     */
    void createBuffers();
    
    /**
     * Textures used by the skybox.
     */
    QOpenGLTexture *m_textures;
};

#endif // SKYBOX_GL33_H
