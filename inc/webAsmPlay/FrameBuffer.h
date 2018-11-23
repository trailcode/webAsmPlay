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

#ifndef __WEB_ASM_PLAY_FRAME_BUFFER_H__
#define __WEB_ASM_PLAY_FRAME_BUFFER_H__

#include <glm/vec2.hpp>
#include <webAsmPlay/OpenGL_Util.h>

class FrameBuffer
{
public:

    static FrameBuffer * create(const glm::ivec2 & bufferSize);

    static FrameBuffer * ensureFrameBuffer(FrameBuffer * currBuffer, const glm::ivec2 & bufferSize);

    ~FrameBuffer();

    glm::ivec2 getBufferSize() const;

    void bind();
    void unbind();

    GLuint getTextureID() const;

private:
    
    FrameBuffer(const GLuint       framebuffer,
                const GLuint       textureColorbuffer,
                const GLuint       rbo,
                const glm::ivec2 & bufferSize);

    const GLuint framebuffer;
    const GLuint textureColorbuffer;
    const GLuint rbo;

    const glm::ivec2 bufferSize;
};

#endif // __WEB_ASM_PLAY_FRAME_BUFFER_H__