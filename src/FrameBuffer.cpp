#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
#endif // __EMSCRIPTEN__

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/FrameBuffer.h>

using namespace tce::geom;

FrameBuffer * FrameBuffer::create(const Vec2i & bufferSize)
{
    GLuint framebuffer          = 0;
    GLuint textureColorbuffer   = 0;
    GLuint rbo                  = 0;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferSize.x, bufferSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferSize.x, bufferSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        dmess("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return NULL;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return new FrameBuffer( framebuffer,
                            textureColorbuffer,
                            rbo,
                            bufferSize);
}

FrameBuffer * FrameBuffer::ensureFrameBuffer(FrameBuffer * currBuffer, const Vec2i & bufferSize)
{
    if(!currBuffer || bufferSize != currBuffer->getBufferSize())
    {
        delete currBuffer;

        return create(bufferSize);
    }

    return currBuffer;
}

FrameBuffer::FrameBuffer(   const GLuint framebuffer,
                            const GLuint textureColorbuffer,
                            const GLuint rbo,
                            const Vec2i & bufferSize) : framebuffer         (framebuffer),
                                                        textureColorbuffer  (textureColorbuffer),
                                                        rbo                 (rbo),
                                                        bufferSize          (bufferSize)
{
}

FrameBuffer::~FrameBuffer()
{
    unbind();

    glDeleteTextures        (1, &textureColorbuffer);
 	glDeleteRenderbuffers   (1, &rbo);
    glDeleteFramebuffers    (1, &framebuffer);
}

Vec2i FrameBuffer::getBufferSize() const
{
    return bufferSize;
}

void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBuffer::getTextureID() const
{
    return textureColorbuffer;
}