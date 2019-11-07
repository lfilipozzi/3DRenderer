#include <QApplication>
#include "../include/animationwindow.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("3D viewer");
    
    AnimationWindow animationWindow;
    animationWindow.show();

    return app.exec();
}


/**
 * @mainpage 3D Renderer
 *
 * This program creates an animation of the vehicle from a text file describing
 * the vehicle trajectory (i.e. the chassis position and angles with the wheels
 * positions and angles).
 *
 * @section structure_sec Structure overview
 * 
 */
