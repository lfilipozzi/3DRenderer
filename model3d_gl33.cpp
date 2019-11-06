#include "model3d_gl33.h"

Model3D_GL33::Model3D_GL33(QString filepath, QString texturePath) :
    m_filepath(filepath),
    m_texturePath(texturePath) {
    m_modelLoader = std::make_unique<ModelLoader>();   
}

Model3D_GL33::~Model3D_GL33() {
    
}

void Model3D_GL33::initialize() {
    initializeOpenGLFunctions();
    
    if(!m_modelLoader->Load(m_filepath, m_texturePath)) {
        m_error = true;
        m_modelLoader.reset();
        return;
    }

    createShaderProgram(":/shaders/complex.vert", ":/shaders/complex.frag");
    createShaderProgramForShadowMapping();

    createBuffers();
    createAttributes();
    // For this class, m_materialInfo is the default material. It is applied if
    // the material loaded by Assimp is not supported.
    setupMaterialInfo(QVector3D(0.5f, 0.5f, 0.5f), QVector3D(.6f, .6f, .6f), 
                      QVector3D(.2f, .2f, .2f), 50.0f, 1.0f,
                      QString("../Texture/Default/noTexture.png")
    );
    
    m_rootNode = m_modelLoader->getNodeData();
    m_modelLoader.reset();
}

void Model3D_GL33::getBufferAndTextureData(QVector<float> *&vertices, 
                                           QVector<float> *&normals, 
                                           QVector<QVector<float> > *&textureUV, 
                                           QVector<unsigned int> *&indices) {
    
    m_modelLoader->getBufferData(&vertices, &normals, &indices);
    m_modelLoader->getTextureData(&textureUV, nullptr, nullptr);
}




void Model3D_GL33::update(const CasterLight &light, const QMatrix4x4 view,
                          const QMatrix4x4 projection, 
                          const QMatrix4x4 lightSpaceMatrix) {
    // If an error occured, do nothing
    if(m_error)
        return;

    // Bind shader program
    m_shaderProgram.bind();

    // Set shader uniforms for light information
    // Set shader uniforms for light information
    m_shaderProgram.setUniformValue("lightIntensity", light.getIntensity());
    // Downcast and set the uniform as required
    m_shaderProgram.setUniformValue("lightDirection", 
                                    view * light.getDirection());
    
    // Set shader uniform for skybox and camera position
    QVector3D cameraPosition(view.inverted().column(3));
    m_shaderProgram.setUniformValue("skybox", 2);
    m_shaderProgram.setUniformValue("cameraPosition", cameraPosition);

    // Bind VAO and draw everything
    m_vao.bind();
    // Draw opaque node
    std::multimap<float, TransparentMeshInfo> transparentMeshes;
    drawOpaqueNode(m_rootNode.data(), QMatrix4x4(), view, projection, 
                   lightSpaceMatrix, &transparentMeshes);
    // Draw transparent nodes from farthest to closest
    for(std::map<float,TransparentMeshInfo>::reverse_iterator it = transparentMeshes.rbegin(); 
        it != transparentMeshes.rend(); ++it) {
        drawSingleMesh(it->second, view, projection, lightSpaceMatrix);
    } 
    m_vao.release();
}

