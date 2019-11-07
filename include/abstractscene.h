#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include "camera.h"
#include "abstractvehicle.h"

/// Abstract class Scene.
/**
 * This is an interface for scenes with different version of OpenGL.
 */
class AbstractScene {
public:
    AbstractScene(int refreshRate);
    virtual ~AbstractScene();

    /**
     * Initialize the animation.
     */
    virtual void initialize() = 0;

    /**
     * Resize the animation.
     * @param w The new width of the animation.
     * @param h The new height of the animation.
     */
    virtual void resize(int w, int h) = 0;

    /**
     * Update the animation.
     */
    virtual void update() = 0;

    /**
     * Cleanup the animation.
     */
    virtual void cleanup() = 0;

    

protected:
    
};

#endif // ABSTRACTSCENE_H
