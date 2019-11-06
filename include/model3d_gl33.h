#ifndef MODEL3D_GL33_H
#define MODEL3D_GL33_H

#include "abstractcomplex_gl33.h"
#include "modelloader.h"
#include <memory>

/// Information on transparent meshes
/**
 * Contains information on mesh with transparent material so that they can be
 * draw at the end.
 */
struct TransparentMeshInfo {
    /// A pointer to the mesh.
    QSharedPointer<Mesh> MeshPtr;
    /// The model matrix of the mesh.
    QMatrix4x4 ModelMatrix;
    
    TransparentMeshInfo(const QSharedPointer<Mesh> meshPtr, const QMatrix4x4 modelMatrix) :
        MeshPtr(meshPtr),
        ModelMatrix(modelMatrix) {
    }
};

/// 3D model using Assimp library to load the model in the scene.
class Model3D_GL33 : public AbstractComplex_GL33 {
public:
    Model3D_GL33(QString filepath, QString texturePath="");
    ~Model3D_GL33();

    /**
     * Initialize the object: e.g. load the model, create the buffers, 
     * attributes, lighting.
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
     * Draw a node of the model. Discard the transparent meshes.
     * @param node The node to draw.
     * @param objectMatrix Transformation matrix used to draw each node of the 
     * model recursively.
     */
    void drawOpaqueNode(const Node *node, QMatrix4x4 objectMatrix, 
                  const QMatrix4x4 view, const QMatrix4x4 projection, 
                  const QMatrix4x4 lightSpaceMatrix, 
                  std::multimap<float, TransparentMeshInfo> *transparentMeshes);
    
    /**
     * Draw a single mesh of the model. This is used to draw mesh with 
     * transparent material from farthest to closest to the camera.
     * @param meshInfo Contains all we need to draw the mesh.
     */
    void drawSingleMesh(const TransparentMeshInfo meshInfo, 
                        const QMatrix4x4 view, const QMatrix4x4 projection,
                        const QMatrix4x4 lightSpaceMatrix);
    
    /**
     * Draw a node of the model in OpenGL using shadow shader to compute the 
     * shadow map.
     * @param node The node to draw.
     * @param objectMatrix Transformation matrix used to draw each node of the 
     * model recursively.
     */
    void drawNodeShadow(const Node *node, QMatrix4x4 objectMatrix, 
                        const QMatrix4x4 lightSpaceMatrix);

    /**
     * Root node to the model to load.
     */
    QSharedPointer<Node> m_rootNode;

    /**
     * Path to the 3D model to load.
     */
    QString m_filepath;

    /**
     * Path to the directory containing the texture files.
     */
    QString m_texturePath;
    
    /**
     * @remarks Use pointer to be able to delete the modelloader which becomes
     * useless after initialization
     */
    std::unique_ptr<ModelLoader> m_modelLoader;
};

#endif // MODEL3D_GL33_H
