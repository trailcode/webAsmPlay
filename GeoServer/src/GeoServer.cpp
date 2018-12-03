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

#include <cstdio>
#include <algorithm>
#include <limits>
#include <ctpl.h>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <boost/histogram.hpp>
#include <boost/filesystem.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTWriter.h>
#ifdef __USE_GDAL__
    #include "ogrsf_frmts.h"
#endif
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/GeometryConverter.h>
#include <geoServer/OSM_Reader.h>
#include <geoServer/Topology.h>
#include <geoServer/GeoServer.h>

#ifdef max
#undef max
#endif


using namespace std;
using namespace glm;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using namespace boost;
//using namespace boost::histogram;
using namespace geos::io;
using namespace geos::geom;
using namespace topology;

namespace
{
    ctpl::thread_pool pool(1);
}

GeoServer::GeoServer() :    boundsMinX( numeric_limits<double>::max()),
                            boundsMinY( numeric_limits<double>::max()),
                            boundsMaxX(-numeric_limits<double>::max()),
                            boundsMaxY(-numeric_limits<double>::max())
{
}

GeoServer::~GeoServer()
{
}

string GeoServer::addGeoFile(const string & geomFile)
{
    const string ext = filesystem::path(geomFile).extension().string();

         if(ext == ".osm") { addOsmFile          (geomFile) ;}
#ifdef __USE_GDAL__
    else if(ext == ".shp") { addGdalSupportedFile(geomFile) ;}
#endif
    else if(ext == ".geo") { _addGeoFile         (geomFile) ;}
    else
    {
        cout << "Error! Unsupported file: " << geomFile << endl;

        exit(-1);
    }

    return geomFile;
}

#ifdef __USE_GDAL__
string GeoServer::addGdalSupportedFile(const string & gdalFile)
{
	using geos::geom::Polygon;

    cout << "Loading: " << gdalFile << endl;

    GDALAllRegister();

    GDALDataset * poDS = (GDALDataset *)GDALOpenEx(gdalFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);

    if(!poDS) { dmess("Error opening: " << gdalFile) ;}

    OGRLayer * poLayer = poDS->GetLayer(0);

    if(!poLayer) { dmess("Error!") ;}

    poLayer->ResetReading();

    const GEOSContextHandle_t gctx = OGRGeometry::createGEOSContext();

    vector<AttributedPoligonalArea> polygons;
    vector<AttributedLineString>    lineStrings;

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

        Geometry * geom = (Geometry *)poFeature->GetGeometryRef()->exportToGEOS(gctx);
        
        switch(geom->getGeometryTypeId())
        {
            case GEOS_POLYGON:    polygons   .push_back(AttributedPoligonalArea(attrs, dynamic_cast<Polygon    *>(geom), geom->getArea())); break;
            case GEOS_LINESTRING: lineStrings.push_back(AttributedLineString   (attrs, dynamic_cast<LineString *>(geom))); break;
            default:
                dmess("Implement for " << geom->getGeometryType());
        }
    }
    
    discoverTopologicalRelations(polygons);
    breakLineStrings            (lineStrings);

    for(const AttributedPoligonalArea & g : polygons)    { serializedPolygons.push_back   (GeometryConverter::convert(g)) ;}
    for(const AttributedLineString    & l : lineStrings) { serializedLineStrings.push_back(GeometryConverter::convert(l)) ;}

    OGREnvelope extent;

    if(poLayer->GetExtent(&extent) != CPLE_None)
    {
       dmess("Error getting extent!");

       abort();
    }

    if(boundsMinX > extent.MinX) { boundsMinX = extent.MinX ;}
    if(boundsMinY > extent.MinY) { boundsMinY = extent.MinY ;}
    if(boundsMaxX < extent.MaxX) { boundsMaxX = extent.MaxX ;}
    if(boundsMaxY < extent.MaxY) { boundsMaxY = extent.MaxY ;}

    GDALClose(poDS);

    return gdalFile;
}
#endif

