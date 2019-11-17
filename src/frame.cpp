#include "../include/frame.h"

Frame::Frame(QVector3D origin) : 
    X_AXIS(QVector<QVector3D>({origin, origin+QVector3D(1.0f, 0.0f, 0.0f)}),
           QVector<unsigned int>({0, 1}), 
           QVector3D(1.0f, 0.0f, 0.0f)), 
    Y_AXIS(QVector<QVector3D>({origin, origin+QVector3D(0.0f, 1.0f, 0.0f)}), 
           QVector<unsigned int>({0, 1}),
           QVector3D(0.0f, 1.0f, 0.0f)), 
    Z_AXIS(QVector<QVector3D>({origin, origin+QVector3D(0.0f, 0.0f, 1.0f)}),
           QVector<unsigned int>({0, 1}), 
           QVector3D(0.0f, 0.0f, 1.0f)) {
    
}


void Frame::initialize() {
    X_AXIS.initialize();
    Y_AXIS.initialize();
    Z_AXIS.initialize();
}


void Frame::update(
    const CasterLight &light, const QMatrix4x4 view,
    const QMatrix4x4 projection, const QMatrix4x4 lightSpaceMatrix
) {
    X_AXIS.render(light, view, projection, lightSpaceMatrix);
    Y_AXIS.render(light, view, projection, lightSpaceMatrix);
    Z_AXIS.render(light, view, projection, lightSpaceMatrix);
}


void Frame::cleanup() {
    X_AXIS.cleanUp();
    Y_AXIS.cleanUp();
    Z_AXIS.cleanUp();
}


void Frame::setModelMatrix(QMatrix4x4 const model) {
    X_AXIS.setModelMatrix(model);
    Y_AXIS.setModelMatrix(model);
    Z_AXIS.setModelMatrix(model);
}


