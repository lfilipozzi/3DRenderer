#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H

#include <QWidget>
#include <QSlider>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QLineEdit>
#include "playerbutton.h"
#include "openglwindow.h"

/// Widget to control the animation.
/**
 * This widget is used to play the animation.
 */
class AnimationPlayer : public QWidget {
    Q_OBJECT    // Prepare the compiler to create slots
public:
    AnimationPlayer(OpenGLWindow *openGLWindow, QWidget *parent=nullptr);
    ~AnimationPlayer();

    /**
     * Update the position of the slider and the clock to correspond to the 
     * timestep.
     */
    void updateTimestepValue(float timestep, float initialTimestep, 
                             float finalTimestep);

    /**
     * Update the icon of the play/pause button.
     */
    void updatePlayPauseButton(bool isAnimationPaused);
    
public slots:
    /**
     * Qt slot to update the icon of the lock camera button.
     */
    void unlockCameraButton();
    
    /**
     * Qt slot to update the icon of the lock camera button.
     */
    void lockCameraButton();
    
    /**
     * Qt slot to swith to snapshot mode.
     */
    void setSnapshotMode(bool flag);

private:
    /**
     * Horizontal layout containing button to contorl the animation.
     */
    QHBoxLayout *m_controlHLayout;
    
    /**
     * Horizontal layout containing widget to control the animation time.
     */
    QHBoxLayout *m_timeHLayout;
    
    /**
     * Display the current time of the animation.
     */
    QLCDNumber *m_lcdClock;

    /**
     * Player slider to control time.
     */
    QSlider *m_timeSlider;

    /**
     * Slider to control the time rate of the animation
     */
    QSlider *m_rateSlider;

    /**
     * Play/pause push-button.
     */
    PlayerButton *m_playPauseButton;

    /**
     * Go back to the beginning of the animation.
     */
    PlayerButton *m_goBeginButton;

    /**
     * Go to the end of the animation.
     */
    PlayerButton *m_goEndButton;

    /**
     * Loop the animation button.
     */
    PlayerButton *m_playLoopButton;
    
    /**
     * Lock camera button.
     */
    PlayerButton *m_lockCameraButton;
    
    /**
     * Field for number of snapshot in snapshot mode.
     */
    QLineEdit *m_numSnapshot;
};

#endif // ANIMATIONPLAYER_H
