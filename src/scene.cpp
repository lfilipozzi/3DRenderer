#include "../include/scene.h"


/***
 *       _____                     
 *      / ____|                    
 *     | (___   ___ ___ _ __   ___ 
 *      \___ \ / __/ _ \ '_ \ / _ \
 *      ____) | (_|  __/ | | |  __/
 *     |_____/ \___\___|_| |_|\___|
 *                                 
 *                                 
 */

Scene::Scene(unsigned int refreshRate, QString envFile) : 
    m_camera(0.0f, 0.0f,QVector3D(0.0f, 0.0f, 0.0f)),
    m_frame(QVector3D(0.0f, 0.0f, 1.0f)),
    m_timestep(0.0f),
    m_refreshRate(refreshRate),
    m_timeRate(1.0f),
    m_frameRate(1 / static_cast<float>(m_refreshRate)),
    m_loop(true),
    m_showGlobalFrame(false),
    m_envFile(envFile) {}


Scene::~Scene() {}


void Scene::initialize() {
    // Set up the scene light
    QVector4D lightDirection(1.0f,-1.0f, -1.0f, 0.0f);
    QVector3D lightIntensity(1.0f, 1.0f, 1.0f);
    m_light = CasterLight(lightIntensity, lightDirection);

    // Set up the skybox
    m_skybox.initialize();
    m_frame.initialize();
    
    // Load the objects of the environment
    Loader loader;
    loader.parse(m_envFile);
    m_elements = loader.getObjects();
    
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
    for (auto it = m_elements.begin(); it != m_elements.end(); it++) {
        if (*it !=nullptr) {
            (*it)->render(
                m_light, m_view, m_projection, m_lightSpace, m_cascades
            );
        }
    }
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
    for (auto it = m_elements.begin(); it != m_elements.end(); it++) {
        if (*it !=nullptr) {
            (*it)->renderShadow(m_lightSpace.at(cascadeIdx));
        }
    }
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



/***
 *       _____                         
 *      / ____|                        
 *     | (___   ___ ___ _ __   ___     
 *      \___ \ / __/ _ \ '_ \ / _ \    
 *      ____) | (_|  __/ | | |  __/    
 *     |_____/ \___\___|_| |_|\___|    
 *      | |               | |          
 *      | | ___   __ _  __| | ___ _ __ 
 *      | |/ _ \ / _` |/ _` |/ _ \ '__|
 *      | | (_) | (_| | (_| |  __/ |   
 *      |_|\___/ \__,_|\__,_|\___|_|   
 *                                     
 *                                     
 */

#include <QtXml>
#include <QFile>
#include <QDebug>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

Scene::Loader::Loader() {}

Scene::Loader::~Loader() {}

void Scene::Loader::parse(QString const fileNameIn) {
    QString defaultFile(":/xml/defaultWorld");
    QString fileName = fileNameIn;
    if (fileNameIn.isEmpty()) {
        // Use default file without warning.
        fileName = defaultFile;
    }
    else if (!QFile::exists(fileNameIn)) {
        qWarning() << 
            "The file" << fileName << "does not exist. Use default file" <<
            defaultFile << "instead.";
            fileName = defaultFile;
    }
    
    QDomDocument domDoc;
    // Load XML file as raw data
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        // Error while loading file
        qWarning() << "Error while loading file" << fileName;
    }
    
    // Retrieve the XML schema
    QXmlSchema schema;
    QUrl schemaUrl = QUrl::fromLocalFile("resources/xml/world.xsd");
    if (!schema.load(schemaUrl)) {
        qDebug() << "Cannot load XSD schema. The XML file will not be parsed.";
        return;
    }
    // The XSD resource file cannot be invalid
    if (!schema.isValid()) {
        qCritical() << "The  XML schema (.xsd) is invalid. The XML file will"
            "not be parsed.";
        return;
    }

    // Validate the user file
    QXmlSchemaValidator validator{schema};
    if (!validator.validate(&file, QUrl::fromLocalFile(file.fileName()))) {
        qCritical() << "The file" << fileName << "does not meet the XML "
        "schema definition. The XML will not be parsed.";
        return;
    }
    
    // Set data into the QDomDocument before processing
    file.reset();
    domDoc.setContent(&file);
    file.close();
    
    // Extract the root
    QDomElement world = domDoc.documentElement();
    
    for (
        QDomElement shape = world.firstChildElement(); 
        !shape.isNull();
        shape = shape.nextSiblingElement()
    ) {
        // Process object (first element according to the DTD)
        QDomElement elmt = shape.firstChildElement();
        ABCObject * model = processObject(elmt);
        if (model == nullptr) 
            continue;
        p_objects.push_back(model);
        
        // Process transform
        elmt = elmt.nextSiblingElement();
        if (!elmt.isNull()) {
            QMatrix4x4 matrix;
            if (processTransform(elmt, matrix)) {
                model->setModelMatrix(matrix);
            }
        }
    }
}

