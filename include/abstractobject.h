#ifndef ABSTRACTOBJECT_H
#define ABSTRACTOBJECT_H

#include "light.h"
#include <QMatrix4x4>

/// Abstract object class
/**
 * @brief Define the abstract base class of a 3D object.
 * @author Louis Filipozzi
 */
class ABCObject {
public:
    virtual ~ABCObject() {};
    
    /**
     * @brief Initialize the object, i.e. create the buffers, attributes, ...
     */
    virtual void initialize() = 0;
    
    /**
     * @brief Draw the object.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    virtual void render(
        const CasterLight & light, const QMatrix4x4 & view, 
        const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace
    ) = 0;
    
    /**
     * @brief Draw the object when computing the framebuffer for shadow mapping.
     * @param view The view matrix.
     * @param projection The projection matrix.
     * @param lightSpace The view and projection matrix of the light (used for 
     * shadow mapping).
     */
    virtual void renderShadow(
        const CasterLight & light, const QMatrix4x4 & view, 
        const QMatrix4x4 & projection, const QMatrix4x4 & lightSpace
    ) = 0;
    
    /**
     * @brief Clean up the object.
     */
    virtual void cleanUp() = 0;
    
    /**
     * @brief Set the model matrix of the object to position the object as 
     * desired.
     * @param model The model matrix.
     */
    void setModelMatrix(const QMatrix4x4 & model) {m_model = model;};
    
protected:
    /**
     * Model matrix of the object.
     */
    QMatrix4x4 m_model;
};

#endif // ABSTRACTOBJECT_H
