#include "../include/object.h"

/***
 *       ____   _      _              _   
 *      / __ \ | |    (_)            | |  
 *     | |  | || |__   _   ___   ___ | |_ 
 *     | |  | || '_ \ | | / _ \ / __|| __|
 *     | |__| || |_) || ||  __/| (__ | |_ 
 *      \____/ |_.__/ | | \___| \___| \__|
 *                   _/ |                 
 *                  |__/                  
 */

void Object::initialize() {
    // If the model is not correctly loaded, do nothing
    if(m_error) {
        qDebug() << "There was an error while loading the model, "
            << "the model will not be drawn.";
        return;
    }
    
    createShaderPrograms();
    createBuffers();
    createAttributes();
    
    m_isInitialized = true;
}


void Object::createShaderPrograms() {
    p_objectShader = std::make_unique<ObjectShader>(
        ":/shaders/object.vert", ":/shaders/object.frag"
    );
    p_shadowShader = std::make_unique<ObjectShadowShader>(
        ":/shaders/object_shadow.vert", ":/shaders/object_shadow.frag"
    );
}


void Object::createBuffers() {
    // Create a vertex array object
    m_vao.create();
    m_vao.bind();

    // Create a buffer and copy the vertex data to it
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(
        &(*p_vertices)[0],
        p_vertices->size() * sizeof(float)
    );

    // Create a buffer and copy the vertex data to it
    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_normalBuffer.bind();
    m_normalBuffer.allocate(
        &(*p_normals)[0], 
        p_normals->size() * sizeof(float)
    );

    // Create a buffer and copy the vertex data to it
    if (p_textureUV != nullptr && p_textureUV->size() != 0) {
        m_textureUVBuffer.create();
        m_textureUVBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_textureUVBuffer.bind();
        int texSize = 0;
        for (int i = 0; i < p_textureUV->size(); i++)
            texSize += p_textureUV->at(i).size();
        m_textureUVBuffer.allocate(
            &(*p_textureUV)[0][0], 
            texSize * sizeof(float)
        );
    }

    // Create a buffer and copy the index data to it
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuffer.bind();
    m_indexBuffer.allocate(
        &(*p_indices)[0], 
        p_indices->size() * sizeof(unsigned int)
    );
    
    // Free the buffer data
    p_vertices.reset();
    p_normals.reset();
    p_textureUV.reset();
    p_indices.reset();
}


void Object::createAttributes() {
    m_vao.bind();
    
    // Set up attribute for the shader used for shadow mapping
    p_shadowShader->bind();
    
    // Map vertex data to the vertex shader layout location '0'
    m_vertexBuffer.bind();
    p_shadowShader->enableAttributeArray(0);       // layout location
    p_shadowShader->setAttributeBuffer(0,          // layout location
                                       GL_FLOAT,   // data type
                                       0,          // Offset to data in buffer
                                       3);         // number of components
    
    // Set up the vertex array state
    p_objectShader->bind();

    // Map vertex data to the vertex shader layout location '0'
    m_vertexBuffer.bind();
    p_objectShader->enableAttributeArray(0);       // layout location
    p_objectShader->setAttributeBuffer(0,          // layout location
                                       GL_FLOAT,   // data type
                                       0,          // Offset to data in buffer
                                       3);         // number of components

    // Map normal data to the vertex shader layout location '1'
    m_normalBuffer.bind();
    p_objectShader->enableAttributeArray(1);       // layout location
    p_objectShader->setAttributeBuffer(1,          // layout location
                                       GL_FLOAT,   // data type
                                       0,          // Offset to data in buffer
                                       3);         // number of components

    if(!m_textureUVBuffer.isCreated())
        return;
    m_textureUVBuffer.bind();
    p_objectShader->enableAttributeArray(2);       // layout location
    p_objectShader->setAttributeBuffer(2,          // layout location
                                       GL_FLOAT,   // data type
                                       0,          // Offset to data in buffer
                                       2);         // number of components
}


