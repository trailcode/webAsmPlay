#ifndef __WEB_ASM_PLAY_GEO_CLIENT_H__
#define __WEB_ASM_PLAY_GEO_CLIENT_H__

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <webAsmPlay/Types.h>

class GeoRequestGetNumGeoms;
class GeoRequestLayerBounds;
class GeoRequestGeometry;
class Canvas;

namespace geos
{
    namespace geom
    {
        class Geometry;
    }
}

class GeoClient
{
public:

    static GeoClient * getInstance();

    static void onMessage(const std::string & data);

    void getNumGeoms(const std::function<void (const size_t)> & callback);

    void getLayerBounds(const std::function<void (const AABB2D &)> & callback);

    void getGeometry(const size_t geomIndex, std::function<void (geos::geom::Geometry *)> & callback);

    void loadGeometry(Canvas * canvas);

private:

    GeoClient();
    ~GeoClient();

    typedef std::unordered_map<size_t, GeoRequestGetNumGeoms *> NumGeomsRequests;
    typedef std::unordered_map<size_t, GeoRequestLayerBounds *> LayerBoundsRequests;
    typedef std::unordered_map<size_t, GeoRequestGeometry    *> GeometryRequests;

    NumGeomsRequests    numGeomsRequests;
    LayerBoundsRequests layerBoundsRequests;
    GeometryRequests    geometryRequests;

    std::vector<const geos::geom::Geometry *> geoms;
};

#endif // __WEB_ASM_PLAY_GEO_CLIENT_H__

