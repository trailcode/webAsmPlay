/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/

#include <webAsmPlay/Util.h>
#include <webAsmPlay/FrameBuffer.h>

using namespace glm;

FrameBuffer * FrameBuffer::create(const ivec2 & bufferSize)
{
    GLuint framebuffer          = 0;
    GLuint textureColorbuffer   = 0;
    GLuint rbo                  = 0;

    GL_CHECK(glGenFramebuffers(1, &framebuffer));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

    // create a color attachment texture
    GL_CHECK(glGenTextures(1, &textureColorbuffer));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureColorbuffer));
    //GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferSize.x, bufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    //GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferSize.x, bufferSize.y, 0, GL_RGBA, GL_FLOAT, NULL));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bufferSize.x, bufferSize.y, 0, GL_RGBA, GL_FLOAT, NULL));
    //GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, bufferSize.x, bufferSize.y, 0, GL_LUMINANCE, GL_FLOAT, NULL));
    //GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferSize.x, bufferSize.y, 0, GL_RGBA, GL_UNSIGNED_   INT, NULL));
    //GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferSize.x, bufferSize.y, 0, GL_RGB, GL_HALF_FLOAT, NULL));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0));

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    GL_CHECK(glGenRenderbuffers(1, &rbo));
    GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
    GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferSize.x, bufferSize.y)); // use a single renderbuffer object for both a depth AND stencil buffer.
    //GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, bufferSize.x, bufferSize.y)); // use a single renderbuffer object for both a depth AND stencil buffer.
    GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo)); // now actually attach it
    //GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo)); // now actually attach it

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        dmess("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

        GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        return NULL;
    }
    
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    return new FrameBuffer( framebuffer,
                            textureColorbuffer,
                            rbo,
                            bufferSize);
}

FrameBuffer * FrameBuffer::ensureFrameBuffer(FrameBuffer * currBuffer, const ivec2 & bufferSize)
{
    if(!currBuffer || bufferSize != currBuffer->getBufferSize())
    {
        delete currBuffer;

        return create(bufferSize);
    }

    return currBuffer;
}

FrameBuffer::FrameBuffer(   const GLuint   framebuffer,
                            const GLuint   textureColorbuffer,
                            const GLuint   rbo,
                            const ivec2  & bufferSize) : framebuffer         (framebuffer),
                                                         textureColorbuffer  (textureColorbuffer),
                                                         rbo                 (rbo),
                                                         bufferSize          (bufferSize)
{
}

FrameBuffer::~FrameBuffer()
{
    unbind();

    GL_CHECK(glDeleteTextures        (1, &textureColorbuffer));
 	GL_CHECK(glDeleteRenderbuffers   (1, &rbo));
    GL_CHECK(glDeleteFramebuffers    (1, &framebuffer));
}

ivec2 FrameBuffer::getBufferSize() const { return bufferSize ;}

void FrameBuffer::bind()
{
    GL_CHECK(glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevFB));

    //dmess("prevFB " << prevFB << " framebuffer " << framebuffer);

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

    //GL_CHECK(glViewport(0, 0, bufferSize.x, bufferSize.y));
}

void FrameBuffer::unbind() { GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, prevFB)) ;}
//void FrameBuffer::unbind() { GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 1)) ;}

GLuint FrameBuffer::getTextureID() const { return textureColorbuffer ;}