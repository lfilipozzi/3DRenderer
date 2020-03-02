#include <QApplication>
#include "../include/animationwindow.h"
#include <iostream>


void helpPrinter() {
    std::cout << "Usage: VirtualModel [options]\n"
    << " Create a 3D animation of a vehicle from a text file\n\n"
    << "Options:\n"
    << "  -h, --help        Displays help on command line options.\n"
    << "  -v <file>         Load vehicle trajectory data file." 
    << "  -e, --env <file>  Load environment XML file." << std::endl;
}


int main(int argc, char *argv[]) {
    // Parse arguments
    QString vehicle;
    QString environment;
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"--help") == 0)) {
            helpPrinter();
            return 0;
        }
        if (strcmp(argv[i],"-v") == 0) {
            if (i+1 >= argc) {
                std::cout << "Argument '-v' must be followed by a value." 
                    << std::endl;
                return -1;
            }
            vehicle = QString(argv[++i]);
        }
        else if ((strcmp(argv[i],"-e") == 0)||(strcmp(argv[i],"--env") == 0)) {
            if (i+1 >= argc) {
                std::cout << "Argument '-v' must be followed by a value." 
                    << std::endl;
                return -1;
            }
            environment = QString(argv[++i]);
        }
        else {
            std::cout << "Invalid argument: " << argv[i] << "." << std::endl;
            return -1;
        }
    }
    
    // Start application
    QApplication app(argc, argv);
    app.setApplicationName("3D viewer");
    
    AnimationWindow animationWindow(environment);
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
 * @section install_sec Installation
 * 
 * The procedure to build the project is as follow:
 * @code{.sh}
 * mkdir build
 * cd build
 * qmake ..
 * make
 * @endcode
 * 
 * When running the program, the running directory must be the project folder, 
 * i.e. 'build/..'.
 * 
 */
