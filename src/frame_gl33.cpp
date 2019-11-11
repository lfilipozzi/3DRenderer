#include "../include/frame_gl33.h"

Frame_GL33::Frame_GL33(QVector3D origin) : 
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

Frame_GL33::~Frame_GL33() {
    
}


void Frame_GL33::initialize() {
    X_AXIS.initialize();
    Y_AXIS.initialize();
    Z_AXIS.initialize();
}

void Frame_GL33::update(const CasterLight &light, const QMatrix4x4 view,
                        const QMatrix4x4 projection, 
                        const QMatrix4x4 lightSpaceMatrix) {
    X_AXIS.render(light, view, projection, lightSpaceMatrix);
    Y_AXIS.render(light, view, projection, lightSpaceMatrix);
    Z_AXIS.render(light, view, projection, lightSpaceMatrix);
}

void Frame_GL33::cleanup() {
    X_AXIS.cleanUp();
    Y_AXIS.cleanUp();
    Z_AXIS.cleanUp();
}
