/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 //
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

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>
#endif

#include <utility>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/algorithm/distance/DistanceToPoint.h>
#include <geos/algorithm/distance/PointPairDistance.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKBReader.h>
#include <geos/index/quadtree/Quadtree.h>
#include <geoServer/GeoServerBase.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeometryConverter.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderableMesh.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceShader3D.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/GeoClientRequest.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GeoClient.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::io;
using namespace geos::geom;
using namespace geos::index::quadtree;
using namespace geos::algorithm::distance;

typedef unordered_map<size_t, GeoRequestGetNumGeoms      *> NumGeomsRequests;
typedef unordered_map<size_t, GeoRequestLayerBounds      *> LayerBoundsRequests;
typedef unordered_map<size_t, GeoRequestGeometry         *> GeometryRequests;
typedef unordered_map<size_t, GetRequestGetAllGeometries *> GetAllGeometriesRequests;

namespace
{
    NumGeomsRequests            numGeomsRequests;
    LayerBoundsRequests         layerBoundsRequests;
    GeometryRequests            geometryRequests;
    GetAllGeometriesRequests    getAllGeometriesRequests;
}

GeoClient::GeoClient(GLFWwindow * window, Canvas * canvas) : canvas(canvas)
{
#ifndef __EMSCRIPTEN__

    client = new Client;

    // We expect there to be a lot of errors, so suppress them
    client->clear_access_channels(websocketpp::log::alevel::all);
    client->clear_error_channels (websocketpp::log::elevel::all);
   
    // Initialize ASIO
    client->init_asio();

    client->set_open_handler   (bind(&on_open,    this, placeholders::_1));
    client->set_message_handler(bind(&on_message, this, placeholders::_1, placeholders::_2));

    websocketpp::lib::error_code ec;

    std::string uri = "ws://localhost:9002";

    con = client->get_connection(uri, ec);

    client->connect(con);

    cout << "Done connect!" << endl;

    Client * _client = client;

    clientThread = new thread([_client, window]()
    {
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        GLFWwindow * threadWin = glfwCreateWindow(1, 1, "Thread Window", NULL, window);

        glfwMakeContextCurrent(threadWin);

        gl3wInit();

        _client->run();

        // TODO cleanup threadWin!
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(150)); // TODO Find a better way

#endif

    quadTreePolygons    = new Quadtree();
    quadTreeLineStrings = new Quadtree();
    quadTreePoints      = new Quadtree();
}

GeoClient::~GeoClient()
{
    // TODO cleanup!
}

void GeoClient::getNumPolygons(const function<void (const size_t)> & callback)
{
    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    numGeomsRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_NUM_POLYGONS_REQUEST, request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_NUM_POLYGONS_REQUEST;

    *(uint32_t *)&data[1] = request->ID;

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getNumPolylines(const function<void (const size_t)> & callback)
{
    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    numGeomsRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_NUM_POLYLINES_REQUEST, request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_NUM_POLYLINES_REQUEST;

    *(uint32_t *)&data[1] = request->ID;

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getNumPoints(const function<void (const size_t)> & callback)
{
    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    numGeomsRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_NUM_POINTS_REQUEST, request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_NUM_POINTS_REQUEST;

    *(uint32_t *)&data[1] = request->ID;

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getLayerBounds(const function<void (const AABB2D &)> & callback)
{
    GeoRequestLayerBounds * request = new GeoRequestLayerBounds(callback);

    layerBoundsRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_LAYER_BOUNDS_REQUEST, request->ID);

    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_LAYER_BOUNDS_REQUEST;

    *(uint32_t *)&data[1] = request->ID;

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPolygons(const size_t startIndex, const size_t numPolys, function<void (vector<AttributedGeometry> geoms)> callback)
{
    GetRequestGetAllGeometries * request = new GetRequestGetAllGeometries(callback);

    getAllGeometriesRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(13); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "dv.setUint32(9, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n",
                        GeoServerBase::GET_POLYGONS_REQUEST,
                        request->ID,
                        startIndex,
                        numPolys);
                
    emscripten_run_script(buf);

#else

    vector<char> data(1 + sizeof(uint32_t) * 3);

    data[0] = GeoServerBase::GET_POLYGONS_REQUEST;

    char * ptr = &data[1];

    *(uint32_t *)ptr = request->ID; ptr += sizeof(uint32_t);

    *(uint32_t *)ptr = startIndex; ptr += sizeof(uint32_t);

    *(uint32_t *)ptr = numPolys; ptr += sizeof(uint32_t);

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPolylines(const size_t startIndex, const size_t numPolylines,
                                function<void(vector<AttributedGeometry> geoms)> callback) // TODO Code duplication.
{
    GetRequestGetAllGeometries * request = new GetRequestGetAllGeometries(callback);

    getAllGeometriesRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(13); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "dv.setUint32(9, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n",
                        GeoServerBase::GET_POLYLINES_REQUEST,
                        request->ID,
                        startIndex,
                        numPolylines);
                
    emscripten_run_script(buf);

#else

    vector<char> data(1 + sizeof(uint32_t) * 3);

    data[0] = GeoServerBase::GET_POLYLINES_REQUEST;

    char * ptr = &data[1];

    *(uint32_t *)ptr = request->ID; ptr += sizeof(uint32_t);

    *(uint32_t *)ptr = startIndex; ptr += sizeof(uint32_t);

    *(uint32_t *)ptr = numPolylines; ptr += sizeof(uint32_t);

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPoints(const size_t startIndex, const size_t numPoints, function<void (vector<AttributedGeometry> geoms)> callback)
{
    GetRequestGetAllGeometries * request = new GetRequestGetAllGeometries(callback);

    getAllGeometriesRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(13); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "dv.setUint32(9, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n",
                        GeoServerBase::GET_POINTS_REQUEST,
                        request->ID,
                        startIndex,
                        numPoints);
                
    emscripten_run_script(buf);

#else

    vector<char> data(1 + sizeof(uint32_t) * 3);

    data[0] = GeoServerBase::GET_POINTS_REQUEST;

    char * ptr = &data[1];

    *(uint32_t *)ptr = request->ID; ptr += sizeof(uint32_t);

    *(uint32_t *)ptr = startIndex; ptr += sizeof(uint32_t);

    *(uint32_t *)ptr = numPoints; ptr += sizeof(uint32_t);

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::onMessage(const string & data)
{
    const char * ptr = (const char *)data.data();

    switch(ptr[0])
    {
        case GeoServerBase::GET_NUM_POLYGONS_RESPONCE:
        case GeoServerBase::GET_NUM_POLYLINES_RESPONCE:
        case GeoServerBase::GET_NUM_POINTS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);

            const uint32_t numGeoms = getUint32(ptr);

            NumGeomsRequests::const_iterator i = numGeomsRequests.find(requestID);

            unique_ptr<GeoRequestGetNumGeoms> request(i->second);

            request->callback(numGeoms);

            numGeomsRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_POLYGONS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);

            GetAllGeometriesRequests::const_iterator i = getAllGeometriesRequests.find(requestID);

            unique_ptr<GetRequestGetAllGeometries> request(i->second);

            request->callback(GeometryConverter::getGeosPolygons(ptr));

            getAllGeometriesRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_POLYLINES_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);;

            GetAllGeometriesRequests::const_iterator i = getAllGeometriesRequests.find(requestID);

            unique_ptr<GetRequestGetAllGeometries> request(i->second);

            request->callback(GeometryConverter::getGeosLineStrings(ptr));

            getAllGeometriesRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_POINTS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);;

            GetAllGeometriesRequests::const_iterator i = getAllGeometriesRequests.find(requestID);

            unique_ptr<GetRequestGetAllGeometries> request(i->second);

            request->callback(GeometryConverter::getGeosPoints(ptr));

            getAllGeometriesRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_LAYER_BOUNDS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);

            const AABB2D & bounds = *(AABB2D *)ptr;

            LayerBoundsRequests::const_iterator i = layerBoundsRequests.find(requestID);

            unique_ptr<GeoRequestLayerBounds> request(i->second);

            request->callback(bounds);

            layerBoundsRequests.erase(i);

            break;
        }
    }
}

