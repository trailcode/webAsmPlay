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

#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderableLineString.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/Renderable.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

namespace
{
    Shader * defaultShader = NULL;
}

void Renderable::ensureShader()
{
    if(defaultShader) { return ;}
    
    dmess("Renderable::ensureShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 vertIn;
        out vec4 vertexColor;
        uniform mat4 MVP;
        uniform vec4 colorIn;

        void main()
        {
            gl_Position = MVP * vec4(vertIn.xy, 0, 1);
            vertexColor = colorIn;
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec4 vertexColor;

        void main()
        {
            outColor = vertexColor;
        }
    )glsl";

    defaultShader = Shader::create(vertexSource, fragmentSource);
}

Shader * Renderable::getDefaultShader() { return defaultShader ;}

Renderable * Renderable::create(const Geometry::Ptr & geom,
                                const mat4          & trans,
                                const GLuint fillColor,
                                const GLuint outlineColor,
                                const bool            renderOutline,
                                const bool            renderFill)
{
    return create(  geom.get(),
                    trans,
                    fillColor,
                    outlineColor,
                    renderOutline,
                    renderFill);
}

Renderable * Renderable::create(const Geometry  * geom,
                                const mat4      & trans,
                                const GLuint fillColor,
                                const GLuint outlineColor,
                                const bool        renderOutline,
                                const bool        renderFill)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POINT:                dmess("Implement me!"); return NULL;
        case GEOS_LINESTRING:           
        case GEOS_LINEARRING:           return RenderableLineString::create(dynamic_cast<const LineString *>(geom),
                                                                            trans,
                                                                            fillColor,
                                                                            outlineColor,
                                                                            renderOutline,
                                                                            renderFill);

        case GEOS_POLYGON:              return RenderablePolygon::create(   dynamic_cast<const Polygon *>(geom),
                                                                            trans,
                                                                            fillColor,
                                                                            outlineColor,
                                                                            renderOutline,
                                                                            renderFill);

        case GEOS_MULTIPOINT:           dmess("Implement me!"); return NULL;
        case GEOS_MULTILINESTRING:      dmess("Implement me!"); return NULL;
        case GEOS_MULTIPOLYGON:         return RenderablePolygon::create(   dynamic_cast<const MultiPolygon *>(geom),
                                                                            trans,
                                                                            fillColor,
                                                                            outlineColor,
                                                                            renderOutline,
                                                                            renderFill);

        case GEOS_GEOMETRYCOLLECTION:   dmess("Implement me!"); return NULL;
        default:
            dmess("Error!");
            abort();
    }

    return NULL;
}

Renderable::Renderable( const bool   isMulti,
                        const GLuint fillColor,
                        const GLuint outlineColor,
                        const bool   renderOutline,
                        const bool   renderFill) :  fillColor       (fillColor),
                                                    outlineColor    (outlineColor),
                                                    renderOutline   (renderOutline),
                                                    renderFill      (renderFill),
                                                    isMulti         (isMulti)
{
}

Renderable::~Renderable()
{
    for(OnDelete & callback : onDeleteCallbacks) { callback(this) ;}
}

void Renderable::render(const Canvas * canvas) const { render(canvas->getMVP_Ref(), canvas->getMV_Ref()) ;}

GLuint Renderable::setFillColor(const GLuint fillColor) { return this->fillColor = fillColor ;}

GLuint Renderable::getFillColor() const { return fillColor ;}

GLuint Renderable::setOutlineColor(const GLuint outlineColor) { return this->outlineColor = outlineColor ;}

GLuint Renderable::getOutlineColor() const { return outlineColor ;}

bool Renderable::setRenderOutline(const bool renderOutline) { return this->renderOutline = renderOutline ;}

bool Renderable::getRenderOutline() const { return renderOutline ;}

bool Renderable::setRenderFill(const bool renderFill) { return this->renderFill = renderFill ;}

bool Renderable::getRenderFill() const { return renderFill ;}

void Renderable::addOnDeleteCallback(const OnDelete & callback) { onDeleteCallbacks.push_back(callback) ;}

namespace
{
    GLuint defaultFillColor = 1;
    GLuint defaultOutlineColor = 1;
    bool defaultDoFill = true;
    bool defaultDoOutline = true;
}

GLuint Renderable::setDefaultFillColor(const GLuint fillColor) { return defaultFillColor = fillColor ;}
GLuint Renderable::getDefaultFillColor() { return defaultFillColor ;}
GLuint Renderable::setDefaultOutlineColor(const GLuint outlineColor) { return defaultOutlineColor = outlineColor ;}
GLuint Renderable::getDefaultOutlineColor() { return defaultOutlineColor ;}
bool Renderable::setDefaultRenderFill(const bool renderFill) { return defaultDoFill = renderFill ;}
bool Renderable::getDefaultRenderFill() { return defaultDoFill ;}
bool Renderable::setDefaultRenderOutline(const bool renderOutline) { return defaultDoOutline = renderOutline ;}
bool Renderable::getDefaultRenderOutline() { return defaultDoOutline ;}