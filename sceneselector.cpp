#include "sceneselector.h"
#include "scene_gl33.h"


AbstractScene* SceneSelector::getScene(QPair<int, int> glVersion, 
                                       bool isOpenGLES, int refreshRate) {
    if (glVersion == qMakePair(3,3) && !isOpenGLES) {
        // Use Scene class with openGL 3.3
        qDebug() << "Using OpenGL 3.3 context";
        return new Scene_GL33(refreshRate);
    }
    else if (glVersion == qMakePair(2,1)) {
        // Use Scene class with openGL 2.1/OpenGL ES
        // TODO: add support for OpenGL 2.1
        qDebug() << "OpenGL 2.1/OpenGL ES is not supported for now.";
        exit(1);
    }
    else {
        qDebug() << "OpenGL" << glVersion.first << "." << 
            glVersion.second << " is not supported.";
        exit(1);
    }
}
