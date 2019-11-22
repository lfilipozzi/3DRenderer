#include "../include/line.h"

#include <QOpenGLContext>

void Line::initialize() {
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning() << __FILE__ << __LINE__ <<
                      "Requires a valid current OpenGL context. \n" <<
                      "Unable to draw the object.";
        return;
    }
    p_glFunctions = context->functions();
    
    p_shader = std::make_unique<Shader>(
        ":/shaders/line.vert", ":/shaders/line.frag"
    );
    createBuffers();
    createAttributes();
    
    m_isInitialized = true;
}


void Line::render(
    const CasterLight & /*light*/, const QMatrix4x4 & view, 
    const QMatrix4x4 & projection, 
    const std::array<QMatrix4x4,NUM_CASCADES> & /*lightSpace*/
) {
    // Check if the object has been initialized
    if (!m_isInitialized) {
        qCritical() << __FILE__ << __LINE__
            << "The object must be initialized before being rendered.";
        exit(1);
    }
    
    // Bind shader program
    p_shader->bind();
    
    // Bind VAO and draw everything
    m_vao.bind();
    QMatrix4x4 mvp = projection * view * m_model;

    p_shader->setUniformValue("MVP", mvp); // Matrix to clip space
    p_shader->setUniformValue("color", m_color);

    p_glFunctions->glDrawElements(
        GL_LINES, static_cast<GLsizei>(m_indices.size()+1), GL_UNSIGNED_INT,
        reinterpret_cast<const void*>(0)
    );
    m_vao.release();
    float width;
    p_glFunctions->glGetFloatv(GL_LINE_WIDTH, &width);
}


void Line::renderShadow(const QMatrix4x4 & /*lightSpace*/) {
    // Nothing to do: Do not render the shadow of the line.
}


void Line::cleanUp() {}


void Line::getBufferData(
    QVector<float> & vertices, QVector<unsigned int> & indices
) {
    // Define vertices
    for (
        QVector<QVector3D>::iterator it = m_vertices.begin(); 
        it != m_vertices.end(); ++it
    ) {
        vertices.push_back(it->x());
        vertices.push_back(it->y());
        vertices.push_back(it->z());
    }
    
    // Define indices
    indices = m_indices;
}


void Line::createBuffers() {
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


void Line::createAttributes() {
    m_vao.bind();
    // Set up the vertex array state
    p_shader->bind();

    // Map vertex data to the vertex shader layout location '0'
    m_vertexBuffer.bind();
    p_shader->enableAttributeArray(0);        // layout location
    p_shader->setAttributeBuffer(0,           // layout location
                                 GL_FLOAT,    // data type
                                 0,           // Offset to data in buffer
                                 3);          // number of components
}





