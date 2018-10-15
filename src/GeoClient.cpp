#include <memory>
#ifdef __EMSCRIPTEN__

    #include <emscripten/emscripten.h>
    #include <emscripten/bind.h>
#else

    #include <ctpl.h>
    #include <chrono>
    #include <thread>

#endif
#include <glm/glm.hpp>
//#include <glm/mat4x4.hpp>
//#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/io/WKTReader.h>
#include <geoServer/GeoServerBase.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/GeoClientRequest.h>
#include <webAsmPlay/GeoClient.h>

using namespace std;
using namespace glm;
using namespace geos::io;
using namespace geos::geom;

typedef unordered_map<size_t, GeoRequestGetNumGeoms *> NumGeomsRequests;
typedef unordered_map<size_t, GeoRequestLayerBounds *> LayerBoundsRequests;
typedef unordered_map<size_t, GeoRequestGeometry    *> GeometryRequests;

typedef vector<const Geometry *> GeomVector;

namespace
{
    uint32_t lastID = 0;

    NumGeomsRequests    numGeomsRequests;
    LayerBoundsRequests layerBoundsRequests;
    GeometryRequests    geometryRequests;

    //std::vector<const geos::geom::Geometry *> geoms;
}

GeoClient::GeoClient() : ID(++lastID)
{
#ifndef __EMSCRIPTEN__

    client = new Client;

    // We expect there to be a lot of errors, so suppress them
    client->clear_access_channels(websocketpp::log::alevel::all);
    client->clear_error_channels(websocketpp::log::elevel::all);
   
    // Initialize ASIO
    client->init_asio();

    client->set_open_handler(bind(&on_open, this, placeholders::_1));
    client->set_message_handler(bind(&on_message, this, placeholders::_1, placeholders::_2));

    websocketpp::lib::error_code ec;

    std::string uri = "ws://localhost:9002";

    con = client->get_connection(uri, ec);

    client->connect(con);

    cout << "Done connect!" << endl;

    Client * _client = client;

    clientThread = new thread([_client]()
    {
        _client->run();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Find a better way

#endif
}

GeoClient::~GeoClient()
{
    // TODO cleanup!
}

void GeoClient::getNumGeoms(const function<void (const size_t)> & callback)
{
    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    numGeomsRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(9); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,0); \r\n"                        // GET_NUM_GEOMETRIES_REQUEST
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_NUM_GEOMETRIES_REQUEST;

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
                    "dv.setUint8(0,4); \r\n"                        // GET_LAYER_BOUNDS_REQUEST
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", request->ID);

    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_LAYER_BOUNDS_REQUEST;

    dmess("request->ID " << request->ID);

    *(uint32_t *)&data[1] = request->ID;

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getGeometry(const size_t geomIndex, function<void (Geometry * geom)> & callback)
{
    GeoRequestGeometry * request = new GeoRequestGeometry(callback);

    geometryRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(9); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,2); \r\n"                        // GET_GEOMETRY_REQUEST
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", request->ID, geomIndex);

    emscripten_run_script(buf);

#else

    vector<char> data(9);

    data[0] = GeoServerBase::GET_GEOMETRY_REQUEST;

    char * ptr = &data[1];

    *(uint32_t *)ptr = request->ID; ptr += sizeof(uint32_t);

    *(uint32_t *)ptr = geomIndex;

    client->send(con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

// https://stackoverflow.com/questions/17779340/glfw-3-0-resource-loading-with-opengl

void GeoClient::onMessage(const string & data)
{
    char * ptr = (char *)data.data();

    switch(ptr[0])
    {
        case GeoServerBase::GET_NUM_GEOMETRIES_RESPONCE:
        {
            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            const uint32_t numGeoms = *(uint32_t *)ptr;

            NumGeomsRequests::const_iterator i = numGeomsRequests.find(requestID);

            unique_ptr<GeoRequestGetNumGeoms> request(i->second);

            request->callback(numGeoms);

            numGeomsRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_GEOMETRY_RESPONCE:
        {
            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            const uint32_t dataSize = *(uint32_t *)ptr; ptr += sizeof(uint32_t);

            WKTReader reader(GeometryFactory::getDefaultInstance());

            Geometry * geom = reader.read(string(ptr));

            GeometryRequests::const_iterator i = geometryRequests.find(requestID);

            unique_ptr<GeoRequestGeometry> request(i->second);

            request->callback(geom);

            geometryRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_LAYER_BOUNDS_RESPONCE:
        {
            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            const AABB2D & bounds = *(AABB2D *)ptr;

            dmess("requestID " << requestID);

            LayerBoundsRequests::const_iterator i = layerBoundsRequests.find(requestID);

            unique_ptr<GeoRequestLayerBounds> request(i->second);

            request->callback(bounds);

            layerBoundsRequests.erase(i);

            break;
        }
    }
}

void GeoClient::loadGeometry(Canvas * canvas)
{
    dmess("GeoClient::loadGeometry");

    GeoClient * _ = this;

    GeomVector * geoms = new GeomVector;

    std::function<void (const size_t)> getNumGeomsFunctor = [_, canvas, geoms](const size_t numGeoms)
    {
        _->getLayerBounds([_, numGeoms, canvas, geoms](const AABB2D & bounds)
        {
            const mat4 s = scale(mat4(1.0), vec3(30.0, 30.0, 30.0));

            const mat4 trans = translate(   
                                            //mat4(1.0),
                                            s,
                                            vec3((get<0>(bounds) + get<2>(bounds)) * -0.5,
                                                    (get<1>(bounds) + get<3>(bounds)) * -0.5,
                                                    0.0));

            std::function<void (Geometry *)> getGeom = [trans,
                                                        canvas,
                                                        numGeoms,
                                                        geoms](Geometry * geom)
            {
                geoms->push_back(geom);

                if(geoms->size() == numGeoms)
                {
                    dmess("Done!");

                    Renderiable * r = RenderiablePolygon2D::create(*geoms, trans);

                    delete geoms;

                    r->setFillColor(vec4(0.3,0.0,0.3,0.3));
                        
                    r->setOutlineColor(vec4(0,1,0,1));

                    canvas->addRenderiable(r);

                    dmess("Done creating renderiable.");
                }
            };

            for(size_t i = 0; i < numGeoms; ++i) { _->getGeometry(i, getGeom) ;}
        });
    };

    getNumGeoms(getNumGeomsFunctor);
}

uint32_t GeoClient::getID() const { return ID ;}

#ifndef __EMSCRIPTEN__

void GeoClient::on_open(GeoClient * client, websocketpp::connection_hdl hdl)
{
    dmess("Connection Open!");
}

void GeoClient::on_message(GeoClient * client, websocketpp::connection_hdl hdl, message_ptr msg)
{
    dmess("On message");

    Client::connection_ptr con = client->client->get_con_from_hdl(hdl);

    onMessage(msg->get_payload());
}

#else

EMSCRIPTEN_BINDINGS(GeoClientBindings)
{
    emscripten::function("onMessage", &GeoClient::onMessage);
}

#endif