void Model3D_GL33::drawOpaqueNode(const Node *node, QMatrix4x4 objectMatrix, 
                            const QMatrix4x4 view, const QMatrix4x4 projection,
                            const QMatrix4x4 lightSpaceMatrix, 
                            std::multimap<float, TransparentMeshInfo> *transparentMeshes) {
    // Prepare matrices
    objectMatrix *= node->transformation;
    QMatrix4x4 modelMatrix = m_model * objectMatrix;
    QMatrix4x4 modelViewMatrix = view * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = projection * modelViewMatrix;
    QMatrix4x4 lightMVP = lightSpaceMatrix * modelMatrix;

    m_shaderProgram.setUniformValue("M", modelMatrix);
    m_shaderProgram.setUniformValue("MV", modelViewMatrix);
    m_shaderProgram.setUniformValue("MVP", mvp);
    m_shaderProgram.setUniformValue("lightMVP", lightMVP);
    m_shaderProgram.setUniformValue("N", normalMatrix);
    
    // Draw mesh in this node
    for(int imm = 0; imm < node->meshes.size(); ++imm) {
        // Draw only non-transparent mesh
        if (node->meshes[imm]->material->Alpha == 1.0) {
            // Set the material and draw the mesh
            if(node->meshes[imm]->material->Name == QString("DefaultMaterial")) {
                setMaterialUniforms(m_materialInfo);
                }
            else {
                setMaterialUniforms(*(node->meshes[imm]->material));
            }
            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(node->meshes[imm]->indexCount),
                GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(node->meshes[imm]->indexOffset * 
                    sizeof(unsigned int)
                )
            );
        }
        // Save non transparent node and draw them after sorting them
        else {
            // Compute the distance of the object to the camera
            QVector3D objectPosition(modelMatrix * QVector3D(0.0f, 0.0f, 0.0f));
            QVector3D cameraPosition(view.inverted().column(3));
            float distance(cameraPosition.distanceToPoint(objectPosition));
            
            // Add the info on the mesh to the map to draw it later
            TransparentMeshInfo meshInfo(node->meshes[imm], modelMatrix);
            transparentMeshes->insert(std::make_pair(distance, meshInfo));
        }
    }
    
    // Recursively draw this nodes children nodes
    for(int inn = 0; inn<node->nodes.size(); ++inn)
        drawOpaqueNode(&node->nodes[inn], objectMatrix, view, projection, 
                       lightSpaceMatrix, transparentMeshes);
}

void Model3D_GL33::drawSingleMesh(const TransparentMeshInfo meshInfo, 
    const QMatrix4x4 view, const QMatrix4x4 projection,
    const QMatrix4x4 lightSpaceMatrix) {
    QMatrix4x4 modelMatrix = meshInfo.ModelMatrix;
    QMatrix4x4 modelViewMatrix = view * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = projection * modelViewMatrix;
    QMatrix4x4 lightMVP = lightSpaceMatrix * modelMatrix;

    m_shaderProgram.setUniformValue("MV", modelViewMatrix);
    m_shaderProgram.setUniformValue("N", normalMatrix);
    m_shaderProgram.setUniformValue("MVP", mvp);
    m_shaderProgram.setUniformValue("lightMVP", lightMVP);
    
    // Set the material and draw the mesh
    if(meshInfo.MeshPtr->material->Name == QString("DefaultMaterial")) {
        setMaterialUniforms(m_materialInfo);
        }
    else {
        setMaterialUniforms(*(meshInfo.MeshPtr->material));
    }
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(meshInfo.MeshPtr->indexCount),
        GL_UNSIGNED_INT,
        reinterpret_cast<const void*>(meshInfo.MeshPtr->indexOffset * 
            sizeof(unsigned int)
        )
    );
}


void Model3D_GL33::renderShadow(const QMatrix4x4 lightSpaceMatrix) {
    // If model is not correctly loaded: do nothing
    if(m_error)
        return;
    
    // Bind shader program
    m_shaderShadow.bind();
    // Bind VAO and draw everything
    m_vao.bind();
    drawNodeShadow(m_rootNode.data(), QMatrix4x4(), lightSpaceMatrix);
    m_vao.release();
}

void Model3D_GL33::drawNodeShadow(const Node *node, QMatrix4x4 objectMatrix, 
                            const QMatrix4x4 lightSpaceMatrix) {
    // Prepare matrices
    objectMatrix *= node->transformation;
    QMatrix4x4 modelMatrix = m_model * objectMatrix;
    QMatrix4x4 lightMVP = lightSpaceMatrix * modelMatrix;
    
    m_shaderShadow.setUniformValue("lightMVP", lightMVP);

    // Draw each mesh in this node
    for(int imm = 0; imm<node->meshes.size(); ++imm) {
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(node->meshes[imm]->indexCount),
                       GL_UNSIGNED_INT,
                       reinterpret_cast<const void*>(node->meshes[imm]->indexOffset * sizeof(unsigned int)));
    }

    // Recursively draw this nodes children nodes
    for(int inn = 0; inn<node->nodes.size(); ++inn)
        drawNodeShadow(&node->nodes[inn], objectMatrix, lightSpaceMatrix);
}


void Model3D_GL33::cleanup() {

}
