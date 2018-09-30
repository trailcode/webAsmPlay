#ifndef __WEB_ASM_PLAY_FRAME_BUFFER_H__
#define __WEB_ASM_PLAY_FRAME_BUFFER_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
#endif // __EMSCRIPTEN__
#include <tceGeom/vec2.h>

class FrameBuffer
{
public:

    static FrameBuffer * create(const tce::geom::Vec2i & bufferSize);

    static FrameBuffer * ensureFrameBuffer(FrameBuffer * currBuffer, const tce::geom::Vec2i & bufferSize);

    ~FrameBuffer();

    tce::geom::Vec2i getBufferSize() const;

    void bind();
    void unbind();

    GLuint getTextureID() const;

private:
    
    FrameBuffer(const GLuint framebuffer,
                const GLuint textureColorbuffer,
                const GLuint rbo,
                const tce::geom::Vec2i & bufferSize);

    const GLuint framebuffer;
    const GLuint textureColorbuffer;
    const GLuint rbo;
    const tce::geom::Vec2i bufferSize;
};

#endif // __WEB_ASM_PLAY_FRAME_BUFFER_H__