void GeoClient::loadGeoServerGeometry()
{
    dmess("GeoClient::loadGeoServerGeometry");

    for(auto renderiable : canvas->getRenderiables()) { delete renderiable ;}

    getLayerBounds([this](const AABB2D & bounds)
    {
        const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

        trans = translate(  s,
                            dvec3((get<2>(bounds) + get<0>(bounds)) * -0.5,
                                  (get<3>(bounds) + get<1>(bounds)) * -0.5,
                                  0.0));
        
        inverseTrans = inverse(trans);

        getNumPoints([this](const size_t numPoints)
        {
            dmess("numPoints " << numPoints);

            if(!numPoints) { return ;}

            auto startTime = system_clock::now();

            const size_t blockSize = std::min((size_t)1024, numPoints);

            shared_ptr<vector<AttributedGeometry> > geoms(new vector<AttributedGeometry>());

            for(size_t i = 0; i < numPoints / blockSize; ++i)
            {
                const size_t startIndex = i * blockSize;

                const bool isLast = i + 1 >= numPoints / blockSize;

                getPoints(startIndex, std::min(blockSize, numPoints - startIndex - blockSize),
                             [this, isLast, geoms, startIndex, &startTime, numPoints]
                             (vector<AttributedGeometry> geomsIn)
                {
                    doProgress("(1/6) Loading points:", geoms->size(), numPoints, startTime, 1);

                    geoms->insert(geoms->end(), geomsIn.begin(), geomsIn.end());

                    if(isLast) { createPointRenderiables(*geoms.get()) ;}
                });
            }
        });

        getNumPolylines([this](const size_t numPolylines)
        {
            if(!numPolylines) { return ;}

            auto startTime = system_clock::now();

            const size_t blockSize = std::min((size_t)1024, numPolylines);

            shared_ptr<vector<AttributedGeometry> > geoms(new vector<AttributedGeometry>());

            for(size_t i = 0; i < numPolylines / blockSize; ++i)
            {
                const size_t startIndex = i * blockSize;

                const bool isLast = i + 1 >= numPolylines / blockSize;

                getPolylines(startIndex, std::min(blockSize, numPolylines - startIndex - blockSize),
                             [this, isLast, geoms, startIndex, &startTime, numPolylines]
                             (vector<AttributedGeometry> geomsIn)
                {
                    doProgress("(1/6) Loading linestrings:", geoms->size(), numPolylines, startTime, 1);

                    geoms->insert(geoms->end(), geomsIn.begin(), geomsIn.end());

                    if(isLast) { createLineStringRenderiables(*geoms.get()) ;}
                });
            }
        });

        getNumPolygons([this](const size_t numPolys)
        {
            if(!numPolys) { return ;}

            auto startTime = system_clock::now();

            const size_t blockSize = std::min((size_t)1024, numPolys);

            shared_ptr<vector<AttributedGeometry> > geoms(new vector<AttributedGeometry>());

            for(size_t i = 0; i < numPolys / blockSize; ++i)
            {
                const size_t startIndex = i * blockSize;

                const bool isLast = i + 1 >= numPolys / blockSize;

                getPolygons(startIndex, std::min(blockSize, numPolys - startIndex - blockSize),
                                [this, geoms, isLast, startIndex, numPolys, &startTime](vector<AttributedGeometry> geomsIn)
                {
                    doProgress("(4/6) Loading polygons:", geoms->size(), numPolys, startTime, 1);

                    geoms->insert(geoms->end(), geomsIn.begin(), geomsIn.end());

                    if(isLast) { createPolygonRenderiables(*geoms.get()) ;}
                });
            }
        });
    });
}

