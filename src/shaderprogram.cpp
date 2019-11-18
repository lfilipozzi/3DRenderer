#include "../include/shaderprogram.h"


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
    
    // Apply the texture
    if (material.getTexture() != nullptr)
        material.getTexture()->bind(0);
    setUniformValue("textureSampler", 0);
    setUniformValue("shadowMap", 1);
    setUniformValue("skybox", 2);
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
    setUniformValue("lMVP", lMVP);
    setUniformValue("N", N);
    
    QVector3D cameraPosition(V.inverted().column(3));
    setUniformValue("cameraPosition", cameraPosition);
}


void ObjectShader::setLightUniforms(
    const CasterLight & light, const QMatrix4x4 & V
) {
    setUniformValue("lightIntensity", light.getIntensity());
    setUniformValue("lightDirection", V * light.getDirection());
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


void ObjectShadowShader::setMatrixUniforms(const QMatrix4x4 & M, 
                                           const QMatrix4x4 & /*V*/, 
                                           const QMatrix4x4 & /*P*/, 
                                           const QMatrix4x4 & lVP) {
    QMatrix4x4 lMVP = lVP * M;
    
    setUniformValue("lMVP", lMVP);
}


void ObjectShadowShader::setLightUniforms(
    const CasterLight & /*light*/, const QMatrix4x4 & /*V*/
) {
    // Nothing to do
}








