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
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/simplify/DouglasPeuckerLineSimplifier.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Network.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geos::simplify;
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

    unique_ptr<Renderable> pathAnnotation;
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

void Network::findPath(const PointOnEdge & end)
{
    vector<Coordinate> * coords = findPath(startPoint, end);

    if(!coords) { return ;}

    Geometry::Ptr path(GeometryFactory::getDefaultInstance()->createLineString(new CoordinateArraySequence(coords, 2)));

    Geometry::Ptr buffered(path->buffer(0.00005, 3));

    pathAnnotation = unique_ptr<Renderable>(Renderable::create(buffered, client->getTrans()));

    pathAnnotation->setRenderFill(true);
    pathAnnotation->setRenderOutline(true);

    client->getCanvas()->addRenderable(pathAnnotation.get());
}

// Modified from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
vector<Coordinate> * Network::findPath(const PointOnEdge & start, const PointOnEdge & end)
{
    //dmess("start Network::findPath");

    toRender.clear();

    endPoint = end;

    if(!get<1>(start)) { return NULL ;}

    priority_queue<Path, vector<Path> , PathCmp> pq;

    const size_t startIndex = nodeMap[start.second->start];

    //dmess("startIndex " << startIndex << " nodeMap " << nodeMap.size());

    vector<int> dist(nodes.size(), INF);

    vector<bool> seen(nodes.size(), false);

    vector<size_t> parent(nodes.size());

    dist[startIndex] = 0;

    parent[startIndex] = startIndex;

    pq.push(Path(0.0, startIndex));

    size_t v;

    while(!pq.empty())
    {
        const size_t u = pq.top().second; pq.pop();

        seen[u] = true;

        for(const auto x : nodes[u].neighbors)
        {
            v = x.first;

            if(x.second == end.second)
            {
                //dmess("Done!");

                parent[v] = u;

                goto done;
            }

            const int weight = x.second->getWeight();

            const int newDist = dist[u] + weight;

            if(!seen[v] && dist[v] > newDist)
            {
                dist[v] = newDist;

                parent[v] = u;

                //toRender.push_back(x.second->getRenderable());

                pq.push(Path(newDist, v));
            }
        }
    }

    return NULL;

    done:;

    vector<dvec2> path;

    vector<Edge *> edges;

    do
    {
        const size_t next = parent[v];

        for(const auto & i : nodes[next].neighbors)
        {
            if(i.first != v) { continue ;} // TODO should have O(1) access to edge

            //toRender.push_back(i.second->getRenderable());

            edges.push_back(i.second);

            break;
        }

        v = next;

    } while(v != startIndex);

    if(edges.size() > 1)
    {
        vector<Coordinate> * coords = new vector<Coordinate>();

        Edge * A = edges[0];
        Edge * B = edges[1];

        dvec2 lastPoint;

        const vector<Coordinate> * points = A->getGeometry()->getCoordinatesRO()->toVector();

        if(A->end == B->start || A->end == B->end)
        {
            lastPoint = A->end;

            coords->insert(coords->end(), points->begin(), points->end());
        }
        else
        {
            lastPoint = A->start;

            coords->insert(coords->end(), points->rbegin(), points->rend());
        }
        
        for(size_t i = 1; i < edges.size(); ++i)
        {
            A = edges[i];
            
            const vector<Coordinate> * points = A->getGeometry()->getCoordinatesRO()->toVector();

            if(lastPoint == A->start)
            {
                coords->insert(coords->end(), points->begin(), points->end());

                lastPoint = A->end;
            }
            else
            {
                coords->insert(coords->end(), points->rbegin(), points->rend());

                lastPoint = A->start;
            }
        }

        return coords;        
    }
    else
    {
        //dmess("Implement me!");
    }

    //dmess("end Network::findPath");

    return NULL;
}

Canvas * theCanvas = NULL;

unique_ptr<vector<Coordinate> > Network::getRandomPath()
{
    unique_ptr<vector<Coordinate> > coords = NULL;

    for(; !coords ;)
    {
        Edge * A = edges[rand() % edges.size()];
        Edge * B = edges[rand() % edges.size()];

        coords = unique_ptr<vector<Coordinate> >(findPath(PointOnEdge(A->start, A), PointOnEdge(B->end, B)));
    }

    transformInPlace(*coords, client->getTrans());

    return DouglasPeuckerLineSimplifier::simplify(*coords, 0.000001);
}