void Object::render(
    const CasterLight & light, const QMatrix4x4 & view, 
    const QMatrix4x4 & projection, const QMatrix4x4 lightSpace[], 
    ObjectShader * shader
)  {
    // If the model is not correctly loaded, do nothing
    if (m_error)
        return;
    
    // Check if the object has been initialized
    if (!m_isInitialized) {
        qCritical() << __FILE__ << __LINE__
            << "The object must be initialized before being rendered.";
        exit(1);
    }
    
    // Bind shader program
    shader->bind();

    // Set light shader uniform
    shader->setLightUniforms(light, view);
    
    // Set cascade uniform
    float cascade[NUM_CASCADES] = {-10.0f, -30.0f, -60.0f}; // TODO need to update the cascade outside of object
    shader->setCascadeUniforms(cascade);

    // Bind VAO and draw everything
    m_vao.bind();
    
    // Draw opaque node
    MeshesToDrawLater tMeshes;
    p_rootNode->drawNode(
        m_model, view, projection, lightSpace, tMeshes, shader
    );
    
    // Draw transparent nodes from farthest to closest
    for (
        MeshesToDrawLater::reverse_iterator it = tMeshes.rbegin(); 
        it != tMeshes.rend(); it++
    ) {
        if (it->second.second != nullptr) {
            shader->setMatrixUniforms(
                it->second.first, view, projection, lightSpace
            );
            it->second.second->drawMesh(shader);
        }
    }
    m_vao.release();
}


void Object::render(
    const CasterLight & light, const QMatrix4x4 & view, 
    const QMatrix4x4 & projection, 
    const std::array<QMatrix4x4,NUM_CASCADES> & lightSpace
) {
    render(light, view, projection, lightSpace.data(), p_objectShader.get());
}


void Object::renderShadow(const QMatrix4x4 & lightSpace) {
    render(
        CasterLight(), QMatrix4x4(), QMatrix4x4(), &lightSpace, 
        p_shadowShader.get()
    );
}


void Object::cleanUp() {
    // If the model is not correctly loaded, do nothing
    if(m_error)
        return;
}



/***
 *      _   _             _       
 *     | \ | |           | |      
 *     |  \| |  ___    __| |  ___ 
 *     | . ` | / _ \  / _` | / _ \
 *     | |\  || (_) || (_| ||  __/
 *     |_| \_| \___/  \__,_| \___|
 *                                
 *                                
 */

void Object::Node::drawNode(
    const QMatrix4x4 & model, const QMatrix4x4 & view, 
    const QMatrix4x4 & projection, const QMatrix4x4 lightSpace[],
    Object::MeshesToDrawLater& drawLaterMeshes, ObjectShader* objectShader
) const {
    if (!objectShader) {
        qWarning() << __FILE__ << __LINE__ <<
             "The pointer to the shader is null.";
        return;
    }
    
    // Compute model matrix of the node and set uniforms
    QMatrix4x4 object = model * m_transformation;
    objectShader->setMatrixUniforms(object, view, projection, lightSpace);
    
    // Draw the meshes of the node
    for (unsigned int i = 0; i < m_meshes.size(); i++) {
        // Check if the mesh is opaque or transparent
        if (m_meshes[i]->isOpaque()) {
            // Draw now
            m_meshes[i]->drawMesh(objectShader);
        }
        else {
            // Store the mesh in the container to draw it later
            QVector3D objectPosition(object * QVector3D(0.0f, 0.0f, 0.0f));
            QVector3D cameraPosition(view.inverted().column(3));
            float distance(cameraPosition.distanceToPoint(objectPosition));
            
            // Add the info on the mesh to the map to draw it later
            drawLaterMeshes.insert(
                std::make_pair(
                    distance,                   // Key of the map
                    std::make_pair(             // Mesh and model matrix
                        object, m_meshes[i].get()
                    )
                )
            );
        }
    }
    
    // Draw the children recursively
    for (unsigned int i = 0; i < m_children.size(); i++) {
        m_children[i]->drawNode(
            object, view, projection, lightSpace, drawLaterMeshes, objectShader
        );
    }
}



/***
 *      __  __             _     
 *     |  \/  |           | |    
 *     | \  / |  ___  ___ | |__  
 *     | |\/| | / _ \/ __|| '_ \ 
 *     | |  | ||  __/\__ \| | | |
 *     |_|  |_| \___||___/|_| |_|
 *                               
 *                               
 */

#include <QOpenGLFunctions>

