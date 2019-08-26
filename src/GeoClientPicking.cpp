/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 //
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

#include <geos/geom/GeometryFactory.h>
#include <geos/algorithm/distance/DistanceToPoint.h>
#include <geos/algorithm/distance/PointPairDistance.h>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/renderables/Renderable.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/GeoClient.h>

#ifdef max
#undef max
#endif

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geos::algorithm::distance;
using namespace geosUtil;

PointOnEdge GeoClient::pickLineStringRenderable(const vec3 & _pos) const
{
    const vec4 pos = m_inverseTrans * vec4(_pos, 1.0);
    
    vector<void *> query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    m_quadTreeLineStrings->query(&bounds, query);
    
    double minDist = numeric_limits<double>::max();

    Edge * closest = nullptr;

    Coordinate pointOnEdge;

    const Coordinate p(pos.x, pos.y);

    for(const void * _data : query)
    {
        Edge * data = (Edge *)_data;

        const LineString * geom = data->getGeometry();

        PointPairDistance ptDist;

        DistanceToPoint::computeDistance(*geom, p, ptDist);

        if(ptDist.getDistance() >= minDist) { continue ;}

        minDist     = ptDist.getDistance();
        closest     = data;
        pointOnEdge = ptDist.getCoordinate(0);
    }

    return PointOnEdge(__(pointOnEdge), closest);
}

pair<Renderable *, Attributes *> GeoClient::pickPolygonRenderable(const vec3 & _pos) const
{
    const vec4 pos = m_inverseTrans * vec4(_pos, 1.0);
    
    vector< void * > query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    m_quadTreePolygons->query(&bounds, query);
    
    // TODO, there might be a method accepting a Coordinate
    Point * p = scopedGeosGeometry(GeometryFactory::getDefaultInstance()->createPoint(Coordinate(pos.x, pos.y)));

    double minArea = numeric_limits<double>::max();

    Renderable * smallest      = nullptr;
    Attributes * smallestAttrs = nullptr;

    for(const void * _data : query)
    {
        tuple<Renderable *, const Geometry *, Attributes *> * data = (tuple<Renderable *, const Geometry *, Attributes *> *)_data;

        const Geometry * geom = get<1>(*data);

        if(!p->within(geom)) { continue ;}

        const double area = geom->getArea();

        if(area >= minArea) { continue ;}

        minArea       = area; 
        smallest      = get<0>(*data);
        smallestAttrs = get<2>(*data); 
    }

    return make_pair(smallest, smallestAttrs);
}

vector<pair<Renderable *, Attributes *> > GeoClient::pickPolygonRenderables(const vec3 & _pos) const
{
    const vec4 pos = m_inverseTrans * vec4(_pos, 1.0);
    
    vector< void * > query;
    
    const Envelope bounds(pos.x, pos.x, pos.y, pos.y);
    
    m_quadTreePolygons->query(&bounds, query);
    
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

string GeoClient::doPicking(const char mode, const dvec4 & pos) const
{
    Renderable * renderable;
    Attributes * attrs;

    switch(mode)
    {
        case GUI::PICK_MODE_LINESTRING:
        case GUI::SET_PATH_START_POINT:
        case GUI::FIND_PATH:
        {
            Edge * edge;
            dvec2  pointOnEdge;

            tie(pointOnEdge, edge) = pickLineStringRenderable(m_canvas->getCursorPosWC());

            if(!edge) { break ;}

			edge->getRenderable()->ensureVAO();

            edge->getRenderable()->render(m_canvas);

            m_canvas->renderCursor(m_trans * dvec4(pointOnEdge, 0, 1));

            return edge->getAttributes()->toString();
        }
        case GUI::PICK_MODE_POLYGON_SINGLE:
        {
            tie(renderable, attrs) = pickPolygonRenderable(m_canvas->getCursorPosWC());

            if(!renderable) { return "" ;}
            
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

			renderable->ensureVAO();

			renderable->render(m_canvas);

            return attrs->toString();
        }
        case GUI::PICK_MODE_POLYGON_MULTIPLE:
        {
            vector<pair<Renderable *, Attributes *> > picked = pickPolygonRenderables(m_canvas->getCursorPosWC());

            if(!picked.size()) { return "" ;}
            
            tie(renderable, attrs) = picked[0];

			renderable->ensureVAO();

			renderable->render(m_canvas);

            string attrsStr = attrs->toString();

            for(size_t i = 1; i < picked.size(); ++i)
            {
                attrsStr += "\n";

                attrsStr += get<1>(picked[i])->toString();
            }
            
            return attrsStr;
        }
    }

    return "";
}