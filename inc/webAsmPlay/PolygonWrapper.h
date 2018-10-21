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

    static geos::geom::CoordinateSequence * getGeosCoordinateSequence(const char *& lineString);

private:

    void writeLineString(const geos::geom::LineString * lineString);

    std::stringstream data;
};

#endif // __WEB_ASM_PLAY_POLYGON_H__