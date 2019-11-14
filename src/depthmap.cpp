#include "../include/depthmap.h"

#include <QDebug>

DepthMap::DepthMap(unsigned int width, unsigned int height) :
c_width(width),
c_height(height) {
    // Get pointer to OpenGL functions
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qCritical() << __FILE__ << __LINE__ <<
            "Requires a valid current OpenGL context. \n" <<
            "Unable to draw the object.";
        exit(1);
    }
    p_glFunctions = context->versionFunctions<QOpenGLFunctions_3_0>();
    if (!p_glFunctions) {
        qCritical() << __FILE__ << __LINE__ <<
            "Could not obtain required OpenGL context version";
        exit(1);
    }
    
    // Create the frame buffer and texture for shadow mapping
    p_glFunctions->glGenTextures(1, &m_textureId);
    p_glFunctions->glBindTexture(GL_TEXTURE_2D, m_textureId);
    p_glFunctions->glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, c_width, c_height, 0, 
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL
    );
    p_glFunctions->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST
    );
    p_glFunctions->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST
    );
    p_glFunctions->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER
    );
    p_glFunctions->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER
    );
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    p_glFunctions->glTexParameterfv(
        GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor
    ); 
    
    // Attach the depth texture to the FBO
    p_glFunctions->glGenFramebuffers(1, &m_FBOId); 
    p_glFunctions->glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
    p_glFunctions->glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textureId, 0
    );
    p_glFunctions->glDrawBuffer(GL_NONE);
    p_glFunctions->glReadBuffer(GL_NONE);
    p_glFunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    
#ifdef SHADOW_FBO_DEBUG
    p_debugShader = std::make_unique<Shader>(
        ":/shaders/shadow_debug.vert", ":/shaders/shadow_debug.frag"
    );
    
    QVector<float> vertices({
        -1.0f,  1.0f, 0.0f, 
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    });
    QVector<float> normals;
    QVector<float> textureUV({
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    });
    
    // Create a vertex array object
    m_vao.create();
    m_vao.bind();

    // Create a buffer and copy the vertex data to it
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(
        &(vertices)[0], vertices.size() * static_cast<int>(sizeof(float))
    );

    // Create a buffer and copy the vertex data to it
    m_textureUVBuffer.create();
    m_textureUVBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_textureUVBuffer.bind();
    m_textureUVBuffer.allocate(
        &(textureUV)[0], textureUV.size() * static_cast<int>(sizeof(float))
    );
    
    m_vao.bind();
    p_debugShader->bind();

    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    p_debugShader->enableAttributeArray(0);
    p_debugShader->setAttributeBuffer(0, GL_FLOAT, 0, 3);

    if(!m_textureUVBuffer.isCreated())
        return;
    m_textureUVBuffer.bind();
    p_debugShader->enableAttributeArray(1);       // layout location
    p_debugShader->setAttributeBuffer(1, GL_FLOAT, 0, 2);
#endif // SHADOW_FBO_DEBUG
}


DepthMap::~DepthMap() {}


void DepthMap::bind() {
    if (p_glFunctions != nullptr) {
        p_glFunctions->glGetIntegerv(GL_VIEWPORT, m_previousViewport);
        p_glFunctions->glViewport(0, 0, c_width, c_height);
        p_glFunctions->glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
        p_glFunctions->glClear(GL_DEPTH_BUFFER_BIT);
    }
}


void DepthMap::release() {
    if (p_glFunctions != nullptr) {
        p_glFunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
        p_glFunctions->glViewport(
            m_previousViewport[0], m_previousViewport[1], 
            m_previousViewport[2], m_previousViewport[3]
        );
        p_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}


void DepthMap::bindTexture(GLenum GL_TEXTUREi) {
    if (p_glFunctions != nullptr) {
        p_glFunctions->glActiveTexture(GL_TEXTUREi);
        p_glFunctions->glBindTexture(GL_TEXTURE_2D, m_FBOId);
    }
}


#ifdef SHADOW_FBO_DEBUG
void DepthMap::render() {
    p_debugShader->bind();
    p_debugShader->setUniformValue("depthMap", 1);
    p_debugShader->setUniformValue("near_plane", 0.1f);
    p_debugShader->setUniformValue("far_plane", 10);
        
    m_vao.bind();
    p_glFunctions->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_vao.release();
}
#endif // SHADOW_FBO_DEBUG






