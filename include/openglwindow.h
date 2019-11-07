#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QWindow>
#include "scene_gl33.h"

#include <memory>

class AnimationPlayer;

///3D animation OpenGL window.
/**
 * This class creates a 3D animation OpenGL window. Once the context has been 
 * created, the initializeGL(), updateGL(), resizeGL(), cleanup() mainly execute
 * the method of the same name from the Scene class. This class also have access
 * to the InputManager private methods and variable and is responsible to 
 * register key and button press and release.
 */
class OpenGLWindow : public QWindow {
    Q_OBJECT    // Prepare the compiler to create slots and signals

public:
    OpenGLWindow(unsigned int refreshRate, QScreen* screen = nullptr);
    ~OpenGLWindow();

    /**
     * Set a pointer to the animation player.
     */
    void setPlayer(AnimationPlayer *player);

public slots:
    /**
     * Qt slot used to play or pause the animation.
     */
    void playPauseAnimation();

    /**
     * Qt slot used to enable/disable the looping of the animation.
     */
    void toggleLoopAnimation();

    /**
     * Qt slot used to restart the animation from the beginning.
     */
    void restartAnimation();

    /**
     * Qt slot used to go at the end of the animation.
     */
    void goEndAnimation();

    /**
     * Qt slot to update the timestep after moving the slider.
     */
    void setTimestep(int sliderPosition);

    /**
     * Qt slot to update the rate of th animation using the slider.
     */
    void setTimeRate(int sliderPosition);
    
    /**
     * Qt slot to reset the camera target to zero.
     */
    void resetCameraOffset();
    
    /**
     * Qt slot to toggle the global frame of the scene.
     */
    void toggleGlobalFrame() {m_scene->toggleGlobalFrame();}
    
    /**
     * Qt slot to toggle the tire forces.
     */
    void toggleTireForce() {m_scene->toggleTireForce();}
    
signals:
    /**
     * Indicate the camera has been offset.
     */
    void unlockOffsetCamera();
    
    /**
     * Indicate the camera is not offset.
     */
    void lockOffsetCamera();

protected:
    /**
     * This function is called once before the first call to paintGL() or 
     * resizeGL(). This function execute the initialize() method of the scene.
     */
    void initializeGL();

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

private:
    /**
     * Timer used for the refresh rate.
     */
    QTimer *m_timer;

    /**
     * Scene.
     */
    std::unique_ptr<Scene_GL33> m_scene;

    /**
     * OpenGL context.
     */
    QOpenGLContext * m_context;

    /**
     * Pointer to the player used for the animation.
     */
    AnimationPlayer * m_player;
    
    /**
     * Boolean to indicate if the camera was offset at the last update
     */
    bool m_wasCameraOffset;

protected slots:
    /**
     * This function is called whenever the window contents needs to be painted.
     * It corresponds to the main graphics loop. The function execute the 
     * update() method of the scene.
     */
    void updateGL();

    /**
     * This function is called whenever the widget has been resized. The 
     * function execute the resize() method of the scene.
     */
    void resizeGL();

    /**
     * Slot to cleanup the window. THis function execute the cleanup() method of
     * the scene.
     */
    void cleanup();
};

#endif // OPENGLWINDOW_H
