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
#include <limits>
#include <ctpl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/filesystem.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTWriter.h>
#ifdef __USE_GDAL__
    #include "ogrsf_frmts.h"
#endif
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/geom/GeometryConverter.h>
#include <geoServer/OSM_Reader.h>
#include <geoServer/Topology.h>
#include <geoServer/GeoServer.h>

using namespace std;
using namespace glm;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using namespace boost;
using namespace geos::io;
using namespace geos::geom;
using namespace topology;

namespace
{
    ctpl::thread_pool pool(1);
}

GeoServer::GeoServer() : m_boundsMinX( numeric_limits<double>::max()),
                         m_boundsMinY( numeric_limits<double>::max()),
                         m_boundsMaxX(-numeric_limits<double>::max()),
                         m_boundsMaxY(-numeric_limits<double>::max())
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
    else if(ext == ".las" || ext == ".laz") { addLasFile(geomFile) ;}
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
    
	polygons = discoverTopologicalRelations(polygons);
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

    const MapData mapData = OSM_Reader::import(osmFile);

    dmess("geometry " << mapData.m_geometry.size());

    dvec2 center;

    Polygon    * polygon;
    LineString * lineString;
    Point      * point;

    size_t numVertices = 0;

    size_t numEmptyPoints = 0;

    vector<AttributedPoligonalArea> polygons;
    vector<AttributedLineString>    lineStrings;

    for(const AttributedGeometry & i : mapData.m_geometry)
    {
        if((polygon = dynamic_cast<Polygon *>(i.second)))
        {
            const double area = polygon->getArea();

            i.first->m_doubles["area"] = area;

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
            lineStrings.push_back(AttributedLineString(i.first, lineString));
        }
        else if((point = dynamic_cast<Point *>(i.second)))
        {
            if(i.first) { m_serializedPoints.push_back(GeometryConverter::convert(AttributedPoint(i.first, point))) ;}
            else        { ++numEmptyPoints ;}
        }
        else
        {
            dmess("Unknown type! " << i.second->getGeometryType());
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

    dmess("numPolygons " << polygons.size());

	polygons = discoverTopologicalRelations(polygons);

	cutPolygonHoles(polygons);

    breakLineStrings(lineStrings);

    //createNavigationPaths(lineStrings);


    for(const AttributedPoligonalArea & g : polygons)    { m_serializedPolygons   .push_back(GeometryConverter::convert(g)) ;}
    for(const AttributedLineString    & l : lineStrings) { m_serializedLineStrings.push_back(GeometryConverter::convert(l)) ;}
    
    dmess("numEmptyPoints " << numEmptyPoints);

    dmess("numVertices " << numVertices);

    center /= double(numVertices);

    m_boundsMinX = center.x;
    m_boundsMaxX = center.x;
    m_boundsMinY = center.y;
    m_boundsMaxY = center.y;

    if( mapData.m_boundsMinX != 0.0 &&
        mapData.m_boundsMaxX != 0.0 &&
        mapData.m_boundsMinY != 0.0 &&
        mapData.m_boundsMaxY != 0.0)
    {
		dmess("Here!");

        m_boundsMinX = mapData.m_boundsMinX;
        m_boundsMaxX = mapData.m_boundsMaxX;
        m_boundsMinY = mapData.m_boundsMinY;
        m_boundsMaxY = mapData.m_boundsMaxY;

		dmess("boundsMinX " << m_boundsMinX);
		dmess("boundsMaxX " << m_boundsMaxX);
		dmess("boundsMinY " << m_boundsMinY);
		dmess("boundsMaxY " << m_boundsMaxY);
    }

	/*
	boundsMinX = mapData.boundsMinX;
	boundsMaxX = mapData.boundsMaxX;
	boundsMinY = mapData.boundsMinY;
	boundsMaxY = mapData.boundsMaxY;
	*/

    const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

    // TODO code dup!
    m_trans = translate(  s,
                        dvec3(  (m_boundsMinY + m_boundsMinX) * -0.5,
                                (m_boundsMaxY + m_boundsMaxX) * -0.5,
                                0.0));

    saveGeoFile("data.geo");

    return osmFile;
}

namespace
{
    void _saveData(FILE * fp, const vector<string> & data)
    {
        const uint32_t num = (uint32_t)data.size();

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

    fwrite(&m_boundsMinX, sizeof(double), 1, fp);
    fwrite(&m_boundsMinY, sizeof(double), 1, fp);
    fwrite(&m_boundsMaxX, sizeof(double), 1, fp);
    fwrite(&m_boundsMaxY, sizeof(double), 1, fp);

    _saveData(fp, m_serializedPolygons);
    _saveData(fp, m_serializedLineStrings);
    _saveData(fp, m_serializedPoints);
    _saveData(fp, m_serializedRelations);

    fclose(fp);

    return fileName;
}

string GeoServer::_addGeoFile(const string & geoFile)
{
    cout << "Loading: " << geoFile << endl;

    FILE * fp = fopen(geoFile.c_str(), "rb");

    fread(&m_boundsMinX, sizeof(double), 1, fp);
    fread(&m_boundsMinY, sizeof(double), 1, fp);
    fread(&m_boundsMaxX, sizeof(double), 1, fp);
    fread(&m_boundsMaxY, sizeof(double), 1, fp);

    loadData(fp, m_serializedPolygons);
    loadData(fp, m_serializedLineStrings);
    loadData(fp, m_serializedPoints);
    loadData(fp, m_serializedRelations);

    fclose(fp);

    return geoFile;
}

string GeoServer::addLasFile(const string & lasFile)
{

    return lasFile;
}

// Define a callback to handle incoming messages
void GeoServer::onMessage(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg)
{
    hdl.lock().get();

    try {

        Server * s = &server->m_serverEndPoint;

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

                    *(uint32_t *)ptr = (uint32_t)server->m_serializedPolygons.size();

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

                    *(uint32_t *)ptr = (uint32_t)server->m_serializedLineStrings.size();

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

                    *(uint32_t *)ptr = (uint32_t)server->m_serializedPoints.size();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_POLYGONS_REQUEST:
            {
                const uint32_t startIndex = *(const uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                const uint32_t numGeoms = *(const uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                pool.push([hdl, s, server, requestID, startIndex, numGeoms](int ID)
                {
                    const vector<string> & serializedPolygons = server->m_serializedPolygons;

                    uint32_t bufferSize = sizeof(char) + sizeof(uint32_t) * 2;

                    for(size_t i = 0; i < numGeoms; ++i) { bufferSize += uint32_t(serializedPolygons[startIndex + i].length() + sizeof(uint32_t)) ;}

                    vector<char> data(bufferSize);

                    char * ptr = &data[0];
                    
                    *ptr = GET_POLYGONS_RESPONCE; ptr += sizeof(char);

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = numGeoms; ptr += sizeof(uint32_t);

                    for(size_t i = 0; i < numGeoms; ++i)
                    {
                        const string & geom = serializedPolygons[startIndex + i];

                        *(uint32_t *)ptr = (uint32_t)geom.length(); ptr += sizeof(uint32_t);

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
                    const vector<string> & serializedLineStrings = server->m_serializedLineStrings;

                    uint32_t bufferSize = sizeof(char) + sizeof(uint32_t) * 2;

                    for(uint i = 0; i < numGeoms; ++i) { bufferSize += uint32_t(serializedLineStrings[startIndex + i].length() + sizeof(uint32_t)) ;}

                    vector<char> data(bufferSize);

                    char * ptr = &data[0];
                    
                    *ptr = GET_POLYLINES_RESPONCE; ptr += sizeof(char);

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = numGeoms; ptr += sizeof(uint32_t);

                    for(size_t i = 0; i < numGeoms; ++i)
                    {
                        const string & geom = serializedLineStrings[startIndex + i];

                        *(uint32_t *)ptr = (uint32_t)geom.length(); ptr += sizeof(uint32_t);

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

                    *((AABB2D *)ptr) = AABB2D(  server->m_boundsMinX,
                                                server->m_boundsMinY,
                                                server->m_boundsMaxX,
                                                server->m_boundsMaxY);

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

    dmess("   serializedPolygons: " << m_serializedPolygons.size());
    dmess("serializedLineStrings: " << m_serializedLineStrings.size());
    dmess("     serializedPoints: " << m_serializedPoints.size());
    dmess("  serializedRelations: " << m_serializedRelations.size());

    dmess("boundsMinX " << m_boundsMinX);
    dmess("boundsMinY " << m_boundsMinY);
    dmess("boundsMaxX " << m_boundsMaxX);
    dmess("boundsMaxY " << m_boundsMaxY);

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
        m_serverEndPoint.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload); 
        
        // this will turn off everything in console output
        m_serverEndPoint.clear_access_channels(websocketpp::log::alevel::all); 

        // Initialize ASIO
        m_serverEndPoint.init_asio();

        // Register our message handler
        m_serverEndPoint.set_message_handler(::bind(&onMessage, this, ::_1, ::_2));

        // Listen on port 9002
        m_serverEndPoint.listen(9002);

        // Start the server accept loop
        m_serverEndPoint.start_accept();

        // Start the ASIO io_service run loop
        m_serverEndPoint.run();

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
