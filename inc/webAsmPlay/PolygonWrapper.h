#ifndef __WEB_ASM_PLAY_POLYGON_H__
#define __WEB_ASM_PLAY_POLYGON_H__

#include <sstream>

namespace geos
{
    namespace geom
    {
        class Polygon;
        class LineString;
        class CoordinateSequence;
    }
}

class PolygonWrapper
{
public:

    PolygonWrapper(const geos::geom::Polygon * poly);

    static geos::geom::Polygon * getGeosPolygon(const char *& poly);

    static std::vector<geos::geom::Geometry *> getGeosPolygons(const char *& polys);

    static geos::geom::CoordinateSequence * getGeosCoordinateSequence(const char *& lineString);

    const std::stringstream & getDataRef() const;

    static void convert(const geos::geom::Polygon * poly, std::stringstream & data);

    static void convert(const std::vector<const geos::geom::Polygon *> & polygons, std::stringstream & data);

    static void convert(const geos::geom::LineString * lineString, std::stringstream & data);

private:

    void writeLineString(const geos::geom::LineString * lineString);

    std::stringstream data;
};

#endif // __WEB_ASM_PLAY_POLYGON_H__