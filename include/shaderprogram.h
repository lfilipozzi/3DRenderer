#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QString>
#include "material.h"


/// Shader program
/**
 * @brief This class defines a shader program from the source files of the 
 * vertex and fragment shaders.
 * @author Louis Filipozzi
 */
class Shader : public QOpenGLShaderProgram {
public:
    /**
     * @brief Constructor of the shader program.
     * @param vShader The path to the source file of the vertex shader.
     * @param fShader The path to the source file of the fragment shader.
     */
    Shader(QString vShader, QString fShader);
    virtual ~Shader() {};
};



/// Object shader
/**
 * @brief Defines a shader to render a 3D object in the scene.
 * @author Louis Filipozzi
 */
class ObjectShader : public Shader {
public:
    /**
     * @brief Constructor of the shader program.
     * @param vShader The path to the source file of the vertex shader.
     * @param fShader The path to the source file of the fragment shader.
     */
    ObjectShader(QString vShader, QString fShader)
    : Shader(vShader, fShader) {};
    virtual ~ObjectShader() {};
    
    /**
     * @brief Set the uniforms in OpenGL defining the material.
     * @param material The material to apply.
     */
    virtual void setMaterialUniforms(const Material * material);
    
    /**
     * @brief Set the matrix uniforms in OpenGL.
     * @param M The model matrix.
     * @param V The view matrix.
     * @param P The projection matrix.
     * @param lVP The light transform matrix. This correspond to the product of 
     * the light projection matrix by the light view matrix.
     */
    virtual void setMatrixUniforms(const QMatrix4x4 & M, 
                                   const QMatrix4x4 & V, 
                                   const QMatrix4x4 & P, 
                                   const QMatrix4x4 & lVP);
};



/// Object shader for shadow mapping
/**
 * @brief Defines a shader to render a 3D object in the scene for shadow
 * mapping.
 * @author Louis Filipozzi
 */
class ObjectShadowShader : public ObjectShader {
public:
    /**
     * @brief Constructor of the shader program.
     * @param vShader The path to the source file of the vertex shader.
     * @param fShader The path to the source file of the fragment shader.
     */
    ObjectShadowShader(QString vShader, QString fShader)
    : ObjectShader(vShader, fShader) {};
    virtual ~ObjectShadowShader() {};
    
    /**
     * @overload
     * @brief Set the uniforms in OpenGL defining the material.
     * @param material The material to apply.
     * @remark This function does not set any uniform as the material is not 
     * used when computing the shadow map.
     */
    virtual void setMaterialUniforms(const Material * material);
    
    /**
     * @overload
     * @brief Set the matrix uniforms in OpenGL.
     * @param M The model matrix.
     * @param V The view matrix.
     * @param P The projection matrix.
     * @param lVP The light transform matrix. This correspond to the product of 
     * the light projection matrix by the light view matrix.
     */
    virtual void setMatrixUniforms(const QMatrix4x4 & M, 
                                   const QMatrix4x4 & V, 
                                   const QMatrix4x4 & P, 
                                   const QMatrix4x4 & lVP);
};

#endif // SHADERPROGRAM_H
