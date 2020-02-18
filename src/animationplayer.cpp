#include "../include/animationplayer.h"
#include <QObject>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTime>

AnimationPlayer::AnimationPlayer(OpenGLWindow *openGLWindow, QWidget *parent) :
    QWidget(parent) {
    // Set the pointer of OpenGLWindow's player to the player
    openGLWindow->setPlayer(this);
    
    // Grid layout
    QVBoxLayout *animationVLayout = new QVBoxLayout(this);
    // Horizontal layout to control the animation 
    m_controlHLayout = new QHBoxLayout();
    // Horizontal layout for time control
    m_timeHLayout = new QHBoxLayout();
    m_lcdClock = new QLCDNumber(9);
    m_timeSlider = new QSlider(Qt::Horizontal);
    m_rateSlider = new QSlider(Qt::Horizontal);
    m_playPauseButton = new PlayerButton("media-playback-paused",
                                         QIcon(":/icons/pause")
    );
    m_goBeginButton = new PlayerButton("media-seek-backward",
                                       QIcon(":/icons/playBackward")
    );
    m_goEndButton = new PlayerButton("media-seek-forward",
                                     QIcon(":/icons/playForward")
    );
    m_playLoopButton = new PlayerButton("media-playlist-repeat",
                                        QIcon(":/icons/repeat")
    );
    m_lockCameraButton = new PlayerButton("lock", 
                                          QIcon(":/icons/lock")
    );

    // Create the slider and buttons
    m_timeSlider->setRange(0, 1000);
    m_timeSlider->setMinimumWidth(800);
    m_rateSlider->setRange(0,200);
    m_rateSlider->setValue(100);
    m_rateSlider->setMinimumWidth(200);
    
    m_lcdClock->setSegmentStyle(QLCDNumber::Flat);
    m_lcdClock->setFrameStyle(0);
    m_lcdClock->display("00:00");
    
    // Horizontal layout for time control
    m_timeHLayout->addWidget(m_lcdClock);
    m_timeHLayout->addWidget(m_timeSlider);
    
    // Horizontal layout to control the animation 
    m_controlHLayout->addWidget(m_goBeginButton);
    m_controlHLayout->addWidget(m_playPauseButton);
    m_controlHLayout->addWidget(m_goEndButton);
    m_controlHLayout->addWidget(m_playLoopButton);
    m_controlHLayout->addWidget(m_lockCameraButton);
    m_controlHLayout->addWidget(m_rateSlider);

    // Grid layout
    animationVLayout->addLayout(m_timeHLayout);
    animationVLayout->addLayout(m_controlHLayout);
    setLayout(animationVLayout);

    // Connect signals to slot (buttons)
    QObject::connect(m_playPauseButton, SIGNAL(clicked()), 
                     openGLWindow, SLOT(playPauseAnimation()));
    QObject::connect(m_goBeginButton, SIGNAL(clicked()), 
                     openGLWindow, SLOT(restartAnimation()));
    QObject::connect(m_goEndButton, SIGNAL(clicked()), 
                     openGLWindow, SLOT(goEndAnimation()));
    QObject::connect(m_playLoopButton, SIGNAL(clicked()), 
                     openGLWindow, SLOT(toggleAnimationLoop()));
    QObject::connect(m_lockCameraButton, SIGNAL(clicked()),
                     openGLWindow, SLOT(resetCameraOffset()));

    // Connect signals to slot (slider)
    QObject::connect(m_timeSlider, SIGNAL(valueChanged(int)), 
                     openGLWindow, SLOT(setTimestep(int)));
    QObject::connect(m_rateSlider, SIGNAL(valueChanged(int)), 
                     openGLWindow, SLOT(setTimeRate(int)));
    
    // Lock the height of the widget to its preferred size
    this->setMaximumHeight(this->sizeHint().height());
}

AnimationPlayer::~AnimationPlayer() {
    delete(m_controlHLayout);
    delete(m_timeHLayout);
    delete(m_lcdClock);
    delete(m_timeSlider);
    delete(m_rateSlider);
    delete(m_playPauseButton);
    delete(m_goBeginButton);
    delete(m_goEndButton);
    delete(m_playLoopButton);
    delete(m_lockCameraButton);
}


void AnimationPlayer::updateTimestepValue(float timestep, 
                                          float initialTimestep, 
                                          float finalTimestep) {
    // Update the clock
    int millisecond = static_cast<int>(timestep*1000);
    int second = millisecond / 1000;
    millisecond %= 1000;
    int minute = second / 60;
    second %= 60;
    QString clockText = "";
    if (minute/10 == 0)
        clockText.append('0');
    clockText.append(QString::number(minute));
    clockText.append(":");
    if (second/10 == 0)
        clockText.append('0');
    clockText.append(QString::number(second));
    clockText.append(":");
    if (millisecond/10 == 0)
        clockText.append('0');
    if (millisecond/100 == 0)
        clockText.append('0');
    clockText.append(QString::number(millisecond));
    m_lcdClock->display(clockText);

    // Update the position of the slider
    int sliderMin = m_timeSlider->minimum();
    int sliderMax = m_timeSlider->maximum();
    int sliderValue = static_cast<int>(
        (timestep-initialTimestep) / (finalTimestep-initialTimestep) *
        (sliderMax - sliderMin)
                                  );
    m_timeSlider->setValue(sliderValue);
}

void AnimationPlayer::updatePlayPauseButton(bool isAnimationPaused) {
    if (isAnimationPaused)
        m_playPauseButton->setIcon(QIcon::fromTheme("media-playback-start",
                                                    QIcon(":/icons/play")
        ));
    else
        m_playPauseButton->setIcon(QIcon::fromTheme("media-playback-paused",
                                                    QIcon(":/icons/pause")
        ));
}

void AnimationPlayer::unlockCameraButton() {
    m_lockCameraButton->setIcon(QIcon::fromTheme("unlock",
                                                 QIcon(":/icons/unlock")
    ));
}

void AnimationPlayer::lockCameraButton() {
    m_lockCameraButton->setIcon(QIcon::fromTheme("lock",
                                                 QIcon(":/icons/lock")
    ));
}

