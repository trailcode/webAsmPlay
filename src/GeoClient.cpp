#include <utility>
#include <memory>
#ifdef __EMSCRIPTEN__

    #include <emscripten/emscripten.h>
    #include <emscripten/bind.h>
#else

    #include <ctpl.h>
    #include <chrono>
    #include <thread>
    

    #include "imgui.h"
    #include "imgui_impl_glfw.h"
    #include "imgui_impl_opengl3.h"

    // About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
    // Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
    // You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
    #if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    #include <GL/glew.h>    // Initialize with glewInit()
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    #include <glad/glad.h>  // Initialize with gladLoadGL()
    #else
    #include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    #endif

    #include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

#endif
//#include <glm/glm.hpp>
//#include <glm/mat4x4.hpp>
//#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKBReader.h>
#include <geos/index/quadtree/Quadtree.h>
#include <geoServer/GeoServerBase.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeometryConverter.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/GeoClientRequest.h>
#include <webAsmPlay/GeoClient.h>

using namespace std;
using namespace glm;
using namespace geos::io;
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

GeoClient::GeoClient(GLFWwindow * window)
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

    quadTree = new Quadtree();
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

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_NUM_GEOMETRIES_REQUEST, request->ID);
                
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

void GeoClient::getAllGeometries(function<void (vector<AttributedGeometry> geoms)> callback)
{
    GetRequestGetAllGeometries * request = new GetRequestGetAllGeometries(callback);

    getAllGeometriesRequests[request->ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_ALL_GEOMETRIES_REQUEST, request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_ALL_GEOMETRIES_REQUEST;

    char * ptr = &data[1];

    *(uint32_t *)ptr = request->ID; ptr += sizeof(uint32_t);

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
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_GEOMETRY_REQUEST, request->ID, geomIndex);

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
    const char * ptr = (const char *)data.data();

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

            const uint32_t dataSize = *(uint32_t *)ptr; ptr += sizeof(uint32_t); // TODO Might not need

            AttributedGeometry geom = GeometryConverter::getGeosPolygon(ptr);

            /*
            WKBReader reader(*GeometryFactory::getDefaultInstance());

            struct MemBuf : streambuf
            {
                MemBuf(char* begin, char* end) {
                    this->setg(begin, begin, end);
                }
            };

            MemBuf buf(ptr, ptr + dataSize);

            std::istream in(&buf);

            //Geometry * geom = reader.read(string(ptr));
            Geometry * geom = reader.read(in);
            //*/

            abort();

            /*
            GeometryRequests::const_iterator i = geometryRequests.find(requestID);

            unique_ptr<GeoRequestGeometry> request(i->second);

            request->callback(geom);

            geometryRequests.erase(i);
            */

            break;
        }

        case GeoServerBase::GET_ALL_GEOMETRIES_RESPONCE:
        {
            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            GetAllGeometriesRequests::const_iterator i = getAllGeometriesRequests.find(requestID);

            unique_ptr<GetRequestGetAllGeometries> request(i->second);

            request->callback(GeometryConverter::getGeosPolygons(ptr));

            getAllGeometriesRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_LAYER_BOUNDS_RESPONCE:
        {
            const uint32_t requestID = *(uint32_t *)(++ptr); ptr += sizeof(uint32_t);

            const AABB2D & bounds = *(AABB2D *)ptr;

            LayerBoundsRequests::const_iterator i = layerBoundsRequests.find(requestID);

            unique_ptr<GeoRequestLayerBounds> request(i->second);

            request->callback(bounds);

            layerBoundsRequests.erase(i);

            break;
        }
    }
}

void GeoClient::createRenderiables(GeomVector * geoms, const mat4 trans, Canvas * canvas)
{
    for(const Geometry * g : *geoms)
    {
        Renderable * r = Renderable::create(g, trans);
        
        if(!r) { continue ;}
        
        quadTree->insert(g->getEnvelopeInternal(), r);
    }

    dmess("quadTree " << quadTree->depth() << " " << geoms->size());
    
    Renderable * r = RenderablePolygon::create(*geoms, trans);
    
    delete geoms;
    
    r->setFillColor(vec4(0.3,0.0,0.3,0.3));
    
    r->setOutlineColor(vec4(0,1,0,1));
    
    canvas->addRenderiable(r);
    
    dmess("Done creating renderiable.");
}

