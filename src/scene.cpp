#include "../include/scene.h"


Scene::Scene(unsigned int refreshRate) : 
    m_frame(QVector3D(0.0f, 0.0f, 1.0f)),
    m_screenWidth(1200),
    m_screenHeight(900),
    m_camera(0.0f, 0.0f,QVector3D(0.0f, 0.0f, 0.0f)),
    m_timestep(0.0f),
    m_timestepBegin(0.0f),
    m_timestepEnd(std::numeric_limits<float>::max()),
    m_refreshRate(refreshRate),
    m_timeRate(1.0f),
    m_frameRate(1 / static_cast<float>(m_refreshRate)),
    m_enableLoop(true),
    m_showGlobalFrame(false) {}


Scene::~Scene() {}


void Scene::initialize() {
    // Get pointer to OpenGL functions
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qCritical() << __FILE__ << __LINE__ <<
            "Requires a valid current OpenGL context. \n" <<
            "Unable to draw the object.";
        exit(1);
    }
    p_glFunctions = context->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!p_glFunctions) {
        qCritical() << __FILE__ << __LINE__ <<
            "Could not obtain required OpenGL context version";
        exit(1);
    }
    
    // Create the frame buffer and texture for shadow mapping
    p_depthMap = std::make_unique<DepthMap>(1024,1024);
    
    setupLightingAndMatrices();

    p_glFunctions->glEnable(GL_DEPTH_TEST);
    p_glFunctions->glEnable(GL_BLEND);
    p_glFunctions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TODO: Use the object manager to initialize all the objects
    m_skybox.initialize();
    m_frame.initialize();
    
    // Load the chassis model
    Object * chassis = nullptr;
    Object::Loader chassisLoader(
        "asset/3DModels/Cars/MustangGT/mustangChassis.obj",
        "asset/3DModels/Cars/MustangGT/"
    );
    if (chassisLoader.build()) {
        chassis = ObjectManager::loadObject(
            "chassis", chassisLoader.getObject()
        );
    }
    
    // Load the wheel model
    Object * wheel = nullptr;
    Object::Loader wheelLoader(
        "asset/3DModels/Cars/MustangGT/mustangWheel.obj",
        "asset/3DModels/Cars/MustangGT/"
    );
    if (wheelLoader.build()) {
        wheel = ObjectManager::loadObject(
            "wheel", wheelLoader.getObject()
        );
    }
    
    // Load the surface 
    Object * surface = nullptr;
    Object::FlatSurfaceBuilder surfaceBuilder(
        6000.0f, 6000.0f, QVector3D(0.0f, 0.0f, 0.0f), 
        QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f), 10.0f
    );
    if (surfaceBuilder.build()) {
        surface = ObjectManager::loadObject(
            "surface", surfaceBuilder.getObject()
        );
    }
    p_surface = surface;
    
    // TODO remove this
    p_line = new Line_GL33(
        QVector<QVector3D>({
        QVector3D(0, 0, 0), QVector3D(1, 0, 0), 
        QVector3D(0, 1, 0), QVector3D(0, 0, 1)}),
        QVector<unsigned int>({0, 1, 0, 2, 0, 3}), 
        QVector3D(0.0f, 0.0f, 1.0f)
    );
    p_line->initialize();
    
    ObjectManager::initialize();
    
    // Create the vehicle
    p_vehicle = std::make_unique<Vehicle>(
        chassis, wheel, p_line, "asset/SimulationData/14DoF.txt"
    );
    
    // Get the simulation duration from the vehicle trajectory
    if (p_vehicle != nullptr) {
        m_timestepBegin = p_vehicle->getFirstTimeStep();
        m_timestepEnd = p_vehicle->getFinalTimeStep();
    }
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
    p_glFunctions->glViewport(0, 0, w, h);

    m_projection.setToIdentity();
    m_projection.perspective(60.0f, static_cast<float>(w)/h, .3f, 1000);
}


void Scene::render() {
    // Get the vehicle position
    Position vehiclePosition;
    if (p_vehicle != nullptr) {
        vehiclePosition = p_vehicle->getPosition(m_timestep);
        p_vehicle->updatePosition(m_timestep);
    }
    
    // Update camera to follow the vehicle
    m_camera.trackObject(vehiclePosition);

    // Render depth map for shadow mapping
    if (p_depthMap != nullptr)
        p_depthMap->bind();
        
    // Compute view and projection matrices of the light source
    QMatrix4x4 lightSpaceMatrix = 
        m_light.getLightSpaceMatrix(
            vehiclePosition.getPoint()
        );
        
    // Render scene to compute the shadow map
    if (p_surface != nullptr)
        p_surface->renderShadow(m_light, m_view, m_projection, lightSpaceMatrix);
    if (p_vehicle != nullptr)
        p_vehicle->renderShadow(m_light, m_view, m_projection, lightSpaceMatrix);
    
    if (p_depthMap != nullptr)
        p_depthMap->release();
    
    // Render the scene with shadow mapping
    p_glFunctions->glViewport(0, 0, m_screenWidth, m_screenHeight);
    p_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    // Get view matrix of the camera
    m_view = m_camera.getViewMatrix();
        
    // Bind the shadow map to texture unit 1
    if (p_depthMap != nullptr)
        p_depthMap->bindTexture(GL_TEXTURE1);
    
    // Call update method of object in the scene
    m_skybox.render(m_view, m_projection);
    if (p_surface != nullptr)
        p_surface->render(m_light, m_view, m_projection, lightSpaceMatrix);
    if (p_vehicle != nullptr)
        p_vehicle->render(m_light, m_view, m_projection, lightSpaceMatrix);
    if (m_showGlobalFrame) 
        m_frame.update(m_light, m_view, m_projection, lightSpaceMatrix);
        
    printOpenGLError();
        
    // Update time-step
    updateTimestep();
}


void Scene::printOpenGLError() {
    GLenum errorCode;
    QString error;
    while ((errorCode = p_glFunctions->glGetError()) != GL_NO_ERROR)
    {
        switch (errorCode)
        {
            case GL_INVALID_ENUM:
                error.append("INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
                error.append("INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
                error.append("INVALID_OPERATION");
                break;
            case GL_STACK_OVERFLOW:
                error.append("STACK_OVERFLOW");
                break;
            case GL_STACK_UNDERFLOW:
                error.append("STACK_UNDERFLOW");
                break;
            case GL_OUT_OF_MEMORY:
                error.append("OUT_OF_MEMORY");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error.append("INVALID_FRAMEBUFFER_OPERATION"); 
                break;
        }
    }
    if (!error.isEmpty())
        qWarning() << __FILE__ << __LINE__ << "OpenGL error:" << error;
}


void Scene::cleanUp() {
    m_skybox.cleanUp();
    m_frame.cleanup();
    
    ObjectManager::cleanUp();
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







