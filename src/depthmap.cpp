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
}


DepthMap::~DepthMap() {}


void DepthMap::bind() {
    if (p_glFunctions != nullptr) {
        p_glFunctions->glViewport(0, 0, c_width, c_height);
        p_glFunctions->glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
        p_glFunctions->glClear(GL_DEPTH_BUFFER_BIT);
    }
}



void DepthMap::bindTexture(GLenum GL_TEXTUREi) {
    if (p_glFunctions != nullptr) {
        p_glFunctions->glActiveTexture(GL_TEXTUREi);
        p_glFunctions->glBindTexture(GL_TEXTURE_2D, m_FBOId);
    }
}

