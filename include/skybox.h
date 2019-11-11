#ifndef SKYBOX_H
#define SKYBOX_H

#include "texture.h"
#include "shaderprogram.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <memory>
#include <QOpenGLFunctions>

/// Class for the skybox
/**
 * @brief Defines a skybox.
 * @author Louis Filipozzi
 */
class Skybox {
public:
    Skybox() : m_isInitialized(false) {};
    ~Skybox() {};
    
    /**
     * @brief Initialize the skybox, i.e. create the buffers, attributes, ...
     */
    void initialize();
    
    /**
     * @brief Draw the skybox.
     * @param view The view matrix.
     * @param projection The projection matrix.
     */
    void render(const QMatrix4x4 & view, const QMatrix4x4 & projection);
    
    /**
     * @brief Clean up the skybox.
     */
    void cleanUp();
    
private:
    /**
     * @brief Create and link the shader program.
     */
    void createShaderProgram();
    
    /**
     * @brief Load the cubemap textures.
     */
    void loadCubeMapTextures();
    
    /**
     * @brief Set attributes of the shaders program.
     */
    void createAttributes();

    /**
     * @brief Create the vertex, normal, texture, and index buffers.
     */
    void createBuffers();
    
private:
    /**
     * Check if the skybox has been initialized;
     */
    bool m_isInitialized;
    
    /**
     * Textures used by the skybox.
     */
    std::unique_ptr<Texture> m_textures;
    
    /**
     * OpenGL Vertex Array Object (VAO)
     */
    QOpenGLVertexArrayObject m_vao;
    
    /**
     * Buffer of vertices
     */
    QOpenGLBuffer m_vertexBuffer;
    
    /**
     * Shader program used to render the skybox.
     */
    std::unique_ptr<Shader> m_shader;
    
    /**
     * Pointer to OpenGL ES 2.0 API functions.
     */
    QOpenGLFunctions * p_glFunctions;
    
};

#endif // SKYBOX_H
