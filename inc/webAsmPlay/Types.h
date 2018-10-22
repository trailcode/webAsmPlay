#ifndef __WEB_ASM_PLAY_TYPES_H__
#define __WEB_ASM_PLAY_TYPES_H__

#include <tuple>

class Attributes;

namespace geos
{
    namespace geom
    {
        class Geometry;
    }
}

typedef std::tuple<double, double, double, double> AABB2D;

typedef std::pair<Attributes *, geos::geom::Geometry *> AttributedGeometry;

#endif // __WEB_ASM_PLAY_TYPES_H__

