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

#include <functional>
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

    virtual void render(Canvas * canvas, const size_t renderStage = 0) = 0;

    static Renderable * create( const geos::geom::Geometry::Ptr & geom,
                                const glm::dmat4                & trans = glm::mat4(1.0),
                                const AABB2D                    & boxUV = AABB2D());

    static Renderable * create( const geos::geom::Geometry  * geom,
                                const glm::dmat4            & trans = glm::mat4(1.0),
                                const AABB2D                & boxUV = AABB2D());

    void addOnDeleteCallback(const OnDelete & callback);

    Shader * getShader() const;
    Shader * setShader(Shader * shader);

    bool getRenderFill() const;
    bool getRenderOutline() const;

    bool setRenderFill(const bool render);
    bool setRenderOutline(const bool render);

	virtual void ensureVAO();

protected:

    Renderable( const bool isMulti          = false,
                const bool renderFill       = false,
                const bool renderOutline    = false);

    Renderable(const Renderable &)              = delete;
    Renderable(      Renderable &&)             = delete;
    Renderable & operator=(const Renderable &)  = delete;
    //Renderable & operator=(      Renderable &)  = delete;

    std::vector<OnDelete> m_DeleteCallbacks;

    bool m_isMulti = false;

    Shader * m_shader = NULL;

    bool m_renderFill	 = true;
    bool m_renderOutline = true;
};

