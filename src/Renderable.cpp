#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
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

Renderable * Renderable::create(const Geometry  * geom,
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

Renderable::~Renderable()
{
    for(OnDelete & callback : onDeleteCallbacks) { callback(this) ;}
}

vec4 Renderable::setFillColor(const vec4 & fillColor) { return this->fillColor = fillColor ;}

vec4 Renderable::getFillColor() const { return fillColor ;}

vec4 Renderable::setOutlineColor(const vec4 & outlineColor) { return this->outlineColor = outlineColor ;}

vec4 Renderable::getOutlineColor() const { return outlineColor ;}

bool Renderable::setRenderOutline(const bool renderOutline) { return this->renderOutline = renderOutline ;}

bool Renderable::getRenderOutline() const { return renderOutline ;}

bool Renderable::setRenderFill(const bool renderFill) { return this->renderFill = renderFill ;}

bool Renderable::getRenderFill() const { return renderFill ;}

void Renderable::addOnDeleteCallback(const OnDelete & callback) { onDeleteCallbacks.push_back(callback) ;}

namespace
{
    vec4 defaultFillColor(0.7, 0.5, 0, 0.5);
    vec4 defaultOutlineColor(1,0,0,1);
    bool defaultDoFill = true;
    bool defaultDoOutline = true;
}

vec4 Renderable::setDefaultFillColor(const vec4 & fillColor) { return defaultFillColor = fillColor ;}
vec4 Renderable::getDefaultFillColor() { return defaultFillColor ;}
vec4 Renderable::setDefaultOutlineColor(const vec4 & outlineColor) { return defaultOutlineColor = outlineColor ;}
vec4 Renderable::getDefaultOutlineColor() { return defaultOutlineColor ;}
bool Renderable::setDefaultRenderFill(const bool renderFill) { return defaultDoFill = renderFill ;}
bool Renderable::getDefaultRenderFill() { return defaultDoFill ;}
bool Renderable::setDefaultRenderOutline(const bool renderOutline) { return defaultDoOutline = renderOutline ;}
bool Renderable::getDefaultRenderOutline() { return defaultDoOutline ;}