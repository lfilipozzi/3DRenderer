#include "modelloader.h"
#include <QDir>

ModelLoader::ModelLoader() {

}

bool ModelLoader::Load(QString filePath, QString textureDir) {
    // Import the model with Assimp
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath.toStdString(),
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

    // Check if the model has been loaded
    if( !scene) {
        qDebug() << "Error loading file: (assimp:) " << importer.GetErrorString();
        return false;
    }

    // Process the material of the model and make sure it uses a supported 
    // shading model (i.e. Phong or Gouraud)
    if(scene->HasMaterials()) {
        for(unsigned int ii=0; ii<scene->mNumMaterials; ++ii) {
            QSharedPointer<MaterialInfo> mater = processMaterial(
                scene->mMaterials[ii], textureDir
            );
            m_materials.push_back(mater);
        }
    }

    // Process all the meshes.
    if(scene->HasMeshes()) {
        for(unsigned int ii=0; ii<scene->mNumMeshes; ++ii) {
            m_meshes.push_back(processMesh(scene->mMeshes[ii]));
        }
    }
    else {
        qDebug() << "Error: No meshes found";
        return false;
    }
    
    // Make sure the model has no light (not supported)
    if(scene->HasLights()) {
        qDebug() << "Has Lights";
    }
    
    // Process the nodes and store the root node of the model.
    if(scene->mRootNode != nullptr) {
        Node *rootNode = new Node;
        processNode(scene, scene->mRootNode, nullptr, *rootNode);
        m_rootNode.reset(rootNode);
    }
    else {
        qDebug() << "Error loading model";
        return false;
    }

    return true;
}

void ModelLoader::getBufferData(QVector<float> **vertices, 
                                QVector<float> **normals,
                                QVector<unsigned int> **indices) {
    if(vertices != nullptr)
        *vertices = &m_vertices;

    if(normals != nullptr)
        *normals = &m_normals;

    if(indices != nullptr)
        *indices = &m_indices;
}

void ModelLoader::getTextureData(QVector<QVector<float> > **textureUV, 
                                 QVector<float> **tangents,
                                 QVector<float> **bitangents) {
    if(textureUV != nullptr)
        *textureUV = &m_textureUV;

    if(tangents != nullptr)
        *tangents = &m_tangents;

    if(bitangents != nullptr)
        *bitangents = &m_bitangents;
}

QVector<OpenGLTexture*> ModelLoader::loadMaterialTextures(const aiMaterial *material,
                                        const OpenGLTexture::TextureType type,
                                        const QString textureDir) {
    QVector<OpenGLTexture*> textures;
    
    // Define corresponding Assimp type
    aiTextureType aiType;
    switch (type) {
        case OpenGLTexture::TextureType::Diffuse:
            aiType = aiTextureType::aiTextureType_DIFFUSE;
            break;
        default:
            qCritical() << "Invalid type";
            break;
    }
        
    for (unsigned int i = 0; i < material->GetTextureCount(aiType); i++) {
        // Get the texture path from Assimp
        aiString pathString;
        material->GetTexture(aiType, i, &pathString);
        
        QString path = QString(pathString.C_Str());
        path.prepend(textureDir);
        
        if (!QFile::exists(path))
            qCritical() << "The path to the texture file '" << path 
                << "' is not valid";
        
        // If this texture has already been loaded, do not load it again
        bool skip = false;
        for(unsigned int j = 0; j < m_texturesLoaded.size(); j++) {
            if (m_texturesLoaded.at(j)->path().compare(path) == 0) {
                textures.push_back(m_texturesLoaded.at(j));
                skip = true;
                break;
            }
        }
        
        // If the texture has not been loaded yet
        if (!skip) {
            // Load the texture and add it to the list of loaded texture
            OpenGLTexture* thisTexture = new OpenGLTexture(path, type);
            textures.push_back(thisTexture);
        }
    }
    return textures;
}

