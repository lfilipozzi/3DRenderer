#include "animationwindow.h"
#include <QApplication>
#include "scene_gl33.h"
#include <QPair>
#include <QVBoxLayout>
#include <QMenuBar>
#include <iostream>
#include <QMessageBox>

AnimationWindow::AnimationWindow() {
    // Menu bar
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QMenu *viewMenu = menuBar()->addMenu("&View");
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *exitAction = fileMenu->addAction("&Exit");
    QAction *toggleGlobFrAction = viewMenu->addAction("Toggle &global frame");
    QAction *toggleTireForceAction = viewMenu->addAction("Toggle &tire forces");
    QAction *aboutAction = helpMenu->addAction("&About");

    // Animation window
    SceneSelector sceneSelector;
    m_openGLWindow = new OpenGLWindow(&sceneSelector, 30, 
                                                  QPair<int, int>(3,3));
    QWidget *windowContainer = QWidget::createWindowContainer(m_openGLWindow);
    windowContainer->setMinimumSize(800, 600);

    // Animation player
    m_player = new AnimationPlayer(m_openGLWindow, this);
    
    // Add the animation and the player to the central widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *vertLayout = new QVBoxLayout(centralWidget);
    vertLayout->addWidget(windowContainer);
    vertLayout->addWidget(m_player);
    vertLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(vertLayout);
    setCentralWidget(centralWidget);
    
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(toggleGlobFrAction, SIGNAL(triggered()), m_openGLWindow,
        SLOT(toggleGlobalFrame()));
    connect(toggleTireForceAction, SIGNAL(triggered()), m_openGLWindow, 
        SLOT(toggleTireForce()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAboutWindow()));
}

AnimationWindow::~AnimationWindow() {
    // The animation window must be deleted since it does not belong to the main
    // window tree.
    delete(m_openGLWindow);
}

void AnimationWindow::openAboutWindow() {
    QMessageBox::information(this, "About", 
                             "3D viewer.\n\nAuthor: Louis Filipozzi\n\n"
                             "Dependencies: Assimp, Qt, OpenGL");
}






