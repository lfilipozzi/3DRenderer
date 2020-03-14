#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include "vehicle.h"
#include "frame.h"
#include "skybox.h"
#include <memory>
#include "camera.h"
#include "object.h"
#include "constants.h"

/// Scene class
/**
 * @brief The scene contains the object to be rendered. It manages all the 
 * elements that will be rendered and update their positions.
 * @author Louis Filipozzi
 */
class Scene {
private:
    class Loader;
    class Node;
    
public:
    Scene(unsigned int refreshRate, QString envFile);

    ~Scene();

    /**
     * @brief Initialize the scene: Load the model, create the buffers, 
     * attributes,  lighting. It also enables GL_DEPTH_TEST and the background 
     * color.
     */
    void initialize();

    /**
     * @brief Resize the animation and reset the projection matrix.
     * @param w The new width of the animation.
     * @param h The new height of the animation.
     */
    void resize(int w, int h);
    
    /**
     * @brief Update the scene.
     */
    void update();

    /**
     * @brief Render the scene.
     */
    void render();
    
    /**
     * @brief Render the scene to generate the shadow map associated to the 
     * cascadeIdx-th cascade.
     * @param cascadeIdx The cascade index.
     */
    void renderShadow(unsigned int cascadeIdx);

    /**
     * @brief Cleanup the animation.
     */
    void cleanUp();
    
    /**
     * @brief Update the timestep of the animation.
     */
    void updateTimestep();
    
    /**
     * @brief Manually set the timestep of the animation.
     */
    void setTimestep(float timestep) {
        m_timestep = std::max(m_firstTimestep, 
                              std::min(timestep, m_finalTimestep));
    }
    
public:
    void playPauseAnimation() {
        if (m_frameRate == 0.0f)
            m_frameRate = 1 / static_cast<float>(m_refreshRate);
        else
            m_frameRate = 0.0f;
    };
    
    void restartAnimation() {m_timestep = m_firstTimestep;};
    
    void goEndAnimation() {
        m_frameRate = 0.0f;
        m_timestep = m_finalTimestep;
    };
    
    void setTimeRate(const float timeRate) {m_timeRate = timeRate;}
    
    void toggleAnimationLoop() {m_loop = !m_loop;}
    
    float getTimestep() const {return m_timestep;}
    
    float getFirstTimestep() const {return m_firstTimestep;}
    
    float getFinalTimestep() const {return m_finalTimestep;}
    
    bool isPaused() {return m_frameRate == 0.0f;}

    void setTimestepFromSlider(float slider) {
        m_timestep = 
            slider * (m_finalTimestep - m_firstTimestep) + m_firstTimestep;
    }
                
    void resetCameraOffset() {m_camera.resetTargetOffset();};
    
    bool isCameraOffset() {return m_camera.isCameraOffset();};
    
    void toggleGlobalFrame() {m_showGlobalFrame = !m_showGlobalFrame;}
    
    void toggleTireForce() {p_vehicle->toggleTireForce();}
    
private:
    /**
     * View matrix: transform from the world (scene) coordinates to the camera 
     * coordinates, this is used to change the position of the camera.
     */
    QMatrix4x4 m_view;

    /**
     * Projection matrix: transform from the camera coordinates to the 
     * homogeneous coordinates (the 2D coordinates of the screen).
     */
    QMatrix4x4 m_projection;
    
    /**
     * Light space matrices used for shadow mapping.
     */
    std::array<QMatrix4x4,NUM_CASCADES> m_lightSpace;

    /**
     * The lighting of the scene.
     */
    CasterLight m_light;
    
    /**
     * The camera of the scene.
     */
    Camera m_camera;
    
    /**
     * The skybox of the scene.
     */
    Skybox m_skybox;
    
    /**
     * The scene graph.
     */
    std::unique_ptr<Node> p_graph;

    /**
     * The vehicle.
     */
    std::unique_ptr<Vehicle> p_vehicle;
    
    /**
     * The XYZ frame of the scene.
     */
    Frame m_frame;

    /**
     * The current timestep at which the frame is drawn.
     */
    float m_timestep;

    /**
     * First timestep of the animation.
     */
    float m_firstTimestep;

    /**
     * Last timestep of the animation
     */
    float m_finalTimestep;

    /**
     * The refresh rate of the animation.
     */
    unsigned int m_refreshRate;

    /**
     * Use to slow down the rate of the animation from the player.
     */
    float m_timeRate;

    /**
     * The (desired) frame rate of the animation.
     */
    float m_frameRate;

    /**
     * Enable/disable the animation loop.
     */
    bool m_loop;
    
