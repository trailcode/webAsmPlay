#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/io/WKTReader.h>
#include <geoServer/GeoServerBase.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeoClientRequest.h>
#include <webAsmPlay/GeoClient.h>

using namespace std;
using namespace geos::io;
using namespace geos::geom;

namespace
{
    GeoClient * instance = NULL;
}

GeoClient * GeoClient::getInstance()
{
    if(instance) { return instance ;}

    return instance = new GeoClient();
}

GeoClient::GeoClient()
{

}

GeoClient::~GeoClient()
{

}

void GeoClient::getNumGeoms(const function<void (const size_t)> & callback)
{
    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    numGeomsRequests[request->ID] = request;

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,0); \r\n"                        // GET_NUM_GEOMETRIES_REQUEST
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", request->ID);
                
    emscripten_run_script(buf);
}

void GeoClient::getLayerBounds(const function<void (const AABB2D &)> & callback)
{
    GeoRequestLayerBounds * request = new GeoRequestLayerBounds(callback);

    layerBoundsRequests[request->ID] = request;

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,4); \r\n"                        // GET_LAYER_BOUNDS_REQUEST
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", request->ID);

    emscripten_run_script(buf);
}

void GeoClient::getGeometry(const size_t geomIndex, function<void (Geometry * geom)> & callback)
{
    GeoRequestGeometry * request = new GeoRequestGeometry(callback);

    geometryRequests[request->ID] = request;

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(9); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,2); \r\n"                        // GET_LAYER_BOUNDS_REQUEST
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", request->ID, geomIndex);

    emscripten_run_script(buf);
}

void GeoClient::onMessage(const string & data)
{
    char * ptr = (char *)data.data();

    switch(ptr[0])
    {
        case GeoServerBase::GET_NUM_GEOMETRIES_RESPONCE:
        {
            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            const uint32_t numGeoms = *(uint32_t *)ptr;

            dmess("     aaaaaaGET_NUM_GEOMETRIES_RESPONCE " << requestID << " numGeoms " << numGeoms);

            NumGeomsRequests::const_iterator i = getInstance()->numGeomsRequests.find(requestID);

            GeoRequestGetNumGeoms * request = i->second;

            request->callback(numGeoms);

            getInstance()->numGeomsRequests.erase(i);

            delete request;

            break;
        }

        case GeoServerBase::GET_GEOMETRY_RESPONCE:
        {
            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            const uint32_t dataSize = *(uint32_t *)ptr; ptr += sizeof(uint32_t);

            //dmess("GET_GEOMETRY_RESPONCE requestID " << requestID << " dataSize " << dataSize);

            const GeometryFactory * factory = GeometryFactory::getDefaultInstance();

            /*
            WKBReader reader(*factory);

            MemBuf buf(ptr, ptr + data.length() - 1);
            istream in(&buf);
            Geometry * geom = reader.read(in);
            */

            WKTReader reader(factory);

            Geometry * geom = reader.read(string(ptr));

            //dmess("geom " << geom);

            //dmess("geom->getGeometryType() " << geom->getGeometryType());

            GeometryRequests::const_iterator i = getInstance()->geometryRequests.find(requestID);

            GeoRequestGeometry * request = i->second;

            request->callback(geom);

            getInstance()->geometryRequests.erase(i);

            delete request;

            //addGeometry(geom);

            break;
        }

        case GeoServerBase::GET_LAYER_BOUNDS_RESPONCE:
        {
            dmess("GET_LAYER_BOUNDS_RESPONCE");

            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            dmess("      requestID " << requestID);

            typedef tuple<double, double, double, double> AABB2D;

            const AABB2D & bounds = *(AABB2D *)ptr;

            dmess("minX " << get<0>(bounds) << " minY " << get<1>(bounds) << " maxX " << get<2>(bounds) << " maxY " << get<3>(bounds));

            LayerBoundsRequests::const_iterator i = getInstance()->layerBoundsRequests.find(requestID);

            GeoRequestLayerBounds * request = i->second;

            request->callback(bounds);

            getInstance()->layerBoundsRequests.erase(i);

            delete request;

            //setLayerBounds(bounds);

            break;
        }
    }
}

EMSCRIPTEN_BINDINGS(GeoClientBindings)
{
    emscripten::function("onMessage", &GeoClient::onMessage);
}