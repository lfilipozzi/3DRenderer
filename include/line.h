#ifndef LINE_H
#define LINE_H

#include "abstractobject.h"
#include "../include/shaderprogram.h"
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

/// Line object
/**
 * @brief This class allow to draw lines in OpenGL.
 * @author Louis Filipozzi
 */
class Line : public ABCObject {
public:
    /**
     * @brief Constructor.
     * @param vertices The vertices composing the line.
     * @param indices The indices defining the line.
     * @param color The color of the line.
     */
    Line(
        QVector<QVector3D> vertices, QVector<unsigned int> indices, 
        QVector3D color
    ) : 
    m_isInitialized(false), 
    m_color(color),
    m_vertices(vertices),
    m_indices(indices),
    m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
    m_indexBuffer(QOpenGLBuffer::IndexBuffer) {};
    ~Line() {};
    
    /**
     * @brief Initialize the object, i.e. create the buffers, attributes, ...
     */
    virtual void initialize();
    
    /**
     * @brief Draw the object.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    virtual void render(
        const CasterLight & light, const QMatrix4x4 & view, 
        const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace
    );
    
    /**
     * @brief Draw the object when computing the framebuffer for shadow mapping.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    virtual void renderShadow(const QMatrix4x4 & lightSpace);
    
    /**
     * @brief Clean up the object.
     */
    virtual void cleanUp();
    
private:
    /**
     * Provide vertex position, normals, and indices.
     * @param vertices
     * @param normals
     * @param indices
     */
    void getBufferData(QVector<float> &vertices, 
                       QVector<unsigned int> &indices);
    
    /**
     * Set attributes of the shaders program.
     */
    void createAttributes();
    
    /**
     * Create the vertex, normal, and index buffers.
     */
    void createBuffers();

private:
    /**
     * Check if the model has been initialized.
     */
    bool m_isInitialized;
    
    /**
     * Pointer to OpenGL ES 2.0 API functions.
     */
    QOpenGLFunctions * p_glFunctions;
    
    /**
     * The color of the axis.
     */
    const QVector3D m_color;
    
    /**
     * The vertices defining the line.
     */
    QVector<QVector3D> m_vertices;
    
    /**
     * Vector of indices to draw the line.
     */
    QVector<unsigned int> m_indices;
    
    /**
     * The shader used to render the line.
     */
    std::unique_ptr<Shader> p_shader;
    
    /**
     * Vertex Array Object containing all the buffer needed to render the 
     * object.
     */
    QOpenGLVertexArrayObject m_vao;
    
    /**
     * Buffer of vertices.
     */
    QOpenGLBuffer m_vertexBuffer;

    /**
     * Buffer of indices.
     */
    QOpenGLBuffer m_indexBuffer;
};

#endif // LINE_H
