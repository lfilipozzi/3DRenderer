#ifndef ANIMATIONWINDOW_H
#define ANIMATIONWINDOW_H

#include <QMainWindow>
#include "openglwindow.h"
#include "animationplayer.h"
#include <memory>

/// Animation window
/**
 * @brief Animation GUI class
 * @author Louis Filipozzi
 * @details This class defines the animation window GUI. It contains a window 
 * with the animation to play and a player for basic control of the animation 
 * (play, pause, animation speed, ...)
 */
class AnimationWindow : public QMainWindow {
    Q_OBJECT    // Prepare the compiler to create slots
    
public:
    AnimationWindow();
    ~AnimationWindow();
    
public slots:
    void openAboutWindow();
    
private:
    std::unique_ptr<OpenGLWindow> p_openGLWindow;
    AnimationPlayer * p_player;
};

#endif // ANIMATIONWINDOW_H
