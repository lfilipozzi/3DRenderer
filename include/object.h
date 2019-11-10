#ifndef OBJECT_H
#define OBJECT_H

#include "material.h"
#include "shaderprogram.h"
#include <QString>
#include <memory>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "light.h"


/// Object
/**
 * @brief Defines a 3D object to be render in the scene.
 * @author Louis Filipozzi
 * @details This class uses an architecture similar to the one used by Assimp.
 * The vertex data (vertices, normals, indices) are stored in a unique Vertex 
 * Array Object (VAO). The object is decomposed of several nodes organized in a 
 * tree. A node is decomposed of a mesh. Each mesh has a unique material.
 * When rendering the object, the VAO is bound. Then each node is draw 
 * recursively.
 */
class Object {
public:
    class IBuilder;
    class Loader;
    
    class Node;
    class Mesh;
    
    
    Object(
        std::unique_ptr<const Node> rootNode,
        std::unique_ptr<QVector<float>> vertices,
        std::unique_ptr<QVector<float>> normals,
        std::unique_ptr<QVector<QVector<float>>> textureUV,
        std::unique_ptr<QVector<unsigned int>> indices
    ) : 
    m_error(!rootNode || !vertices || !normals || !textureUV || !indices),
    p_rootNode(std::move(rootNode)), 
    m_vertexBuffer(QOpenGLBuffer::VertexBuffer), 
    m_normalBuffer(QOpenGLBuffer::VertexBuffer), 
    m_textureUVBuffer(QOpenGLBuffer::VertexBuffer), 
    m_indexBuffer(QOpenGLBuffer::IndexBuffer), 
    p_objectShader(nullptr), 
    p_shadowShader(nullptr), 
    p_vertices(std::move(vertices)), p_normals(std::move(normals)),
    p_textureUV(std::move(textureUV)), p_indices(std::move(indices)) {};
    ~Object() {};
    
    /**
     * @brief Initialize the object, i.e. create the buffers, attributes, ...
     */
    void initialize();
    
    /**
     * @brief Draw the object.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     * @param glFunctions Pointer to class containing OpenGL functions.
     */
    void render(const CasterLight & light, const QMatrix4x4 & view, 
                const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace, 
                QOpenGLFunctions_3_3_Core * glFunctions);
    
    /**
     * @brief Draw the object when computing the framebuffer for shadow mapping.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     * @param glFunctions Pointer to class containing OpenGL functions.
     */
    void renderShadow(const CasterLight & light, const QMatrix4x4 & view, 
                const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace, 
                QOpenGLFunctions_3_3_Core * glFunctions);
    
    /**
     * @brief Clean up the object.
     */
    void cleanUp();
    
private:
    /**
     * @brief Draw the object using a given shader.
     * @details This function is used as the implementation of both the 
     * Object::render() and Object::renderShadow() functions since these two 
     * functions perform the same task but using different shader program.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     * @param shader The shader program used to draw the scene.
     * @param glFunctions Pointer to class containing OpenGL functions.
     */
    void render(const CasterLight & light, const QMatrix4x4 & view, 
                const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace, 
                ObjectShader * shader, 
                QOpenGLFunctions_3_3_Core * glFunctions);
    
    /**
     * @brief Create and link the shader program.
     */
    void createShaderPrograms();
    
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
     * @typedef Container used to store meshes to draw later.
     */
    typedef std::multimap<float, std::pair<QMatrix4x4, const Mesh *>> 
        MeshesToDrawLater;
    
    /**
     * Set to true if the model is not valid.
     */
    bool m_error;
    
    /**
     * The root node of the model.
     */
    std::unique_ptr<const Node> p_rootNode;
    
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
     * Buffer of normal vectors.
     */
    QOpenGLBuffer m_normalBuffer;

    /**
     * Buffer of textures coordinates.
     */
    QOpenGLBuffer m_textureUVBuffer;

    /**
     * Buffer of indices.
     */
    QOpenGLBuffer m_indexBuffer;
    
    /**
     * Model matrix of the object.
     */
    QMatrix4x4 m_model;

    /**
     * The shader used to render the scene.
     */
    std::unique_ptr<ObjectShader> p_objectShader;
    
    /**
     * The shader used to render the object when computing the shadow map.
     */
    std::unique_ptr<ObjectShadowShader> p_shadowShader;
    
    /**
     * Pointer to the vertices data used to fill the vertex buffer at 
     * initialization. This pointer is reset after initialization.
     */
    std::unique_ptr<QVector<float>> p_vertices;
    
