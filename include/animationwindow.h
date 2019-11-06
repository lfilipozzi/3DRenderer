#ifndef ANIMATIONWINDOW_H
#define ANIMATIONWINDOW_H

#include <QMainWindow>
#include "openglwindow.h"
#include "animationplayer.h"

/// Animation window
/**
 * This class defines the animation window, i.e. the openGL widget and the 
 * animation player
 */
class AnimationWindow : public QMainWindow {
    Q_OBJECT    // Prepare the compiler to create slots
public:
    AnimationWindow();
    ~AnimationWindow();
    
public slots:
    void openAboutWindow();
    
private:
    OpenGLWindow *m_openGLWindow;
    AnimationPlayer *m_player;
};

#endif // ANIMATIONWINDOW_H
