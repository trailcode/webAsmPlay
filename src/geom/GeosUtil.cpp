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

#include <fstream>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/union/CascadedPolygonUnion.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/geom/GeosUtil.h>

using namespace std;
using namespace glm;
using namespace nlohmann;
using namespace geos::geom;
using namespace geos::operation::geounion;
using namespace geosUtil;

namespace
{
	auto makeBoxHelper(const GeometryFactory * geomFact, const dvec2 & min, const dvec2 & max)
	{
		auto temp = geomFact->getCoordinateSequenceFactory()->create((size_t) 0, 0);

		temp->add(Coordinate(min.x, min.y));
		temp->add(Coordinate(min.x, max.y));
		temp->add(Coordinate(max.x, max.y));
		temp->add(Coordinate(max.x, min.y));
	
		// Must close the linear ring or we will get an error:
		// "Points of LinearRing do not form a closed linestring"
		temp->add(Coordinate(min.x, min.y));

		return temp;
	}
}

Geometry::Ptr geosUtil::makeBox(const dvec2 & min, const dvec2 & max)
{
	const auto geomFact = GeometryFactory::getDefaultInstance();

    return Geometry::Ptr(geomFact->createPolygon(geomFact->createLinearRing(makeBoxHelper(geomFact, min, max)), nullptr));
}

Geometry::Ptr geosUtil::makeWireBox(const dvec2 & min, const dvec2 & max)
{
	const auto geomFact = GeometryFactory::getDefaultInstance();

    return Geometry::Ptr(geomFact->createLinearRing(makeBoxHelper(geomFact, min, max)));
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

Point * geosUtil::getPoint(const dvec2 & pos)
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

void geosUtil::addPoint(json& coordinates, const Point* P) { coordinates.push_back({P->getX(), P->getY()}) ;}

void geosUtil::addLineString(json & coordinates, const LineString* lineString)
{
	for (size_t i = 0; i < lineString->getNumPoints(); ++i) { addPoint(coordinates, lineString->getPointN(i)); }
}

json geosUtil::addLineString(const LineString* lineString)
{
	json coordinates;

	addLineString(coordinates, lineString);

	return coordinates;
}

void geosUtil::addPolygon(json& coordinates, const Polygon* polygon)
{
	coordinates.push_back(addLineString(polygon->getExteriorRing()));

	for (size_t i = 0; i < polygon->getNumInteriorRing(); ++i) { coordinates.push_back(addLineString(polygon->getInteriorRingN(i))) ;}
}

json geosUtil::addPolygon(const Polygon* polygon)
{
	json coordinates;

	addPolygon(coordinates, polygon);

	return coordinates;
}

string geosUtil::writeGeoJsonFile(const string& fileName, const Geometry* geom) { return writeGeoJsonFile(fileName, vector<const Geometry*>({ geom       })) ;}
string geosUtil::writeGeoJsonFile(const string& fileName, Geometry::Ptr & geom) { return writeGeoJsonFile(fileName, vector<Geometry*>      ({ geom.get() })) ;}

namespace
{
	template<typename Container>
	string writeGeoJsonFileHelper(const string& fileName, Container & geoms)
	{
		json geoJson;

		geoJson["type"] = "FeatureCollection";

		unordered_map<GeometryTypeId, string> typeMap({	{ GEOS_POINT,				"Point"				},
														{ GEOS_LINESTRING,			"LineString"		},
														{ GEOS_LINEARRING,			"LineString"		}, // TODO How to handle?
														{ GEOS_POLYGON,				"Polygon"			},
														{ GEOS_MULTIPOINT,			"MultiPoint"		},
														{ GEOS_MULTILINESTRING,		"MultiLineString"	},
														{ GEOS_MULTIPOLYGON,		"MultiPolygon"		},
														{ GEOS_GEOMETRYCOLLECTION,	"GeometryCollection"}});

		for (const auto& geom : geoms)
		{
			json feature;

			json geometry;

			geometry["type"] = typeMap[geom->getGeometryTypeId()];

			json& coordinates = geometry["coordinates"];

			switch (geom->getGeometryTypeId())
			{
		
			case GEOS_LINESTRING:
			case GEOS_LINEARRING:	addLineString	(coordinates, geosLineStringConst	(geom)); break;
			case GEOS_POINT:		addPoint		(coordinates, geosPointConst		(geom)); break;
			case GEOS_POLYGON:		addPolygon		(coordinates, geosPolygonConst		(geom)); break;
			case GEOS_MULTIPOLYGON:
				{
					const MultiPolygon* multiPolygon = geosMultiPolygonConst(geom);

					for (size_t i = 0; i < multiPolygon->getNumGeometries(); ++i) { coordinates.push_back(addPolygon(geosPolygonConst(multiPolygon->getGeometryN(i)))) ;}

					break;
				}

			default: dmess("Implement me!");
			}

			feature["geometry"] = geometry;

			geoJson["features"].push_back(feature);
		}

		ofstream out(fileName);

		out << geoJson.dump(4);

		out.close();

		return fileName;
	}
}

string geosUtil::writeGeoJsonFile(const string& fileName, const vector<Geometry *>			& geoms) { return writeGeoJsonFileHelper(fileName, geoms) ;}
string geosUtil::writeGeoJsonFile(const string& fileName, const vector<Geometry::Ptr>		& geoms) { return writeGeoJsonFileHelper(fileName, geoms) ;}
string geosUtil::writeGeoJsonFile(const string& fileName, const vector<const Geometry *>	& geoms) { return writeGeoJsonFileHelper(fileName, geoms) ;}








