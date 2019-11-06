#ifndef ABSTRACTCOMPLEX_GL33_H
#define ABSTRACTCOMPLEX_GL33_H

#include "abstractobject3d_gl33.h"
#include "materialinfo.h"

/// Interface for 3D object using the 'complex' fragment and vertex shaders.
/**
 * This class defines an interface for objects using the "complex' fragment and 
 * vertex shaders. Objects of this class needs to define vertices, normals, 
 * indices, textures, and a texture file.
 */
class AbstractComplex_GL33 : public AbstractObject3D_GL33 {
public:
    AbstractComplex_GL33();
    
    /**
     * This function draws the scene, similarly to update(), but uses the shader
     * for shadow mapping and transform to the light space. This is used to 
     * obtain the shadow map.
     */
    virtual void renderShadow(const QMatrix4x4 lightSpaceMatrix) = 0;
    
protected:
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
                                 QVector<unsigned int> *&indices) = 0;
    
    /**
     * Create the shaders used for shadow mapping and link the shaders to the 
     * program.
     */
    void createShaderProgramForShadowMapping();
    
    /**
     * Create the vertex, normal, texture, and index buffers.
     */
    void createBuffers();
    
    /**
     * Set attributes of the shaders program.
     */
    void createAttributes();
    
    /**
     * Setup default material of the 3D model.
     */
    void setupMaterialInfo(QVector3D ambient, QVector3D diffuse, 
                           QVector3D specular, float shininess,
                           float alpha, QString textureFilePath);
    
    /**
     * Set the fragment shader's uniforms for material.
     * @param mater
     */
    void setMaterialUniforms(const MaterialInfo &mater);
    
    /**
     * Information on the material used for the lighting effect by the shader.
     */
    MaterialInfo m_materialInfo;
    
    /**
     * The shader program used for computing the depth map for shadow mapping.
     */
    QOpenGLShaderProgram m_shaderShadow;
    
    /**
     * If set to true, the object is not rendered but other objects in the 
     * scene are still rendered.
     */
    bool m_error;
    
};

#endif // ABSTRACTCOMPLEX_GL33_H