string GeoServer::addOsmFile(const string & osmFile)
{
	using geos::geom::Polygon;

    cout << "Loading: " << osmFile << endl;

    vector<AttributedGeometry> geometry = OSM_Reader::import(osmFile);

    dmess("geometry " << geometry.size());

    dvec2 center;

    Polygon    * polygon;
    LineString * lineString;
    Point      * point;

    size_t numVertices = 0;

    size_t numEmptyPoints = 0;

    vector<AttributedPoligonalArea> polygons;
    vector<AttributedLineString>    lineStrings;

    for(const AttributedGeometry & i : geometry)
    {
        if((polygon = dynamic_cast<Polygon *>(i.second)))
        {
            const double area = polygon->getArea();

            i.first->doubles["area"] = area;

            polygons.push_back(AttributedPoligonalArea(i.first, polygon, area));

            unique_ptr<CoordinateSequence> coords(i.second->getCoordinates());

            for(auto & c : *coords->toVector())
            {
                center += dvec2(c.x, c.y);

                ++numVertices;
            }
        }
        else if((lineString = dynamic_cast<LineString *>(i.second)))
        {
            //serializedLineStrings.push_back(GeometryConverter::convert(AttributedLineString(i.first, lineString)));

            lineStrings.push_back(AttributedLineString(i.first, lineString));
        }
        else if((point = dynamic_cast<Point *>(i.second)))
        {
            if(i.first) { serializedPoints.push_back(GeometryConverter::convert(AttributedPoint(i.first, point))) ;}
            else        { ++numEmptyPoints ;}
        }

        /*
        const Envelope * extent = i.second->getEnvelopeInternal();

        if(boundsMinX > extent->getMinX()) { boundsMinX = extent->getMinX() ;}
        if(boundsMinY > extent->getMinY()) { boundsMinY = extent->getMinY() ;}
        if(boundsMaxX < extent->getMaxX()) { boundsMaxX = extent->getMaxX() ;}
        if(boundsMaxY < extent->getMaxY()) { boundsMaxY = extent->getMaxY() ;}

        center += dvec2((extent->getMinX() + extent->getMaxX()) * 0.5,
                        (extent->getMinY() + extent->getMaxY()) * 0.5);
                        */

        //unique_ptr<CoordinateSequence>(i.second->getCoordinates())->toVector();
    }

    discoverTopologicalRelations(polygons);

    breakLineStrings(lineStrings);

    createNavigationPaths(lineStrings);

    for(const AttributedPoligonalArea & g : polygons)    { serializedPolygons   .push_back(GeometryConverter::convert(g)) ;}
    for(const AttributedLineString    & l : lineStrings) { serializedLineStrings.push_back(GeometryConverter::convert(l)) ;}
    
    dmess("numEmptyPoints " << numEmptyPoints);

    dmess("numVertices " << numVertices);

    center /= double(numVertices);

    boundsMinX = center.x;
    boundsMaxX = center.x;
    boundsMinY = center.y;
    boundsMaxY = center.y;

    const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

    // TODO code dup!
    trans = translate(  s,
                        dvec3((boundsMinY + boundsMinX) * -0.5,
                                (boundsMaxY + boundsMaxX) * -0.5,
                                0.0));

    /*
    boundsMinX = -104.979;
    boundsMinY = 39.75;
    boundsMaxX = -104.961;
    boundsMaxY = 39.7685;
    */

    //dmess("shift x " << boundsMaxX + boundsMinX);
    //dmess("shift y " << boundsMinY + boundsMaxY);
    //dmess("here " << int(boundsMinY < boundsMaxX) << " boundsMinY " << boundsMinY << " boundsMaxX " << boundsMaxX);

    /*
    auto histX = make_static_histogram(axis::regular<>(1024, boundsMinX, boundsMaxX, "x"));
    auto histY = make_static_histogram(axis::regular<>(1024, boundsMinY, boundsMaxY, "y"));

    for(const AttributedGeometry & i : polygons)
    {
        CoordinateSequence * coords = i.second->getCoordinates(); // TODO memory leak.

        for(size_t i = 0; i < coords->getSize(); ++i)
        {
            histX(coords->getX(i));
            histY(coords->getY(i));
        }
    }

    //histX.sum

    for(size_t i = 0; i < 1024; ++i)
    {
        dmess(i << " " << histX.at(i).value());
    }
    */

    //*/

    saveGeoFile("data.geo");

    return osmFile;
}

