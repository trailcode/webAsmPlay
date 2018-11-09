#ifndef __WEB_ASM_PLAY_VERTEX_ARRAY_OBJECT_H__
#define __WEB_ASM_PLAY_VERTEX_ARRAY_OBJECT_H__

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

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

#include <webAsmPlay/Tessellation.h>

class VertexArrayObject
{
public:

    static VertexArrayObject * create(const Tessellations & tessellations);

    ~VertexArrayObject();

    void bind() const;

    void bindTriangles() const;

    void bindLines() const;

    void drawTriangles() const;

    void drawLines() const;

    bool isMulti() const;

private:

    template<bool IS_3D>
    static VertexArrayObject * _create(const Tessellations & tessellations);

    VertexArrayObject(  const GLuint      vao,
                        const GLuint      ebo,
                        const GLuint      ebo2,
                        const GLuint      vbo,
                        const GLuint      numTrianglesIndices,
                        const Uint32Vec & counterVertIndices,
                        const size_t      numContourLines,
                        const bool        isMulti);

    VertexArrayObject(const VertexArrayObject&) {}                            // Prevent copy-construction
    VertexArrayObject& operator=(const VertexArrayObject&) { return *this ;}  // Prevent assignment

    const GLuint    vao                 = 0;
    const GLuint    ebo                 = 0;
    const GLuint    ebo2                = 0;
    const GLuint    vbo                 = 0;
    const GLuint    numTrianglesIndices = 0;
    const size_t    numContourLines     = 0;
    const bool      _isMulti            = false;

    const Uint32Vec counterVertIndices;
};

#endif // __WEB_ASM_PLAY_VERTEX_ARRAY_OBJECT_H__