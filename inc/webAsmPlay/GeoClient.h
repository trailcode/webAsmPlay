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
class Canvas;
class Renderable;

namespace geos
{
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

    void getNumPolygons(const std::function<void (const size_t)> & callback);

    void getNumPolylines(const std::function<void (const size_t)> & callback);

    void getLayerBounds(const std::function<void (const AABB2D &)> & callback);

    void getPolygons(const size_t startIndex, const size_t numPolys, std::function<void (std::vector<AttributedGeometry> geoms)> callback);

    void getPolylines(const size_t startIndex, const size_t numPolylines, std::function<void (std::vector<AttributedGeometry> geoms)> callback);

    void loadAllGeometry(Canvas * canvas);
    
    std::pair<Renderable *, Attributes *> pickRenderable(const glm::vec3 & pos);

    std::vector<std::pair<Renderable *, Attributes *> > pickRenderables(const glm::vec3 & pos);

    glm::dmat4 getTrans() const;
    glm::dmat4 getInverseTrans() const;

private:
    
    typedef std::vector<const geos::geom::Geometry *> GeomVector;
    
    void createRenderiables(GeomVector * geoms, const glm::mat4 trans, Canvas * canvas);

    void createPolygonRenderiables(const std::vector<AttributedGeometry> & geoms, Canvas * canvas);

    void createLineStringRenderiables(const std::vector<AttributedGeometry> & geoms, Canvas * canvas);
    
    geos::index::quadtree::Quadtree * quadTree;
    
    glm::dmat4 trans;
    glm::dmat4 inverseTrans;
    
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