    /**
     * Show the global frame of the scene.
     */
    bool m_showGlobalFrame;
    
    /**
     * Define the different zone for cascade shadow mapping.
     */
    std::array<float,NUM_CASCADES+1> m_cascades;
    
    /**
     * XML file describing the scene.
     */
    QString m_envFile;
};



#include <QDomElement>

/// Scene loader
/**
 * @brief Load scene from XML file.
 */
class Scene::Loader {
public:
    Loader();
    ~Loader();
    
    /**
     * @brief Parse the file.
     * @param fileName The name of the XML file to parse.
     */
    void parse(QString const fileName);
    
    /**
     * @brief Return the object container.
     */
    std::unique_ptr<Node> getSceneGraph();
    
private:
    /**
     * @brief Convert a QString with format "# # #" to a QVector3D.
     * @param[in] string The string to convert.
     * @param[out] vec The vector.
     * @return Return true if the string is converted to a QVector3D 
     * successfully.
     */
    static bool qStringToQVector3D(const QString & string, QVector3D & vec);
    
    /**
     * @brief Convert a QString with format "# # # #" to a QVector4D.
     * @param[in] string The string to convert.
     * @param[out] vec The vector.
     * @return Return true if the string is converted to a QVector4D 
     * successfully.
     */
    static bool qStringToQVector4D(const QString & string, QVector4D & vec);
    
    /**
     * @brief Process a group element.
     * @param[in] group The DOM element.
     * @param[out] matrix The model matrix.
     * @return Pointer to a scene node.
     */
    void processGroup(
        const QDomElement & group, std::unique_ptr<Node> & node
    );
    
    /**
     * @brief Process a transform element among the node's children.
     * @param transform The DOM element.
     * @param parentMatrix The world matrix of the parent node.
     * @return Pointer to a scene node.
     */
    std::unique_ptr<Node> processTransform(
        const QDomElement & transform, const QMatrix4x4 parentMatrix = QMatrix()
    );
    
    /**
     * @brief Process all model elements among the node's children.
     * @param elmt The DOM element.
     * @return Pointer to the object, nullptr if an error happened.
     */
    ABCObject * processModel(const QDomElement & elmt);
    
    /**
     * @brief Process all shape elements among the node's children.
     * @param elmt The DOM element.
     * @return Pointer to the object, nulptr if an error happened.
     */
    ABCObject * processShape(const QDomElement & elmt);
    
    /**
     * @brief Process all shape elements among the node's children.
     * @param elmt The DOM element.
     * @return Pointer to the object, nullptr if an error happened.
     */
    ABCObject * processReference(const QDomElement & elmt);
    
private:
    std::unique_ptr<Node> p_rootNode;
};



/// Node to represent a scene graph
/**
 * @brief This class defines a node to represent a scene graph.
 */
class Scene::Node {
public:
    /**
     * @brief Node constructor
     * @param parentWorldMatrix The world matrix of the parentWorldMatrix.
     * @param localMatrix The matrix to transform from the parent to this node.
     */
    Node(
        const QMatrix4x4 & localMatrix = QMatrix4x4(), 
        const QMatrix4x4 & parentWorldMatrix = QMatrix4x4()
    ) : m_worldMatrix(parentWorldMatrix * localMatrix) {};
    ~Node() {};
    
    /**
     * @brief Add a child to the list of children of the node.
     * @param node The node to add.
     */
    void addChild(std::unique_ptr<Node> node) {
        m_children.push_back(std::move(node));
    };
    
    /**
     * @brief Add object to load when drawing the node.
     * @param object The object to add.
     */
    void addObject(ABCObject * object) {
        m_objects.push_back(object);
    };
    
    /**
     * @brief Return the world matrix.
     */
    QMatrix4x4 getWorldMatrix() const {return m_worldMatrix;};
    
    /**
     * @brief Render the node and all its descendants.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     * @param cascades Array containing the distance for cascade shadow mapping.
     */
    void render(
        const CasterLight & light, const QMatrix4x4 & view, 
        const QMatrix4x4 & projection, 
        const std::array<QMatrix4x4,NUM_CASCADES> & lightSpace,
        const std::array<float,NUM_CASCADES+1> & cascades
    );
    
    /**
     * @brief Render the shadow of the node and of all its descendants.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    void renderShadow(const QMatrix4x4 & lightSpace);
    
private:
    QMatrix4x4 m_worldMatrix;
    std::vector<std::unique_ptr<Node>> m_children;
    std::vector<ABCObject *> m_objects;
};

#endif // SCENE_H










