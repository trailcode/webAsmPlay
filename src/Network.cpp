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
#include <queue>
#include <limits>
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
    weight = geom->getLength() * 1000000 + 1;

    dmess("weight " << weight);
}

Renderable       * Edge::getRenderable() const { return renderable ;}
const LineString * Edge::getGeometry()   const { return geom       ;}
Attributes       * Edge::getAttributes() const { return attributes ;}

namespace
{
    typedef vector<Edge *> Edges;

    Edges edges;

    unordered_map<dvec2, Edges> edgeMap;

    PointOnEdge startPoint;
    PointOnEdge endPoint;

    unique_ptr<Renderable> startRenderable;

    class Node;

    typedef pair<size_t, Edge *> NodeEdge;

    typedef vector<NodeEdge> Neighbors;

    class Node
    {
    public:

        Neighbors neighbors;
    };

    vector<Node> nodes;

    unordered_map<dvec2, size_t> nodeMap;

    typedef pair<int, size_t> Path;

    class PathCmp 
    { 
    public:

        int operator() (const Path & p1, const Path & p2) { return get<0>(p1) > get<0>(p2) ;}
    };
}

Network::Network(GeoClient * client) : client(client) {}
Network::~Network() {}

void Network::setEdges(const Edges & _edges)
{
    edges = _edges;

    dmess("start Network::build " << edges.size());

    for(const auto & edge : edges)
    {
        edgeMap[edge->start].push_back(edge);
        edgeMap[edge->end  ].push_back(edge);
    }

    nodes.resize(edgeMap.size());

    size_t currNode = 0;

    for(const auto i : edgeMap) { nodeMap[i.first] = currNode++ ;}

    for(const auto i : edgeMap)
    {
        const dvec2 & currPos = i.first;

        Neighbors & neighbors = nodes[nodeMap[currPos]].neighbors;

        for(const auto & j : i.second)
        {
            if(currPos == j->start) { neighbors.push_back(NodeEdge(nodeMap[j->end],   j)) ;}
            else                    { neighbors.push_back(NodeEdge(nodeMap[j->start], j)) ;}
        }
    }

    dmess("end Network::build " << edges.size());
}

void Network::setStartEdge(const PointOnEdge & start)
{
    dmess("Network::setStartEdge");

    startPoint = start;

    startRenderable = unique_ptr<Renderable>(RenderablePoint::create(vec3(get<0>(start), 0), client->getTrans()));

    client->getCanvas()->addRenderable(startRenderable.get());
}

# define INF 0x3f3f3f3f

vector<Renderable *> toRender;

// Modified from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
void Network::findPath(const PointOnEdge & end)
{
    dmess("start Network::findPath");

    toRender.clear();

    endPoint = end;

    if(!get<1>(startPoint)) { return ;}

    priority_queue<Path, vector<Path> , PathCmp> pq;

    const size_t startIndex = nodeMap[startPoint.second->start];

    dmess("startIndex " << startIndex);

    vector<int> dist(nodes.size(), INF);

    vector<bool> seen(nodes.size(), false);

    dist[startIndex] = 0;

    pq.push(Path(0.0, startIndex));

    const size_t endIndex = nodeMap[end.second->end];

    dmess("endIndex " << endIndex);

    int maxa = 0;

    while(!pq.empty())
    {
        const size_t u = pq.top().second; pq.pop();

        seen[u] = true;

        //dmess("pq " << pq.size() << " nodes[u].neighbors " << nodes[u].neighbors.size() << " u " << u);

        //if(maxa++ > 500) { break ;}

        for(const auto x : nodes[u].neighbors)
        {
            const size_t v = x.first;

            if(x.second == end.second)
            {
                dmess("Done!");

                goto done;
            }

            const int weight = x.second->getWeight();

            const int newDist = dist[u] + weight;

            //dmess("    v " << v << " weight " << weight << " newDist " << newDist << " dist[v] " << dist[v] << " dist[u] " << dist[u]);

            if(!seen[v] && dist[v] > newDist)
            {
                dist[v] = newDist;

                toRender.push_back(x.second->getRenderable());

                pq.push(Path(newDist, v));
            }
        }
    }

    done:;

    dmess("end Network::findPath");
}