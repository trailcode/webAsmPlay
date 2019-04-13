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
#pragma once

#ifndef __EMSCRIPTEN__
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>
#endif

#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/Types.h>

class Canvas;
class Renderable;
class Network;
class Edge;

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

    GeoClient(Canvas * canvas);

    virtual ~GeoClient();

    static void onMessage(const std::string & data);

    void getNumPolygons(const std::function<void (const size_t)> & callback);

    void getNumPolylines(const std::function<void (const size_t)> & callback);

    void getNumPoints(const std::function<void (const size_t)> & callback);

    void getLayerBounds(const std::function<void (const AABB2D &)> & callback);

    void getPolygons(const size_t startIndex, const size_t numPolys, std::function<void (std::vector<AttributedGeometry> geoms)> callback);

    void getPolylines(const size_t startIndex, const size_t numPolylines, std::function<void (std::vector<AttributedGeometry> geoms)> callback);

    void getPoints(const size_t startIndex, const size_t numPoints, std::function<void (std::vector<AttributedGeometry> geoms)> callback);

    void loadGeoServerGeometry();

    void loadGeometry(const std::string fileName);

    void addBingMap(const bool enabled);

    PointOnEdge pickLineStringRenderable(const glm::vec3 & pos) const;
    
    std::pair<Renderable *, Attributes *> pickPolygonRenderable(const glm::vec3 & pos) const;

    std::vector<std::pair<Renderable *, Attributes *> > pickPolygonRenderables(const glm::vec3 & pos) const;

    glm::dmat4 getTrans() const;
    glm::dmat4 getInverseTrans() const;

    std::string doPicking(const char mode, const glm::dvec4 & pos) const;

    void createWorld(const char * data);

	void addGeometry(const char * data);

    Network * getNetwork() const;

    Canvas * getCanvas() const;

    AABB2D getBounds() const;

private:
    
#ifndef __EMSCRIPTEN__

    static void on_open(GeoClient * client, websocketpp::connection_hdl hdl);
    
    // pull out the type of messages sent by our config
    typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

	typedef websocketpp::client<websocketpp::config::asio_client> Client;

    static void on_message(GeoClient * client, websocketpp::connection_hdl hdl, message_ptr msg);
    
    Client::connection_ptr m_con;

    Client * m_client = NULL;

    std::thread * m_clientThread = NULL;

#endif

    void createPolygonRenderiables   (const std::vector<AttributedGeometry> & geoms);
    void createLineStringRenderiables(const std::vector<AttributedGeometry> & geoms);
    void createPointRenderiables     (const std::vector<AttributedGeometry> & geoms);

    void ensureClient();

    geos::index::quadtree::Quadtree * m_quadTreePolygons;
    geos::index::quadtree::Quadtree * m_quadTreeLineStrings;
    geos::index::quadtree::Quadtree * m_quadTreePoints;
    
    glm::dmat4 m_trans;
    glm::dmat4 m_inverseTrans;

    Canvas * m_canvas;

    Network * m_network;

    AABB2D m_bounds;
};


