#include "../include/abstractcomplex_gl33.h"

AbstractComplex_GL33::AbstractComplex_GL33() : 
    m_error(false) {
    
}

void AbstractComplex_GL33::createShaderProgramForShadowMapping() {
    // Compile vertex shader for shadow mapping
    QString vShadowShader(":/shaders/complexShadow.vert");
    if (!m_shaderShadow.addShaderFromSourceFile(QOpenGLShader::Vertex,
        vShadowShader.toUtf8()))
        qCritical() << "Unable to compile vertex shader. Log:" 
            << m_shaderShadow.log();

    // Compile fragment shader for shadow mapping
    QString fShadowShader(":/shaders/complexShadow.frag");
    if (!m_shaderShadow.addShaderFromSourceFile(QOpenGLShader::Fragment, 
        fShadowShader.toUtf8()))
        qCritical() << "Unable to compile fragment shader. Log:" 
            << m_shaderShadow.log();

    // Link the shaders together into a program
    if (!m_shaderShadow.link())
        qCritical() << "Unable to link shader program. Log:"
            << m_shaderShadow.log();
}

void AbstractComplex_GL33::setupMaterialInfo(QVector3D ambient, 
                    QVector3D diffuse, QVector3D specular, float shininess,
                    float alpha, QString textureFilePath) {
    m_materialInfo.Ambient = ambient;
    m_materialInfo.Diffuse = diffuse;
    m_materialInfo.Specular = specular;
    m_materialInfo.Shininess = shininess;
    m_materialInfo.Alpha = alpha;
    
    // Define textures
    m_materialInfo.Texture = new OpenGLTexture(textureFilePath);
    m_materialInfo.Texture->setWrapMode(QOpenGLTexture::MirroredRepeat);
}

void AbstractComplex_GL33::setMaterialUniforms(const MaterialInfo &mater) {
    m_shaderProgram.setUniformValue("Ka", mater.Ambient);
    m_shaderProgram.setUniformValue("Kd", mater.Diffuse);
    m_shaderProgram.setUniformValue("Ks", mater.Specular);
    m_shaderProgram.setUniformValue("shininess", mater.Shininess);
    m_shaderProgram.setUniformValue("alpha", mater.Alpha);
    
    // Apply texture
    mater.Texture->bind(0); // Bind the texture to the texture unit 0
    m_shaderProgram.setUniformValue("textureSampler", 0);   // Texture
    m_shaderProgram.setUniformValue("shadowMap", 1);        // Shadow map
}

void AbstractComplex_GL33::createBuffers() {
    // If an error occured, do nothing
    if(m_error)
        return;

    QVector<float> *vertices;
    QVector<float> *normals;
    QVector<QVector<float>> *textureUV;
    QVector<unsigned int> *indices;

    getBufferAndTextureData(vertices, normals, textureUV, indices);

    // Create a vertex array object
    m_vao.create();
    m_vao.bind();

    // Create a buffer and copy the vertex data to it
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(&(*vertices)[0],
                   vertices->size() * static_cast<int>(sizeof(float)));

    // Create a buffer and copy the vertex data to it
    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_normalBuffer.bind();
    m_normalBuffer.allocate(&(*normals)[0], 
                            normals->size() * static_cast<int>(sizeof(float)));

    // Create a buffer and copy the vertex data to it
    if(textureUV != nullptr && textureUV->size() != 0) {
        m_textureUVBuffer.create();
        m_textureUVBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_textureUVBuffer.bind();
        int texSize = 0;
        for(int ii=0; ii<textureUV->size(); ++ii)
            texSize += textureUV->at(ii).size();

        m_textureUVBuffer.allocate(&(*textureUV)[0][0], 
                                   texSize * static_cast<int>(sizeof(float)));
    }

    // Create a buffer and copy the index data to it
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuffer.bind();
    m_indexBuffer.allocate(&(*indices)[0], 
                           indices->size() * static_cast<int>(sizeof(unsigned int)));
}

void AbstractComplex_GL33::createAttributes() {
    // If an error occured, do nothing
    if(m_error)
        return;
    
    m_vao.bind();
    
    // Set up attribute for the shader used for shadow mapping
    m_shaderShadow.bind();
    
    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    m_shaderShadow.enableAttributeArray(0);       // layout location
    m_shaderShadow.setAttributeBuffer(0,          // layout location
                                      GL_FLOAT,   // data's type
                                      0,          // Offset to data in buffer
                                      3);         // number of components
    
    // Set up the vertex array state
    m_shaderProgram.bind();

    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    m_shaderProgram.enableAttributeArray(0);       // layout location
    m_shaderProgram.setAttributeBuffer(0,          // layout location
                                       GL_FLOAT,   // data's type
                                       0,          // Offset to data in buffer
                                       3);         // number of components

    // Map normal data to the vertex shader's layout location '1'
    m_normalBuffer.bind();
    m_shaderProgram.enableAttributeArray(1);       // layout location
    m_shaderProgram.setAttributeBuffer(1,          // layout location
                                       GL_FLOAT,   // data's type
                                       0,          // Offset to data in buffer
                                       3);         // number of components

    if(!m_textureUVBuffer.isCreated())
        return;
    m_textureUVBuffer.bind();
    m_shaderProgram.enableAttributeArray(2);       // layout location
    m_shaderProgram.setAttributeBuffer(2,          // layout location
                                       GL_FLOAT,   // data's type
                                       0,          // Offset to data in buffer
                                       2);         // number of components
}




