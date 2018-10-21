#include <algorithm>
#include <ctpl.h>
#include <geos/io/WKTWriter.h>
#include "ogrsf_frmts.h"
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/PolygonWrapper.h>

#include <geoServer/GeoServer.h>

using namespace std;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

#include <geos.h>
using namespace geos::io;
using namespace geos::geom;

namespace
{
    ctpl::thread_pool pool(1);

    /*
    struct MemBuf : std::streambuf
    {
        MemBuf(char* begin, char* end) {
            this->setg(begin, begin, end);
        }
    };
    */
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
    //const GEOSContextHandle_t gctx = 0;

    typedef pair<Geometry *, double> GeomAndArea;

    vector<GeomAndArea> geoms;

    for(OGRFeature * poFeature; (poFeature = poLayer->GetNextFeature()) != NULL ;)
    {
        OGRGeometry * poGeometry = poFeature->GetGeometryRef();

        const double simplifyAmount = 0.00001;

        /*
        OGRGeometry * g = poGeometry->Simplify(simplifyAmount);

        geos::geom::Geometry * geom = (geos::geom::Geometry *)g->exportToGEOS(gctx);

        const double area = geom->getArea();

        if(area < simplifyAmount)
        {
            dmess("geom " << geom << " " << area);

            continue;
        }
        */

        Geometry * geom = (Geometry *)poGeometry->exportToGEOS(gctx);

        const double area = geom->getArea();

        if(area < simplifyAmount)
        {
            dmess("geom " << geom << " " << area << " type " << geom->getGeometryType());

            //continue;
        }

        geoms.push_back(GeomAndArea(geom, area));

        //if(!g) dmess("poGeometry " << g);

        //dmess("g->get_Area() " << g->area

        /*
        char * data;

        //poGeometry->exportToWkt(&data);
        g->exportToWkt(&data);

        wkbGeoms.push_back(WkbGeom(data, strlen(data)));

        OGRFeature::DestroyFeature( poFeature );
        */

        //if(++c > 100) { break ;}
    }
    
    sort(geoms.begin(), geoms.end(), [](const GeomAndArea & lhs, const GeomAndArea & rhs)
    {
        return get<1>(lhs) < get<1>(rhs);
    });

    for(const GeomAndArea & g : geoms)
    {
        dmess("g " << get<1>(g) << " " << get<0>(g)->getGeometryType());
    }

    /*
    for(size_t i = 0; i < geoms.size(); ++i)
    {
        Geometry * A = get<0>(geoms[i]);
        
        if(!A) { continue ;}

        for(size_t j = i + 1; j < geoms.size(); ++j)
        {
            Geometry * B = get<0>(geoms[j]);

            if(!B) { continue ;}

            const bool contains   = B->contains(A);
            const bool touches    = B->touches(A);
            const bool intersects = B->intersects(A);

            if(contains || touches || intersects)
            {
                //dmess("contains " << (int)contains << " touches " << (int)touches << " intersects " << (int)intersects);
            }

            if(contains)
            {
                Geometry * diff = B->difference(A);

                if(!diff)
                {
                    dmess("Diff Error!");

                    continue;
                }

                // TODO cleanup.

                delete get<0>(geoms[j]);

                get<0>(geoms[j]) = diff;
            }

            *
            if(!contains && !touches && intersects)
            {
                Geometry * diff = B->difference(A);

                if(!diff)
                {
                    dmess("Diff Error!");

                    continue;
                }

                // TODO cleanup.

                get<0>(geoms[j]) = diff;
            }
        }
    }
    //*/

    /*
    for(size_t i = 0; i < geoms.size(); ++i)
    {
        Geometry * A = get<0>(geoms[i]);
        
        if(!A) { continue ;}

        for(size_t j = i + 1; j < geoms.size(); ++j)
        {
            Geometry * B = get<0>(geoms[j]);

            if(!B) { continue ;}

            const bool contains   = B->contains(A);
            const bool touches    = B->touches(A);
            const bool intersects = B->intersects(A);

            if(contains || touches || intersects)
            {
                dmess("contains " << (int)contains << " touches " << (int)touches << " intersects " << (int)intersects);
            }
        }
    }
    */

    WKTWriter * wkt = new WKTWriter();
    WKBWriter * wkb = new WKBWriter();

    wkt->setOutputDimension(2);

    dmess("geoms " << geoms.size());

    for(const GeomAndArea & g : geoms)
    {
        //dmess("g " << get<1>(g));

        if(!dynamic_cast<const Polygon *>(get<0>(g)))
        {
            string s = get<0>(g)->getGeometryType();

            dmess("Not a poly! "  << s);

            continue;
        }

        //PolygonWrapper pw(dynamic_cast<const Polygon *>(get<0>(g)));

        OGRGeometry * gg = OGRGeometryFactory::createFromGEOS(gctx, (GEOSGeom_t *)get<0>(g));

        //dmess(gg);

        char * data;
        //*
        //poGeometry->exportToWkt(&data);
        gg->exportToWkt(&data);

        //dmess("data " << data);
        

        string wktStr(data);

        dmess("wktStr.length() " << wktStr.length());

        wkbGeoms.push_back(WkbGeom(wktStr, wktStr.length()));
        //*/

        /*
        string wktStr = wkt->write(get<0>(g));

        wkbGeoms.push_back(WkbGeom(wktStr, wktStr.length()));
        //*/

        if(!get<0>(g))
        {
            dmess("Empty");

            continue;
        }

       /*
       stringstream s(ios_base::binary|ios_base::in|ios_base::out);

       wkb->write(*get<0>(g), s);

       string ss = s.str();

       wkbGeoms.push_back(WkbGeom(ss, ss.length()));
       //*/
    }

    OGREnvelope extent;

    poLayer->GetExtent(&extent);

    boundsMinX = extent.MinX;
    boundsMinY = extent.MinY;
    boundsMaxX = extent.MaxX;
    boundsMaxY = extent.MaxY;

    GDALClose( poDS );

    dmess("wkbGeoms " << wkbGeoms.size());

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

                    dmess("geomID " << geomID);

                    pool.push([hdl, s, server, requestID, geomID](int ID)
                    {
                        const WkbGeom & geom = server->getGeom(geomID);
                        
                        vector<char> data(sizeof(char) + sizeof(uint32_t) * 2 + geom.second);

                        data[0] = GET_GEOMETRY_RESPONCE;

                        char * ptr = &data[1];

                        *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                        *(uint32_t *)ptr = geom.second; ptr += sizeof(uint32_t);

                        //dmess("geom.second " << geom.second);

                        memcpy(ptr, geom.first.c_str(), geom.second);

                        s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                    });

                    break;
                }

            case GET_LAYER_BOUNDS_REQUEST:
                {
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
                }

            case GET_ALL_GEOMETRIES_REQUEST:
                
                pool.push([hdl, s, server, requestID](int ID)
                {
                    /*
                    vector<char> data(sizeof(char) + sizeof(uint32_t) + sizeof(AABB2D)); // TODO make a AABB2D class

                    data[0] = GET_LAYER_BOUNDS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *((AABB2D *)ptr) = AABB2D(  server->boundsMinX,
                                                server->boundsMinY,
                                                server->boundsMaxX,
                                                server->boundsMaxY);

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                    */
                });

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

        Server::connection_type; // Does nothing?

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

size_t GeoServer::getNumGeoms() const { return wkbGeoms.size() ;}

GeoServer::WkbGeom GeoServer::getGeom(const size_t index) const { return wkbGeoms[index] ;}