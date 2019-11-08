#include "../include/skybox.h"
#include <memory>


Skybox::Skybox() {
    
}


Skybox::~Skybox() {
    
}


void Skybox::initialize() {
    // Set pointer to functions of the OpenGL ES 2.0 API
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning() << __FILE__ << __LINE__ <<
                      "Requires a valid current OpenGL context. \n" <<
                      "The skybox has not been created.";
        return;
    }
    p_glFunctions = context->functions();
    
    // Initialize the object
    createShaderProgram();
    loadCubeMapTextures();
    createBuffers();
    createAttributes();
}


void Skybox::createShaderProgram() {
    m_shader = std::make_unique<Shader>(":/shaders/skybox.vert", 
                                        ":/shaders/skybox.frag");
}


void Skybox::loadCubeMapTextures() {
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
    
    if (posX.isNull() || posY.isNull() || posZ.isNull() ||
        negX.isNull() || negY.isNull() || negZ.isNull())
        qCritical() << __FILE__ << __LINE__ << "The image file does not exist.";

    
    m_textures = std::make_unique<Texture>(
        Texture::Type::Cubemap, QOpenGLTexture::TargetCubeMap
    );
    
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

void Skybox::createBuffers() {
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

void Skybox::createAttributes() {
    m_vao.bind();
    
    // Bind the shader program to the active OpenGL context and make it the
    // current shader program
    m_shader->bind();

    // Map vertex data to the shader layout location '0'
    m_vertexBuffer.bind();
    m_shader->enableAttributeArray(0);       // layout location
    m_shader->setAttributeBuffer(0,          // layout location
                                 GL_FLOAT,   // data's type
                                 0,          // Offset to data in buffer
                                 3);         // number of components
}

void Skybox::render(const QMatrix4x4 & view, const QMatrix4x4 & projection) {
    // Bind shader program
    m_shader->bind();
    
    // Bind the texture and draw the skybox
    m_textures->bind(2);
    
    // Remove the translation of the view matrix
    QMatrix4x4 skyboxView = view;
    QMatrix3x3 skyboxViewNormal = skyboxView.normalMatrix();
    skyboxView = QMatrix4x4(*(skyboxViewNormal.data()),
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
    m_shader->setUniformValue("VP", projection * skyboxView);
    m_shader->setUniformValue("skybox", 2);
    
    // Bind VAO and the surface
    m_vao.bind();
    p_glFunctions->glDepthMask(GL_FALSE); // Disable depth writing to make sure 
                                          // the skybox is at the back
    p_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 36);
    p_glFunctions->glDepthMask(GL_TRUE);   // Re-enable depth writing
    m_vao.release();
}


void Skybox::cleanUp() {
    
}