OpenGLTexture* ModelLoader::loadDefaultTexture() {
    QString path = QString("../Texture/Default/noTexture.png");
    
     if (!QFile::exists(path))
        qCritical() << "The path to the texture file '" << path 
            << "' is not valid";
     
     // If this texture has already been loaded, do not load it again
    for(unsigned int j = 0; j < m_texturesLoaded.size(); j++) {
        if (m_texturesLoaded.at(j)->path().compare(path) == 0) {
            return m_texturesLoaded.at(j);
        }
    }
    
    // If the texture has not been loaded yet: load the texture and 
    // add it to the list of loaded texture
    OpenGLTexture* defaultTexture = new OpenGLTexture(path, 
                                        OpenGLTexture::TextureType::Diffuse);
    return defaultTexture;
}


QSharedPointer<MaterialInfo> ModelLoader::processMaterial(aiMaterial *material, 
                                                          QString textureDir) {
    QSharedPointer<MaterialInfo> mater(new MaterialInfo);

    aiString mname;
    material->Get(AI_MATKEY_NAME, mname);
    if(mname.length > 0)
        mater->Name = mname.C_Str();

    int shadingModel;
    material->Get(AI_MATKEY_SHADING_MODEL, shadingModel);

    // Check if the model is using a supported shading model (i.e. Phong or Gouraud)
    if(shadingModel != aiShadingMode_Phong && shadingModel != aiShadingMode_Gouraud) {
        qDebug() << "This mesh's shading model is not implemented in this loader," 
            << "setting to default material";
        mater->Name = "DefaultMaterial";
        mater->Alpha = 1.0f;
    }
    // The shading model is supported
    else {
        aiColor3D dif (0.f,0.f,0.f);
        aiColor3D amb (0.f,0.f,0.f);
        aiColor3D spec (0.f,0.f,0.f);
        float shine = 0.0;
        float alpha = 1.0;

        material->Get(AI_MATKEY_COLOR_AMBIENT, amb);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, dif);
        material->Get(AI_MATKEY_COLOR_SPECULAR, spec);
        material->Get(AI_MATKEY_SHININESS, shine);
        material->Get(AI_MATKEY_OPACITY, alpha);

        mater->Ambient = QVector3D(amb.r, amb.g, amb.b);
        mater->Diffuse = QVector3D(dif.r, dif.g, dif.b);
        mater->Specular = QVector3D(spec.r, spec.g, spec.b);
        mater->Shininess = shine;
        mater->Alpha = alpha;
        
        // Load the texture of the material
        // TODO: For now only one texture is supported, also only diffuse textures are used
        QVector<OpenGLTexture*> theseTextures = loadMaterialTextures(material, OpenGLTexture::Diffuse, textureDir);
        // Load a default texture if there is none (the fragment shader needs one)
        if (theseTextures.isEmpty()) {
            theseTextures.push_back(loadDefaultTexture());
        }
        mater->Texture = theseTextures.at(0);
    }

    return mater;
}

