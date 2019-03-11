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

#include <geos/geom/LineString.h>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/OpenSteerGlue.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/geom/GeometryConverter.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderableMesh.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/OpenSteerGlue.h>
#include <webAsmPlay/GeoClient.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;
using namespace geos::index::quadtree;

void GeoClient::addGeometry(const char * data)
{
    dmess("GeoClient::addGeometry");

    GUI::progress("Polygon index:", 0.0f);

    bounds = *(AABB2D *)data; data += sizeof(double) * 4;

    const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

    trans = translate(  s,
                        dvec3(  (get<2>(bounds) + get<0>(bounds)) * -0.5,
                                (get<3>(bounds) + get<1>(bounds)) * -0.5,
                                0.0));
    
    inverseTrans = inverse(trans);

    createPolygonRenderiables   (GeometryConverter::getGeosPolygons   (data));
    createLineStringRenderiables(GeometryConverter::getGeosLineStrings(data));
    createPointRenderiables     (GeometryConverter::getGeosPoints     (data));

    OpenSteerGlue::init(canvas, getNetwork());
}

namespace
{
    pair<double, double> getHeight(Attributes * attrs)
    {
        const double scale = 0.005;

        double minHeight = 0;
        double height = scale;
        
        // See: https://wiki.openstreetmap.org/wiki/OSM-3D.org
        
             if(attrs->hasStringKey("height"))          { height    = atof(attrs->strings["height"]         .c_str()) * scale * 0.3 ;}
        else if(attrs->hasStringKey("building:height")) { height    = atof(attrs->strings["building:height"].c_str()) * scale * 0.3 ;}
        else if(attrs->hasStringKey("building:levels")) { height    = atof(attrs->strings["building:levels"].c_str()) * scale       ;}
        
        if(attrs->hasStringKey("min_height")) { minHeight = atof(attrs->strings["min_height"].c_str()) * scale * 0.3 ;}

        return make_pair(height, minHeight);
    }
}

void GeoClient::createPolygonRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("GeoClient::createPolygonRenderiables " << geoms.size());

    dmess("Start polygon quadTree...");

    auto startTime = system_clock::now();

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        doProgress("(5/6) Indexing polygons:", i, geoms.size(), startTime);

        Attributes     * attrs;
        const Geometry * g;
        
        tie(attrs, g) = geoms[i];

        Renderable * r = Renderable::create(g, trans);
        
        if(!r) { continue ;}
        
        r->setRenderFill   (true);
        r->setRenderOutline(true);

        tuple<Renderable *, const Geometry *, Attributes *> * data = new tuple<Renderable *, const Geometry *, Attributes *>(r, g, attrs);

        quadTreePolygons->insert(g->getEnvelopeInternal(), data);
    }

    GUI::progress("", 1.0f);

    dmess("quadTree " << quadTreePolygons->depth() << " " << geoms.size());
    
    dmess("Start base geom...");

    ColoredGeometryVec polygons;

    ColoredExtrudedGeometryVec polygons3D;

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        Attributes * attrs = geoms[i].first;

        GLuint colorID   = 0;
        double height    = 0.0;
        double minHeight = 0.0;

        // TODO look into the OSM docs for extruded types. E.G. barrier wall

        if( attrs->hasStringKey("addr_house")        ||
            attrs->hasStringKey("addr::housenumber") ||
            attrs->hasStringKey("addr::housename")   ||
            attrs->hasStringKeyValue("building", "house")) // TODO Are the ones above even doing anything?
        {
            colorID = 0;

            tie(height, minHeight) = getHeight(attrs);
        }
        else if(attrs->hasStringKey("building")      ||
                attrs->hasStringKey("building:part") ||
                attrs->hasStringKey("height"))
        {
            colorID = 1;

            tie(height, minHeight) = getHeight(attrs);
        }
        else if(attrs->hasStringKeyValue("landuse", "grass") ||
                attrs->hasStringKeyValue("surface", "grass"))    { colorID = 1 ;}
        else if(attrs->hasStringKeyValue("landuse", "reservor")) { colorID = 2 ;}

        if(height == 0.0) { polygons.push_back(ColoredGeometry(geoms[i].second, colorID)) ;}

        else { polygons3D.push_back(ColoredExtrudedGeometry(geoms[i].second, colorID, height, minHeight)) ;}
    }

    dmess("polygons " << polygons.size() << " polygons3D " << polygons3D.size());

    Renderable * r;

    if((r = RenderablePolygon::create(polygons, trans, true)))
    {
        r->setShader(ColorDistanceShader::getDefaultInstance());

        r->setRenderFill    (GUI::renderSettingsFillPolygons);
        r->setRenderOutline (GUI::renderSettingsRenderPolygonOutlines);

        canvas->addRenderable(r);
    }

    if((r = RenderableMesh::create(polygons3D, trans, true)))
    {
        r->setShader(ColorDistanceDepthShader3D::getDefaultInstance());

        r->setRenderFill    (GUI::renderSettingsFillMeshes);
        r->setRenderOutline (GUI::renderSettingsRenderMeshOutlines);

        canvas->addRenderable(r);
    }
    
    dmess("End base geom");
}

