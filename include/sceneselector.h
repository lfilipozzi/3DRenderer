#ifndef SCENESELECTOR_H
#define SCENESELECTOR_H

#include <QPair>
#include "abstractscene.h"

/// Select the scene to buidl based on the version of OpenGL.
/**
 * This class is used to choose among different OpenGL context.
 */
class SceneSelector {
public:
    /**
     * Create the scene according to the OpenGL version used.
     * @param glVersion The OpenGL Version.
     * @param isOpenGLES Boolean for OpenGL ES.
     * @param refreshRate THe refresh rate of the animation.
     * @return The scene.
     */
    AbstractScene* getScene(QPair<int,int> glVersion, 
                            bool isOpenGLES, int refreshRate);
};

#endif // SCENESELECTOR_H
