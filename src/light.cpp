#include "../include/light.h"
#include <cmath>

#define NUMBER_FRUSTUM_CORNERS 8


/***
 *       _____          _             
 *      / ____|        | |            
 *     | |     __ _ ___| |_ ___ _ __  
 *     | |    / _` / __| __/ _ \ '__| 
 *     | |___| (_| \__ \ ||  __/ |    
 *      \_____\__,_|___/\__\___|_|_   
 *          | |    (_)     | |   | |  
 *          | |     _  __ _| |__ | |_ 
 *          | |    | |/ _` | '_ \| __|
 *          | |____| | (_| | | | | |_ 
 *          |______|_|\__, |_| |_|\__|
 *                     __/ |          
 *                    |___/           
 */

std::array<QMatrix4x4,NUM_CASCADES> CasterLight::getLightSpaceMatrix(
    const Camera & camera, std::array<float,NUM_CASCADES+1> cascades
) const {
    std::array<QMatrix4x4,NUM_CASCADES> lightSpace;
    
    // Compute the light view matrix
    QMatrix4x4 lightView;
    lightView.lookAt(
        QVector3D(0.0f, 0.0f, 0.0f),    // (Dummy) light position
        m_direction.toVector3D(),       // Light direction
        QVector3D(0.0f, 0.0f, 1.0f)     // Light up axis
    );
    
    // Compute the light projection matrices
    std::array<QMatrix4x4,NUM_CASCADES> lightProjection;
    lightProjection = getProjectionMatrix(camera, cascades);
    for (unsigned int i = 0; i < NUM_CASCADES; i++) {
        lightSpace[i] = lightProjection[i] * lightView;
    }
    
    return lightSpace;
}

std::array<QMatrix4x4,NUM_CASCADES> CasterLight::getProjectionMatrix(
    const Camera& camera, std::array<float,NUM_CASCADES+1> cascades
) const {
    QMatrix4x4 lightView = getViewMatrix();
    
    // Compute the light projection matrices
    std::array<QMatrix4x4,NUM_CASCADES> lightProjection;
    /* Four steps are needed:
     *  1. Compute the eight corners of each cascade in the camera view space.
     *  2. Transform the coordinates from camera view space to world space (with
     *     inverse of camera view matrix).
     *  3. Transform from world space to light view space (with light view 
     *     matrix).
     *  4. Compute light projection matrix which encompass the bounding box (the
     *     eight corner of the frustum).
     */

    // Compute tangent of vertical and horizontal FOV
    std::pair<float, float> FOV = camera.getFOV();
    float tanHalfHFOV = std::tan(FOV.first  / 2 * PI / 180);
    float tanHalfVFOV = std::tan(FOV.second / 2 * PI / 180);

    for (unsigned int i = 0; i < NUM_CASCADES; i++) {
        float xn = cascades[i]   * tanHalfHFOV;
        float xf = cascades[i+1] * tanHalfHFOV;
        float yn = cascades[i]   * tanHalfVFOV;
        float yf = cascades[i+1] * tanHalfVFOV;
        
        // Compute the eight corners of each cascade in the camera view space
        QVector4D frustumCorners[NUMBER_FRUSTUM_CORNERS] = {
            // Near face
            QVector4D( xn,  yn, cascades[i], 1.0f),
            QVector4D(-xn,  yn, cascades[i], 1.0f),
            QVector4D( xn, -yn, cascades[i], 1.0f),
            QVector4D(-xn, -yn, cascades[i], 1.0f),
            // Far face
            QVector4D( xf,  yf, cascades[i+1], 1.0f),
            QVector4D(-xf,  yf, cascades[i+1], 1.0f),
            QVector4D( xf, -yf, cascades[i+1], 1.0f),
            QVector4D(-xf, -yf, cascades[i+1], 1.0f)
        };
        
        // Corners of each cascade in the light view space
        QVector4D frustumCornersLight[NUMBER_FRUSTUM_CORNERS];
        QMatrix4x4 V2lightV = lightView * camera.getViewMatrix().inverted();
        float left   =  std::numeric_limits<float>::max();
        float right  = -std::numeric_limits<float>::max();
        float bottom =  std::numeric_limits<float>::max();
        float top    = -std::numeric_limits<float>::max();
        float near   =  std::numeric_limits<float>::max();
        float far    = -std::numeric_limits<float>::max();
        
        // Compute the eight corners of each cascade in the light view space
        for (unsigned int j = 0; j < NUMBER_FRUSTUM_CORNERS; j++) {
            // Transform the frustum coordinate from camera view to light view
            frustumCornersLight[j] = V2lightV * frustumCorners[j];
            
            // Find the box of the cascade in light view space
            left   = std::min(left,   frustumCornersLight[j].x());
            right  = std::max(right,  frustumCornersLight[j].x());
            bottom = std::min(bottom, frustumCornersLight[j].y());
            top    = std::max(top,    frustumCornersLight[j].y());
            near   = std::min(near,   frustumCornersLight[j].z());
            far    = std::max(far,    frustumCornersLight[j].z());
        }
        
        // Compute the projection matrix which contains the frustum corners
        QMatrix4x4 projection;
        projection.ortho(left, right, bottom, top, -far, -near);
        lightProjection[i] = projection;
    }
    
    return lightProjection;
}



/***
 *      _____      _       _          
 *     |  __ \    (_)     | |         
 *     | |__) |__  _ _ __ | |_        
 *     |  ___/ _ \| | '_ \| __|       
 *     | |  | (_) | | | | | |_        
 *     |_|   \___/|_|_| |_|\__|   _   
 *          | |    (_)     | |   | |  
 *          | |     _  __ _| |__ | |_ 
 *          | |    | |/ _` | '_ \| __|
 *          | |____| | (_| | | | | |_ 
 *          |______|_|\__, |_| |_|\__|
 *                     __/ |          
 *                    |___/           
 */

// PointLight::PointLight(QVector3D intensity, QVector3D position) : 
//     ABCLight(intensity), m_position(position) {
// }
// 
// PointLight::~PointLight() {
// }
// 
// QMatrix4x4 PointLight::getLightSpaceMatrix(QVector3D lightTarget) const {
//     // Compute light view matrix
//     QMatrix4x4 lightView;
//     lightView.lookAt(m_position, lightTarget, QVector3D(0.0f, 0.0f, 1.0f));
//     // Compute light projection matrix
//     QMatrix4x4 lightProjection;
//     lightProjection.ortho(-4.0f, 4.0f, -4.0f, 4.0f, 1.0f, 10.0f);
//     // Compute light projection view matrix
//     QMatrix4x4 lightSpaceMatrix = lightProjection * lightView;
//     return lightSpaceMatrix;
// }