    /**
     * Pointer to the vertices data used to fill the normal buffer at 
     * initialization. This pointer is reset after initialization.
     */
    std::unique_ptr<QVector<float>> p_normals;
    
    /**
     * Pointer to the vertices data used to fill the texture buffer at 
     * initialization. This pointer is reset after initialization.
     * @details The textureUV is a vector of vectof of float. The vector of 
     * float corresponds to the i-th UV channel of all the vertices. Thus, the 
     * vector of vector of float corresponds to all the UV channels of all 
     * vertices. 
     * Other implementation of the textureUV could be:
     * - a vector of float which combines all the data for each vertices.
     * - a vector of vector of float which corresponds to a list of points with 
     *   different dimension depending of the number of UV channel of each mesh.
     * The reason the texture UV is implemented this way is because it allowed 
     * to easily resize all the textureUV to use the same number of UV channels.
     */
    std::unique_ptr<QVector<QVector<float>>> p_textureUV;
    
    /**
     * Pointer to the vertices data used to fill the indices buffer at 
     * initialization. This pointer is reset after initialization.
     */
    std::unique_ptr<QVector<unsigned int>> p_indices;
};



#include <QMatrix4x4>
#include <memory>
#include <QOpenGLFunctions_3_3_Core>

/// Node of a 3D object
/**
 * @brief Define a node of a 3D object. The node is made of several meshes.
 * @author Louis Filipozzi
 */
class Object::Node {
public:
    /**
     * @brief Node constructor.
     * @param name The name of the node.
     */
    Node(const QString name, const QMatrix4x4 transformation, 
         const std::vector<std::shared_ptr<const Mesh>> meshes,
         std::vector<std::unique_ptr<const Node>> children)
    : m_name(name), m_transformation(transformation), m_meshes(meshes) ,
    m_children(std::move(children)) {};
    ~Node() {};
    
    const QString getName() const {return m_name;};
    
    /**
     * @brief Draw the node and its children.
     * @param model The model matrix use to position the node. Note that the 
     * transformation stored in the node is applied for positioning the node.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     * @param drawLaterMeshes Container of meshes to draw later (transparent
     * meshes).
     * @param objectShader The shader used to render the object.
     * @param glFunctions Pointer to class containing OpenGL functions.
     */
    void drawNode(const QMatrix4x4 & model, const QMatrix4x4 & view, 
                  const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace, 
                  MeshesToDrawLater & drawLaterMeshes, 
                  ObjectShader * objectShader, 
                  QOpenGLFunctions_3_3_Core * glFunctions) const;
    
private:
    /**
     * The name of the node.
     */
    const QString m_name;
    
    /**
     * The transformation to move from the parent's node to the current node.
     */
    const QMatrix4x4 m_transformation;
    
    /**
     * The meshes contained by the node.
     */
    const std::vector<std::shared_ptr<const Mesh>> m_meshes;
    
    /**
     * The children of this node.
     */
    std::vector<std::unique_ptr<const Node>> m_children;
};



/// Mesh
/**
 * @brief Define a mesh of a 3D object. 
 * @author Louis Filipozzi
 * @remark The mesh does not contains any vertices, normals, and indices data. 
 * It only has information about the number of vertices in the mesh, and the 
 * offset of the first index in the index buffer. The data must be stored in the
 * Vertex Array Object (VAO) of the class containing the mesh. This is done to 
 * reduce number of VAO that have to be bind for rendering.
 */
class Object::Mesh {
public:
    /**
     * @brief Constructor of the mesh.
     * @param name The name of the mesh.
     * @param count The number of indices in the mesh.
     * @param offset The offset of the first mesh index in the index buffer.
     * @param material The material used by the mesh.
     */
    Mesh(const QString name, const unsigned int count, 
         const unsigned int offset, 
         const std::shared_ptr<const Material> material
    ) : m_name(name), m_indexCount(count), m_indexOffset(offset), 
    m_material(material) {};
    ~Mesh() {};
    
    /**
     * @brief Set material uniform and draw the mesh.
     * @param objectShader The shader used to render the object.
     * @param glFunctions Pointer to class containing OpenGL functions.
     * @remark This function does not set the uniform for the model, view, and
     * projection matrices. It only set the uniforms related to the material.
     */
    void drawMesh(ObjectShader * objectShader, 
                  QOpenGLFunctions_3_3_Core * glFunctions) const;
    