void Object::Mesh::drawMesh(ObjectShader * objectShader) const {
    if (!objectShader) {
        qWarning() << __FILE__ << __LINE__ <<
             "The pointer to the shader is null.";
        return;
    }
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning() << __FILE__ << __LINE__ <<
                      "Requires a valid current OpenGL context. \n" <<
                      "Unable to draw the object.";
        return;
    }
    QOpenGLFunctions * glFunctions = context->functions();
    
    // Set material uniforms in OpenGL
    objectShader->setMaterialUniforms(*m_material);
    
    // Draw the mesh
    glFunctions->glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(m_indexCount),
        GL_UNSIGNED_INT,
        reinterpret_cast<const void*>(m_indexOffset * sizeof(unsigned int))
    );
}



/***
 *                             _                     
 *            /\              (_)                    
 *           /  \    ___  ___  _  _ __ ___   _ __    
 *          / /\ \  / __|/ __|| || '_ ` _ \ | '_ \   
 *         / ____ \ \__ \\__ \| || | | | | || |_) |  
 *        /_/    \_\|___/|___/|_||_| |_| |_|| .__/   
 *       ____   _      _              _     | |      
 *      / __ \ | |    (_)            | |    |_|      
 *     | |  | || |__   _   ___   ___ | |_            
 *     | |  | || '_ \ | | / _ \ / __|| __|           
 *     | |__| || |_) || ||  __/| (__ | |_            
 *      \____/ |_.__/ | | \___| \___| \__|           
 *            _      _/ |              _             
 *           | |    |__/              | |            
 *           | |      ___    __ _   __| |  ___  _ __ 
 *           | |     / _ \  / _` | / _` | / _ \| '__|
 *           | |____| (_) || (_| || (_| ||  __/| |   
 *           |______|\___/  \__,_| \__,_| \___||_|   
 *                                                   
 *                                                   
 */

#include <QFile>

std::unique_ptr<Object> Object::Loader::getObject() {
    return move(p_object);
}


bool Object::Loader::build() {
    // Check the file exists
    if (!QFile::exists(m_filePath)) {
            qDebug() << __FILE__ << __LINE__
                << "The path" << m_filePath 
                << "to the model is not valid.";
            return false;
    }
    
    // Load the model with Assimp
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile(m_filePath.toStdString(),
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);
    
    if (!scene) {
        qDebug() << __FILE__ << __LINE__ <<"Error loading file: (assimp:) " <<
            importer.GetErrorString();
        return false;
    }
    
    // Process the materials of the model
    std::vector<std::shared_ptr<const Material>> materials;
    if (scene->HasMaterials()) {
        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            materials.push_back(
                processMaterial(scene->mMaterials[i], m_textureDir)
            );
        }
    }
    
    // Process the meshes
    std::vector<std::shared_ptr<const Mesh>> meshes;
    std::unique_ptr<QVector<float>> vertices;
    std::unique_ptr<QVector<float>> normals;
    std::unique_ptr<QVector<QVector<float>>> textureUV;
    std::unique_ptr<QVector<unsigned int>> indices;
    if (scene->HasMeshes()) {
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            meshes.push_back(
                processMesh(
                    scene->mMeshes[i], materials,
                    vertices, normals, textureUV, indices
                )
            );
        }
    }
    else {
        qDebug() << "Error while loading the model: no meshes found.";
        return false;
    }
    
    // Process light sources of the model
    if (scene->HasLights()) {
        qDebug() << "The model has light sources. Ignore it.";
    }
    
    // Process the nodes
    std::unique_ptr<const Node> rootNode;
    if (scene->mRootNode != nullptr) {
        rootNode = processNode(scene->mRootNode, scene, meshes);
    }
    else {
        qDebug() << "Error while loading the model.";
        return false;
    }
    // Build the object
    p_object = std::make_unique<Object>(
        std::move(rootNode), std::move(vertices), std::move(normals), 
        std::move(textureUV), std::move(indices)
    );
    
    return true;
}