namespace
{
#ifdef __EMSCRIPTEN__

    void downloadSucceeded(emscripten_fetch_t *fetch)
    {
        ((GeoClient *)fetch->userData)->addGeometry(fetch->data);

        // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
        emscripten_fetch_close(fetch); // Free data associated with the fetch.
    }

    void downloadFailed(emscripten_fetch_t *fetch)
    {
        printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
        emscripten_fetch_close(fetch); // Also free data on failure.
    }

    void downloadProgress(emscripten_fetch_t *fetch)
    {
        if (fetch->totalBytes) { GUI::progress("Downloading:", float(fetch->dataOffset) / (fetch->totalBytes)) ;}
        else                   { GUI::progress("Done Downloading:", 1.0) ;}
    }

#endif
}

void GeoClient::loadGeometry(const string fileName)
{
    dmess("GeoClient::loadGeometry " << this);

#ifdef __EMSCRIPTEN__

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = downloadSucceeded;
    attr.onerror = downloadFailed;
    attr.onprogress = downloadProgress;
    attr.userData = this;
    emscripten_fetch(&attr, fileName.c_str());

#endif
}

void GeoClient::addGeometry(const char * data)
{
    dmess("GeoClient::addGeometry");

    GUI::progress("Polygon index:", 0.0f);

    AABB2D bounds;
    
    bounds = *(AABB2D *)data; data += sizeof(double) * 4;

    const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

    trans = translate(  s,
                        dvec3((get<2>(bounds) + get<0>(bounds)) * -0.5,
                                (get<3>(bounds) + get<1>(bounds)) * -0.5,
                                0.0));
    
    inverseTrans = inverse(trans);

    createPolygonRenderiables   (GeometryConverter::getGeosPolygons   (data));
    createLineStringRenderiables(GeometryConverter::getGeosLineStrings(data));
}

