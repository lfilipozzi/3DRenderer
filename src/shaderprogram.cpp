#include "../include/shaderprogram.h"

#include <QOpenGLFunctions>


/***
 *       _____  _                 _             
 *      / ____|| |               | |            
 *     | (___  | |__    __ _   __| |  ___  _ __ 
 *      \___ \ | '_ \  / _` | / _` | / _ \| '__|
 *      ____) || | | || (_| || (_| ||  __/| |   
 *     |_____/ |_| |_| \__,_| \__,_| \___||_|   
 *                                              
 *                                              
 */

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



/***
 *          ____   _      _              _      
 *         / __ \ | |    (_)            | |     
 *        | |  | || |__   _   ___   ___ | |_    
 *        | |  | || '_ \ | | / _ \ / __|| __|   
 *        | |__| || |_) || ||  __/| (__ | |_    
 *         \____/ |_.__/ | | \___| \___| \__|   
 *       _____  _       _/ |      _             
 *      / ____|| |     |__/      | |            
 *     | (___  | |__    __ _   __| |  ___  _ __ 
 *      \___ \ | '_ \  / _` | / _` | / _ \| '__|
 *      ____) || | | || (_| || (_| ||  __/| |   
 *     |_____/ |_| |_| \__,_| \__,_| \___||_|   
 *                                              
 *                                              
 */

void ObjectShader::setMaterialUniforms(const Material & material) {
    // Set material properties
    setUniformValue("Ka", material.getAmbientColor());
    setUniformValue("Kd", material.getDiffuseColor());
    setUniformValue("Ks", material.getSpecularColor());
    setUniformValue("shininess", material.getShininess());
    setUniformValue("alpha", material.getAlpha());
    setUniformValue("heightScale", material.getHeightScale());
    
    // Apply the texture
    if (material.getDiffuseTexture() != nullptr)
        material.getDiffuseTexture()->bind(COLOR_TEXTURE_UNIT);
    if (material.getNormalTexture() != nullptr)
        material.getNormalTexture()->bind(NORMAL_TEXTURE_UNIT);
    if (material.getBumpTexture() != nullptr)
        material.getBumpTexture()->bind(BUMP_TEXTURE_UNIT);
    setUniformValue("diffuseSampler", COLOR_TEXTURE_UNIT);
    setUniformValue("normalSampler",  NORMAL_TEXTURE_UNIT);
    setUniformValue("depthSampler",   BUMP_TEXTURE_UNIT);
    for (unsigned int i = 0; i < NUM_CASCADES; i++) {
        char name[128] = {0};
        snprintf(name, sizeof(name), "shadowMap[%d]", i);
        setUniformValue(name, SHADOW_TEXTURE_UNITS[i]);
    }
}


void ObjectShader::setMatrixUniforms(
    const QMatrix4x4 & M, const QMatrix4x4 & V, const QMatrix4x4 & P, 
    const QMatrix4x4 lVP[]
) {
    QMatrix4x4 MV = V*M;
    QMatrix4x4 MVP = P * MV;
    QMatrix3x3 N = MV.normalMatrix();

    // Set matrices uniform
    setUniformValue("M", M);
    setUniformValue("MV", MV);
    setUniformValue("MVP", MVP);
    setUniformValue("N", N);
    
    // Set camera position uniform
    QVector3D cameraPosition(V.inverted().column(3));
    setUniformValue("cameraPosition", 
        QVector3D(V*QVector4D(cameraPosition))
    );
    
    // Set light transform uniform for shadow mapping for all cascades
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning() << __FILE__ << __LINE__ <<
                      "Requires a valid current OpenGL context. \n" <<
                      "Unable to draw the object.";
        return;
    }
    QOpenGLFunctions * glFunctions = context->functions();
    for(unsigned int i = 0; i < NUM_CASCADES; i++) {
        // Find the location of the uniform
        char name[128] = {0};
        snprintf(name, sizeof(name), "lMVP[%d]", i);
        GLuint location = glFunctions->glGetUniformLocation(
            programId(), name
        );
        // Set uniform
        QMatrix4x4 lMVP = lVP[i] * M;
        glFunctions->glUniformMatrix4fv(location, 1, GL_FALSE, lMVP.constData());
    }
}


void ObjectShader::setLightUniforms(
    const CasterLight & light, const QMatrix4x4 & V
) {
    setUniformValue("lightIntensity", light.getIntensity());
    setUniformValue("lightDirection", V * light.getDirection());
}


void ObjectShader::setCascadeUniforms(
    const std::array<float,NUM_CASCADES+1> & cascades
) {
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning() << __FILE__ << __LINE__ <<
                      "Requires a valid current OpenGL context. \n" <<
                      "Unable to draw the object.";
        return;
    }
    QOpenGLFunctions * glFunctions = context->functions();
    
    GLuint location = glFunctions->glGetUniformLocation(
        programId(), "endCascade"
    );
    glFunctions->glUniform1fv(location, NUM_CASCADES, cascades.data()+1);
}




/***
 *          ____   _      _              _            
 *         / __ \ | |    (_)            | |           
 *        | |  | || |__   _   ___   ___ | |_          
 *        | |  | || '_ \ | | / _ \ / __|| __|         
 *        | |__| || |_) || ||  __/| (__ | |_          
 *         \____/ |_.__/ | | \___| \___| \__|         
 *       _____  _       _/ |      _                   
 *      / ____|| |     |__/      | |                  
 *     | (___  | |__    __ _   __| |  ___ __      __  
 *      \___ \ | '_ \  / _` | / _` | / _ \\ \ /\ / /  
 *      ____) || | | || (_| || (_| || (_) |\ V  V /   
 *     |_____/ |_| |_| \__,_| \__,_| \___/  \_/\_/    
 *             _____  _                 _             
 *            / ____|| |               | |            
 *           | (___  | |__    __ _   __| |  ___  _ __ 
 *            \___ \ | '_ \  / _` | / _` | / _ \| '__|
 *            ____) || | | || (_| || (_| ||  __/| |   
 *           |_____/ |_| |_| \__,_| \__,_| \___||_|   
 *                                                    
 *                                                    
 */

void ObjectShadowShader::setMaterialUniforms(const Material & /*material*/) {
    // Nothing to do: no need to apply material to render the shadow 
    // frame buffer.
}


void ObjectShadowShader::setMatrixUniforms(
    const QMatrix4x4 & M, const QMatrix4x4 & /*V*/, const QMatrix4x4 & /*P*/, 
    const QMatrix4x4 lVP[]
) {
    QMatrix4x4 lMVP = lVP[0] * M;
    
    setUniformValue("lMVP", lMVP);
}


void ObjectShadowShader::setLightUniforms(
    const CasterLight & /*light*/, const QMatrix4x4 & /*V*/
) {
    // Nothing to do
}

void ObjectShadowShader::setCascadeUniforms(
    const std::array<float,NUM_CASCADES+1> & cascades
) {
    // Nothing to do
    (void)cascades;
}