std::shared_ptr<const Material> Object::Loader::processMaterial(
    const aiMaterial* material, const QString textureDir
) {
    std::shared_ptr<Material> mater(nullptr);

    // Get material name
    aiString mname;
    material->Get(AI_MATKEY_NAME, mname);
    QString name;
    if(mname.length > 0)
        name = QString(mname.C_Str());
    
    // Check if the model is using a supported shading model (Phong or Gouraud)
    int shadingModel;
    material->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
    if(shadingModel != aiShadingMode_Phong 
        && shadingModel != aiShadingMode_Gouraud) {
        qDebug() << __FILE__ << __LINE__ <<
            "The shading model of the mesh" << name << 
            "is not implemented in this object loader." <<
            "Use default material.";
        
        mater = std::make_shared<Material>(name);
        mater->setAlpha(1.0f);
    }
    // The shading model is supported
    else {
        aiColor3D dif (0.f,0.f,0.f);
        aiColor3D amb (0.f,0.f,0.f);
        aiColor3D spec (0.f,0.f,0.f);
        float shine = 0.0;
        float alpha = 1.0;
        
        // Retrieve material information
        material->Get(AI_MATKEY_COLOR_AMBIENT, amb);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, dif);
        material->Get(AI_MATKEY_COLOR_SPECULAR, spec);
        material->Get(AI_MATKEY_SHININESS, shine);
        material->Get(AI_MATKEY_OPACITY, alpha);
        
        // Load the texture of the material
        // TODO: For now only one diffuse texture is supported
        std::vector<Texture *> textures = loadMaterialTextures(
            material, Texture::Type::Diffuse, textureDir
        );
        if (textures.size() == 0)
            textures.push_back(nullptr);
        mater = std::make_shared<Material>(name, textures.at(0));
        mater->setAmbientColor(QVector3D(amb.r, amb.g, amb.b));
        mater->setDiffuseColor(QVector3D(dif.r, dif.g, dif.b));
        mater->setSpecularColor(QVector3D(spec.r, spec.g, spec.b));
        mater->setShininess(shine);
        mater->setAlpha(alpha);
    }
    
    return mater;
}


std::vector<Texture *> Object::Loader::loadMaterialTextures(
    const aiMaterial* material, const Texture::Type type, 
    const QString textureDir
) {
    // Convert to the corresponding Assimp texture type
    aiTextureType aiType;
    switch (type) {
        case Texture::Type::Diffuse:
            aiType = aiTextureType::aiTextureType_DIFFUSE;
            break;
        case Texture::Type::Reflection:
            aiType = aiTextureType::aiTextureType_REFLECTION;
            break;
        case Texture::Type::Normal:
            aiType = aiTextureType::aiTextureType_NORMALS;
            break;
        case Texture::Type::Displacement:
            aiType = aiTextureType::aiTextureType_DISPLACEMENT;
            break;
        default:
            qCritical() << __FILE__ << __LINE__ <<
            "No corresponding Assimp type for type" << type;
            return std::vector<Texture *>();
    }
    
    // Load all the textures used by the model
    std::vector<Texture *> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(aiType); i++) {
        // Get the texture path from Assimp
        aiString pathString;
        material->GetTexture(aiType, i, &pathString);
        QString path = QString(pathString.C_Str());
        path.prepend(textureDir);
        
        // Check the texture file exists
        if (!QFile::exists(path))
            qCritical() << __FILE__ << __LINE__ << 
                "The path" << path 
                << "to the texture file is not valid";
        QImage image(path);
        if (image.isNull())
            qCritical() << __FILE__ << __LINE__ << 
                "The image file does not exist.";
        
        // Load the texture
        Texture * thisTexture = TextureManager::loadTexture(path, type, image);
        textures.push_back(thisTexture);
    }
    return textures;
}


