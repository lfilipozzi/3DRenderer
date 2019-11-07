#include "../include/scene.h"
#include <iostream>


Scene::Scene(int refreshRate) : 
    m_surface(6000.0f, 6000.0f,
              QVector3D(0.0f, 0.0f, 0.0f),
              QVector3D(1.0f, 0.0f, 0.0f), 
              QVector3D(0.0f, 1.0f, 0.0f), 
              10.0f),
    m_frame(QVector3D(0.0f, 0.0f, 1.0f)),
    m_screenWidth(1200),
    m_screenHeight(900),
    SHADOW_WIDTH(1024),
    SHADOW_HEIGHT(1024), 
    m_timestepBegin(0.0f),
    m_timestepEnd(std::numeric_limits<float>::max()),
    m_refreshRate(refreshRate),
    m_timeRate(1.0f),
    m_frameRate(1 / static_cast<float>(m_refreshRate)),
    m_enableLoop(true),
    m_showGlobalFrame(false) {
    m_camera = Camera(0.0f, 0.0f,QVector3D(0.0f, 0.0f, 0.0f));
    m_timestep = 0.0f;
    
    // Initialize the surface and the vehicle
    m_vehicle = new Vehicle_GL33(QString("asset/SimulationData/14DoF.txt"));
    // TODO check path to simulation data file is valid
    
    // Get the simulation duration from the vehicle trajectory
    m_timestepBegin = m_vehicle->getInitialTime();
    m_timestepEnd = m_vehicle->getFinalTime();
}


Scene::~Scene() {
    delete(m_vehicle);
}


void Scene::initialize() {
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
    m_vehicle->initialize();
    m_frame.initialize();
}


void Scene::setupLightingAndMatrices() {
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


void Scene::resize(int w, int h) {
    m_screenWidth = w;
    m_screenHeight = h;
    glViewport(0, 0, w, h);

    m_projection.setToIdentity();
    m_projection.perspective(60.0f, static_cast<float>(w)/h, .3f, 1000);
}


void Scene::update() {
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
    m_vehicle->renderShadow(lightSpaceMatrix);
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Release the shadow FBO
    
    // Render the scene with shadow mapping
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    // Get view matrix of the camera
    m_view = m_camera.getViewMatrix();
        
    // Bind the shadow map to texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_shadowFBO);
    
    // Call update method of object in the scene
    m_skybox.update(m_light, m_view, m_projection, lightSpaceMatrix);
    if (m_showGlobalFrame) 
        m_frame.update(m_light, m_view, m_projection, lightSpaceMatrix);
    m_surface.update(m_light, m_view, m_projection, lightSpaceMatrix);
    m_vehicle->update(m_light, m_view, m_projection, lightSpaceMatrix);
        
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


void Scene::cleanup() {
    m_skybox.cleanup();
    m_surface.cleanup();
    m_vehicle->cleanup();
    m_frame.cleanup();
}


void Scene::updateTimestep() {
    // Update the timestep
    m_timestep += m_frameRate * m_timeRate;

    // Restart the animation if necessary
    if (m_enableLoop && m_timestep > m_timestepEnd)
        m_timestep = m_timestepBegin;

    // Make sure the timestep is between the min and max value
    m_timestep = std::max(m_timestepBegin, std::min(m_timestep, m_timestepEnd));
}


void Scene::updateCamera() {
    // Update camera based of user inputs (this is done here rather than
    // in the scene class to prevent writing this code for every scene).

    // Handle translations
    m_camera.processKeyboard();

    // Handle rotations
    m_camera.processMouseMovement();
}


void Scene::playPauseAnimation() {
    if (m_frameRate == 0.0f)
        m_frameRate = 1 / static_cast<float>(m_refreshRate);
    else
        m_frameRate = 0.0f;
}


void Scene::restartAnimation() {
    m_timestep = m_timestepBegin;
}


void Scene::goEndAnimation() {
    m_frameRate = 0.0f;
    m_timestep = m_timestepEnd;
}







