#include <QApplication>
#include "animationwindow.h"

/**
 * @author Louis Filipozzi
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("3D viewer");
    
    AnimationWindow animationWindow;
    animationWindow.show();

    return app.exec();
}

/**
 * @mainpage 3D Renderer
 *
 * This project uses OpenGL and the Qt framework to create a simple 3D animation
 * of a vehicle. OpenGL 3.3 is the only version supported.
 * 
 * The animation uses forward rendering. The lighting is based on the 
 * Blinn-Phong model. Shadow mapping is used for shadows.
 *
 * @section structure_sec Structure overview
 *
 * The OpenGL context is created inside an OpenGLWindow. The OpenGLWindow 
 * constructor checks if a supported version of OpenGL is available. If not, an 
 * error message is displayed. If it finds a supported version, a sceneScelector 
 * object is created. 
 * 
 * OpenLWindow is also responsible to register the key and button event through 
 * the methods OpenGLWindow::keyPressEvent, OpenGLWindow::keyReleaseEvent,
 * OpenGLWindow::mousePressEvent, and OpenGLWindow::mouseReleaseEvent. Inputs 
 * are then dealt by the InputManager (see \ref inputmanager_sec).
 *
 * The sceneScelector object aims to create an object inherited from the 
 * AbstractScene class through the SceneScelector::getScene(glVersion, 
 * isOpenGLES) method. For now, this class is pretty useless since only openGL 
 * 3.3 is supported. If An OpenGL 3.3 context is requested, the getScene method
 * creates a Scene_GL33 object which contains all the objects inherited from
 * the AbstractObject3D_GL33 in the scene. More precisely, it contains a 
 * Model3D_GL33 object which load a 3D object (the vehicle) via the Assimp 
 * library, and a FlatSurface for the plan* which represent the road surface.
 *
 * @section scene_sec Scene
 *
 * The scene contains all the objects present in the virtual scene, the camera, 
 * the projection, andinformation on the lighting of the scene.
 *
 * @section inputmanager_sec Input Management
 *
 * Deplacement of the camera is allowed like in a video game. Keys WASDQE are 
 * used to move the camera, and the mouse can be used to rotate the camera while
 * the left button is pressed. The key and mouse inputs are processed by the 
 * InputManager.
 *
 * The inputs managed by this class are of three types: keys from the keyboard, 
 * mouse button, and cursor position. A state is associated to each key and 
 * button. A key or button can be in six different states as defined by the
 * InputManager::InputState. Possible states are:
 * - InputInvalid: indicates an invalid input.
 * - InputRegistered: Once a new input occurs, it must be registered via the 
 *   methods InputManager::registerKeyPressed() or 
 *   InputManager::registerButtonPressed(). The input is added to the 
 *   corresponding container (for key or button) with the state InputRegistered.
 * - InputTrigerred: After registering the input, it is updated to a new state: 
 *   InputTriggered. This allows to call functions only when the input has been 
 *   triggered rather than when it is pressed continuously via the methods
 *   InputManager::isKeyTriggered() or InputManager::isButtonTriggered().
 * - InputPressed: After triggering the input, its state is updated to
 *   InputPressed. This allow to call functions when a key is pressed 
 *   continuously via the methods InputManager::isKeyPressed or 
 *   InputManager::isButtonPressed.
 * - InputUnregistered: When a button or key is released, its state must be 
 *   switch to InputUnregistered. Then, when the InputManager updates the inputs
 *   container, the state will be switched to InputReleased.
 * - InputReleased: The inputs with state InputReleased are removed from the
 *   containers when the method InputManager::update() is called.
 *
 * The buttons and keys pressed, and the mouse position are stored in static 
 * variables sg_keyInstances, sg_buttonInstances, sg_mouseCurrPosition, 
 * sg_mousePrevPosition, and sg_mouseDelta. The inputs are updated via the 
 * static private method InputManager::update().
 *
 * The state of the buttons and keys, and the mouse position and siaplcement are
 * available through the public methods. However, the registration of the input 
 * (key and button press and release, and mouse position), and the update of the
 * inputs via InputManager::update() is not. This is done by giving access to 
 * the private methods of InputManager to OpenGLWindow with friendship.
 *
 * @section camera_sec Camera
 *
 * The camera is defined by the target it is looking at, its forward axis, its 
 * distance from the target along the its forward axis, and its up vector. 
 * Since the camera can move for two reasons (to follow the vehicle or as 
 * requested by the user), several variables are introduced to distinct the 
 * different use case. Properties m_pitchOffset, m_yawOffset, and m_targetOffset
 * are used to offset the pitch and yaw angle, and target of the camera in order
 * control the camera from the user input. Properties m_pitchTrack, m_yawTrack, 
 * and m_targetTrack are used to track an object in the scene.
 *
 * The camera can be translated and rotated by the user. The camera is updated 
 * through the method Camera::processMouseMovement() and 
 * Camera::processKeyboard().
 *
 * @section vehicle_sec Vehicle
 *
 * The vehicle is defined by a class which has two main roles:
 * - Load the data from the simulation and obtain the position of the vehicle 
 *   and its wheels at the corresponding timestep.
 * - Animate the 3D virtual scene.
 *
 * Since the virtual scene can be based on different OpenGL version, different 
 * implementation are required for each supported version of OpenGL. 
 * AbstractVehicle creates an interface for all the vehicle classes (with 
 * different version of OpenGL). It mainly gathers the methods and properties to
 * load the simulation results. At the construction of the vehicle, the 
 * simulation results are loaded from the text file containing the vehile 
 * trajectory. The method AbstractVehicle::getChassisPosition() is used to 
 * obtain the chassis position and modify the position of the Camera to track 
 * the vehicle.
 * 
 * @section texture_unit Texture units
 * 
 * The texture unit 0 is used for the model's texture. The texture unit 1 is 
 * used for the depth map for shadow mapping. The texture unit 2 is used for the
 * skybox's texture.
 *
 * @section todo_sec TODO
 *
 * -# New architecture for 3D object: one for non moving, one for moving object,
 *    one for light, and one for camera.
 * -# Add support for point light. This requires to set up a cubemap for the shadow 
 *    mapping.
 * -# Add support for normal and parralax mapping
 * -# Improve texture management, check OpenGLTexture is used instead of 
 *    QOpenGLTexture and make sure a texture is loaded only once (even between
 *    two different objects).
 * -# Create an iterator as property of the abstractvehicle, and create custom 
 *    method to update the iterator instead of using std::upperbound which
 *    iterate from the beginning of the container at each timestep.
 * -# create XML file which contains the 3D model to use, the simulation data of
 *    each vehicle, the environment... and add it to the scene (abstractscene).
 * -# Create Scene for OpenGL2.1/OpenGL ES
 * -# Implement interface (or template?) for object to be independent of openGL 
 *    version
 * -# TODO: https://stackoverflow.com/questions/21513763/drawing-multiple-models-using-multiple-opengl-vbos
 */
