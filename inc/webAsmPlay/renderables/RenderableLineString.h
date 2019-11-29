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
#pragma once

#include <geos/geom/Coordinate.h>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/renderables/Renderable.h>

namespace geos
{
    namespace geom
    {
        class LineString;
    }
}

class RenderableLineString : public Renderable
{
public:

    ~RenderableLineString();

    static Renderable * create( const geos::geom::LineString * lineString,
                                const glm::dmat4             & trans    = glm::dmat4(1.0));
    
    static Renderable * create( const std::vector<geos::geom::Coordinate> & coords,
                                const glm::dmat4                          & trans    = glm::dmat4(1.0));

    static Renderable * create( const ColoredGeometryVec & lineStrings,
                                const glm::dmat4         & trans          = glm::dmat4(1.0),
                                const bool                 showProgress   = false);

    void render(Canvas * canvas, const size_t renderStage = 0) override;

	void render(const glm::mat4 & model,
				const glm::mat4 & view,
				const glm::mat4 & projection,
				const size_t	  renderStage) override;

    static void ensureShaders();

	void ensureVAO() override;

private:

    RenderableLineString(   const GLuint      ebo,
                            const GLuint      vbo,
                            const size_t      numElements,
                            const bool        isMulti);

    static Renderable * create( const FloatVec   & verts,
                                const Uint32Vec  & indices,
                                const bool         isMulti);

    GLuint m_vao = 0;

    const GLuint m_ebo;
    const GLuint m_vbo;
    const size_t m_numElements;
};
