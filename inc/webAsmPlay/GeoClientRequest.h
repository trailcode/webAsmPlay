#ifndef __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__
#define __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__

#include <functional>
#include <webAsmPlay/Types.h>

class GeoClient;

namespace geos
{
    namespace geom
    {
        class Geometry;
    }
}

class GeoRequestGetNumGeoms
{
public:

    GeoRequestGetNumGeoms(const std::function<void (const size_t numGeoms)> & callback);

private:

    friend class GeoClient;

    const std::function<void (const size_t)> callback;

    const size_t ID;
};

class GeoRequestLayerBounds
{
public:

    GeoRequestLayerBounds(const std::function<void (const AABB2D &)> & callback);

private:

    friend class GeoClient;

    const std::function<void (const AABB2D &)> callback;

    const size_t ID;
};

class GeoRequestGeometry
{
public:

    GeoRequestGeometry(const std::function<void (geos::geom::Geometry *)> & callback);

private:

    friend class GeoClient;

    const std::function<void (geos::geom::Geometry *)> callback;

    const size_t ID;
};

class GetRequestGetAllGeometries
{
public:

    GetRequestGetAllGeometries(const std::function<void (std::vector<geos::geom::Geometry *> geoms)> & callback);

private:

    friend class GeoClient;

    const std::function<void (std::vector<geos::geom::Geometry *> geoms)> callback;

    const size_t ID;
};

#endif // __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__