QSharedPointer<Mesh> ModelLoader::processMesh(aiMesh *mesh) {
    QSharedPointer<Mesh> newMesh(new Mesh);
    newMesh->name = mesh->mName.length != 0 ? mesh->mName.C_Str() : "";
    newMesh->indexOffset = static_cast<unsigned int>(m_indices.size());
    unsigned int indexCountBefore = static_cast<unsigned int>(m_indices.size());
    unsigned int vertindexoffset = static_cast<unsigned int>(m_vertices.size()/3);

    // Get the vertices of the mesh being processed
    if(mesh->mNumVertices > 0) {
        for(uint ii=0; ii<mesh->mNumVertices; ++ii) {
            aiVector3D &vec = mesh->mVertices[ii];

            m_vertices.push_back(vec.x);
            m_vertices.push_back(vec.y);
            m_vertices.push_back(vec.z);
        }
    }

    // Get the normals of the mesh being processed
    if(mesh->HasNormals()) {
        for(uint ii=0; ii<mesh->mNumVertices; ++ii) {
            aiVector3D &vec = mesh->mNormals[ii];
            m_normals.push_back(vec.x);
            m_normals.push_back(vec.y);
            m_normals.push_back(vec.z);
        }
    }

    // Get the texture coordinates of the mesh being processed
    if(mesh->GetNumUVChannels() > 0)  {
        if(static_cast<unsigned int>(m_textureUV.size()) < mesh->GetNumUVChannels()) { // Caution, assumes all meshes in this model have same number of uv channels
            m_textureUV.resize(static_cast<int>(mesh->GetNumUVChannels()));
            m_textureUVComponents.resize(static_cast<int>(mesh->GetNumUVChannels()));
        }

        for(int ich = 0; ich < static_cast<int>(mesh->GetNumUVChannels()); ++ich) {
            m_textureUVComponents[ich] = mesh->mNumUVComponents[ich];

            for(uint iind = 0; iind<mesh->mNumVertices; ++iind) {
                m_textureUV[ich].push_back(mesh->mTextureCoords[ich][iind].x);
                if(mesh->mNumUVComponents[ich] > 1) {
                    m_textureUV[ich].push_back(mesh->mTextureCoords[ich][iind].y);
                    if(mesh->mNumUVComponents[ich] > 2) {
                        m_textureUV[ich].push_back(mesh->mTextureCoords[ich][iind].z);
                    }
                }
            }
        }
    }

    // Get Tangents and bitangents of the mesh being processed
    if(mesh->HasTangentsAndBitangents()) {
        //addTangentsAndBitangents(mesh, m_tangents, m_bitangents);
        for(uint ii=0; ii<mesh->mNumVertices; ++ii) {
            aiVector3D &vec = mesh->mTangents[ii];
            m_tangents.push_back(vec.x);
            m_tangents.push_back(vec.y);
            m_tangents.push_back(vec.z);

            aiVector3D &vec2 = mesh->mBitangents[ii];
            m_bitangents.push_back(vec2.x);
            m_bitangents.push_back(vec2.y);
            m_bitangents.push_back(vec2.z);
        }
    }

    // Get mesh indices of the mesh being processed
    for(uint t = 0; t<mesh->mNumFaces; ++t) {
        aiFace* face = &mesh->mFaces[t];
        if(face->mNumIndices != 3) {
            qDebug() << "Warning: Mesh face with not exactly 3 indices, ignoring this primitive." << face->mNumIndices;
            continue;
        }

        m_indices.push_back(face->mIndices[0] + vertindexoffset);
        m_indices.push_back(face->mIndices[1] + vertindexoffset);
        m_indices.push_back(face->mIndices[2] + vertindexoffset);
    }

    newMesh->indexCount = static_cast<unsigned int>(m_indices.size()) - indexCountBefore;
    newMesh->material = m_materials.at(static_cast<int>(mesh->mMaterialIndex));

    return newMesh;
}

void ModelLoader::processNode(const aiScene *scene, aiNode *node, Node *parentNode, Node &newNode) {
    newNode.name = node->mName.length != 0 ? node->mName.C_Str() : "";

    newNode.transformation = QMatrix4x4(node->mTransformation[0]);

    newNode.meshes.resize(static_cast<int>(node->mNumMeshes));
    for(int imesh = 0; imesh < static_cast<int>(node->mNumMeshes); ++imesh) {
        QSharedPointer<Mesh> mesh = m_meshes[static_cast<int>(node->mMeshes[imesh])];
        newNode.meshes[imesh] = mesh;
    }

    for(int ich = 0; ich < static_cast<int>(node->mNumChildren); ++ich) {
        newNode.nodes.push_back(Node());
        processNode(scene, node->mChildren[ich], parentNode, newNode.nodes[ich]);
    }
}






