#ifndef ANIMATIONWINDOW_H
#define ANIMATIONWINDOW_H


#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include "openglwindow.h"

/// Recorder dialog window
/**
 * @brief GUI class for the recorder dialog
 */
class RecordDialog : public QWidget {
    Q_OBJECT
    
public:
    RecordDialog(OpenGLWindow * window, QWidget * parent = nullptr);
    ~RecordDialog();
    
public slots:
    /**
     * @brief Change the filename using a file browser.
     */
    void setFileName();
    
    /**
     * @brief Record the animation given the current settings and exit the 
     * RecorderDialog window.
     */
    void record();
    
private:
    QLineEdit * p_fileNameLineEdit;
    QSpinBox * p_fpsSpinBox;
    QComboBox * p_resolutionComboBox;
    OpenGLWindow * const p_openGLWindow;
};



#include <QMainWindow>
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
    /**
     * @brief Show information on the application.
     */
    void openAboutWindow();
    
private:
    std::unique_ptr<OpenGLWindow> p_openGLWindow;
    AnimationPlayer * p_player;
    std::unique_ptr<RecordDialog> p_recordDialog;
};





#endif // ANIMATIONWINDOW_H
