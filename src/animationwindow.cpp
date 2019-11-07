#include "../include/animationwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>


AnimationWindow::AnimationWindow() {
    // Menu bar
    QMenu * fileMenu = menuBar()->addMenu("&File");
    QMenu * viewMenu = menuBar()->addMenu("&View");
    QMenu * helpMenu = menuBar()->addMenu("&Help");
    QAction * exitAction = fileMenu->addAction("&Exit");
    QAction * toggleGlobFrAction = viewMenu->addAction("Toggle &global frame");
    QAction * toggleTireForceAction = viewMenu->addAction("Toggle &tire forces");
    QAction * aboutAction = helpMenu->addAction("&About");

    // Animation window
    unsigned int refreshRate(30);
    p_openGLWindow = std::make_unique<OpenGLWindow>(refreshRate);
    QWidget * windowContainer = 
        QWidget::createWindowContainer(p_openGLWindow.get());
    windowContainer->setMinimumSize(800, 600);

    // Animation player
    p_player = new AnimationPlayer(p_openGLWindow.get(), this);
    
    // Add the animation and the player to the central widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *vertLayout = new QVBoxLayout(centralWidget);
    vertLayout->addWidget(windowContainer);
    vertLayout->addWidget(p_player);
    vertLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(vertLayout);
    setCentralWidget(centralWidget);
    
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(toggleGlobFrAction, SIGNAL(triggered()), p_openGLWindow.get(),
        SLOT(toggleGlobalFrame()));
    connect(toggleTireForceAction, SIGNAL(triggered()), p_openGLWindow.get(), 
        SLOT(toggleTireForce()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAboutWindow()));
}


AnimationWindow::~AnimationWindow() {
    
}


void AnimationWindow::openAboutWindow() {
    QMessageBox::information(this, "About", 
                             "3D viewer.\n\nAuthor: Louis Filipozzi\n\n"
                             "Dependencies: Assimp, Qt, OpenGL");
}






