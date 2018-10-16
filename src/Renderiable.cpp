#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderiableLineString2D.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/Renderiable.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

namespace
{
    Shader * defaultShader = NULL;
}

void Renderiable::ensureShader()
{
    if(defaultShader) { return ;}
    
    dmess("Renderiable::ensureShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 position;
        out vec4 vertexColor;
        uniform mat4 MVP;
        uniform vec4 vertexColorIn;

        void main()
        {
            gl_Position = MVP * vec4(position.xy, 0, 1);
            vertexColor = vertexColorIn;
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

Shader * Renderiable::getDefaultShader() { return defaultShader ;}

Renderiable * Renderiable::create(  const Geometry::Ptr & geom,
                                    const mat4          & trans,
                                    const vec4          & fillColor, 
                                    const vec4          & outlineColor,
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

Renderiable * Renderiable::create(  const Geometry  * geom,
                                    const mat4      & trans,
                                    const vec4      & fillColor,
                                    const vec4      & outlineColor,
                                    const bool        renderOutline,
                                    const bool        renderFill)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POINT:                dmess("Implement me!"); return NULL;
        case GEOS_LINESTRING:           dmess("Implement me!"); return NULL;
        case GEOS_LINEARRING:           return RenderiableLineString2D::create( dynamic_cast<const LineString *>(geom),
                                                                                trans,
                                                                                fillColor,
                                                                                outlineColor,
                                                                                renderOutline,
                                                                                renderFill);

        case GEOS_POLYGON:              return RenderiablePolygon2D::create(dynamic_cast<const Polygon *>(geom),
                                                                            trans,
                                                                            fillColor,
                                                                            outlineColor,
                                                                            renderOutline,
                                                                            renderFill);

        case GEOS_MULTIPOINT:           dmess("Implement me!"); return NULL;
        case GEOS_MULTILINESTRING:      dmess("Implement me!"); return NULL;
        case GEOS_MULTIPOLYGON:         return RenderiablePolygon2D::create(dynamic_cast<const MultiPolygon *>(geom),
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

Renderiable::Renderiable(   const bool   isMulti,
                            const vec4 & fillColor,
                            const vec4 & outlineColor,
                            const bool   renderOutline,
                            const bool   renderFill) :  fillColor       (fillColor),
                                                        outlineColor    (outlineColor),
                                                        renderOutline   (renderOutline),
                                                        renderFill      (renderFill),
                                                        isMulti         (isMulti)
{
}

Renderiable::~Renderiable()
{
    for(OnDelete & callback : onDeleteCallbacks) { callback(this) ;}
}

vec4 Renderiable::setFillColor(const vec4 & fillColor) { return this->fillColor = fillColor ;}

vec4 Renderiable::getFillColor() const { return fillColor ;}

vec4 Renderiable::setOutlineColor(const vec4 & outlineColor) { return this->outlineColor = outlineColor ;}

vec4 Renderiable::getOutlineColor() const { return outlineColor ;}

bool Renderiable::setRenderOutline(const bool renderOutline) { return this->renderOutline = renderOutline ;}

bool Renderiable::getRenderOutline() const { return renderOutline ;}

bool Renderiable::setRenderFill(const bool renderFill) { return this->renderFill = renderFill ;}

bool Renderiable::getRenderFill() const { return renderFill ;}

void Renderiable::addOnDeleteCallback(const OnDelete & callback) { onDeleteCallbacks.push_back(callback) ;}

namespace
{
    vec4 defaultFillColor(0.7, 0.5, 0, 0.5);
    vec4 defaultOutlineColor(1,0,0,1);
    bool defaultDoFill = true;
    bool defaultDoOutline = true;
}

vec4 Renderiable::setDefaultFillColor(const vec4 & fillColor) { return defaultFillColor = fillColor ;}
vec4 Renderiable::getDefaultFillColor() { return defaultFillColor ;}
vec4 Renderiable::setDefaultOutlineColor(const vec4 & outlineColor) { return defaultOutlineColor = outlineColor ;}
vec4 Renderiable::getDefaultOutlineColor() { return defaultOutlineColor ;}
bool Renderiable::setDefaultRenderFill(const bool renderFill) { return defaultDoFill = renderFill ;}
bool Renderiable::getDefaultRenderFill() { return defaultDoFill ;}
bool Renderiable::setDefaultRenderOutline(const bool renderOutline) { return defaultDoOutline = renderOutline ;}
bool Renderiable::getDefaultRenderOutline() { return defaultDoOutline ;}