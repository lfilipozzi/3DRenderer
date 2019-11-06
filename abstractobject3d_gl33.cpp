#include "abstractobject3d_gl33.h"

AbstractObject3D_GL33::AbstractObject3D_GL33() :
    m_indexBuffer(QOpenGLBuffer::IndexBuffer) {
    m_model.setToIdentity();
}

void AbstractObject3D_GL33::createShaderProgram(QString vShader,
                                                QString fShader) {
    // Compile vertex shader
    if (!m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, 
        vShader.toUtf8()))
        qCritical() << "Unable to compile vertex shader. Log:" 
        << m_shaderProgram.log();

    // Compile fragment shader
    if (!m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
        fShader.toUtf8()))
        qCritical() << "Unable to compile fragment shader. Log:" 
        << m_shaderProgram.log();

    // Link the shaders together into a program
    if (!m_shaderProgram.link())
        qCritical() << "Unable to link shader program. Log:" 
        << m_shaderProgram.log();
}

