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
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/RenderableLineString.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/Renderable.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

Renderable * Renderable::create(const Geometry::Ptr & geom,
                                const mat4          & trans)
{
    return create(geom.get(), trans);
}

Renderable * Renderable::create(const Geometry * geom,
                                const mat4     & trans)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POINT:                dmess("Implement me!"); return NULL;
        case GEOS_LINESTRING:           
        case GEOS_LINEARRING:           return RenderableLineString::create(dynamic_cast<const LineString *>(geom), trans);

        case GEOS_POLYGON:              return RenderablePolygon::create(   dynamic_cast<const Polygon *>(geom), trans);

        case GEOS_MULTIPOINT:           dmess("Implement me!"); return NULL;
        case GEOS_MULTILINESTRING:      dmess("Implement me!"); return NULL;
        case GEOS_MULTIPOLYGON:         return RenderablePolygon::create(   dynamic_cast<const MultiPolygon *>(geom), trans);

        case GEOS_GEOMETRYCOLLECTION:   dmess("Implement me!"); return NULL;
        default:
            dmess("Error!");
            abort();
    }

    return NULL;
}

Renderable::Renderable(const bool isMulti) : isMulti(isMulti), shader(ColorShader::getDefaultInstance())
{
}

Renderable::~Renderable()
{
    for(OnDelete & callback : onDeleteCallbacks) { callback(this) ;}
}

void Renderable::render(const Canvas * canvas) const { render(canvas->getMVP_Ref(), canvas->getMV_Ref()) ;}

void Renderable::addOnDeleteCallback(const OnDelete & callback) { onDeleteCallbacks.push_back(callback) ;}

Shader * Renderable::getShader() const { return shader ;}
Shader * Renderable::setShader(Shader * shader) { return this->shader = shader ;}