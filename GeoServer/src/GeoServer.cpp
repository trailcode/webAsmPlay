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

#include <cstdio>
#include <limits>
#include <ctpl.h>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTWriter.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/geom/GeometryConverter.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/StreetSide.h>
#include <geoServer/OSM_Reader.h>
#include <geoServer/Topology.h>
#include <geoServer/GeoServer.h>

using namespace std;
using namespace glm;
using namespace boost;
using namespace geos::io;
using namespace geos::geom;
using namespace topology;
using namespace geosUtil;

namespace
{
    ctpl::thread_pool pool(1);
}

GeoServer::GeoServer() : m_boundsMinX( numeric_limits<double>::max()),
                         m_boundsMinY( numeric_limits<double>::max()),
                         m_boundsMaxX(-numeric_limits<double>::max()),
                         m_boundsMaxY(-numeric_limits<double>::max())
{
}

GeoServer::~GeoServer()
{
}

string GeoServer::addGeoFile(const string & geomFile)
{
    const string ext = filesystem::path(geomFile).extension().string();

         if(ext == ".osm") { addOsmFile          (geomFile) ;}
#ifdef __USE_GDAL__
    else if(ext == ".shp") { addGdalSupportedFile(geomFile) ;}
#endif
    else if(ext == ".geo") { _addGeoFile         (geomFile) ;}
    else if(ext == ".las" || ext == ".laz") { addLasFile(geomFile) ;}
    else
    {
        cout << "Error! Unsupported file: " << geomFile << endl;

        exit(-1);
    }

    return geomFile;
}

string GeoServer::addOsmFile(const string & osmFile)
{
	using geos::geom::Polygon;

    cout << "Loading: " << osmFile << endl;

    const auto mapData = OSM_Reader::import(osmFile);

    dmess("geometry " << mapData.m_geometry.size());

    dvec2 center;

    size_t numVertices = 0;

    size_t numEmptyPoints = 0;

    vector<AttributedPoligonalArea> polygons;
    vector<AttributedLineString>    lineStrings;

    for(const AttributedGeometry & i : mapData.m_geometry)
    {
        if(auto polygon = dynamic_cast<Polygon *>(i.second))
        {
            const double area = polygon->getArea();

            i.first->m_doubles["area"] = area;

            polygons.push_back(AttributedPoligonalArea(i.first, polygon, area));

            unique_ptr<CoordinateSequence> coords(i.second->getCoordinates());

            for(auto & c : *coords->toVector())
            {
                center += dvec2(c.x, c.y);

                ++numVertices;
            }
        }
        else if(auto lineString = dynamic_cast<LineString *>(i.second)) { lineStrings.push_back(AttributedLineString(i.first, lineString)) ;}
        else if(auto point		= dynamic_cast<Point *>		(i.second))
        {
            if(i.first) { m_serializedPoints.push_back(GeometryConverter::convert(AttributedPoint(i.first, point))) ;}
            else        { ++numEmptyPoints																			;}
        }
        else
        {
            dmess("Unknown type! " << i.second->getGeometryType());
        }
    }

    dmess("numPolygons " << polygons.size());

	auto AOI = dynamic_cast<Polygon *>(makeBox({mapData.m_boundsMinX, mapData.m_boundsMinY}, {mapData.m_boundsMaxX, mapData.m_boundsMaxY}).release());

	polygons.push_back(AttributedPoligonalArea(new Attributes(), AOI, AOI->getArea()));

	polygons = discoverTopologicalRelations(polygons);

    breakLineStrings(lineStrings);

    //createNavigationPaths(lineStrings);

    for(const auto & g : polygons)    { m_serializedPolygons   .push_back(GeometryConverter::convert(g)) ;}
    for(const auto & l : lineStrings) { m_serializedLineStrings.push_back(GeometryConverter::convert(l)) ;}
    
    dmess("numEmptyPoints " << numEmptyPoints);

    dmess("numVertices " << numVertices);

    center /= double(numVertices);

    m_boundsMinX = center.x;
    m_boundsMaxX = center.x;
    m_boundsMinY = center.y;
    m_boundsMaxY = center.y;

    if( mapData.m_boundsMinX != 0.0 &&
        mapData.m_boundsMaxX != 0.0 &&
        mapData.m_boundsMinY != 0.0 &&
        mapData.m_boundsMaxY != 0.0)
    {
        m_boundsMinX = mapData.m_boundsMinX;
        m_boundsMaxX = mapData.m_boundsMaxX;
        m_boundsMinY = mapData.m_boundsMinY;
        m_boundsMaxY = mapData.m_boundsMaxY;

		dmess("boundsMinX " << m_boundsMinX);
		dmess("boundsMaxX " << m_boundsMaxX);
		dmess("boundsMinY " << m_boundsMinY);
		dmess("boundsMaxY " << m_boundsMaxY);
    }

	/*
	boundsMinX = mapData.boundsMinX;
	boundsMaxX = mapData.boundsMaxX;
	boundsMinY = mapData.boundsMinY;
	boundsMaxY = mapData.boundsMaxY;
	*/

	m_bubbles = StreetSide::query(m_boundsMinX, m_boundsMaxX, m_boundsMinY, m_boundsMaxY);

	dmess("m_bubbles " << m_bubbles.size());

    const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

    // TODO code dup!
    m_trans = translate(  s,
                        dvec3(  (m_boundsMinY + m_boundsMinX) * -0.5,
                                (m_boundsMaxY + m_boundsMaxX) * -0.5,
                                0.0));

    saveGeoFile("data.geo");

    return osmFile;
}

