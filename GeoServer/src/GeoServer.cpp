/**
╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/

#include <algorithm>
#include <ctpl.h>
#include <geos/geom/Polygon.h>
#include <geos/io/WKTWriter.h>
#include "ogrsf_frmts.h"
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/GeometryConverter.h>
#include <geoServer/GeoServer.h>

using namespace std;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using namespace geos::io;
using namespace geos::geom;

namespace
{
    ctpl::thread_pool pool(1);
}

GeoServer::GeoServer(const string & geomFile)
{
    addGeoFile(geomFile);
}

GeoServer::~GeoServer()
{
}

string GeoServer::addGeoFile(const string & geomFile)
{
    GDALAllRegister();

    GDALDataset * poDS = (GDALDataset *)GDALOpenEx(geomFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);

    if(!poDS) { dmess("Error opening: " << geomFile) ;}

    OGRLayer * poLayer = poDS->GetLayer(0);

    if(!poLayer) { dmess("Error!") ;}

    poLayer->ResetReading();

    size_t c = 0;

    const GEOSContextHandle_t gctx = OGRGeometry::createGEOSContext();

    typedef tuple<Geometry *, double, Attributes *> GeomAndArea;

    vector<GeomAndArea> geoms;

    for(const auto & poFeature : *poLayer)
    {
        Attributes * attrs = new Attributes();

        for(const auto & oField: *poFeature)
        {
            if(oField.IsNull()) { continue ;}

            const string key = oField.GetName();

            switch(oField.GetType())
            {
                case OFTInteger   : attrs->ints32 [key] = oField.GetInteger();   break;
                case OFTInteger64 : attrs->ints64 [key] = oField.GetInteger64(); break;
                case OFTReal      : attrs->doubles[key] = oField.GetDouble();    break;
                case OFTString    : attrs->strings[key] = oField.GetString();    break;
                default           : attrs->strings[key] = oField.GetAsString();  break;
            }
        }

        OGRGeometry * poGeometry = poFeature->GetGeometryRef();

        const double simplifyAmount = 0.00001;

        Geometry * geom = (Geometry *)poGeometry->exportToGEOS(gctx);

        const double area = geom->getArea();

        geoms.push_back(GeomAndArea(geom, area, attrs));

        if(++c > 100000) { break ;}
    }
    
    sort(geoms.begin(), geoms.end(), [](const GeomAndArea & lhs, const GeomAndArea & rhs)
    {
        return get<1>(lhs) < get<1>(rhs);
    });

    for(const GeomAndArea & g : geoms)
    {
        //dmess("g " << get<1>(g) << " " << get<0>(g)->getGeometryType());
    }

    for(const GeomAndArea & g : geoms)
    {
        if(!dynamic_cast<const Polygon *>(get<0>(g)))
        {
            string s = get<0>(g)->getGeometryType();

            dmess("Not a poly! "  << s);

            continue;
        }

        if(!get<0>(g))
        {
            dmess("Empty");

            continue;
        }

        serializedGeoms.push_back(GeometryConverter::convert(dynamic_cast<const Polygon *>(get<0>(g)), get<2>(g)));
    }

    OGREnvelope extent;

    if(poLayer->GetExtent(&extent) != CPLE_None)
    {
       dmess("Error getting extent!");

       abort();
    }

    boundsMinX = extent.MinX;
    boundsMinY = extent.MinY;
    boundsMaxX = extent.MaxX;
    boundsMaxY = extent.MaxY;

    GDALClose( poDS );

    dmess("serializedGeoms " << serializedGeoms.size());

    return geomFile;
}

// Define a callback to handle incoming messages
void GeoServer::on_message(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg)
{
    hdl.lock().get();

    try {

        Server * s = &server->serverEndPoint;

        const char * data = (char *)msg->get_payload().data();

        const char * dataPtr = &data[1];

        const uint32_t requestID = *(const uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

        switch(data[0])
        {
            case GET_NUM_GEOMETRIES_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) * 2);

                    data[0] = GET_NUM_GEOMETRIES_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = server->getNumGeoms();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

            break;

            case GET_GEOMETRY_REQUEST:
                {
                    const uint32_t geomID = *(const uint32_t *)dataPtr;

                    pool.push([hdl, s, server, requestID, geomID](int ID)
                    {
                        const string & geom = server->getGeom(geomID);
                        
                        vector<char> data(sizeof(char) + sizeof(uint32_t) * 2 + geom.length());

                        data[0] = GET_GEOMETRY_RESPONCE;

                        char * ptr = &data[1];

                        *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                        *(uint32_t *)ptr = geom.length(); ptr += sizeof(uint32_t);

                        memcpy(ptr, geom.data(), geom.length());

                        s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                    });

                    break;
                }

            case GET_ALL_GEOMETRIES_REQUEST:
                
                pool.push([hdl, s, server, requestID](int ID)
                {
                    const vector<string> & serializedGeoms = server->serializedGeoms;

                    const uint32_t numGeoms = serializedGeoms.size();

                    uint32_t bufferSize = sizeof(char) + sizeof(uint32_t) * 2;

                    for(uint i = 0; i < numGeoms; ++i) { bufferSize += serializedGeoms[i].length() ;}

                    vector<char> data(bufferSize);

                    char * ptr = &data[0];
                    
                    *ptr = GET_ALL_GEOMETRIES_RESPONCE; ptr += sizeof(char);

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = numGeoms; ptr += sizeof(uint32_t);

                    for(const string & geom : serializedGeoms)
                    {
                        memcpy(ptr, geom.data(), geom.length()); 

                        ptr += geom.length();
                    }

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_LAYER_BOUNDS_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) + sizeof(AABB2D)); // TODO make a AABB2D class

                    data[0] = GET_LAYER_BOUNDS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *((AABB2D *)ptr) = AABB2D(  server->boundsMinX,
                                                server->boundsMinY,
                                                server->boundsMaxX,
                                                server->boundsMaxY);

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            default:

                dmess("Error!");
        };
    }
    catch (const websocketpp::lib::error_code &e)
    {
        dmess("Failed because: " << e << "(" << e.message() << ")");
    }
}

void GeoServer::start()
{
    dmess("GeoServer::start");

    try
    {
        // Set logging settings
        //serverEndPoint.set_access_channels(websocketpp::log::alevel::all);
        //serverEndPoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // this will turn off console output for frame header and payload
        serverEndPoint.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload); 
        
        // this will turn off everything in console output
        serverEndPoint.clear_access_channels(websocketpp::log::alevel::all); 

        // Initialize ASIO
        serverEndPoint.init_asio();

        // Register our message handler
        serverEndPoint.set_message_handler(::bind(&on_message, this, ::_1, ::_2));

        // Listen on port 9002
        serverEndPoint.listen(9002);

        // Start the server accept loop
        serverEndPoint.start_accept();

        // Start the ASIO io_service run loop
        serverEndPoint.run();

    }
    catch (const std::exception &e)
    {
        dmess(e.what());
    }
    catch (websocketpp::lib::error_code e)
    {
        dmess(e.message());
    }
    catch (...)
    {
        dmess("other exception");
    }

    dmess("Exit");
}

size_t GeoServer::getNumGeoms() const { return serializedGeoms.size() ;}

const string & GeoServer::getGeom(const size_t index) const { return serializedGeoms[index] ;}