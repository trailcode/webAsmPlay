#ifndef __WEB_ASM_PLAY_GEO_SERVER_BASE_H__
#define __WEB_ASM_PLAY_GEO_SERVER_BASE_H__

#include <utility>

class GeoServerBase
{
public:
    
    enum
    {
        GET_NUM_GEOMETRIES_REQUEST = 0,
        GET_NUM_GEOMETRIES_RESPONCE,
        GET_GEOMETRY_REQUEST,
        GET_GEOMETRY_RESPONCE,
        GET_LAYER_BOUNDS_REQUEST,
        GET_LAYER_BOUNDS_RESPONCE,
    };

    typedef std::pair<unsigned char *, const size_t> WkbGeom;

    virtual ~GeoServerBase() {}

protected:

    GeoServerBase() {}
};

#endif // __WEB_ASM_PLAY_GEO_SERVER_BASE_H__