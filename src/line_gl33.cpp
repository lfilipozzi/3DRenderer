#include "../include/line_gl33.h"

Line_GL33::Line_GL33(QVector<QVector3D> vertices, 
                     QVector<unsigned int> indices, QVector3D color) : 
    //START_POINT(startPoint), 
    //AXIS(axis), 
    m_color(color),
    m_vertices(vertices),
    m_indices(indices) {
    
}

Line_GL33::~Line_GL33() {
}


void Line_GL33::initialize() {
    initializeOpenGLFunctions();
    
    createShaderProgram(":/shaders/arrow.vert", ":/shaders/arrow.frag");
    
    createBuffers();
    createAttributes();
}

void Line_GL33::getBufferData(QVector<float> &vertices, 
                               QVector<unsigned int> &indices) {
    // Define vertices
    QVector<QVector3D>::iterator it;// = m_vertices.begin();
    for (it=m_vertices.begin(); it!=m_vertices.end(); ++it) {
        vertices.push_back(it->x());
        vertices.push_back(it->y());
        vertices.push_back(it->z());
    }
    
    // Define indices
    indices = m_indices;
}

void Line_GL33::createBuffers() {
    QVector<float> vertices;
    QVector<unsigned int> indices;
    
    getBufferData(vertices, indices);
    
    // Create a vertex array object
    m_vao.create();
    m_vao.bind();

    // Create a buffer and copy the vertex data to it
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(&(vertices)[0], vertices.size() 
        * static_cast<int>(sizeof(float)));

    // Create a buffer and copy the index data to it
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuffer.bind();
    m_indexBuffer.allocate(&(indices)[0], indices.size() 
        * static_cast<int>(sizeof(unsigned int)));
}

void Line_GL33::createAttributes() {
    m_vao.bind();
    // Set up the vertex array state
    m_shaderProgram.bind();

    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    m_shaderProgram.enableAttributeArray(0);        // layout location
    m_shaderProgram.setAttributeBuffer(0,           // layout location
                                       GL_FLOAT,    // data's type
                                       0,           // Offset to data in buffer
                                       3);          // number of components
}

void Line_GL33::update(const CasterLight &/*light*/, 
                       const QMatrix4x4 view,
                       const QMatrix4x4 projection, 
                       const QMatrix4x4 /*lightSpaceMatrix*/) {
    // Bind shader program
    m_shaderProgram.bind();
    
    // Bind VAO and draw everything
    m_vao.bind();
    QMatrix4x4 mvp = projection * view * m_model;

    m_shaderProgram.setUniformValue("MVP", mvp); // Matrix to clip space
    m_shaderProgram.setUniformValue("color", m_color);

    glDrawElements(GL_LINES, static_cast<GLsizei>(m_indices.size()+1), GL_UNSIGNED_INT,
                        reinterpret_cast<const void*>(0));
    m_vao.release();
    float width;
    glGetFloatv(GL_LINE_WIDTH, &width);
}

void Line_GL33::cleanup() {
    
}
