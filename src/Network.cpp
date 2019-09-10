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

    // Dijkstra state arrays
    vector<int>    dist;
    vector<bool>   seen;
    vector<size_t> parent;
    vector<Path>   Q;
}

Network::Network(GeoClient * client) : m_client(client) {}

Network::Network(const vector<AttributedLineString> & lineStrings, const dmat4 & trans) : m_trans(trans)
{
    setEdges(lineStrings);
}

Network::~Network() {}

void Network::setEdges(const Edges & _edges)
{
    edges = _edges;

    dmess("start Network::build " << edges.size());

    for(const auto & edge : edges)
    {
        edgeMap[edge->m_start].push_back(edge);
        edgeMap[edge->m_end  ].push_back(edge);
    }

    nodes.resize(edgeMap.size());

    Q.reserve(nodes.size());

    dist   = vector<int>   (nodes.size(), INF);
    seen   = vector<bool>  (nodes.size(), false);
    parent = vector<size_t>(nodes.size());

    size_t currNode = 0;

    for(const auto i : edgeMap) { nodeMap[i.first] = currNode++ ;}

    for(const auto i : edgeMap)
    {
        const dvec2 & currPos = i.first;

        Neighbors & neighbors = nodes[nodeMap[currPos]].neighbors;

        for(const auto & j : i.second)
        {
            if(currPos == j->m_start) { neighbors.push_back(NodeEdge(nodeMap[j->m_end],   j)) ;}
            else                    { neighbors.push_back(NodeEdge(nodeMap[j->m_start], j)) ;}
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

    startPoint = start;

    startRenderable = unique_ptr<Renderable>(RenderablePoint::create(vec3(get<0>(start), 0), m_client->getTrans()));

    m_client->getCanvas()->addRenderable(startRenderable.get());
}

void Network::findPath(const PointOnEdge & end)
{
    vector<Coordinate> * coords = findPath(startPoint, end);

    if(!coords) { return ;}

    Geometry::Ptr path(GeometryFactory::getDefaultInstance()->createLineString(new CoordinateArraySequence(coords, 2)));

    Geometry::Ptr buffered(path->buffer(0.00005, 3));

	vector<ColoredGeometry> geoms;

	geoms.push_back(ColoredGeometry(buffered.get(), 1));

    pathAnnotation = unique_ptr<Renderable>(Renderable::create(buffered, m_client->getTrans()));
	//pathAnnotation = unique_ptr<Renderable>(RenderablePolygon::create(geoms, m_client->getTrans(), true));

	//pathAnnotation->setShader(ColorDistanceShader::getDefaultInstance());
	pathAnnotation->setShader(ColorVertexShader::getDefaultInstance());
    pathAnnotation->setRenderFill(true);
    pathAnnotation->setRenderOutline(true);

    m_client->getCanvas()->addRenderable(pathAnnotation.get());
}

class nodeTotalGreater
{

	public:
		nodeTotalGreater() { ;}
		bool operator() ( const Path & p1, const Path & p2)
		const
		{
            return get<0>(p1) > get<0>(p2);
		}
};

Path popPriorityQueue()
{
	Path path = Q.front();
	pop_heap(Q.begin(), Q.end(), nodeTotalGreater() );
	Q.pop_back();
	return path;
}

void pushPriorityQueue(const Path & path)
{
	Q.push_back(path);

	push_heap(Q.begin(), Q.end(), nodeTotalGreater() );
}

void updateTileOnPriorityQueue(const size_t path, const int newDist)
{
	vector<Path>::iterator i;

	for(i = Q.begin(); i != Q.end(); ++i)
	{
		if(i->second != path) { continue ;}
		
        if(i->first >= newDist) { return ;}
        
        i->first = newDist;

        push_heap(Q.begin(), i + 1, nodeTotalGreater());

        return;
	}
}

//priority_queue<Path, vector<Path> , PathCmp> pq;

// Modified from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
vector<Coordinate> * Network::findPath(const PointOnEdge & start, const PointOnEdge & end)
{
    //dmess("start Network::findPath");

    endPoint = end;

    if(!get<1>(start)) { return nullptr ;}

    Q.clear();
    Q.reserve(nodes.size());

    priority_queue<Path, vector<Path> , PathCmp> pq(PathCmp(), Q);
    //pq.clear();

    const size_t startIndex = nodeMap[start.second->m_start];

    // TODO, try to only reset the ones which have been modified.
    for(size_t i = 0; i < nodes.size(); ++i)
    {
        dist[i] = INF;

        seen[i] = false;
    }

    dist[startIndex] = 0;

    parent[startIndex] = startIndex;

    pq.push(Path(0.0, startIndex));

    //Q.clear();

    pushPriorityQueue(Path(0.0, startIndex));

    size_t v;

    while(!pq.empty())
    //while(!Q.empty())
    {
        const size_t u = pq.top().second; pq.pop();

        //const size_t u = popPriorityQueue().second;

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

            if(seen[v])
            {
                //updateTileOnPriorityQueue(v, newDist);
            }
            else if(dist[v] > newDist)
            {
                dist[v] = newDist;

                parent[v] = u;

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
        const size_t next = parent[v];

        for(const auto & i : nodes[next].neighbors)
        {
            if(i.first != v) { continue ;} // TODO should have O(1) access to edge

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
            
            const vector<Coordinate> * points = A->getGeometry()->getCoordinatesRO()->toVector();

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
        Edge * A = edges[rand() % edges.size()];
        Edge * B = edges[rand() % edges.size()];

        coords = unique_ptr<vector<Coordinate> >(findPath(PointOnEdge(A->m_start, A), PointOnEdge(B->m_end, B)));
    }

    if(m_client) { m_trans = m_client->getTrans() ;}

    transformInPlace(*coords, m_trans);

    return __(DouglasPeuckerLineSimplifier::simplify(*coords, 0.000001));
}