bool Scene::Loader::qStringToQVector3D(const QString & string, QVector3D & vec) {
    QStringList list = string.split(" ");
    if (list.size() != 3)
        return false;
    bool ok;
    float x = list[0].toFloat(&ok); if (!ok) return false;
    float y = list[1].toFloat(&ok); if (!ok) return false;
    float z = list[2].toFloat(&ok); if (!ok) return false;
    vec = QVector3D(x, y, z);
    return true;
}


bool Scene::Loader::qStringToQVector4D(const QString & string, QVector4D & vec) {
    QStringList list = string.split(" ");
    if (list.size() != 4)
        return false;
    bool ok;
    float x = list[0].toFloat(&ok); if (!ok) return false;
    float y = list[1].toFloat(&ok); if (!ok) return false;
    float z = list[2].toFloat(&ok); if (!ok) return false;
    float w = list[3].toFloat(&ok); if (!ok) return false;
    vec = QVector4D(x, y, z, w);
    return true;
}


ABCObject *  Scene::Loader::processObject(const QDomElement& elmt) {
    if (elmt.tagName().compare("plane") == 0) {
        return processPlane(elmt);
    }
    else if (elmt.toElement().tagName().compare("model") == 0) {
        return processModel(elmt);
    }
//     else if (elmt.toElement().tagName().compare("node") == 0) {
//         return processNode(elmt);
//     }
    return nullptr;
}


ABCObject *  Scene::Loader::processModel(const QDomElement & elmt) {
    if (elmt.tagName().compare("model") != 0)
        return nullptr;
    
    // Retrieve name
    QString name = elmt.attribute("name","");
    if (name.isEmpty()) {
        qWarning() << 
            "The attribute 'name' of the element 'model' must be provided. "
            "The object will not be rendered.";
        return nullptr;
    }
    
    // Check that the ID of the plan is unique
    if (ObjectManager::getObject(name) != nullptr) {
        qWarning() << 
            "An object with name" << name << "already exists. The object will "
            "not be rendered.";
        return nullptr;
    }
    
    // Retrieve attributes
    QString fileName = elmt.attribute("url","");
    QString textureDir = elmt.attribute("textureFolder","");
    
    // Build object and add it to the container
    ABCObject * model = nullptr;
    Object::Loader modelLoader(fileName, textureDir);
    if (modelLoader.build()) {
        model = ObjectManager::loadObject(name, modelLoader.getObject());
        return model;
        
    }
    return nullptr;
}


ABCObject *  Scene::Loader::processPlane(const QDomElement & elmt) {
    if (elmt.tagName().compare("plane") != 0)
        return nullptr;
    
    // Retrieve name
    QString name = elmt.attribute("name","");
    if (name.isEmpty()) {
        qWarning() << 
            "The attribute 'name' of the element 'plane' must be provided. "
            "The object will not be rendered.";
        return nullptr;
    }
    
    // Check that the ID of the plan is unique
    if (ObjectManager::getObject(name) != nullptr) {
        qWarning() << 
            "An object with name" << name << "already exists. The object will "
            "not be rendered.";
        return nullptr;
    }
    
    // Retrieve attributes
    float length = elmt.attribute("length","100").toFloat();
    float width = elmt.attribute("width","100").toFloat();
    QString originString = elmt.attribute("origin","0 0 0");
    QString longAxisString = elmt.attribute("longAxis","1 0 0");
    QString latAxisString = elmt.attribute("latAxis","0 1 0");
    QVector3D origin, longAxis, latAxis;
    if (!qStringToQVector3D(originString, origin)) {
        origin = QVector3D(0, 0, 0);
    }
    if (!qStringToQVector3D(longAxisString, longAxis)) {
        longAxis = QVector3D(1, 0, 0);
    }
    if (!qStringToQVector3D(latAxisString, latAxis)) {
        latAxis = QVector3D(0, 1, 0);
    };
    float textureGridSize = elmt.attribute("textureSize","5.0").toFloat();
    
    // Build object and add it to the container
    ABCObject * plane = nullptr;
    Object::FlatSurfaceBuilder planeBuilder(
        length, width, origin, longAxis, latAxis, textureGridSize
    );
    if (planeBuilder.build()) {
        plane = ObjectManager::loadObject(name, planeBuilder.getObject());
        return plane;
    }
    return nullptr;
}


bool  Scene::Loader::processTransform(
    const QDomElement & elmt, QMatrix4x4 & model
) {
    if (elmt.tagName().compare("transform") != 0)
        return false;
    
    QString transString = elmt.attribute("translation","0.0 0.0 0.0");
    QVector3D trans;
    if (!qStringToQVector3D(transString, trans))
        trans = QVector3D(0, 0, 0);
    
    QString rotString = elmt.attribute("rotation", "0.0 0.0 0.0 0.0");
    QVector4D rot;
    if (!qStringToQVector4D(rotString, rot))
        rot = QVector4D(1, 0, 0, 0);
    
    QString scaleString = elmt.attribute("scale", "1.0 1.0 1.0");
    QVector3D scale;
    if (!qStringToQVector3D(scaleString, scale))
        scale = QVector3D(1, 1, 1);
    
    model.setToIdentity();
    model.translate(trans);
    model.rotate(QQuaternion(rot));
    model.scale(scale);
    return true;
}










