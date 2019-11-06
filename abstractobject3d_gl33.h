#ifndef ABSTRACTOBJECT3D_GL33_H
#define ABSTRACTOBJECT3D_GL33_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include "light.h"

/// 3D Object interface.
/**
 * Interface used for each 3D object in the OpenGL 3.3 Scene.
 */
class AbstractObject3D_GL33 : public QOpenGLFunctions_3_3_Core {
public:
    AbstractObject3D_GL33();

    /**
     * @brief Set the model matrix of the system used to place the model in the
     * virtual world.
     * @param modelMatrix The model matrix.
     */
    void setModelMatrix(const QMatrix4x4 modelMatrix) {m_model = modelMatrix;}

    /**
     * Initialize the object: e.g. load the model, create the buffers, 
     * attributes, lighting.
     */
    virtual void initialize() = 0;

    /**
     * Update the animation.
     */
    virtual void update(const CasterLight &light, 
                        const QMatrix4x4 view,
                        const QMatrix4x4 projection, 
                        const QMatrix4x4 lightSpaceMatrix) = 0;

    /**
     * Cleanup the animation.
     */
    virtual void cleanup() = 0;

protected:
    /**
     * Create the shaders and link the shaders to the program.
     * @param vShader The vertex shader.
     * @param fShader The fragment shader.
     */
    void createShaderProgram(QString vShader, QString fShader);

    /**
     * OpenGL Vertex Array Object (VAO): Object that keep track of all the 
     * buffer that have been activated, instead of binding (activate) and 
     * unbinding all OpenGL Buffer, we only have to bind the VAO.
     */
    QOpenGLVertexArrayObject m_vao;

    /**
     * Model matrix: transform from the model coordinates to the world 
     * coordinates, i.e. to change the position of the model within the world.
     */
    QMatrix4x4 m_model;

    /**
     * The shader used to render the scene.
     */
    QOpenGLShaderProgram m_shaderProgram;

    /**
     * Buffer of vertices
     */
    QOpenGLBuffer m_vertexBuffer;

    /**
     * Buffer of normal vectors.
     */
    QOpenGLBuffer m_normalBuffer;

    /**
     * Buffer of textures.
     */
    QOpenGLBuffer m_textureUVBuffer;

    /**
     * Buffer of indices. This enable to reuse a vertex several times.
     */
    QOpenGLBuffer m_indexBuffer;
};

#endif // ABSTRACTOBJECT3D_GL33_H