std::shared_ptr<const Object::Mesh> Object::Loader::processMesh(
    const aiMesh* mesh, 
    const std::vector<std::shared_ptr<const Material>> & materials, 
    std::unique_ptr<QVector<float>> & vertices, 
    std::unique_ptr<QVector<float>> & normals,
    std::unique_ptr<QVector<QVector<float>>> & textureUV,
    std::unique_ptr<QVector<unsigned int>> & indices
) {
    if (!vertices)
        vertices = std::make_unique<QVector<float>>();
    if (!normals)
        normals = std::make_unique<QVector<float>>();
    if (!textureUV)
        textureUV = std::make_unique<QVector<QVector<float>>>();
    if (!indices)
        indices = std::make_unique<QVector<unsigned int>>();
    
    // Get the mesh name
    QString name;
    if (mesh->mName.length != 0)
        name = QString(mesh->mName.C_Str());
    else
        name = QString("");
    
    // Get the mesh offset index in the index buffer
    unsigned int offset = static_cast<unsigned int>(indices->size());
    unsigned int vertexOffset = static_cast<unsigned int>(vertices->size()/3);
    
    // Retrieve the vertices of the mesh
    if (mesh->mNumVertices > 0) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D & vec = mesh->mVertices[i];
            vertices->push_back(vec.x);
            vertices->push_back(vec.y);
            vertices->push_back(vec.z);
        }
    }
    
    // Retrieve the normals of the mesh
    if (mesh->HasNormals()) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D & vec = mesh->mNormals[i];
            normals->push_back(vec.x);
            normals->push_back(vec.y);
            normals->push_back(vec.z);
        }
    }
    
    // Retrieve the texture coordinates
    unsigned int numUV = mesh->GetNumUVChannels();
    if (numUV > 0) {
        if (textureUV->size() < static_cast<int>(numUV)) {
            // It is assumed that all the meshes have the same number of UV 
            // channels (number of texture coordinates). Resize the textureUV
            // if the number of UV channel is not the same.
            textureUV->resize(numUV);
        }
        for (unsigned int iChannel = 0; iChannel < numUV; iChannel++) {
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                (*textureUV)[iChannel].push_back(
                    mesh->mTextureCoords[iChannel][i].x
                );
                if (mesh->mNumUVComponents[iChannel]> 1) {
                    (*textureUV)[iChannel].push_back(
                        mesh->mTextureCoords[iChannel][i].y
                    );
                    if (mesh->mNumUVComponents[iChannel] > 2) {
                        (*textureUV)[iChannel].push_back(
                            mesh->mTextureCoords[iChannel][i].z
                        );
                    }
                }
            }
        }
    }
    
    // Retrieve the indices of the mesh
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace * face = &mesh->mFaces[i];
        if (face->mNumIndices != 3) {
            qDebug() << "Model loading: Mesh face with not exactly 3 indices,"
                << "ignore the primitive" << face->mNumIndices;
            continue;
        }
        indices->push_back(face->mIndices[0] + vertexOffset);
        indices->push_back(face->mIndices[1] + vertexOffset);
        indices->push_back(face->mIndices[2] + vertexOffset);
    }
    unsigned int count = static_cast<unsigned int>(indices->size()) - offset;
    
    // Find the material of the mesh
    std::shared_ptr<const Material> material = materials.at(mesh->mMaterialIndex);
    
    // Create the mesh
    std::shared_ptr<const Mesh> newMesh = std::make_shared<Mesh>(
        name, count, offset, material
    );
    return newMesh;
}


std::unique_ptr<const Object::Node> Object::Loader::processNode(
    const aiNode * node, const aiScene * scene, 
    const std::vector<std::shared_ptr<const Mesh>> & sceneMeshes
) {    
    // Get the node name
    QString name;
    if (node->mName.length != 0)
        name = QString(node->mName.C_Str());
    else
        name = QString("");
    
    // Define the transformation
    QMatrix4x4 transformation(node->mTransformation[0]);
    
    // Get the node meshes
    std::vector<std::shared_ptr<const Mesh>> nodeMeshes;
    nodeMeshes.resize(node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        std::shared_ptr<const Mesh> mesh = sceneMeshes[node->mMeshes[i]];
        nodeMeshes[i] = mesh;
    }
    
    // Create the children of the node
    std::vector<std::unique_ptr<const Node>> children;
    children.resize(node->mNumChildren);
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        std::unique_ptr<const Node> child = processNode(
            node->mChildren[i], scene, sceneMeshes
        );
        children[i] = std::move(child);
    }
    
    // Create the node
    return std::make_unique<Node>(
        name, transformation, nodeMeshes, std::move(children)
    );
}



/***
 *               ______ _       _            
 *              |  ____| |     | |           
 *              | |__  | | __ _| |_          
 *              |  __| | |/ _` | __|         
 *              | |    | | (_| | |_          
 *       _____  |_|    |_|\__,_|\__|         
 *      / ____|           / _|               
 *     | (___  _   _ _ __| |_ __ _  ___ ___  
 *      \___ \| | | | '__|  _/ _` |/ __/ _ \ 
 *      ____) | |_| | |  | || (_| | (_|  __/ 
 *     |_____/_\__,_|_|  |_| \__,_|\___\___| 
 *          |  _ \      (_) |   | |          
 *          | |_) |_   _ _| | __| | ___ _ __ 
 *          |  _ <| | | | | |/ _` |/ _ \ '__|
 *          | |_) | |_| | | | (_| |  __/ |   
 *          |____/ \__,_|_|_|\__,_|\___|_|   
 *                                           
 *                                           
 */

