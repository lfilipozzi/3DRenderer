#include "../include/skybox_gl33.h"
#include <QDebug>

Skybox_GL33::Skybox_GL33() {
    
}

Skybox_GL33::~Skybox_GL33() {
    
}

void Skybox_GL33::initialize() {
    initializeOpenGLFunctions();
    
    createShaderProgram(":/shaders/skybox.vert", ":/shaders/skybox.frag");
    
    loadCubeMapTextures();
    
    createBuffers();
    createAttributes();
}

void Skybox_GL33::loadCubeMapTextures() {
    QTransform posRotation;
    posRotation.rotate(90);
    QTransform negRotation;
    negRotation.rotate(-90);
    
    // Load the texture of the skybox
    const QImage posX = QImage(
        "asset/Texture/Skybox/front.jpg"
    ).mirrored().transformed(posRotation);
    const QImage posY = QImage(
        "asset/Texture/Skybox/left.jpg"
    ).mirrored();
    const QImage posZ = QImage(
        "asset/Texture/Skybox/top.jpg"
    ).mirrored(true, false).transformed(negRotation);
    const QImage negX = QImage(
        "asset/Texture/Skybox/back.jpg"
    ).mirrored(true,false).transformed(posRotation);
    const QImage negY = QImage(
        "asset/Texture/Skybox/right.jpg"
    ).mirrored(true, false);
    const QImage negZ = QImage(
        "asset/Texture/Skybox/bottom.jpg"
    ).mirrored(true, false).transformed(negRotation);
    
    if (posX.isNull())
        qCritical() << __FILE__ << __LINE__ << "Image file does not exist.";
    if (posY.isNull())
        qCritical() << __FILE__ << __LINE__ << "Image file does not exist.";
    if (posZ.isNull())
        qCritical() << __FILE__ << __LINE__ << "Image file does not exist.";
    if (negX.isNull())
        qCritical() << __FILE__ << __LINE__ << "Image file does not exist.";
    if (negY.isNull())
        qCritical() << __FILE__ << __LINE__ << "Image file does not exist.";
    if (negZ.isNull())
        qCritical() << __FILE__ << __LINE__ << "Image file does not exist.";
    
    m_textures = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
    
    m_textures->create();
    m_textures->setSize(posX.width(), posX.height(), posX.depth());
    m_textures->setFormat(QOpenGLTexture::RGBA8_UNorm);
    m_textures->allocateStorage();
    m_textures->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,
                        QOpenGLTexture::BGRA, QOpenGLTexture::UInt8,
                        (const void*)posX.constBits(), 0);
    m_textures->setData(0, 0, QOpenGLTexture::CubeMapPositiveY,
                        QOpenGLTexture::BGRA, QOpenGLTexture::UInt8,
                        (const void*)posY.constBits(), 0);
    m_textures->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ,
                        QOpenGLTexture::BGRA, QOpenGLTexture::UInt8,
                        (const void*)posZ.constBits(), 0);
    m_textures->setData(0, 0, QOpenGLTexture::CubeMapNegativeX,
                        QOpenGLTexture::BGRA, QOpenGLTexture::UInt8,
                        (const void*)negX.constBits(), 0);
    m_textures->setData(0, 0, QOpenGLTexture::CubeMapNegativeY,
                        QOpenGLTexture::BGRA, QOpenGLTexture::UInt8,
                        (const void*)negY.constBits(), 0);
    m_textures->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ,
                        QOpenGLTexture::BGRA, QOpenGLTexture::UInt8,
                        (const void*)negZ.constBits(), 0);
    m_textures->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textures->setMinificationFilter(QOpenGLTexture::Linear);
    m_textures->setMagnificationFilter(QOpenGLTexture::Linear);
    m_textures->generateMipMaps();
}

void Skybox_GL33::createBuffers() {
    QVector<float> vertices({
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    });

    // Create a vertex array object
    m_vao.create();
    m_vao.bind();

    // Create a buffer and copy the vertex data to it
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(&(vertices)[0], vertices.size() 
        * static_cast<int>(sizeof(float)));
}

void Skybox_GL33::createAttributes() {
    m_vao.bind();
    
    // Set up the vertex array state
    m_shaderProgram.bind();

    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    m_shaderProgram.enableAttributeArray(0);       // layout location
    m_shaderProgram.setAttributeBuffer(0,          // layout location
                                       GL_FLOAT,   // data's type
                                       0,          // Offset to data in buffer
                                       3);         // number of components
}

void Skybox_GL33::update(const CasterLight &/*light*/, const QMatrix4x4 view, 
                         const QMatrix4x4 projection, 
                         const QMatrix4x4 /*lightSpaceMatrix*/) {
    // Bind shader program
    m_shaderProgram.bind();
    
    // Bind the texture and draw the skybox
    m_textures->bind(2);
    
    // Remove the translation of the view lightSpaceMatrix
    QMatrix4x4 skyboxView = view;
    QMatrix3x3 skyboxViewNormal = skyboxView.normalMatrix();
    skyboxView = QMatrix4x4(*skyboxViewNormal.data(),
                            *(skyboxViewNormal.data()+3), 
                            *(skyboxViewNormal.data()+6), 
                            0.0f, 
                            *(skyboxViewNormal.data()+1),
                            *(skyboxViewNormal.data()+4),
                            *(skyboxViewNormal.data()+7),
                            0.0f,
                            *(skyboxViewNormal.data()+2),
                            *(skyboxViewNormal.data()+5),
                            *(skyboxViewNormal.data()+8),
                            0.0f,
                            0.0f,
                            0.0f,
                            0.0f,
                            1.0f
    );
    
    // Set shader uniforms for transformation matrices
    m_shaderProgram.setUniformValue("VP", projection * skyboxView);
    m_shaderProgram.setUniformValue("skybox", 2);
    
    // Bind VAO and the surface
    m_vao.bind();
    glDepthMask(GL_FALSE); /* Disable depth writing to make sure the skybox is
    at th background of all other objects*/
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE); // Reenable depth writing
    m_vao.release();
}


void Skybox_GL33::cleanup() {
    
}
