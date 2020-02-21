#include "../include/videorecorder.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>
#include <cstring>

VideoRecorder::VideoRecorder(
    const int fps, const int width, const int height, const QString fileName
) :
    m_fps(fps), m_width(width), m_height(height) {
    // Start ffmpeg: read raw RGBA frames from stdin
    std::string cmdString = "ffmpeg -r " + std::to_string(m_fps) + 
        " -f rawvideo -pix_fmt rgba -s " + std::to_string(m_width) + 
        'x' + std::to_string(m_height) + 
        " -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 " + 
        "-vf vflip " + fileName.toStdString();
    const char * cmd = cmdString.data();

    // Open pipe to ffmpeg's stdin in binary write mode
    #ifdef __linux__ 
        p_ffmpeg = popen(cmd, "w");
    #elif _WIN32
        p_ffmpeg = _popen(cmd, "wb");
    #endif

    p_buffer = new int[m_width * m_height];
}

VideoRecorder::~VideoRecorder() {
    // Close the stream
    #ifdef __linux
        pclose(p_ffmpeg);
    #elif _WIN32
        _pclose(p_ffmpeg);
    #endif
    delete[](p_buffer);
}

void VideoRecorder::recordFrame() {
    if (p_buffer==nullptr)
        return;
    
    // Get OpenGL context
    QOpenGLContext * context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning() << __FILE__ << __LINE__ <<
                      "Requires a valid current OpenGL context. \n" <<
                      "Unable to draw the object.";
        return;
    }
    QOpenGLFunctions * glFunctions = context->functions();
    
    glFunctions->glReadPixels(
        0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, p_buffer
    );

    fwrite(p_buffer, sizeof(int) * m_width * m_height, 1, p_ffmpeg);
}
