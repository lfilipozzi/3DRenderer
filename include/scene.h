#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include "vehicle.h"
#include "frame_gl33.h"
#include "skybox.h"
#include <memory>
#include "camera.h"
#include "object.h"


/// Scene class
/**
 * @brief The scene contains the object to be rendered. It manages all the 
 * elements that will be rendered and update their positions.
 * @author Louis Filipozzi
 */
class Scene {
public:
    Scene(unsigned int refreshRate);

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
     * @brief Render the animation.
     */
    void render();

    /**
     * @brief Cleanup the animation.
     */
    void cleanUp();
    
    /**
     * @brief Update the timestep of the animation.
     */
    void updateTimestep();

    /**
     * @brief Update the view of the camera based on the user inputs from the 
     * InputManager.
     */
    void updateCamera();

    /**
     * Play or pause the animation.
     */
    void playPauseAnimation();

    /**
     * Restart the animation from the beginning.
     */
    void restartAnimation();

    /**
     * Pause the animation and go to the end of the animation.
     */
    void goEndAnimation();

    /**
     * @brief Set the time rate of the animation to slow down or speed up the 
     * animation.
     * @param timeRate The time rate of the animation.
     */
    void setTimeRate(const float timeRate) {m_timeRate = timeRate;}

    /**
     * @brief Enable/disable the animation loop.
     */
    void toggleLoopAnimation() {m_enableLoop = !m_enableLoop;}

    /**
     * Return the timestep of the scene.
     */
    float getTimestep() const {return m_timestep;}

    /**
     * Return the first timestep of the scene.
     */
    float getTimestepBegin() const {return m_timestepBegin;}

    /**
     * Return the final timestep of the scene.
     */
    float getTimestepEnd() const {return m_timestepEnd;}

    /**
     * Return true if the animation is paused
     */
    bool isPaused() {return m_frameRate == 0.0f;}

    void setTimestepFromSlider(float slider) {m_timestep = slider *
                (m_timestepEnd - m_timestepBegin) + m_timestepBegin;}
                
    void resetCameraOffset() {m_camera.resetTargetOffset();};
    
    bool isCameraOffset() {return m_camera.isCameraOffset();};
    
    void toggleGlobalFrame() {m_showGlobalFrame = !m_showGlobalFrame;}
    
    void toggleTireForce() {p_vehicle->toggleTireForce();}

private:
    /**
     * @brief Print OpenGL errors if any.
     */
    void printOpenGLError();
    
    /**
     * @brief Set projection and view matrices. Set lightning.
     */
    void setupLightingAndMatrices();
    
private:
    /**
     * Store the OpenGL context.
     */
    QOpenGLContext * p_context;
    
    /**
     * Store the OpenGL functions.
     */
    QOpenGLFunctions_3_3_Core * p_glFunctions;

    /**
     * Projection matrix: transform from the camera coordinates to the 
     * homogeneous coordinates (the 2D coordinates of the screen).
     */
    QMatrix4x4 m_projection;

    /**
     * View matrix: transform from the world (scene) coordinates to the camera 
     * coordinates, this is used to change the position of the camera.
     */
    QMatrix4x4 m_view;

    /**
     * The lighting of the scene.
     */
    CasterLight m_light;
    
    /**
     * The skybox of the scene.
     */
    Skybox m_skybox;

    /**
     * The surface of the scene.
     */
    Object * p_surface;
    
    /**
     * The XYZ frame of the scene.
     */
    Frame_GL33 m_frame;
    
    /**
     * The animation widget width.
     */
    int m_screenWidth;
    
    /**
     * The animation widget height.
     */
    int m_screenHeight;
    
    /**
     * Width of the texture produced by the frame buffer object for shadow
     * mapping.
     */
    const unsigned int c_shadowWidth;
    
    /**
     * Height of the texture produced by the frame buffer object for shadow
     * mapping.
     */
    const unsigned int c_shadowHeight;

    /**
     * ID of the frame buffer used to obtain the shadow by shadow mapping. Qt 
     * provide a class for an OpenGL frame buffer object, but this class cannot
     * be used for shadow mapping as it doesn't give access to the framebuffer's
     * depth buffer.
      */
    unsigned int m_shadowFBO;
    
    /**
     * Texture storing the framebuffer's depth buffer.
     */
    unsigned int m_shadowDepthMap;
    
    /**
     * @brief The camera of the scene.
     */
    Camera m_camera;

    /**
     * @brief The current timestep at which the frame is drawn.
     */
    float m_timestep;

    /**
     * @brief First timestep of the animation.
     */
    float m_timestepBegin;

    /**
     * @brief Last timestep of the animation
     */
    float m_timestepEnd;

    /**
     * @brief The refresh rate of the animation.
     */
    unsigned int m_refreshRate;

    /**
     * @brief Use to slow down the rate of the animation from the player.
     */
    float m_timeRate;

    /**
     * @brief The (desired) frame rate of the animation.
     */
    float m_frameRate;

    /**
     * @brief Enable/disable the animation loop.
     */
    bool m_enableLoop;

    /**
     * The vehicle.
     */
    std::unique_ptr<Vehicle> p_vehicle;
    
    /**
     * Show the global frame of the scene.
     */
    bool m_showGlobalFrame;
    
    Line_GL33 * p_line; // TODO remove this
};

#endif // SCENE_H










