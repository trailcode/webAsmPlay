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

#ifndef __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <geos/geom/Geometry.h>
#include <webAsmPlay/Types.h>

class Shader;

class Canvas;

class Renderable
{
public:

    typedef std::function<void (Renderable *)> OnDelete;

    virtual ~Renderable();

    void render(const Canvas * canvas) const;

    virtual void render(const glm::mat4 & MVP, const glm::mat4 & MV) const = 0;

    static Renderable * create( const geos::geom::Geometry::Ptr & geom,
                                const glm::mat4                 & trans         = glm::mat4(1.0),
                                const GLuint fillColor     = 0,
                                const GLuint outlineColor  = 0,
                                const bool                        renderOutline = getDefaultRenderOutline(),
                                const bool                        renderFill    = getDefaultRenderFill());

    static Renderable * create( const geos::geom::Geometry  * geom,
                                const glm::mat4             & trans         = glm::mat4(1.0),
                                const GLuint fillColor     = 0,
                                const GLuint outlineColor  = 0,
                                const bool                    renderOutline = getDefaultRenderOutline(),
                                const bool                    renderFill    = getDefaultRenderFill());

    static void ensureShader();

    static Shader * getDefaultShader();

    GLuint setFillColor(const GLuint fillColor);
    GLuint getFillColor() const;

    GLuint setOutlineColor(const GLuint outlineColor);
    GLuint getOutlineColor() const;

    bool setRenderOutline(const bool renderOutline);
    bool getRenderOutline() const;

    bool setRenderFill(const bool renderFill);
    bool getRenderFill() const;

    void addOnDeleteCallback(const OnDelete & callback);

    static GLuint setDefaultFillColor(const GLuint fillColor);
    static GLuint getDefaultFillColor();

    static GLuint setDefaultOutlineColor(const GLuint outlineColor);
    static GLuint getDefaultOutlineColor();

    static bool setDefaultRenderFill(const bool renderFill);
    static bool getDefaultRenderFill();

    static bool setDefaultRenderOutline(const bool renderOutline);
    static bool getDefaultRenderOutline();

protected:

    Renderable( const bool        isMulti,
                const GLuint fillColor,
                const GLuint outlineColor,
                const bool        renderOutline,
                const bool        renderFill);

    std::vector<OnDelete> onDeleteCallbacks;

    bool        isMulti;
    GLuint   fillColor;
    GLuint   outlineColor;
    bool        renderOutline;
    bool        renderFill;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__