void GeoClient::createLineStringRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("Start create linestrings " << geoms.size());

    auto startTime = system_clock::now();

    ColoredGeometryVec polylines;

    vector<Edge *> edges;

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        doProgress("(2/6) Indexing linestrings:", i, geoms.size(), startTime);

        Attributes     * attrs   = geoms[i].first;
        const Geometry * geom    = geoms[i].second;
        GLuint           colorID = 0;

        // TODO Code dup!

             if(attrs->hasStringKeyValue("highway", "motorway"))     { colorID = 1 ;}
        else if(attrs->hasStringKeyValue("highway", "trunk"))        { colorID = 2 ;}
        else if(attrs->hasStringKeyValue("highway", "primary"))      { colorID = 3 ;}
        else if(attrs->hasStringKeyValue("highway", "secondary"))    { colorID = 4 ;}
        else if(attrs->hasStringKeyValue("highway", "tertiary"))     { colorID = 5 ;}
        else if(attrs->hasStringKeyValue("highway", "unclassified")) { colorID = 6 ;}
        else if(attrs->hasStringKeyValue("highway", "residential"))  { colorID = 7 ;}
        else if(attrs->hasStringKeyValue("highway", "service"))      { colorID = 8 ;}
        else if(attrs->hasStringKey     ("highway"))                 { colorID = 9 ;}

        unique_ptr<Geometry> buffered(geom->buffer(0.00001, 3));

        Renderable * r = Renderable::create(buffered.get(), trans);
        
        if(!r) { continue ;}
        
        r->setRenderFill   (true);
        r->setRenderOutline(true);

        Edge * edge = new Edge(r, dynamic_cast<const LineString *>(geom), attrs);

        edges.push_back(edge);

        quadTreeLineStrings->insert(geom->getEnvelopeInternal(), edge);

        polylines.push_back(make_pair(geom, colorID));
    }
    
    GUI::progress("Linestring index:", 1.0);

    dmess("edges " << edges.size());

    network->setEdges(edges);

    //OpenSteerGlue::init(canvas, network);

    dmess("linestring quadTree " << quadTreeLineStrings->depth() << " " << geoms.size());

    Renderable * r = RenderableLineString::create(polylines, trans, true);

    r->setShader(ColorDistanceShader::getDefaultInstance());

    r->setRenderOutline(GUI::renderSettingsRenderLinearFeatures);

    canvas->addRenderable(r);
    
    dmess("Done creating renderable.");

#ifndef __EMSCRIPTEN__

    GUI::progress("Waiting for server:", 0.0);

#endif
}

void GeoClient::createPointRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("Start create points " << geoms.size());

    auto startTime = system_clock::now();

    //vector<const Geometry *> points;
    ColoredGeometryVec points;

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        doProgress("(2/6) Indexing points:", i, geoms.size(), startTime);

        Attributes     * attrs = geoms[i].first;
        const Geometry * geom  = geoms[i].second;
        
        Renderable * r = Renderable::create(geom, trans);
        
        if(!r) { dmess("!r"); continue ;}
        
        tuple<Renderable *, const Geometry *, Attributes *> * data = new tuple<Renderable *, const Geometry *, Attributes *>(r, geom, attrs);

        quadTreePoints->insert(geom->getEnvelopeInternal(), data);

        points.push_back(ColoredGeometry(geom->buffer(0.0001), 1));
        //points.push_back(geom);
    }
    
    GUI::progress("", 1.0);

    dmess("Points quadTree " << quadTreePoints->depth() << " " << geoms.size());

    //Renderable * r = RenderablePoint::create(points, trans, true);
    Renderable * r = RenderablePolygon::create(points, trans, true);

    r->setShader(ColorDistanceShader::getDefaultInstance());

    canvas->addRenderable(r);
    
    dmess("Done creating renderable.");

#ifndef __EMSCRIPTEN__

    GUI::progress("Waiting for server:", 0.0);

#endif
}