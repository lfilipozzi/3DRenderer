#ifndef Line_GL33_H
#define Line_GL33_H

#include "abstractobject3d_gl33.h"

// 3D arrow
/**
 * This class is used to draw a line (GL_LINES) in OpenGL 3.3.
 */
class Line_GL33 : public AbstractObject3D_GL33 {
public:
    Line_GL33(QVector<QVector3D> vertices, QVector<unsigned int> indices, 
              QVector3D color);
    ~Line_GL33();
    
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
};

#endif // Line_GL33_H
