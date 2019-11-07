#include "../include/shaderprogram.h"

Shader::Shader(QString vShader, QString fShader) {
    // Compile vertex shader
    if (!addShaderFromSourceFile(QOpenGLShader::Vertex, vShader.toUtf8()))
        qCritical() << "Unable to compile vertex shader. Log:" << log();

    // Compile fragment shader
    if (!addShaderFromSourceFile(QOpenGLShader::Fragment,fShader.toUtf8()))
        qCritical() << "Unable to compile fragment shader. Log:" << log();

    // Link the shaders together into a program
    if (!link())
        qCritical() << "Unable to link shader program. Log:" << log();
}


void ObjectShader::setMaterialUniforms(const Material * material) {
    // Set material properties
    setUniformValue("Ka", material->getAmbientColor());
    setUniformValue("Kd", material->getDiffuseColor());
    setUniformValue("Ks", material->getSpecularColor());
    setUniformValue("shininess", material->getShininess());
    setUniformValue("alpha", material->getAlpha());
    
    // Apply the texture
    material->getTexture()->bind(0);
    setUniformValue("textureSampler", 0);
}


void ObjectShadowShader::setMaterialUniforms(const Material* /*material*/) {
    // Nothing to do: no need to apply material to render the shadow 
    // frame buffer.
}


void ObjectShader::setMatrixUniforms(const QMatrix4x4 & M, const QMatrix4x4 & V, 
                                     const QMatrix4x4 & P, 
                                     const QMatrix4x4 & lVP) {
    QMatrix4x4 MV = V*M;
    QMatrix4x4 MVP = P * MV;
    QMatrix4x4 lMVP = lVP * M;
    QMatrix3x3 N = MV.normalMatrix();

    setUniformValue("M", M);
    setUniformValue("MV", MV);
    setUniformValue("MVP", MVP);
    setUniformValue("lightMVP", lMVP);
    setUniformValue("N", N);
}


void ObjectShadowShader::setMatrixUniforms(const QMatrix4x4 & M, 
                                           const QMatrix4x4 & /*V*/, 
                                           const QMatrix4x4 & /*P*/, 
                                           const QMatrix4x4 & lVP) {
    QMatrix4x4 lMVP = lVP * M;
    
    setUniformValue("lightMVP", lMVP);
}







