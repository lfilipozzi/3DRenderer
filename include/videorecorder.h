#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <stdio.h>
#include <QString>

/// Video VideoRecorder
/**
 * @brief Record the animation in a video file.
 * @author Louis Filipozzi
 */
class VideoRecorder {
public:
    /**
     * @brief Constructor of the recorder.
     * @param fps The frame rate of the video.
     * @param width The width of the video.
     * @param height The height of the video.
     * @param filename The name of the video file to create
     */
    VideoRecorder(
        const int fps, const int width, const int height, const QString filename
    );
    ~VideoRecorder();
    
    /**
     * @brief Record one frame from the current OpenGL context.
     */
    void recordFrame();
    
private:
    const int m_fps, m_width, m_height;
    FILE * p_ffmpeg;
    int* p_buffer;
};

#endif // VIDEORECORDER_H