namespace
{
    double getHeight(Attributes * attrs)
    {
        // See: https://wiki.openstreetmap.org/wiki/OSM-3D.org
        if(attrs->hasStringKey("height")) { return atof(attrs->strings["height"].c_str()) * 0.001 * 0.3;}

        if(attrs->hasStringKey("building:levels")) { return atof(attrs->strings["building:levels"].c_str()) * 0.001 ;}

        return 0.001;
    }
}

void GeoClient::createPolygonRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("GeoClient::createPolygonRenderiables " << geoms.size());

    dmess("Start polygon quadTree...");

    auto startTime = system_clock::now();

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        doProgress("(5/6) Indexing polygons:", i, geoms.size(), startTime);

        Attributes * attrs;

        const Geometry * g;
        
        tie(attrs, g) = geoms[i];

        Renderable * r = Renderable::create(g, trans);
        
        if(!r) { continue ;}
        
        tuple<Renderable *, const Geometry *, Attributes *> * data = new tuple<Renderable *, const Geometry *, Attributes *>(r, g, attrs);

        quadTreePolygons->insert(g->getEnvelopeInternal(), data);
    }

    GUI::progress("", 1.0f);

    dmess("quadTree " << quadTreePolygons->depth() << " " << geoms.size());
    
    dmess("Start base geom...");

    ColoredGeometryVec polygons;

    ColoredExtrudedGeometryVec polygons3D;

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        Attributes * attrs = geoms[i].first;

        GLuint colorID = 0;

        double height = 0.0;

        if( attrs->hasStringKey("addr_house") ||
            attrs->hasStringKey("addr::housenumber") ||
            attrs->hasStringKey("addr::housename") ||
            attrs->hasStringKeyValue("building", "house")) // TODO Are the ones above even doing anything?
        {
            colorID = 1;

            height = getHeight(attrs);
        }
        else if(attrs->hasStringKey("building"))
        {
            colorID = 2;

            height = getHeight(attrs);
        }
        else if(attrs->hasStringKeyValue("landuse", "grass") || attrs->hasStringKeyValue("surface", "grass"))
        {
            colorID = 3;
        }
        else if(attrs->hasStringKeyValue("landuse", "reservor"))
        {
            colorID = 4;
        }

        if(height == 0.0) { polygons.push_back(ColoredGeometry(geoms[i].second, colorID)) ;}

        else { polygons3D.push_back(ColoredExtrudedGeometry(geoms[i].second, colorID, height)) ;}
    }

    dmess("polygons " << polygons.size() << " polygons3D " << polygons3D.size());

    Renderable * r = RenderablePolygon::create(polygons, trans, true);

    if(r)
    {
        r->setShader(ColorDistanceShader::getDefaultInstance());
        //r->setShader(ColorDistanceShader3D::getDefaultInstance());

        canvas->addRenderiable(r);
    }

    //*
    r = RenderableMesh::create(polygons3D, trans, true);

    if(r)
    {
        //r->setShader(ColorDistanceShader3D::getDefaultInstance());
        r->setShader(ColorDistanceShader::getDefaultInstance());

        canvas->addRenderiable(r);
    }
    //*/
    
    dmess("End base geom");
}

