#ifndef __WEB_ASM_PLAY_POLYGON_H__
#define __WEB_ASM_PLAY_POLYGON_H__

#include <sstream>
#include <webAsmPlay/Types.h>

namespace geos
{
    namespace geom
    {
        class Polygon;
        class LineString;
        class CoordinateSequence;
    }
}

class Attributes;

class GeometryConverter
{
public:

    static std::string convert(const geos::geom::Polygon * poly, const Attributes * attrs);

    static AttributedGeometry getGeosPolygon(const char *& poly);

    static std::vector<AttributedGeometry> getGeosPolygons(const char *& polys);

    static geos::geom::CoordinateSequence * getGeosCoordinateSequence(const char *& lineString);

    static void convert(const geos::geom::Polygon * poly, const Attributes * attrs, std::stringstream & data);

    static void convert(const geos::geom::LineString * lineString, std::stringstream & data);

private:

    GeometryConverter() {}
    ~GeometryConverter() {}
};

#endif // __WEB_ASM_PLAY_POLYGON_H__