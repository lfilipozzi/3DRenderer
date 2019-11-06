#ifndef FLATSURFACE_GL33_H
#define FLATSURFACE_GL33_H

#include "abstractcomplex_gl33.h"

/// Create a flat surface to represent the road.
/**
 * Create a flat surface to represent the road.
 */
class FlatSurface_GL33 : public AbstractComplex_GL33 {
public:
    FlatSurface_GL33(float length, float width,
                     QVector3D origin, 
                     QVector3D longitudinal, 
                     QVector3D lateral, 
                     float textureGridSize
                    );
    ~FlatSurface_GL33();

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
     * This function draws the scene, similarly to update(), but uses the shader
     * for shadow mapping and transform to the light space. This is used to 
     * obtain the shadow map.
     */
    virtual void renderShadow(const QMatrix4x4 lightSpaceMatrix);

    /**
     * Cleanup the animation.
     */
    virtual void cleanup();

private:
     /**
     * Provide pointers to data used to create the vertex, position, normals, 
     * textures, and indices buffers.
     * @param vertices
     * @param normals
     * @param textureUV
     * @param indices
     */
    virtual void getBufferAndTextureData(QVector<float> *&vertices, 
                                 QVector<float> *&normals,
                                 QVector<QVector<float>> *&textureUV,
                                 QVector<unsigned int> *&indices);
    
    /**
     * Define data for the vertex position, normals, textures, and indices
     * buffers.
     */
    void setBufferAndTextureData();

    /**
     * Define the size of the texture pattern.
     */
    float m_textureSize;

    /**
     * The length of the surface.
     */
    float m_surfaceLength;

    /**
     * The width of the surface.
     */
    float m_surfaceWidth;
    
    /**
     * Origin of the plane
     */
    QVector3D m_origin;

    /**
     * Longitudinal axis of the surface plane.
     */
    QVector3D m_longitudinalSurface;

    /**
     * Lateral axis of the surface plane.
     */
    QVector3D m_lateralSurface;

    /**
     * The normal of the surface.
     */
    QVector3D m_normalSurface;
    
    QVector<float> m_verticesData;
    QVector<float> m_normalsData;
    QVector<QVector<float>> m_textureUVData;
    QVector<unsigned int> m_indicesData;
};

#endif // FLATSURFACE_GL33_H