void GeoClient::createLineStringRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("Start create linestrings " << geoms.size());

    auto startTime = system_clock::now();

    ColoredGeometryVec polylines;

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        doProgress("(2/6) Indexing linestrings:", i, geoms.size(), startTime);

        Attributes * attrs = geoms[i].first;

        const Geometry * geom = geoms[i].second;

        GLuint colorID = 0;

        if(attrs->hasStringKeyValue("highway", "motorway"))
        {
            colorID = 1;
        }
        else if(attrs->hasStringKeyValue("highway", "trunk"))
        {
            colorID = 2;
        }
        else if(attrs->hasStringKeyValue("highway", "primary"))
        {
            colorID = 3;
        }
        else if(attrs->hasStringKeyValue("highway", "secondary"))
        {
            colorID = 4;
        }
        else if(attrs->hasStringKeyValue("highway", "tertiary"))
        {
            colorID = 5;
        }
        else if(attrs->hasStringKeyValue("highway", "unclassified"))
        {
            colorID = 6;
        }
        else if(attrs->hasStringKeyValue("highway", "residential"))
        {
            colorID = 7;
        }
        else if(attrs->hasStringKeyValue("highway", "service"))
        {
            colorID = 8;
        }
        else if(attrs->hasStringKey("highway"))
        {
            colorID = 9;
        }

        Renderable * r = Renderable::create(geom, trans);
        
        if(!r) { continue ;}
        
        tuple<Renderable *, const Geometry *, Attributes *> * data = new tuple<Renderable *, const Geometry *, Attributes *>(r, geom, attrs);

        quadTreeLineStrings->insert(geom->getEnvelopeInternal(), data);

        polylines.push_back(make_pair(geom, colorID));
    }
    
    GUI::progress("Linestring index:", 1.0);

    dmess("linestring quadTree " << quadTreeLineStrings->depth() << " " << geoms.size());

    Renderable * r = RenderableLineString::create(polylines, trans, true);

    r->setShader(ColorDistanceShader::getDefaultInstance());

    canvas->addRenderiable(r);
    
    dmess("Done creating renderable.");

#ifndef __EMSCRIPTEN__

    GUI::progress("Waiting for server:", 0.0);

#endif
}

void GeoClient::createPointRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("Start create points " << geoms.size());

    auto startTime = system_clock::now();

    vector<const Geometry *> points;

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        doProgress("(2/6) Indexing points:", i, geoms.size(), startTime);

        Attributes * attrs = geoms[i].first;

        const Geometry * geom = geoms[i].second;
        
        Renderable * r = Renderable::create(geom, trans);
        
        if(!r) { dmess("!r"); continue ;}
        
        tuple<Renderable *, const Geometry *, Attributes *> * data = new tuple<Renderable *, const Geometry *, Attributes *>(r, geom, attrs);

        quadTreePoints->insert(geom->getEnvelopeInternal(), data);

        points.push_back(geom);
    }
    
    GUI::progress("", 1.0);

    dmess("Points quadTree " << quadTreePoints->depth() << " " << geoms.size());

    Renderable * r = RenderablePoint::create(points, trans, true);

    //r->setShader(ColorDistanceShader::getDefaultInstance());

    canvas->addRenderiable(r);
    
    dmess("Done creating renderable.");

#ifndef __EMSCRIPTEN__

    GUI::progress("Waiting for server:", 0.0);

#endif
}

pair<Renderable *, Attributes *> GeoClient::pickLineStringRenderable(const vec3 & _pos) const
{
    const vec4 pos = inverseTrans * vec4(_pos, 1.0);
    
    vector< void * > query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    quadTreeLineStrings->query(&bounds, query);
    
    double minDist = numeric_limits<double>::max();

    Renderable * closest      = NULL;
    Attributes * closestAttrs = NULL;

    const Coordinate p(pos.x, pos.y);

    for(const void * _data : query)
    {
        tuple<Renderable *, const Geometry *, Attributes *> * data = (tuple<Renderable *, const Geometry *, Attributes *> *)_data;

        const LineString * geom = dynamic_cast<const LineString *>(get<1>(*data));

        PointPairDistance ptDist;

        DistanceToPoint::computeDistance(*geom, p, ptDist);

        if(ptDist.getDistance() >= minDist) { continue ;}

        minDist = ptDist.getDistance();

        closest      = get<0>(*data);
        closestAttrs = get<2>(*data);
    }

    return make_pair(closest, closestAttrs);
}

