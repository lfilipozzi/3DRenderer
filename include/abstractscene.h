#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include "camera.h"
#include "abstractvehicle.h"

/// Abstract class Scene.
/**
 * This is an interface for scenes with different version of OpenGL.
 */
class AbstractScene {
public:
    AbstractScene(int refreshRate);
    virtual ~AbstractScene();

    /**
     * Initialize the animation.
     */
    virtual void initialize() = 0;

    /**
     * Resize the animation.
     * @param w The new width of the animation.
     * @param h The new height of the animation.
     */
    virtual void resize(int w, int h) = 0;

    /**
     * Update the animation.
     */
    virtual void update() = 0;

    /**
     * Cleanup the animation.
     */
    virtual void cleanup() = 0;

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
     * @return
     */
    void setTimeRate(float timeRate) {m_timeRate = timeRate;}

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
    
    void toggleTireForce() {m_vehicle->toggleTireForce();}

protected:
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
    int m_refreshRate;

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
    AbstractVehicle *m_vehicle;
    
    /**
     * Show the global frame of the scene.
     */
    bool m_showGlobalFrame;
};

#endif // ABSTRACTSCENE_H
