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
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/simplify/DouglasPeuckerLineSimplifier.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/canvas/Canvas.h>
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
            Attributes       * attributes) : m_renderable	(renderable),
                                             m_geom			(geom),
                                             m_attributes	(attributes),
                                             m_start		(getStartPoint(geom)),
                                             m_end			(getEndPoint  (geom))
{
    m_weight = geom->getLength() * 1000000 + 1;
}

Renderable       * Edge::getRenderable() const { return m_renderable ;}
const LineString * Edge::getGeometry()   const { return m_geom       ;}
Attributes       * Edge::getAttributes() const { return m_attributes ;}

Renderable		 * Edge::setRenderable(Renderable * renderable) { return m_renderable = renderable ;}

#define INF 0x3f3f3f3f // TODO use numeric_limits!

namespace
{
    typedef vector<Edge *> Edges;

    Edges a_edges;

    unordered_map<dvec2, Edges> a_edgeMap;

    PointOnEdge a_startPoint;
    PointOnEdge a_endPoint;

    unique_ptr<Renderable> a_startRenderable;

    class Node;

    typedef pair<size_t, Edge *> NodeEdge;

    typedef vector<NodeEdge> Neighbors;

    class Node
    {
    public:

        Neighbors m_neighbors;
    };

    vector<Node> a_nodes;

    unordered_map<dvec2, size_t> a_nodeMap;

    typedef pair<int, size_t> Path;

    class PathCmp 
    { 
    public:

        int operator() (const Path & p1, const Path & p2) { return get<0>(p1) > get<0>(p2) ;}
    };

    unique_ptr<Renderable> a_pathAnnotation;

    // Dijkstra state arrays
    vector<int>    a_dist;
    vector<bool>   a_seen;
    vector<size_t> a_parent;
    vector<Path>   a_Q;
}

Network::Network(GeoClient * client) : m_client(client) {}

Network::Network(const vector<AttributedLineString> & lineStrings, const dmat4 & trans) : m_trans(trans)
{
    setEdges(lineStrings);
}

Network::~Network() {}

void Network::setEdges(const Edges & edges)
{
    a_edges = edges;

    dmess("start Network::build " << a_edges.size());

    for(const auto & edge : a_edges)
    {
        a_edgeMap[edge->m_start].push_back(edge);
        a_edgeMap[edge->m_end  ].push_back(edge);
    }

    a_nodes.resize(a_edgeMap.size());

    a_Q.reserve(a_nodes.size());

    a_dist   = vector<int>   (a_nodes.size(), INF);
    a_seen   = vector<bool>  (a_nodes.size(), false);
    a_parent = vector<size_t>(a_nodes.size());

    size_t currNode = 0;

    for(const auto i : a_edgeMap) { a_nodeMap[i.first] = currNode++ ;}

    for(const auto & [currPos, edges] : a_edgeMap)
    {
        auto & neighbors = a_nodes[a_nodeMap[currPos]].m_neighbors;

        for(const auto & edge : edges)
        {
            if(currPos == edge->m_start)	{ neighbors.push_back(NodeEdge(a_nodeMap[edge->m_end],   edge)) ;}
            else							{ neighbors.push_back(NodeEdge(a_nodeMap[edge->m_start], edge)) ;}
        }
    }

    dmess("end Network::build " << edges.size());
}

void Network::setEdges(const vector<AttributedLineString> & lineStrings)
{
    Edges edges;

    for(size_t i = 0; i < lineStrings.size(); ++i)
    {
        Attributes     * attrs   = lineStrings[i].first;
        const Geometry * geom    = lineStrings[i].second;

        edges.push_back(new Edge(nullptr, dynamic_cast<const LineString *>(geom), attrs));
    }

    setEdges(edges);
}

void Network::setStartEdge(const PointOnEdge & start)
{
    dmess("Network::setStartEdge");

    a_startPoint = start;

    a_startRenderable = unique_ptr<Renderable>(RenderablePoint::create(vec3(get<0>(start), 0), m_client->getTrans()));

    m_client->getCanvas()->addRenderable(a_startRenderable.get());
}

void Network::findPath(const PointOnEdge & end)
{
    auto coords = findPath(a_startPoint, end);

    if(!coords) { return ;}

    Geometry::Ptr path(GeometryFactory::getDefaultInstance()->createLineString(new CoordinateArraySequence(coords, 2)));

    Geometry::Ptr buffered(path->buffer(0.00005, 3));

	vector<ColoredGeometry> geoms;

	geoms.push_back(ColoredGeometry(buffered.get(), 1));

    a_pathAnnotation = unique_ptr<Renderable>(Renderable::create(buffered, m_client->getTrans()));
	
	a_pathAnnotation->setShader(ColorVertexShader::getDefaultInstance());
    a_pathAnnotation->setRenderFill(true);
    a_pathAnnotation->setRenderOutline(true);

    m_client->getCanvas()->addRenderable(a_pathAnnotation.get());
}

namespace
{
	class NodeTotalGreater
	{
		public:

			NodeTotalGreater() { ;}

			bool operator() ( const Path & p1, const Path & p2) const { return get<0>(p1) > get<0>(p2) ;}
	};
}

Path popPriorityQueue()
{
	auto path = a_Q.front();

	pop_heap(a_Q.begin(), a_Q.end(), NodeTotalGreater() );

	a_Q.pop_back();

	return path;
}

void pushPriorityQueue(const Path & path)
{
	a_Q.push_back(path);

	push_heap(a_Q.begin(), a_Q.end(), NodeTotalGreater() );
}

void updateTileOnPriorityQueue(const size_t path, const int newDist)
{
	vector<Path>::iterator i;

	for(i = a_Q.begin(); i != a_Q.end(); ++i)
	{
		if(i->second != path) { continue ;}
		
        if(i->first >= newDist) { return ;}
        
        i->first = newDist;

        push_heap(a_Q.begin(), i + 1, NodeTotalGreater());

        return;
	}
}

//priority_queue<Path, vector<Path> , PathCmp> pq;

// Modified from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
vector<Coordinate> * Network::findPath(const PointOnEdge & start, const PointOnEdge & end)
{
    //dmess("start Network::findPath");

    a_endPoint = end;

    if(!get<1>(start)) { return nullptr ;}

    a_Q.clear();
    a_Q.reserve(a_nodes.size());

    priority_queue<Path, vector<Path> , PathCmp> pq(PathCmp(), a_Q);
    //pq.clear();

    const size_t startIndex = a_nodeMap[start.second->m_start];

    // TODO, try to only reset the ones which have been modified.
    for(size_t i = 0; i < a_nodes.size(); ++i)
    {
        a_dist[i] = INF;

        a_seen[i] = false;
    }

    a_dist[startIndex] = 0;

    a_parent[startIndex] = startIndex;

    pq.push(Path(0.0, startIndex));

    //Q.clear();

    pushPriorityQueue(Path(0.0, startIndex));

    size_t v;

    while(!pq.empty())
    //while(!Q.empty())
    {
        const size_t u = pq.top().second; pq.pop();

        //const size_t u = popPriorityQueue().second;

        a_seen[u] = true;

        for(const auto x : a_nodes[u].m_neighbors)
        {
            v = x.first;

            if(x.second == end.second)
            {
                //dmess("Done!");

                a_parent[v] = u;

                goto done;
            }

            const int weight = x.second->getWeight();

            const int newDist = a_dist[u] + weight;

            if(a_seen[v])
            {
                //updateTileOnPriorityQueue(v, newDist);
            }
            else if(a_dist[v] > newDist)
            {
                a_dist[v] = newDist;

                a_parent[v] = u;

                pq.push(Path(newDist, v));
                //pushPriorityQueue(Path(newDist, v));
            }
        }
    }

    return nullptr;

    done:;

    vector<dvec2> path;

    vector<Edge *> edges;

    do
    {
        const size_t next = a_parent[v];

        for(const auto & i : a_nodes[next].m_neighbors)
        {
            if(i.first != v) { continue ;} // TODO should have O(1) access to edge

            edges.push_back(i.second);

            break;
        }

        v = next;

    } while(v != startIndex);

    if(edges.size() > 1)
    {
        auto coords = new vector<Coordinate>();

        Edge * A = edges[0];
        Edge * B = edges[1];

        dvec2 lastPoint;

        const auto points = A->getGeometry()->getCoordinatesRO()->toVector();

        if(A->m_end == B->m_start || A->m_end == B->m_end)
        {
            lastPoint = A->m_end;

            coords->insert(coords->end(), points->begin(), points->end());
        }
        else
        {
            lastPoint = A->m_start;

            coords->insert(coords->end(), points->rbegin(), points->rend());
        }
        
        for(size_t i = 1; i < edges.size(); ++i)
        {
            A = edges[i];
            
            const auto points = A->getGeometry()->getCoordinatesRO()->toVector();

            if(lastPoint == A->m_start)
            {
                coords->insert(coords->end(), points->begin(), points->end());

                lastPoint = A->m_end;
            }
            else
            {
                coords->insert(coords->end(), points->rbegin(), points->rend());

                lastPoint = A->m_start;
            }
        }

        return coords;        
    }
    else
    {
        //dmess("Implement me!");
    }

    //dmess("end Network::findPath");

    return nullptr;
}

vector<dvec2> Network::getRandomPath()
{
    unique_ptr<vector<Coordinate> > coords = nullptr;

    for(; !coords ;)
    {
        auto A = a_edges[rand() % a_edges.size()];
        auto B = a_edges[rand() % a_edges.size()];

        coords = unique_ptr<vector<Coordinate> >(findPath(PointOnEdge(A->m_start, A), PointOnEdge(B->m_end, B)));
    }

    if(m_client) { m_trans = m_client->getTrans() ;}

    transformInPlace(*coords, m_trans);

    return __(DouglasPeuckerLineSimplifier::simplify(*coords, 0.000001));
}