pair<Renderable *, Attributes *> GeoClient::pickPolygonRenderable(const vec3 & _pos) const
{
    const vec4 pos = inverseTrans * vec4(_pos, 1.0);
    
    vector< void * > query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    quadTreePolygons->query(&bounds, query);
    
    // TODO, there might be a method accepting a Coordinate
    Point * p = scopedGeosGeometry(GeometryFactory::getDefaultInstance()->createPoint(Coordinate(pos.x, pos.y)));

    double minArea = numeric_limits<double>::max();

    Renderable * smallest      = NULL;
    Attributes * smallestAttrs = NULL;

    for(const void * _data : query)
    {
        tuple<Renderable *, const Geometry *, Attributes *> * data = (tuple<Renderable *, const Geometry *, Attributes *> *)_data;

        const Geometry * geom = get<1>(*data);

        if(!p->within(geom)) { continue ;}

        const double area = geom->getArea();

        if(area >= minArea) { continue ;}

        minArea = area; 

        smallest = get<0>(*data);

        smallestAttrs = get<2>(*data); 
    }

    return make_pair(smallest, smallestAttrs);
}

vector<pair<Renderable *, Attributes *> > GeoClient::pickPolygonRenderables(const vec3 & _pos) const
{
    const vec4 pos = inverseTrans * vec4(_pos, 1.0);
    
    vector< void * > query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    quadTreePolygons->query(&bounds, query);
    
    Point * p = scopedGeosGeometry(GeometryFactory::getDefaultInstance()->createPoint(Coordinate(pos.x, pos.y)));

    vector<tuple<Renderable *, Attributes *, double> > picked;

    for(const void * _data : query)
    {
        tuple<Renderable *, const Geometry *, Attributes *> * data = (tuple<Renderable *, const Geometry *, Attributes *> *)_data;

        const Geometry * geom = get<1>(*data);

        if(!p->within(geom)) { continue ;}

        picked.push_back(make_tuple(get<0>(*data), get<2>(*data), geom->getArea()));
    }

    // Sort by area, smallest first.
    sort(picked.begin(), picked.end(), [](const auto & lhs, const auto & rhs) { return get<2>(lhs) < get<2>(rhs) ;});

    vector<pair<Renderable *, Attributes *> > ret(picked.size());

    for(size_t i = 0; i < picked.size(); ++i) { ret[i] = make_pair(get<0>(picked[i]), get<1>(picked[i])) ;}
    
    return ret;
}

dmat4 GeoClient::getTrans() const { return trans ;}

dmat4 GeoClient::getInverseTrans() const { return inverseTrans ;}

string GeoClient::doPicking(const char mode, const dvec4 & pos) const
{
    Renderable * renderiable;
    Attributes * attrs;

    string attrsStr;

    if(mode == GUI::PICK_MODE_LINESTRING)
    {
        tie(renderiable, attrs) = pickLineStringRenderable(canvas->getCursorPosWC());

        if(renderiable)
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            renderiable->render(canvas);

            attrsStr = attrs->toString();
        }
    }
    if(mode == GUI::PICK_MODE_POLYGON_SINGLE)
    {
        tie(renderiable, attrs) = pickPolygonRenderable(canvas->getCursorPosWC());

        if(renderiable)
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            renderiable->render(canvas);

            attrsStr = attrs->toString();
        }
    }
    else if(mode == GUI::PICK_MODE_POLYGON_MULTIPLE)
    {
        vector<pair<Renderable *, Attributes *> > picked = pickPolygonRenderables(canvas->getCursorPosWC());

        if(picked.size())
        {
            tie(renderiable, attrs) = picked[0];

            renderiable->render(canvas);

            attrsStr = attrs->toString();

            for(size_t i = 1; i < picked.size(); ++i)
            {
                attrsStr += "\n";

                attrsStr += get<1>(picked[i])->toString();
            }
        }
    }

    return attrsStr;
}

#ifndef __EMSCRIPTEN__

void GeoClient::on_open(GeoClient * client, websocketpp::connection_hdl hdl)
{
    dmess("Connection Open!");
}

void GeoClient::on_message(GeoClient * client, websocketpp::connection_hdl hdl, message_ptr msg)
{
    Client::connection_ptr con = client->client->get_con_from_hdl(hdl);

    onMessage(msg->get_payload());
}

#else

EMSCRIPTEN_BINDINGS(GeoClientBindings)
{
    emscripten::function("onMessage", &GeoClient::onMessage);
}

#endif