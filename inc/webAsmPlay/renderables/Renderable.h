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

#ifndef __WEB_ASM_PLAY__GEOS_RENDERABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERABLE_H__

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
                                const glm::mat4                 & trans = glm::mat4(1.0));

    static Renderable * create( const geos::geom::Geometry  * geom,
                                const glm::mat4             & trans = glm::mat4(1.0));

    void addOnDeleteCallback(const OnDelete & callback);

    Shader * getShader() const;
    Shader * setShader(Shader * shader);

    bool getRenderFill() const;
    bool getRenderOutline() const;

    bool setRenderFill(const bool render);
    bool setRenderOutline(const bool render);

protected:

    Renderable(const bool isMulti);

    std::vector<OnDelete> onDeleteCallbacks;

    bool isMulti;

    Shader * shader;

    bool renderFill;
    bool renderOutline;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERABLE_H__