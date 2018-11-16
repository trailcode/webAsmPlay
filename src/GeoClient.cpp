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

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/index/quadtree/Quadtree.h>
#include <geoServer/GeoServerBase.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeometryConverter.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/GeoClientRequest.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GeoClient.h>

#ifdef min
#undef min
#endif

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;
using namespace geos::index::quadtree;

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

GeoClient::GeoClient(Canvas * canvas) : canvas(canvas)
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

    dmess("_client " << _client);

    clientThread = new thread([_client]()
    {
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        GLFWwindow * threadWin = glfwCreateWindow(1, 1, "Thread Window", NULL, GUI::getMainWindow());

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
    network             = new Network(this);
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

void GeoClient::getPolygons(const size_t                                      startIndex,
                            const size_t                                      numPolys,
                            function<void (vector<AttributedGeometry> geoms)> callback)
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

    char * ptr = appendChar(data, GeoServerBase::GET_POLYGONS_REQUEST);

    appendUint32s(ptr, request->ID, startIndex, numPolys);

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPolylines(const size_t                                     startIndex,
                             const size_t                                     numPolylines,
                             function<void(vector<AttributedGeometry> geoms)> callback)
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

    char * ptr = appendChar(data, GeoServerBase::GET_POLYLINES_REQUEST);

    appendUint32s(ptr, request->ID, startIndex, numPolylines);

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPoints(const size_t                              startIndex,
                          const size_t                              numPoints,
                          function<void (vector<AttributedGeometry> geoms)> callback)
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

    char * ptr = appendChar(data, GeoServerBase::GET_POINTS_REQUEST);

    appendUint32s(ptr, request->ID, startIndex, numPoints);

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

dmat4 GeoClient::getTrans() const { return trans ;}

dmat4 GeoClient::getInverseTrans() const { return inverseTrans ;}

Network * GeoClient::getNetwork() const { return network ;}

Canvas * GeoClient::getCanvas() const { return canvas ;}

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