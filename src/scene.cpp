#include "../include/scene.h"


Scene::Scene(unsigned int refreshRate) : 
    m_camera(0.0f, 0.0f,QVector3D(0.0f, 0.0f, 0.0f)),
    m_frame(QVector3D(0.0f, 0.0f, 1.0f)),
    m_timestep(0.0f),
    m_refreshRate(refreshRate),
    m_timeRate(1.0f),
    m_frameRate(1 / static_cast<float>(m_refreshRate)),
    m_loop(true),
    m_showGlobalFrame(false) {}


Scene::~Scene() {}


void Scene::initialize() {
    // Set up the scene light
    QVector4D lightDirection(1.0f,-1.0f, -1.0f, 0.0f);
    QVector3D lightIntensity(1.0f, 1.0f, 1.0f);
    m_light = CasterLight(lightIntensity, lightDirection);

    // Set up the skybox
    m_skybox.initialize();
    m_frame.initialize();
    
    // Load the chassis model
    ABCObject * chassis = nullptr;
    Object::Loader chassisLoader(
        "asset/Models/Cars/Mustang_GT/mustangChassis.obj",
        "asset/Models/Cars/Mustang_GT/"
    );
    if (chassisLoader.build()) {
        chassis = ObjectManager::loadObject(
            "chassis", chassisLoader.getObject()
        );
    }
    
    // Load the wheel model
    ABCObject * wheel = nullptr;
    Object::Loader wheelLoader(
        "asset/Models/Cars/Mustang_GT/mustangWheel.obj",
        "asset/Models/Cars/Mustang_GT/"
    );
    if (wheelLoader.build()) {
        wheel = ObjectManager::loadObject(
            "wheel", wheelLoader.getObject()
        );
    }
    
    // Load the surface 
    ABCObject * surface = nullptr;
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
    
    // Load line
    ABCObject * line = nullptr;
    line = ObjectManager::loadObject(
        "line", 
        std::make_unique<Line>(
            QVector<QVector3D>({
            QVector3D(0, 0, 0), QVector3D(1, 0, 0), 
            QVector3D(0, 1, 0), QVector3D(0, 0, 1)}),
            QVector<unsigned int>({0, 1, 0, 2, 0, 3}), 
            QVector3D(0.0f, 0.0f, 1.0f)
        )
    );
    
    ObjectManager::initialize();
    
    // Create the vehicle
    p_vehicle = std::make_unique<Vehicle>(
        chassis, wheel, line, "asset/SimulationData/data.txt"
    );
    
    // Get the simulation duration from the vehicle trajectory
    if (p_vehicle != nullptr) {
        m_firstTimestep = p_vehicle->getFirstTimeStep();
        m_finalTimestep = p_vehicle->getFinalTimeStep();
    }
}


void Scene::resize(int w, int h) {
    m_camera.setAspectRatio(static_cast<float>(w)/h);
}


void Scene::update() {
    // Get the vehicle position
    Position vehiclePosition;
    if (p_vehicle != nullptr) {
        vehiclePosition = p_vehicle->getPosition(m_timestep);
        p_vehicle->updatePosition(m_timestep);
    }
    
    // Update camera
    m_camera.trackObject(vehiclePosition);
    m_camera.processKeyboard();
    m_camera.processMouseMovement();
    m_camera.updateAxes();
    
    // Update the camera view matrix
    m_view = m_camera.getViewMatrix();
    // Update the camera projection matrix
    m_projection = m_camera.getProjectionMatrix();
    
    // Compute view and projection matrices of the light source
    m_cascades = {-0.3f, -10.0f, -30.0f, -60.0f};
//     m_view = m_light.getViewMatrix();
//     m_projection = m_light.getProjectionMatrix(m_camera, m_cascades).at(2);
    m_lightSpace = m_light.getLightSpaceMatrix(m_camera, m_cascades);
}


void Scene::render() {
    // Compute the end of each cascade in the clip space
    std::vector<float> endCascadeClip;
    for (unsigned int i = 0; i < m_cascades.size() - 1; i++) {
        QVector4D v(0.0f, 0.0f, m_cascades[i+1], 1.0f);
        v = m_projection * v;
        endCascadeClip.push_back(v.z());
    }
    
    // Call the render method of object in the scene
    m_skybox.render(m_view, m_projection);
    if (p_surface != nullptr)
        p_surface->render(
            m_light, m_view, m_projection, m_lightSpace, m_cascades
        );
    if (p_vehicle != nullptr)
        p_vehicle->render(
            m_light, m_view, m_projection, m_lightSpace, m_cascades
        );
    if (m_showGlobalFrame) {
        m_frame.setModelMatrix(QMatrix4x4());
        m_frame.render(m_light, m_view, m_projection, m_lightSpace, m_cascades);
    }
}


void Scene::renderShadow(unsigned int cascadeIdx) {
    // Render the shadow map
    if (p_surface != nullptr)
        p_surface->renderShadow(m_lightSpace.at(cascadeIdx));
    if (p_vehicle != nullptr)
        p_vehicle->renderShadow(m_lightSpace.at(cascadeIdx));
}


void Scene::cleanUp() {
    m_skybox.cleanUp();
    m_frame.cleanup();
    ObjectManager::cleanUp();
    TextureManager::cleanUp();
}


void Scene::updateTimestep() {
    // Update the timestep
    m_timestep += m_frameRate * m_timeRate;

    // Restart the animation if necessary
    if (m_loop && m_timestep > m_finalTimestep)
        m_timestep = m_firstTimestep;

    // Make sure the timestep is between the min and max value
    m_timestep = std::max(
        m_firstTimestep, std::min(m_timestep, m_finalTimestep)
    );
}







