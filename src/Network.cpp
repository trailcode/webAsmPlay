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

#include <memory>
#include <unordered_map>
#include <glm/gtx/hash.hpp>
#include <geos/geom/LineString.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Network.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

Edge::Edge( Renderable       * renderable,
            const LineString * geom,
            Attributes       * attributes) : renderable(renderable),
                                             geom      (geom),
                                             attributes(attributes),
                                             start     (getStartPoint(geom)),
                                             end       (getEndPoint(geom))
{

}

Renderable       * Edge::getRenderable() const { return renderable ;}
const LineString * Edge::getGeometry()   const { return geom       ;}
Attributes       * Edge::getAttributes() const { return attributes ;}

namespace
{
    typedef vector<Edge *> Edges;

    Edges edges;

    unordered_map<dvec2, Edges> edgeMap;

    PointOnEdge start;
    PointOnEdge endPoint;

    unique_ptr<Renderable> startRenderable;
}

Network::Network(GeoClient * client) : client(client) {}
Network::~Network() {}

void Network::setEdges(const Edges & _edges)
{
    edges = _edges;

    dmess("Network::build " << edges.size());

    for(const auto & edge : edges)
    {
        edgeMap[edge->start].push_back(edge);
        edgeMap[edge->end  ].push_back(edge);
    }
}

void Network::setStartEdge(const PointOnEdge & _start)
{
    dmess("Network::setStartEdge");

    start = _start;

    startRenderable = unique_ptr<Renderable>(RenderablePoint::create(vec3(get<0>(start), 0), client->getTrans()));

    client->getCanvas()->addRenderable(startRenderable.get());
}

void Network::findPath(const PointOnEdge & end)
{
    endPoint = end;
}