namespace
{
    void _saveData(FILE * fp, const vector<string> & data)
    {
        const uint32_t num = (uint32_t)data.size();

        fwrite(&num, sizeof(uint32_t), 1, fp);

        for(const auto & i : data)
        {
            const uint32_t numBytes = (uint32_t)i.length();

            fwrite(&numBytes, sizeof(uint32_t), 1, fp);

            fwrite(i.data(), sizeof(char), numBytes, fp);
        }
    }

    void loadData(FILE * fp, vector<string> & data)
    {
        uint32_t num;

        fread(&num, sizeof(uint32_t), 1, fp);

        for(size_t i = 0; i < num; ++i)
        {
            uint32_t numBytes;

            fread(&numBytes, sizeof(uint32_t), 1, fp);

            data.push_back(string());

            string & str = *data.rbegin();

            str.resize(numBytes);

            fread((char *)str.data(), sizeof(char), numBytes, fp);
        }
    }
}

string GeoServer::saveGeoFile(const string & fileName)
{
    FILE * fp = fopen(fileName.c_str(), "wb");

    fwrite(&m_boundsMinX, sizeof(double), 1, fp);
    fwrite(&m_boundsMinY, sizeof(double), 1, fp);
    fwrite(&m_boundsMaxX, sizeof(double), 1, fp);
    fwrite(&m_boundsMaxY, sizeof(double), 1, fp);

    _saveData(fp, m_serializedPolygons);
    _saveData(fp, m_serializedLineStrings);
    _saveData(fp, m_serializedPoints);
    _saveData(fp, m_serializedRelations);

	dmess("m_serializedPoints " << m_serializedPoints.size());
	dmess("m_serializedRelations " << m_serializedRelations.size());

	Bubble::save(fp, m_bubbles);

    fclose(fp);

    return fileName;
}

string GeoServer::_addGeoFile(const string & geoFile)
{
    cout << "Loading: " << geoFile << endl;

    FILE * fp = fopen(geoFile.c_str(), "rb");

    fread(&m_boundsMinX, sizeof(double), 1, fp);
    fread(&m_boundsMinY, sizeof(double), 1, fp);
    fread(&m_boundsMaxX, sizeof(double), 1, fp);
    fread(&m_boundsMaxY, sizeof(double), 1, fp);

    loadData(fp, m_serializedPolygons);
    loadData(fp, m_serializedLineStrings);
    loadData(fp, m_serializedPoints);
    loadData(fp, m_serializedRelations);

    fclose(fp);

    return geoFile;
}

string GeoServer::addLasFile(const string & lasFile)
{

    return lasFile;
}