void GeoClient::loadGeometry(Canvas * canvas)
{
    dmess("GeoClient::loadGeometry");

    // TODO we need a class to hold the request data. What if there are multiple calls to this function and one or more has not finished.
    
    GeomVector * geoms = new GeomVector;

    getNumGeoms([this, canvas, geoms](const size_t numGeoms)
    {
        getLayerBounds([this, numGeoms, canvas, geoms](const AABB2D & bounds)
        {
            const mat4 s = scale(mat4(1.0), vec3(30.0, 30.0, 30.0));

            trans = translate(  s,
                                vec3((get<0>(bounds) + get<2>(bounds)) * -0.5,
                                     (get<1>(bounds) + get<3>(bounds)) * -0.5,
                                     0.0));
            
            inverseTrans = inverse(trans);

            std::function<void (Geometry *)> getGeom = [this,
                                                        canvas,
                                                        numGeoms,
                                                        geoms](Geometry * geom)
            {
                geoms->push_back(geom);

                if(geoms->size() == numGeoms) { createRenderiables(geoms, trans, canvas) ;}
            };

            for(size_t i = 0; i < numGeoms; ++i) { getGeometry(i, getGeom) ;}
        });
    });
}

void GeoClient::loadAllGeometry(Canvas * canvas)
{
    dmess("GeoClient::loadAllGeometry");

    getAllGeometries([this, canvas](vector<AttributedGeometry> geomsIn)
    {
        dmess("geomsIn " << geomsIn.size());

        getLayerBounds([this, canvas, geomsIn](const AABB2D & bounds)
        {
            const mat4 s = scale(mat4(1.0), vec3(30.0, 30.0, 30.0));

            trans = translate(  s,
                                vec3((get<0>(bounds) + get<2>(bounds)) * -0.5,
                                     (get<1>(bounds) + get<3>(bounds)) * -0.5,
                                     0.0));
            
            inverseTrans = inverse(trans);

            for(int i = geomsIn.size() - 1; i >= 0; --i) // TODO code duplication
            {
                Attributes * attrs;

                const Geometry * g;
                
                tie(attrs, g) = geomsIn[i];

                Renderable * r = Renderable::create(g, trans);
                
                if(!r) { continue ;}
                
                tuple<Renderable *, const Geometry *, Attributes *> * data = new tuple<Renderable *, const Geometry *, Attributes *>(r, g, attrs);

                quadTree->insert(g->getEnvelopeInternal(), data);
            }

            dmess("quadTree " << quadTree->depth() << " " << geomsIn.size());
            
            /*
            vector<const Geometry *> polys(geomsIn.size());

            for(size_t i = 0; i < polys.size(); ++i) { polys[i] = dynamic_cast<const Geometry *>(geomsIn[i].second) ;}

            Renderable * r = RenderablePolygon::create(polys, trans);

            r->setFillColor(vec4(0.3,0.0,0.3,0.3));
            
            r->setOutlineColor(vec4(0,1,0,1));
            
            canvas->addRenderiable(r);
            //*/

            //*
            //vector<tuple<const Geometry *, const vec4, const vec4> > polysAndColors(geomsIn.size());
            vector<tuple<const Geometry *, const vec4, const vec4> > polysAndColors;

            //for(size_t i = 0; i < polysAndColors.size(); ++i)
            for(size_t i = 0; i < geomsIn.size(); ++i)
            {
                //polys[i] = dynamic_cast<const Geometry *>(geomsIn[i].second);
                Attributes * attrs = geomsIn[i].first;

                //dmess("attrs " << attrs);

                const Geometry * geom = dynamic_cast<const Geometry *>(geomsIn[i].second);

                if(!geom)
                {
                    dmess("!geom");
                }

                const vec4 outlineColor(0,1,0,1);

                vec4 fillColor = vec4(0,0,1,0.5);

                if(attrs->hasStringKey("addr_house"))
                {
                    fillColor = vec4(0.7,0.5,0,0.5);
                }
                else if(attrs->hasStringKey("building"))
                {
                    fillColor = vec4(1,0.5,0,0.5);
                }
                else if(attrs->hasStringKeyValue("landuse", "grass") || attrs->hasStringKeyValue("surface", "grass"))
                {
                    fillColor = vec4(0,0.7,0,0.5);
                }
                
                polysAndColors.push_back(make_tuple(geom, fillColor, outlineColor));
            }
            
            dmess("polysAndColors " << polysAndColors.size());

            Renderable * r = RenderablePolygon::create(polysAndColors, trans);

            dmess("after " << r);
            
            //r->setFillColor(vec4(0.3,0.0,0.3,0.3));
            
            r->setOutlineColor(vec4(0,1,0,1));
            
            canvas->addRenderiable(r);
            //*/
            
            dmess("Done creating renderiable.");
        });
    });
}

pair<Renderable *, Attributes *> GeoClient::pickRenderable(const vec3 & _pos)
{
    const vec4 pos = inverseTrans * vec4(_pos, 1.0);
    
    vector< void * > query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    quadTree->query(&bounds, query);
    
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

vector<pair<Renderable *, Attributes *> > GeoClient::pickRenderables(const vec3 & _pos)
{
    const vec4 pos = inverseTrans * vec4(_pos, 1.0);
    
    vector< void * > query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    quadTree->query(&bounds, query);
    
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