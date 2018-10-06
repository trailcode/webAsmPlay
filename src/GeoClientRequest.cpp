#include <webAsmPlay/GeoClientRequest.h>

using namespace std;
using namespace geos::geom;

namespace
{
    size_t lastRequestID = 0;
} 

GeoRequestGetNumGeoms::GeoRequestGetNumGeoms(const function<void (const size_t)> & callback) : callback(callback), ID(++lastRequestID)
{

}

GeoRequestLayerBounds::GeoRequestLayerBounds(const function<void (const AABB2D &)> & callback) : callback(callback), ID(++lastRequestID)
{

}

GeoRequestGeometry::GeoRequestGeometry(const function<void (Geometry *)> & callback) : callback(callback), ID(++lastRequestID)
{

}