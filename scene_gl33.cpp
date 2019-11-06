#include "scene_gl33.h"
#include <iostream>

Scene_GL33::Scene_GL33(int refreshRate) : AbstractScene (refreshRate),
    m_surface(6000.0f, 6000.0f,
              QVector3D(0.0f, 0.0f, 0.0f),
              QVector3D(1.0f, 0.0f, 0.0f), 
              QVector3D(0.0f, 1.0f, 0.0f), 
              10.0f),
    m_frame(QVector3D(0.0f, 0.0f, 1.0f)),
    m_screenWidth(1200),
    m_screenHeight(900),
    SHADOW_WIDTH(1024),
    SHADOW_HEIGHT(1024) {
    // Initialize the surface and the vehicle
    m_vehicle = new Vehicle_GL33(QString("../SimulationData/14DoF.txt"));
    
    // Get the simulation duration from the vehicle trajectory
    m_timestepBegin = m_vehicle->getInitialTime();
    m_timestepEnd = m_vehicle->getFinalTime();
}

Scene_GL33::~Scene_GL33() {
    delete(m_vehicle);
}

void Scene_GL33::initialize() {
    initializeOpenGLFunctions();
    
    // Create the frame buffer and texture for shadow mapping
    glGenTextures(1, &m_shadowDepthMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
    // Attach the depth texture to the FBO
    glGenFramebuffers(1, &m_shadowFBO); 
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    
    setupLightingAndMatrices();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_skybox.initialize();
    m_surface.initialize();
//     m_vehicle->initialize();
    m_frame.initialize();
    
    #ifdef SHADOW_FBO_DEBUG
    // Compile vertex shader
    QString vShader(":/shaders/debugQuad.vert");
    QString fShader(":/shaders/debugQuad.frag");
    if (!m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, vShader.toUtf8()))
        qCritical() << "Unable to compile vertex shader. Log:" << m_shaderProgram.log();

    // Compile fragment shader
    if (!m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, fShader.toUtf8()))
        qCritical() << "Unable to compile fragment shader. Log:" << m_shaderProgram.log();

    // Link the shaders together into a program
    if (!m_shaderProgram.link())
        qCritical() << "Unable to link shader program. Log:" << m_shaderProgram.log();
    
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
    m_vertexBuffer.allocate(&(vertices)[0], vertices.size() * static_cast<int>(sizeof(float)));

    // Create a buffer and copy the vertex data to it
    m_textureUVBuffer.create();
    m_textureUVBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_textureUVBuffer.bind();
    m_textureUVBuffer.allocate(&(textureUV)[0], textureUV.size() * static_cast<int>(sizeof(float)));
    
    m_vao.bind();
    m_shaderProgram.bind();

    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    m_shaderProgram.enableAttributeArray(0);       // layout location
    m_shaderProgram.setAttributeBuffer(0,          // layout location
                                       GL_FLOAT,   // data's type
                                       0,          // Offset to data in buffer
                                       3);         // number of components

    if(!m_textureUVBuffer.isCreated())
        return;
    m_textureUVBuffer.bind();
    m_shaderProgram.enableAttributeArray(1);       // layout location
    m_shaderProgram.setAttributeBuffer(1,          // layout location
                                       GL_FLOAT,   // data's type
                                       0,          // Offset to data in buffer
                                       2);         // number of components
    #endif // SHADOW_FBO_DEBUG
}

void Scene_GL33::setupLightingAndMatrices() {
    m_view = m_camera.getViewMatrix();

    float aspect = 4.0f/3.0f;
    m_projection.setToIdentity();
    m_projection.perspective(
                60.0f,          // field of vision
                aspect,         // aspect ratio
                0.3f,           // near clipping plane
                1000.0f);       // far clipping plane

    QVector4D lightDirection(1.0f,-1.0f, -1.0f, 0.0f);
    QVector3D lightIntensity(1.0f, 1.0f, 1.0f);
    m_light = CasterLight(lightIntensity, lightDirection);
}

void Scene_GL33::resize(int w, int h) {
    m_screenWidth = w;
    m_screenHeight = h;
    glViewport(0, 0, w, h);

    m_projection.setToIdentity();
    m_projection.perspective(60.0f, static_cast<float>(w)/h, .3f, 1000);
}

void Scene_GL33::update() {
    // Get the vehicle position and update the model matrices of the vehicle
    Position chassisPosition = m_vehicle->getChassisPosition(m_timestep);
    m_vehicle->updateModelMatrices(m_timestep);
    // Update camera to follow the vehicle
    m_camera.trackObject(chassisPosition);

    // Render depth map for shadow mapping
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO); // Bind the shadow FBO
    glClear(GL_DEPTH_BUFFER_BIT);
        
    // Compute view and projection matrices of the light source
    QMatrix4x4 lightSpaceMatrix = 
        m_light.getLightSpaceMatrix(chassisPosition.linearPosition);
        
    // Render scene to compute the shadow map
    m_surface.renderShadow(lightSpaceMatrix);
//     m_vehicle->renderShadow(lightSpaceMatrix);
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Release the shadow FBO
    
    // Render the scene with shadow mapping
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    // Get view matrix of the camera
    m_view = m_camera.getViewMatrix();
        
    // Bind the shadow map to texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_shadowFBO);

    #ifndef SHADOW_FBO_DEBUG    // Render scene
    
    // Call update method of object in the scene
    m_skybox.update(m_light, m_view, m_projection, lightSpaceMatrix);
    if (m_showGlobalFrame) 
        m_frame.update(m_light, m_view, m_projection, lightSpaceMatrix);
    m_surface.update(m_light, m_view, m_projection, lightSpaceMatrix);
//     m_vehicle->update(m_light, m_view, m_projection, lightSpaceMatrix);
        
    #else   // Draw shadow map on a quad for debug
    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("depthMap", 1);
    m_shaderProgram.setUniformValue("near_plane", 1);
    m_shaderProgram.setUniformValue("far_plane", 1);
        
    m_vao.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_vao.release();
        
    #endif // SHADOW_FBO_DEBUG
        
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM"; 
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE"; 
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION"; 
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW"; 
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW"; 
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY"; 
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION"; 
                break;
        }
        std::cout << "OpenGL error:" << error << std::endl;
    }
        
    // Update time-step
    updateTimestep();
}

void Scene_GL33::cleanup() {
    m_skybox.cleanup();
    m_surface.cleanup();
//     m_vehicle->cleanup();
    m_frame.cleanup();
}









