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

#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/union/CascadedPolygonUnion.h>
#include <nlohmann/json.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/geom/GeosUtil.h>

using namespace std;
using namespace glm;
using namespace nlohmann;
using namespace geos::geom;
using namespace geos::operation::geounion;

Geometry::Ptr geosUtil::makeBox(const double xmin, const double ymin, const double xmax, const double ymax)
{
    const GeometryFactory * geomFact = GeometryFactory::getDefaultInstance();

    CoordinateSequence * temp = geomFact->getCoordinateSequenceFactory()->create((size_t) 0, 0);

    temp->add(Coordinate(xmin, ymin));
    temp->add(Coordinate(xmin, ymax));
    temp->add(Coordinate(xmax, ymax));
    temp->add(Coordinate(xmax, ymin));
	
    // Must close the linear ring or we will get an error:
    // "Points of LinearRing do not form a closed linestring"
    temp->add(Coordinate(xmin, ymin));

    LinearRing * shell = geomFact->createLinearRing(temp);

    // NULL in this case could instead be a collection of one or more holes
    // in the interior of the polygon
    return Geometry::Ptr(geomFact->createPolygon(shell, NULL));
}

Geometry::Ptr geosUtil::makeBox(const dvec2 & min, const dvec2 & max)
{
    return makeBox(min.x, min.y, max.x, max.y);
}

Geometry::Ptr geosUtil::unionPolygons(const initializer_list<Geometry::Ptr> & polys)
{
    vector<Polygon *> _polys;

    for(const Geometry::Ptr & g : polys)
    {
        Polygon * p = dynamic_cast<Polygon *>(g.get());

        if(p) { _polys.push_back(p) ;}

        else { dmess("Warning not a polygon!") ;}
    }

    CascadedPolygonUnion unioner(&_polys);

    return Geometry::Ptr(unioner.Union());
}

vector<const LineString *> geosUtil::getExternalRings(const Geometry::Ptr & geom)
{
    return getExternalRings(geom.get());
}

vector<const LineString *> geosUtil::getExternalRings(const Geometry * geom)
{
    vector<const LineString *> ret;

    getExternalRings(ret, geom);

    return ret;
}

void geosUtil::getExternalRings(vector<const LineString *> & rings, const Geometry * geom)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POLYGON: rings.push_back(dynamic_cast<const Polygon *>(geom)->getExteriorRing()); break;

        case GEOS_GEOMETRYCOLLECTION:
        case GEOS_MULTIPOLYGON:
        {
            const GeometryCollection * collection = dynamic_cast<const GeometryCollection *>(geom);

            for(size_t i = 0; i < collection->getNumGeometries(); ++i) { getExternalRings(rings, collection->getGeometryN(i)) ;}

            break;
        }

        default:
            break;
    }
}

bool geosUtil::contains(const Geometry * A, const Geometry * B) { return A->contains(B) ;}

bool geosUtil::contains(const unique_ptr<Geometry> & A,
                        const unique_ptr<Geometry> & B) { return A->contains(B.get()) ;}

unique_ptr<Geometry> geosUtil::difference(  const unique_ptr<Geometry> & A,
                                            const unique_ptr<Geometry> & B) { return unique_ptr<Geometry>(A->difference(B.get())) ;}

bool geosUtil::difference(  const unique_ptr<Geometry> & A,
                            const unique_ptr<Geometry> & B,
                            unique_ptr<Geometry>       & out)
{
    Geometry * diff = A->difference(B.get());

    if(!diff) { return false ;}

    out = unique_ptr<Geometry>(diff);

    return true;
}

bool geosUtil::subtract(  Geometry *& A,
                    const Geometry *  B)
{
    Geometry * diff = A->difference(B);

    if(!diff) { return false ;}

    A = diff;

    return true;
}

bool geosUtil::subtract(      unique_ptr<Geometry> & A,
                        const unique_ptr<Geometry> & B) { return difference(A, B, A) ;}

vector<Geometry::Ptr> __(const vector<const LineString *> & lineStrings)
{
    vector<Geometry::Ptr> ret(lineStrings.size());

    for(size_t i = 0; i < lineStrings.size(); ++i) { ret.push_back(Geometry::Ptr(lineStrings[i]->clone())) ;}

    return ret;
}

Point * geosUtil::__(const dvec2 & pos)
{
    return GeometryFactory::getDefaultInstance()->createPoint(Coordinate(pos.x, pos.y));
}

dvec2 geosUtil::__(const Coordinate & point) { return dvec2(point.x, point.y) ;}

Coordinate ___(const dvec2 & point) { return Coordinate(point.x, point.y) ;}

dvec2 geosUtil::getStartPoint(const LineString * ls) { return __(ls->getCoordinateN(0)) ;}
dvec2 geosUtil::getEndPoint  (const LineString * ls) { return __(ls->getCoordinateN(int(ls->getNumPoints() - 1))) ;}

void geosUtil::transformInPlace(vector<Coordinate> & points, const dmat4 & trans)
{
    for(size_t i = 0; i < points.size(); ++i) { points[i] = ___(trans * dvec4(__(points[i]), 0, 1)) ;}
}

LineString * geosUtil::getLineString(const vector<dvec2> & verts)
{
    vector<Coordinate> * coords = new vector<Coordinate>(verts.size());

    for(size_t i = 0; i < verts.size(); ++i) { (*coords)[i] = ___(verts[i]) ;}

    return GeometryFactory::getDefaultInstance()->createLineString(new CoordinateArraySequence(coords, 2));
}

vector<dvec2> geosUtil::__(const vector<Coordinate> & coords)
{
    vector<dvec2> ret(coords.size());

    for(size_t i = 0; i < coords.size(); ++i) { ret[i] = __(coords[i]) ;}

    return ret;
}

vector<dvec2> geosUtil::__(const vector<Coordinate> * coords) { return __(*coords) ;}

vector<dvec2> geosUtil::__(const unique_ptr<vector<Coordinate> > & coords) { return __(coords.get()) ;}

_ScopedGeosGeometry::_ScopedGeosGeometry(Geometry * geom) : m_geom(geom) {}

_ScopedGeosGeometry::~_ScopedGeosGeometry() { GeometryFactory::getDefaultInstance()->destroyGeometry(m_geom) ;}

string writeGeoJsonFile(const string& fileName, const Geometry * geom)
{
	json geoJson;

	geoJson["type"] = "FeatureCollection";

	json feature;

	json geometry;

	geometry["type"] = "LineString";

	return fileName;
}






