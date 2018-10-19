#ifndef __WEB_ASM_PLAY_GEO_CLIENT_H__
#define __WEB_ASM_PLAY_GEO_CLIENT_H__

#ifndef __EMSCRIPTEN__
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>
#endif

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/Types.h>

class GLFWwindow;
class GeoRequestGetNumGeoms;
class GeoRequestLayerBounds;
class GeoRequestGeometry;
class Canvas;
class Renderable;

namespace geos
{
    namespace geom
    {
        class Geometry;
    }
    
    namespace index
    {
        namespace quadtree
        {
            class Quadtree;
        }
    }
}

class GeoClient
{
public:

    GeoClient(GLFWwindow * window);

    virtual ~GeoClient();

    static void onMessage(const std::string & data);

    void getNumGeoms(const std::function<void (const size_t)> & callback);

    void getLayerBounds(const std::function<void (const AABB2D &)> & callback);

    void getGeometry(const size_t geomIndex, std::function<void (geos::geom::Geometry *)> & callback);

    void loadGeometry(Canvas * canvas);
    
    std::vector<Renderable *> pickRenderables(const glm::vec3 & pos);

private:
    
    typedef std::vector<const geos::geom::Geometry *> GeomVector;
    
    void createRenderiables(GeomVector * geoms, const glm::mat4 trans, Canvas * canvas);
    
    geos::index::quadtree::Quadtree * quadTree;
    
    glm::mat4 trans;
    glm::mat4 inverseTrans;
    
#ifndef __EMSCRIPTEN__

    static void on_open(GeoClient * client, websocketpp::connection_hdl hdl);
    
    // pull out the type of messages sent by our config
    typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

    static void on_message(GeoClient * client, websocketpp::connection_hdl hdl, message_ptr msg);

    typedef websocketpp::client<websocketpp::config::asio_client> Client;
    
    Client::connection_ptr con;

    Client * client;

    std::thread * clientThread;

#endif
};

#endif // __WEB_ASM_PLAY_GEO_CLIENT_H__