    /**
     * @brief Check if the material applied to the node is opaque.
     * @return Return true for an opaque material.
     */
    bool isOpaque() const {return (m_material->getAlpha() == 1.0f);};
    
private:
    /**
     * The name of the mesh.
     */
    const QString m_name;
    
    /**
     * Number of indices used by the vertices of the mesh.
     */
    const unsigned int m_indexCount;
    
    /**
     * Offset in the index buffer of the first mesh index.
     */
    const unsigned int m_indexOffset;
    
    /**
     * Pointer to the material of the mesh.
     */
    const std::shared_ptr<const Material> m_material;
};



/// Object builder interface
/**
 * @brief Interface of an object builder.
 * @author Louis Filipozzi
 * @details The construction of the object is dealt with the builder pattern.
 */
class Object::IBuilder {
public:
    /**
     * @brief Build the object. Return true if the model has been loaded 
     * successfully and false otherwise.
     */
    virtual bool build() = 0;
    
    /**
     * @brief Return a smart pointer to the loaded object.
     */
    virtual std::unique_ptr<Object> getObject() = 0;
};



#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>


/// Object Assimp loader
/**
 * @brief Load an object.
 * @author Louis Filipozzi
 * @details The construction of the object is dealt with the builder pattern.
 */
class Object::Loader : public Object::IBuilder {
public:
    /**
     * This function loads the 3D model from filePath and return a pointer to 
     * the object.
     * @param filePath The path to the object to load.
     * @param textureDir The path to the directory containing the textures to load.
     */
    Loader(QString filePath, QString textureDir = QString(""))
    : m_filePath(filePath), m_textureDir(textureDir), p_object(nullptr) {};
    
    virtual bool build();
    virtual std::unique_ptr<Object>  getObject();
    
private:
    /**
     * @brief Process Assimp material into a Material. 
     * @details Check if the lighting model is supported and retrieve material
     * information (color, shininess, ...).
     * @param mater The Assimp material.
     * @param textureDir The directory containing the textures to load.
     * @return The processed material.
     */
    std::shared_ptr<const Material> processMaterial(const aiMaterial * material,
                                              const QString textureDir);
    
    /**
     * @brief Loads the textures of the material 'material' and of type
     * 'type'.
     * @param material The Assimp material.
     * @param type The type of texture.
     * @param textureDir The folder containing the texture files.
     * @return Vector of pointer to the material textures.
     */
    std::vector<Texture *> loadMaterialTextures(const aiMaterial * material,
                                   const Texture::Type type,
                                   const QString textureDir);
    
    /**
     * @brief Process the Assimp mesh into a Mesh.
     * @details Retrieve the data to fill the vertices, normals, indices, and 
     * texture coordinate buffer of the object, and create the meshes.
     * @param[in] aiMesh The Assimp mesh.
     * @param[in] materials The vector of the processed materials of the object.
     * @param[out] vertices The data used to fill the vertex buffer.
     * @param[out] normals The data used to fill the normal buffer.
     * @param[out] textureUV The data used to fill the texture coordinates 
     * buffer.
     * @param[out] indices The data used to fill the index buffer.
     * @remark The std::unique_ptr vertices, normals, textureUV, and indices are
     * passed by reference as we don't want to give ownership of the pointer to 
     * the function, but only perform some action to change its content
     * @return The processed mesh.
     */
    std::shared_ptr<const Mesh> processMesh(const aiMesh * mesh,
            const std::vector<std::shared_ptr<const Material>> & materials,
            std::unique_ptr<QVector<float>> & vertices,
            std::unique_ptr<QVector<float>> & normals,
            std::unique_ptr<QVector<QVector<float>>> & textureUV,
            std::unique_ptr<QVector<unsigned int>> & indices
    );
    
    /**
     * @brief Process the Assimp node into a Node.
     * @param[in] node The Assimp node.
     * @param[in] scene The Assimp scene.
     * @param[in] meshes The processed meshes of the scene.
     * @return The processed node.
     */
    std::unique_ptr<const Node> processNode(const aiNode * node, 
            const aiScene * scene, 
            const std::vector<std::shared_ptr<const Mesh>> & sceneMeshes);
    
private:
    /**
     * The path to the object to load.
     */
    QString m_filePath;
    
    /**
     * The directory in which the texture are stored. It is assumed that all
     * the textures are stored in the same directory.
     */
    QString m_textureDir;
    
    /**
     * Pointer to the object.
     */
    std::unique_ptr<Object> p_object;
};


#endif // OBJECT_H
