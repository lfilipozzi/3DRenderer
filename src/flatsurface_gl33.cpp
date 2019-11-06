#include "../include/flatsurface_gl33.h"

#include <QImage>

FlatSurface_GL33::FlatSurface_GL33(float length, float width,
                                   QVector3D origin, 
                                   QVector3D longitudinalAxis,
                                   QVector3D lateralAxis, 
                                   float textureGridSize) : 
    m_textureSize(textureGridSize), 
    m_surfaceLength(length),
    m_surfaceWidth(width),
    m_origin(origin),
    m_longitudinalSurface(longitudinalAxis),
    m_lateralSurface(lateralAxis),
    m_normalSurface(QVector3D::crossProduct(longitudinalAxis, lateralAxis)) {
    setBufferAndTextureData();
}

FlatSurface_GL33::~FlatSurface_GL33() {
    
}

void FlatSurface_GL33::initialize() {
    initializeOpenGLFunctions();

    createShaderProgram(":/shaders/complex.vert", ":/shaders/complex.frag");
    createShaderProgramForShadowMapping();

    createBuffers();
    createAttributes();
    setupMaterialInfo(QVector3D(0.5f, 0.5f, 0.5f), QVector3D(.6f, .6f, .6f), 
                      QVector3D(.2f, .2f, .2f), 50.0f, 1.0f,
                      QString("../Texture/RoadMaterials/MyRoad/Road_texture.png")
    );
}

void FlatSurface_GL33::getBufferAndTextureData(QVector<float> *&vertices, 
                                 QVector<float> *&normals,
                                 QVector<QVector<float>> *&textureUV,
                                 QVector<unsigned int> *&indices) { 
    vertices = &m_verticesData;
    normals = &m_normalsData;
    textureUV = &m_textureUVData;
    indices = &m_indicesData;
}

void FlatSurface_GL33::setBufferAndTextureData() { 
    // Define vertices
    QVector3D cornerRL = m_origin-m_surfaceWidth/2*m_lateralSurface +
            -m_surfaceLength/2*m_longitudinalSurface;
    QVector3D cornerRR = m_origin+m_surfaceWidth/2*m_lateralSurface +
            -m_surfaceLength/2*m_longitudinalSurface;
    QVector3D cornerFL = m_origin-m_surfaceWidth/2*m_lateralSurface +
             m_surfaceLength/2*m_longitudinalSurface;
    QVector3D cornerFR = m_origin+m_surfaceWidth/2*m_lateralSurface +
             m_surfaceLength/2*m_longitudinalSurface;
    
    m_verticesData.append(
        QVector<float>({
            // RL corner
            cornerRL.x(), cornerRL.y(), cornerRL.z(),
            // RR corner
            cornerRR.x(), cornerRR.y(), cornerRR.z(),
            // FL corner
            cornerFL.x(), cornerFL.y(), cornerFL.z(),
            // FR corner
            cornerFR.x(), cornerFR.y(), cornerFR.z()
        })
    );
    
    // Define texture UV coordinates
    m_textureUVData.append(   // x channel
            QVector<float>({
            // RL corner
            0.0f, 0.0f,
            // RR corner
            m_surfaceWidth/m_textureSize, 0.0f,
            // FL corner
            0.0f, m_surfaceLength/m_textureSize,
            // RR corner
            m_surfaceWidth/m_textureSize, m_surfaceLength/m_textureSize
        })
    );

    // Define normals
    for (int i = 0; i < 4; i++) {
        m_normalsData.append(QVector<float>({
            m_normalSurface.x(),
            m_normalSurface.y(),
            m_normalSurface.z()
        }));
    }

    // Define indices
    // Create triangle from corners RL, RR, and FL
    m_indicesData.append(QVector<unsigned int>({0, 1, 2}));
    // Create triangle from corners FL, RR, and FR
    m_indicesData.append(QVector<unsigned int>({2, 1, 3}));
}

void FlatSurface_GL33::update(const CasterLight &light, 
                              const QMatrix4x4 view,
                              const QMatrix4x4 projection, 
                              const QMatrix4x4 lightSpaceMatrix) {
    // If an error occured, do nothing
    if(m_error)
        return;
    
    // Bind shader program
    m_shaderProgram.bind();

    // Set shader uniforms for light information
    m_shaderProgram.setUniformValue("lightIntensity", light.getIntensity());
    // Downcast and set the uniform as required
    m_shaderProgram.setUniformValue("lightDirection", 
                                    view * light.getDirection());
    
    // Set shader uniform for skybox and camera position
    QVector3D cameraPosition(view.inverted().column(3));
    m_shaderProgram.setUniformValue("skybox", 2);
    m_shaderProgram.setUniformValue("cameraPosition", cameraPosition);
    
    
    // Set shader uniforms for transformation matrices
    QMatrix4x4 modelViewMatrix = view * m_model;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = projection * modelViewMatrix;
    QMatrix4x4 lightMVP = lightSpaceMatrix * m_model;
    m_shaderProgram.setUniformValue("M", m_model);
    m_shaderProgram.setUniformValue("MV", modelViewMatrix);
    m_shaderProgram.setUniformValue("MVP", mvp);
    m_shaderProgram.setUniformValue("lightMVP", lightMVP);
    m_shaderProgram.setUniformValue("N", normalMatrix);
    
    // Set shader uniforms for material information
    setMaterialUniforms(m_materialInfo);
    
    // Bind VAO and the surface
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(6), GL_UNSIGNED_INT,
                        reinterpret_cast<const void*>(0));
    m_vao.release();
}

void FlatSurface_GL33::renderShadow(const QMatrix4x4 lightSpaceMatrix) {
    // If an error occured, do nothing
    if(m_error)
        return;
    
    // Bind shader program
    m_shaderShadow.bind();
    
    // Set shader uniforms for transformation matrices
    QMatrix4x4 lightMVP = lightSpaceMatrix * m_model;
    m_shaderShadow.setUniformValue("lightMVP", lightMVP);
    
    // Bind VAO and the surface
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(6), GL_UNSIGNED_INT,
                        reinterpret_cast<const void*>(0));
    m_vao.release();
}


void FlatSurface_GL33::cleanup() {

}
