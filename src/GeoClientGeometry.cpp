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

#include <ctpl/ctpl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/OpenSteerGlue.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/GeometryConverter.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderableMesh.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/OpenSteerGlue.h>
#include <webAsmPlay/GeoClient.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace ctpl;
using namespace geosUtil;
using namespace geos::geom;
using namespace geos::index::quadtree;

void GeoClient::createWorld(const char * data)
{
    dmess("GeoClient::createWorld");

    GUI::progress("Polygon index:", 0.0f);

    m_bounds = *(AABB2D *)data; data += sizeof(double) * 4;

    const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

    m_trans = translate(s,
                        dvec3(  (get<2>(m_bounds) + get<0>(m_bounds)) * -0.5,
                                (get<3>(m_bounds) + get<1>(m_bounds)) * -0.5,
                                0.0));
    
    m_inverseTrans = inverse(m_trans);

	GUI::guiASync([this]() { addBingMap(GUI::s_renderSettingsRenderBingMaps) ;});

	addGeometry(data);
}

void GeoClient::addGeometry(const char* data)
{
	createPolygonRenderiables   (GeometryConverter::getGeosPolygons   (data));
	createLineStringRenderiables(GeometryConverter::getGeosLineStrings(data));
	createPointRenderiables     (GeometryConverter::getGeosPoints     (data));

	OpenSteerGlue::init(m_canvas, getNetwork());
}

namespace
{
    pair<double, double> getHeight(Attributes * attrs)
    {
        const double scale = 0.005;

        double minHeight = 0;
        double height	 = scale;
        
        // See: https://wiki.openstreetmap.org/wiki/OSM-3D.org
        
             if(attrs->hasStringKey("height"))          { height    = atof(attrs->m_strings["height"]         .c_str()) * scale * 0.3 ;}
        else if(attrs->hasStringKey("building:height")) { height    = atof(attrs->m_strings["building:height"].c_str()) * scale * 0.3 ;}
        else if(attrs->hasStringKey("building:levels")) { height    = atof(attrs->m_strings["building:levels"].c_str()) * scale       ;}
        
        if(attrs->hasStringKey("min_height")) { minHeight = atof(attrs->m_strings["min_height"].c_str()) * scale * 0.3 ;}

        return make_pair(height, minHeight);
    }

	thread_pool indexerPool(1);
}

void GeoClient::createPolygonRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("GeoClient::createPolygonRenderiables " << geoms.size());

    auto startTime = system_clock::now();

	indexerPool.push([this, &geoms](int ID)
	{
		for(size_t i = 0; i < geoms.size(); ++i)
		{
			//doProgress("(5/6) Indexing polygons:", i, geoms.size(), _startTime);

			const auto [attrs, g] = geoms[i];

			m_quadTreePolygons->insert(g->getEnvelopeInternal(), new tuple{ nullptr, g, attrs });
		}
	});

    GUI::progress("", 1.0f);

    dmess("quadTree " << m_quadTreePolygons->depth() << " " << geoms.size());
    
    ColoredGeometryVec polygons;

    ColoredExtrudedGeometryVec polygons3D;

	for(const auto [attrs, geom] : geoms)
    {
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

        if(height == 0.0) { polygons.push_back(ColoredGeometry(geom, colorID)) ;}

        else { polygons3D.push_back(ColoredExtrudedGeometry(geom, colorID, height, minHeight)) ;}
    }

    dmess("polygons " << polygons.size() << " polygons3D " << polygons3D.size());

    if(auto r = RenderablePolygon::create(polygons, m_trans, true))
    {
		r->setShader(new ColorDistanceShader(	ColorSymbology::getInstance("defaultPolygon"),
												// Should render functor
												[](const bool isOutline, const size_t renderingStage) -> bool
												{
													return renderingStage == 1;
												}));

        r->setRenderFill    (GUI::s_renderSettingsFillPolygons);
        r->setRenderOutline (GUI::s_renderSettingsRenderPolygonOutlines);

		GUI::guiASync([this, r]() { m_canvas->addRenderable(r) ;});
    }

    if(auto r = RenderableMesh::create(polygons3D, m_trans, true))
    {
		r->setShader(ColorDistanceDepthShader3D::getDefaultInstance());

        r->setRenderFill    (GUI::s_renderSettingsFillMeshes);
        r->setRenderOutline (GUI::s_renderSettingsRenderMeshOutlines);

		GUI::guiASync([this, r]() { m_canvas->addRenderable(r) ;});
    }
    
    dmess("End base geom");
}

void GeoClient::createLineStringRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("Start create linestrings " << geoms.size());

    auto startTime = system_clock::now();

    ColoredGeometryVec polylines;

    vector<Edge *> edges;

	for(const auto [attrs, geom] : geoms)
	{
		GLuint colorID = 0;

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

		Edge * edge = new Edge(nullptr, dynamic_cast<const LineString *>(geom), attrs);
		
		edges.push_back(edge);

		polylines.push_back(make_pair(geom, colorID));
	}

	indexerPool.push([this, edges](int ID)
	{
		for(const auto edge : edges) { m_quadTreeLineStrings->insert(edge->getGeometry()->getEnvelopeInternal(), edge) ;}
	});
	
	m_network->setEdges(edges);
	
    auto r = RenderableLineString::create(polylines, m_trans, true);

	r->setShader(new ColorDistanceShader(	ColorSymbology::getInstance("defaultLinear"),
											// Should render functor
											[](const bool isOutline, const size_t renderingStage) -> bool
											{
												return renderingStage == 0;
											}));

    r->setRenderOutline(GUI::s_renderSettingsRenderLinearFeatures);

	GUI::guiASync([this, r]() { m_canvas->addRenderable(r) ;});
    
    dmess("Done creating renderable.");

#ifndef __EMSCRIPTEN__

    GUI::progress("Waiting for server:", 0.0);

#endif
}

void GeoClient::createPointRenderiables(const vector<AttributedGeometry> & geoms)
{
    dmess("Start create points " << geoms.size());

	if(!geoms.size()) { return ;}

    auto startTime = system_clock::now();

    ColoredGeometryVec points;

    for(size_t i = 0; i < geoms.size(); ++i)
    {
        doProgress("(2/6) Indexing points:", i, geoms.size(), startTime);

		const auto [attrs, geom] = geoms[i];

        auto r = Renderable::create(geom, m_trans);
        
        if(!r) { dmess("!r"); continue ;}
        
        m_quadTreePoints->insert(geom->getEnvelopeInternal(), new tuple{ r, geom, attrs });

        points.push_back(ColoredGeometry(geom->buffer(0.00001, 3), 1));
    }
    
    GUI::progress("", 1.0);

    Renderable * r = RenderablePolygon::create(points, m_trans, true);

    r->setShader(ColorDistanceShader::getDefaultInstance());
	
	GUI::guiASync([this, r]() { m_canvas->addRenderable(r) ;});
    
    dmess("Done creating renderable.");

#ifndef __EMSCRIPTEN__

    GUI::progress("Waiting for server:", 0.0);

#endif
}