#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QWindow>
#include <memory>
#include "scene.h"
#include "depthmap.h"
#include <QOpenGLFunctions>

class AnimationPlayer;

/// OpenGL Window
/**
 * @brief Open an OpenGL context for rendering.
 * @author Louis Filipozzi
 * @details This class creates a window with OpenGL rendering capabilities. It 
 * try to open an OpenGL 3.3 context. If the context can be created, it starts 
 * to execute the rendering loop through the methods initializeGL(), updateGL(), 
 * resizeGL(), and cleanUpGL(). If the context cannot be created, it executes 
 * the program with error code 1.
 */
class OpenGLWindow : public QWindow {
    Q_OBJECT    // Prepare the compiler to create slots and signals

public:
    OpenGLWindow(unsigned int refreshRate, QScreen * screen = nullptr);
    ~OpenGLWindow();

    /**
     * @brief Set a pointer to the animation player.
     */
    void setPlayer(AnimationPlayer * player);
    
    /**
     * @brief Print OpenGL errors (if any) of the current OpenGL context.
     */
    static void printOpenGLError();
    
protected:
    /**
     * @brief This function is called once before the first call to updateGL() 
     * or resizeGL().
     */
    void initializeGL();
    
    /**
     * @brief This function updates the position of all objects and renders the 
     * scene.
     */
    void renderGL();
    
public:
    /**
     * @brief This function records the animation to a video file. It 
     * temporarily disable user's inputs and restart the animation.
     * @param fps The frame rate of the video.
     * @param width The width of the video.
     * @param height The height of the video.
     * @param filename The name of the video file to create.
     */
    void record(
        const int fps, const int width, const int height, const QString fileName
    );
    
protected slots:
    /**
     * @brief This function corresponds to the main graphics loop. It is called 
     * whenever the window contents needs to be painted. It processes the user 
     * inputs and renders the scene.
     */
    void updateGL();

    /**
     * @brief This function is called whenever the widget has been resized. The 
     * function execute the resize() method of the scene.
     */
    void resizeGL();

    /**
     * @brief Slot to cleanup the window. This function execute the cleanup() 
     * method of the scene.
     */
    void cleanUpGL();
    
protected:
    /**
     * @brief Event created when a key is pressed. The input manager will 
     * process the inputs.
     * @param event A key event.
     */
    void keyPressEvent(QKeyEvent *event);

    /**
     * @brief Event created when a key is released. The input manager will
     * process the inputs.
     * @param event A key event.
     */
    void keyReleaseEvent(QKeyEvent *event);

    /**
     * @brief Event created when a button is pressed. The input manager will
     * process the inputs.
     * @param event A button event.
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief Event created when a button is released. The input manager will
     * process the inputs.
     * @param event A button event
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * @brief Event created when the a wheel event occured.
     * @param event A mouse wheel event.
     */
    void wheelEvent(QWheelEvent *event);
    
public slots:
    /**
     * Qt slot used to play or pause the animation.
     */
    void playPauseAnimation();

    /**
     * Qt slot used to enable/disable the animation loop.
     */
    void toggleAnimationLoop() {
        if (p_scene != nullptr)
            p_scene->toggleAnimationLoop();
    };

    /**
     * Qt slot used to restart the animation from the beginning.
     */
    void restartAnimation() {
        if (p_scene != nullptr)
            p_scene->restartAnimation();
    };

    /**
     * Qt slot used to go at the end of the animation.
     */
    void goEndAnimation();

    /**
     * Qt slot to update the timestep after moving the slider.
     */
    void setTimestep(int sliderPosition);

    /**
     * Qt slot to update the rate of the animation using the slider.
     */
    void setTimeRate(int sliderPosition);
    
    /**
     * Qt slot to reset the camera target to zero.
     */
    void resetCameraOffset();
    
    /**
     * Qt slot to toggle the global frame of the scene.
     */
    void toggleGlobalFrame() {p_scene->toggleGlobalFrame();}
    
    /**
     * Qt slot to toggle the tire forces.
     */
    void toggleTireForce() {p_scene->toggleTireForce();}
    
signals:
    /**
     * Indicate the camera has been offset.
     */
    void unlockOffsetCamera();
    
    /**
     * Indicate the camera is not offset.
     */
    void lockOffsetCamera();

private:
    /**
     * Timer used for the refresh rate.
     */
    QTimer * p_timer;
    
    /**
     * Store the OpenGL functions.
     */
    QOpenGLFunctions * p_glFunctions;
    
    /**
     * The depth map used for shadow mapping
     */
    std::unique_ptr<DepthMap> p_depthMap;

    /**
     * Scene.
     */
    std::unique_ptr<Scene> p_scene;

    /**
     * OpenGL context.
     */
    QOpenGLContext * p_context;

    /**
     * Pointer to the player used for the animation.
     */
    AnimationPlayer * p_player;
    
    /**
     * Boolean to indicate if the camera was offset at the last update.
     */
    bool m_wasCameraOffset;
};

#endif // OPENGLWINDOW_H