namespace
{
    void _saveData(FILE * fp, const vector<string> & data)
    {
        const uint32_t num = data.size();

        fwrite(&num, sizeof(uint32_t), 1, fp);

        for(const auto & i : data)
        {
            const uint32_t numBytes = i.length();

            fwrite(&numBytes, sizeof(uint32_t), 1, fp);

            fwrite(i.data(), sizeof(char), numBytes, fp);
        }
    }

    void loadData(FILE * fp, vector<string> & data)
    {
        uint32_t num;

        fread(&num, sizeof(uint32_t), 1, fp);

        for(size_t i = 0; i < num; ++i)
        {
            uint32_t numBytes;

            fread(&numBytes, sizeof(uint32_t), 1, fp);

            data.push_back(string());

            string & str = *data.rbegin();

            str.resize(numBytes);

            fread((char *)str.data(), sizeof(char), numBytes, fp);
        }
    }
}

string GeoServer::saveGeoFile(const string & fileName)
{
    FILE * fp = fopen(fileName.c_str(), "wb");

    fwrite(&boundsMinX, sizeof(double), 1, fp);
    fwrite(&boundsMinY, sizeof(double), 1, fp);
    fwrite(&boundsMaxX, sizeof(double), 1, fp);
    fwrite(&boundsMaxY, sizeof(double), 1, fp);

    _saveData(fp, serializedPolygons);
    _saveData(fp, serializedLineStrings);
    _saveData(fp, serializedPoints);
    _saveData(fp, serializedRelations);

    fclose(fp);

    return fileName;
}

string GeoServer::_addGeoFile(const string & geoFile)
{
    cout << "Loading: " << geoFile << endl;

    FILE * fp = fopen(geoFile.c_str(), "rb");

    fread(&boundsMinX, sizeof(double), 1, fp);
    fread(&boundsMinY, sizeof(double), 1, fp);
    fread(&boundsMaxX, sizeof(double), 1, fp);
    fread(&boundsMaxY, sizeof(double), 1, fp);

    loadData(fp, serializedPolygons);
    loadData(fp, serializedLineStrings);
    loadData(fp, serializedPoints);
    loadData(fp, serializedRelations);

    fclose(fp);

    return geoFile;
}