bool Object::FlatSurfaceBuilder::build() {
    // Define vertices
    QVector3D cornerRL = m_origin-m_surfaceWidth/2*m_lateralSurface +
            -m_surfaceLength/2*m_longitudinalSurface;
    QVector3D cornerRR = m_origin+m_surfaceWidth/2*m_lateralSurface +
            -m_surfaceLength/2*m_longitudinalSurface;
    QVector3D cornerFL = m_origin-m_surfaceWidth/2*m_lateralSurface +
             m_surfaceLength/2*m_longitudinalSurface;
    QVector3D cornerFR = m_origin+m_surfaceWidth/2*m_lateralSurface +
             m_surfaceLength/2*m_longitudinalSurface;
    
    std::unique_ptr<QVector<float>> vertices;
    std::unique_ptr<QVector<float>> normals;
    std::unique_ptr<QVector<QVector<float>>> textureUV;
    std::unique_ptr<QVector<unsigned int>> indices;
    vertices  = std::make_unique<QVector<float>>();
    normals   = std::make_unique<QVector<float>>();
    textureUV = std::make_unique<QVector<QVector<float>>>();
    indices   = std::make_unique<QVector<unsigned int>>();
    // Define buffer data
    vertices->append(
        QVector<float>({
            // RL corner
            cornerRL.x(), cornerRL.y(), cornerRL.z(),
            // RR corner
            cornerRR.x(), cornerRR.y(), cornerRR.z(),
            // FL corner
            cornerFL.x(), cornerFL.y(), cornerFL.z(),
            // FR corner
            cornerFR.x(), cornerFR.y(), cornerFR.z()
        })
    );
    textureUV->append(   // x channel
            QVector<float>({
            // RL corner
            0.0f, 0.0f,
            // RR corner
            m_surfaceWidth/m_textureSize, 0.0f,
            // FL corner
            0.0f, m_surfaceLength/m_textureSize,
            // RR corner
            m_surfaceWidth/m_textureSize, m_surfaceLength/m_textureSize
        })
    );
    for (int i = 0; i < 4; i++) {
        normals->append(QVector<float>({
            m_normalSurface.x(),
            m_normalSurface.y(),
            m_normalSurface.z()
        }));
    }
    indices->append(QVector<unsigned int>({0, 1, 2})); // RL, RR, and FL
    indices->append(QVector<unsigned int>({2, 1, 3})); // FL, RR, and FR
    unsigned int count = static_cast<unsigned int>(indices->size());
    
    
    // Set up material the surface material
    QString path("asset/Texture/RoadMaterials/MyRoad/Road_texture.png");
    if (!QFile::exists(path))
        qCritical() << __FILE__ << __LINE__ << 
            "The path" << path 
            << "to the texture file is not valid";
    QImage image(path);
    if (image.isNull())
        qCritical() << __FILE__ << __LINE__ << 
            "The image file does not exist.";
    std::shared_ptr<Material> material;
    material = std::make_shared<Material>(
        "surface", 
        TextureManager::loadTexture(path, Texture::Type::Diffuse, image)
    );
    material->setAmbientColor(QVector3D(0.5f, 0.5f, 0.5f));
    material->setDiffuseColor(QVector3D(.6f, .6f, .6f));
    material->setSpecularColor(QVector3D(.2f, .2f, .2f));
    material->setShininess(50.0f);
    material->setAlpha(1.0f);
    
    // Build the mesh
    std::vector<std::shared_ptr<const Mesh>> meshes;
    std::shared_ptr<const Mesh> mesh;
    mesh = std::make_shared<const Mesh>("mesh", count, 0, material);
    meshes.push_back(mesh);
    
    // Build the root node
    std::vector<std::unique_ptr<const Node>> children;
    std::unique_ptr<const Node> rootNode;
    rootNode = std::make_unique<const Node>(
        "root_node", QMatrix4x4(), meshes, std::move(children)
    );
    
    // Build the object
    p_object = std::make_unique<Object>(
        std::move(rootNode), std::move(vertices), std::move(normals), 
        std::move(textureUV), std::move(indices)
    );
    
    return true;
}


std::unique_ptr<Object> Object::FlatSurfaceBuilder::getObject() {
    return move(p_object);
}








