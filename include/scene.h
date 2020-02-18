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
     * The surface of the scene.
     */
    ABCObject * p_surface;

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
};

#endif // SCENE_H