// Define a callback to handle incoming messages
void GeoServer::onMessage(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg)
{
    hdl.lock().get();

    try {

        Server * s = &server->serverEndPoint;

        const char * data = (char *)msg->get_payload().data();

        const char * dataPtr = &data[1];

        const uint32_t requestID = getUint32(dataPtr);

        switch(data[0])
        {
            case GET_NUM_POLYGONS_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) * 2);

                    data[0] = GET_NUM_POLYGONS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = server->serializedPolygons.size();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_NUM_POLYLINES_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) * 2);

                    data[0] = GET_NUM_POLYLINES_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = server->serializedLineStrings.size();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_NUM_POINTS_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) * 2);

                    data[0] = GET_NUM_POINTS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = server->serializedPoints.size();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_POLYGONS_REQUEST:
            {
                const uint32_t startIndex = *(const uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                const uint32_t numGeoms = *(const uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                pool.push([hdl, s, server, requestID, startIndex, numGeoms](int ID)
                {
                    const vector<string> & serializedPolygons = server->serializedPolygons;

                    uint32_t bufferSize = sizeof(char) + sizeof(uint32_t) * 2;

                    for(size_t i = 0; i < numGeoms; ++i) { bufferSize += serializedPolygons[startIndex + i].length() + sizeof(uint32_t) ;}

                    vector<char> data(bufferSize);

                    char * ptr = &data[0];
                    
                    *ptr = GET_POLYGONS_RESPONCE; ptr += sizeof(char);

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = numGeoms; ptr += sizeof(uint32_t);

                    for(size_t i = 0; i < numGeoms; ++i)
                    {
                        const string & geom = serializedPolygons[startIndex + i];

                        *(uint32_t *)ptr = geom.length(); ptr += sizeof(uint32_t);

                        memcpy(ptr, geom.data(), geom.length()); 

                        ptr += geom.length();
                    }

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;
            }
            case GET_POLYLINES_REQUEST:
            {
                const uint32_t startIndex = getUint32(dataPtr);
                const uint32_t numGeoms   = getUint32(dataPtr);

                pool.push([hdl, s, server, requestID, startIndex, numGeoms](int ID)
                {
                    const vector<string> & serializedLineStrings = server->serializedLineStrings;

                    uint32_t bufferSize = sizeof(char) + sizeof(uint32_t) * 2;

                    for(uint i = 0; i < numGeoms; ++i) { bufferSize += serializedLineStrings[startIndex + i].length() + sizeof(uint32_t) ;}

                    vector<char> data(bufferSize);

                    char * ptr = &data[0];
                    
                    *ptr = GET_POLYLINES_RESPONCE; ptr += sizeof(char);

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = numGeoms; ptr += sizeof(uint32_t);

                    for(size_t i = 0; i < numGeoms; ++i)
                    {
                        const string & geom = serializedLineStrings[startIndex + i];

                        *(uint32_t *)ptr = geom.length(); ptr += sizeof(uint32_t);

                        memcpy(ptr, geom.data(), geom.length()); 

                        ptr += geom.length();
                    }

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;
            }
            case GET_POINTS_REQUEST:
            {
                const uint32_t startIndex = getUint32(dataPtr);
                const uint32_t numGeoms   = getUint32(dataPtr);

                pool.push([hdl, s, server, requestID, startIndex, numGeoms](int ID)
                {

                });

                break;
            }
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

            case GET_NAVIGATION_PATHS_REQUEST:

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

                break;

            default:

                dmess("Error!");
        };
    }
    catch (const websocketpp::lib::error_code & e)
    {
        dmess("Failed because: " << e << "(" << e.message() << ")");
    }
}

void GeoServer::start()
{
    dmess("GeoServer::start");

    dmess("   serializedPolygons: " << serializedPolygons.size());
    dmess("serializedLineStrings: " << serializedLineStrings.size());
    dmess("     serializedPoints: " << serializedPoints.size());
    dmess("  serializedRelations: " << serializedRelations.size());

    dmess("boundsMinX " << boundsMinX);
    dmess("boundsMinY " << boundsMinY);
    dmess("boundsMaxX " << boundsMaxX);
    dmess("boundsMaxY " << boundsMaxY);

    cout << "  .-----------------------------------------------------------------." << endl;
    cout << " /  .-.                                                         .-.  \\" << endl;
    cout << "|  /   \\   .oOo.oOo.oOo.  G E O  S E R V E R   .oOo.oOo.oOo.   /   \\  |" << endl;
    cout << "| |\\_.  |                                                     |    /| |" << endl;
    cout << "|\\|  | /|      .oOo.oOo.    S T A R T E D    .oOo.oOo.        |\\  | |/|" << endl;
    cout << "| `---' |                                                     | `---' |" << endl;
    cout << "|       |-----------------------------------------------------|       |" << endl;
    cout << "\\       |                                                     |       /" << endl;
    cout << " \\     /                                                       \\     /" << endl;
    cout << "  `---'                                                         `---'" << endl;

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
        serverEndPoint.set_message_handler(::bind(&onMessage, this, ::_1, ::_2));

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
