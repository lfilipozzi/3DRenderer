#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <QMatrix4x4>
#include "materialinfo.h"
#include "texture.h"

// Forward declaration of Assimp structures
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

/// Mesh used to load modesl using the Assimp library
/**
 * Contains information about the mesh.
 * @var Mesh::name Name of the mesh.
 * @var Mesh::indexCount Number of vertices in the mesh.
 * @var Mesh::indexOffset Position in the buffer where the vertex data begins.
 * @var Mesh::material Material of the mesh.
 */
struct Mesh {
    /// The name of the mesh.
    QString name;
    /// Number of indices used by the vertices of the mesh.
    unsigned int indexCount;
    /// Offset in the index buffer of the first mesh index.
    unsigned int indexOffset;
    /// Pointer to the material of the mesh.
    QSharedPointer<MaterialInfo> material;
};

/// Node used to load models using the Assimp library
/** Defines a node. Each nodes contain the meshes and transformation matrix that
 * position them in the scene. A node may also have a child node.
 */
struct Node {
    /// The name of the node.
    QString name;
    /// The transformation use to position the node.
    QMatrix4x4 transformation;
    /// The meshes of the node.
    QVector<QSharedPointer<Mesh>> meshes;
    /// The child node of the node.
    QVector<Node> nodes;
};

/// Load model with Assimp
/**
 * This class load a 3D model using Assimp library. It can be used as follow:
 *      ModelLoader model;
 *      if (!model.Load("head.3ds"))
 *      {
 *          m_error = true;
 *          return;
 *      }
 *      QVector<float> *vertices;
 *      QVector<float> *normals;
 *      QVector<unsigned int> *indices;
 *      model.getBufferData(&vertices, &normals, &indices);
 */
class ModelLoader {
public:
    ModelLoader();

    /**
     * This function loads the 3D model from filePath and return a boolean to 
     * indicate if the model has been loaded successfully. This processes the
     * materials, the meshes, and the nodes of the model.
     * @param filePath The path to the object to load.
     * @param textureDir The path to the directory containing the textures to load.
     */
    bool Load(QString filePath, QString textureDir);

    /**
     * Retrieve vertex position, normals, and indices of the model.
     * @param vertices
     * @param normals
     * @param indices
     */
    void getBufferData(QVector<float> **vertices, QVector<float> **normals,
                        QVector<unsigned int> **indices);

    /**
     * Retrieve the texture coordinates and tangents for texture and normal mapping.
     * @param textureUV
     * @param tangents
     * @param bitangents
     */
    void getTextureData(QVector<QVector<float> > **textureUV,   // For texture mapping
                         QVector<float> **tangents,             // For normal mapping
                        QVector<float> **bitangents);

    /**
     * Return the root node of the model.
     * @return The root node of the model.
     */
    QSharedPointer<Node> getNodeData() {return m_rootNode;}

    /**
     * Return the number of texture coordinates channel.
     * @return
     */
    int numUVChannels() {return m_textureUV.size();}

    /**
     * Return the coordinate component for the demanded channel.
     * @param channel The desired channel.
     * @return
     */
    unsigned int numUVComponents(int channel) {return m_textureUVComponents.at(channel);}

private:
    /**
     * Load the textures of a given material.
     * @param material The Assimp material.
     * @param type The type of texture.
     * @param textureDir The folder containing the texture files.
     */
    QVector<OpenGLTexture*> loadMaterialTextures(const aiMaterial *material,
                                                 const OpenGLTexture::TextureType type,
                                                 const QString textureDir);
    
    /**
     * Load a default texture.
     */
    OpenGLTexture* loadDefaultTexture();

    /**
     * Private helper function used to process the Assimp material into our own 
     * material. Check if the lighting model of the material is the Phong or 
     * Gouraud model and get material information (color, shininess, ...) 
     * from Assimp.
     * @param mater The Assimp material.
     * @param textureDir The directory containing the textures to load.
     * @return The processed material.
     */
    QSharedPointer<MaterialInfo> processMaterial(aiMaterial *mater, 
                                                 QString textureDir);

    /**
     * Private helper function used to process the Assimp mesh. This saves the 
     * vertices, normals, indices, texture coordinates, tangents, and bitangents
     * in the corresponding containers; it store the material used by the mesh; 
     * and it returns the number of elemets to store, the offset, name of the
     * mesh, and material information in a mesh structure.
     * @param mesh The Assimp mesh.
     * @return The processed mesh.
     */
    QSharedPointer<Mesh> processMesh(aiMesh *mesh);

    /**
     * Private helper function used to process the Assimp nodes. This process
     * the nodes of the model recursively. It processes the meshes used by each 
     * node.
     * @param scene The Assimp scene.
     * @param node The Assimp node.
     * @param parentNode The Assimp parent node.
     * @param newNode
     */
    void processNode(const aiScene *scene, aiNode *node, 
                     Node *parentNode, Node &newNode);
    
    /**
     * Vector of vertices for each point of the model.
     */
    QVector<float> m_vertices;
    
    /**
     * Vector of normal vector for each point of the model
     */
    QVector<float> m_normals;
    
    /**
     * Vector of indices of the model to draw the elements.
     */
    QVector<unsigned int> m_indices;
    
    /**
     * Vector of textures used by the model that have already been loaded.
     */
    QVector<OpenGLTexture*> m_texturesLoaded;
    
    /**
     * Vector of texture coordinates.
     */
    QVector<QVector<float> > m_textureUV; // multiple channels
    
    /**
     * Vector of tangents for normal mapping.
     */
    QVector<float> m_tangents;
    
    /**
     * Vector of bitangents for normal mapping.
     */
    QVector<float> m_bitangents;
    
    /**
     * Vector containing the number of texture channel.
     */
    QVector<unsigned int > m_textureUVComponents; // multiple channels

    
    /**
     * Vector of materials used by the model.
     */
    QVector<QSharedPointer<MaterialInfo> > m_materials;
    
    /**
     * Vector of meshes used by the model.
     */
    QVector<QSharedPointer<Mesh> > m_meshes;
    
    /**
     * Root node of the model.
     */
    QSharedPointer<Node> m_rootNode;
};

#endif // MODELLOADER_H
