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
  \copyright 2019
*/

#ifndef __EMSCRIPTEN__
    #include <curl/curl.h>
    #include <ctpl.h>
#endif

#include <fstream>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <glm/gtx/norm.hpp>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleTile.h>
#include <webAsmPlay/bing/BubbleFaceRender.h>
#include <webAsmPlay/bing/BingTileSystem.h>
#include <webAsmPlay/bing/StreetSide.h>

using namespace std;
using namespace glm;
using namespace nlohmann;
using namespace curlUtil;
using namespace boostGeom;
using namespace bingTileSystem;

namespace bg  = boost::geometry;
namespace bgi = boost::geometry::index;

vector<pair<Bubble *, Renderable *>> StreetSide::s_closestBubbles;

namespace
{
	StreetSide * a_instance = nullptr;

	string a_key;

	typedef std::tuple<Point, Bubble *, Renderable *> Value;

	bgi::rtree<Value, bgi::quadratic<16>> a_rtree;
}

StreetSide::StreetSide()
{
	ifstream keyFile("bingMapsKey.txt");

	if(!keyFile.is_open())
	{
		dmess("Could not open Bing Maps keyt file: bingMapsKey.txt");

		return;
	}

	if(!getline(keyFile, a_key)) { dmessError("Key file: bingMapsKey.txt is not valid!") ;}

	dmess("Key: " << a_key);
	
	keyFile.close();
}

StreetSide::~StreetSide() {}

StreetSide * StreetSide::getInstance()
{
	if(!a_instance) { a_instance = new StreetSide ;}

	return a_instance;
}

vector<Bubble *> StreetSide::query(const glm::dvec2 & pos, const size_t zoomLevel)
{
	const ivec2 pix	 = latLongToPixel	(pos, zoomLevel);
	const dvec2 pos2 = pixelToLatLong	(pix, zoomLevel);
	const ivec2 tile = pixelToTile		(pix);
    
	const dvec2 tMin = tileToLatLong(ivec2(tile.x + 0, tile.y + 1), zoomLevel);
	const dvec2 tMax = tileToLatLong(ivec2(tile.x + 1, tile.y + 0), zoomLevel);

	const string quadKey = tileToQuadKey(tile, zoomLevel);

	const string bubbleTileCachePath = "./bubbles/tile_" + quadKey;

	if(fileExists(bubbleTileCachePath))
	{
		dmess("Cached: " << bubbleTileCachePath);

		return Bubble::load(bubbleTileCachePath);
	}

	char url[2048];
	
	sprintf(url, "http://dev.virtualearth.net/mapcontrol/HumanScaleServices/GetBubbles.ashx?c=2000&e=%f&n=%f&s=%f&w=%f", tMax.y, tMax.x, tMin.x, tMin.y);

	unique_ptr<BufferStruct> buf(download(url).get());

	struct Membuf : streambuf
	{
		Membuf(char* begin, char* end) { setg(begin, begin, end) ;}
	};

	Membuf sbuf(buf->m_buffer, buf->m_buffer + buf->m_size);

	istream in(&sbuf);

	json j;

	j << in;

	//dmess("j " << j.dump(4));

	vector<Bubble *> bubbles;

	for(const auto & bubble : j)
	{
		//dmess(bubble.dump(4));

		//continue;

		auto _bubble = Bubble::create(bubble);

		if(!_bubble) { continue ;}

		bubbles.push_back(_bubble);

		//dmess(*_bubble);

		/*
		try
		{
			const auto id	= bubble["id"]; 
			const auto lat	= bubble["la"]; // Latitude of the Streetside image
			const auto lon	= bubble["lo"]; // Longitude of the Streetside image
			const auto roll = bubble["ro"]; // Roll
			const auto pitch = bubble["pi"]; // Pitch
			//const auto blurring = bubble["bl"]; // Blurring instructions
			const string blurring = "";
			const auto altitude = bubble["al"]; // The bubble altitude, in meters above the WGS84 ellipsoid

			dmess("id " << id << " lat " << lat << " lon " << lon << " roll " << roll << " pitch " << pitch << " altitude " << altitude << " blurring " << blurring);
		}
		catch (const std::exception&)
		{

		}
		*/
	}

	dmess("Num bubbles: " << bubbles.size());

	Bubble::save(bubbleTileCachePath, bubbles);

	return bubbles;
}

void StreetSide::doPicking(const char mode, const dvec4 & pos)
{
	
}

void StreetSide::indexBubble(Bubble * bubble, Renderable * renderiable)
{
	const Point pos = {bubble->m_pos.y, bubble->m_pos.x};

	a_rtree.insert({pos, bubble, renderiable});
}

void StreetSide::indexBubbles(const vector<pair<Bubble *, Renderable *>> & bubbles)
{
	for(auto [bubble, renderable] : bubbles)
	{
		
	}
}

void StreetSide::queryClosestBubbles(const dvec2 & pos, const size_t num)
{
	s_closestBubbles.clear();

	vector<Value> result;

    a_rtree.query(bgi::nearest(Point(pos.x, pos.y), num), std::back_inserter(result));

	if(!result.size()) { return ;}

	vector<pair<Value, double> > distValues(result.size());

	for(size_t i = 0; i < result.size(); ++i)
	{
		distValues[i] = { result[i], glm::distance2(__(get<0>(result[i])), pos) };
	}

	sort(distValues.begin(), distValues.end(), [](const auto & A, const auto & B) { return get<1>(A) > get<1>(B) ;});

	s_closestBubbles.resize(result.size());

	for(size_t i = 0; i < result.size(); ++i)
	{
		s_closestBubbles[i] = { get<1>(get<0>(distValues[i])), get<2>(get<0>(distValues[i])) };
	}

	preFetchBubbleTiles();
}

Bubble * StreetSide::closestBubble()
{
	if(!s_closestBubbles.size()) { return nullptr ;}

	return get<0>(*s_closestBubbles.rbegin());
}

void StreetSide::preFetchBubbleTiles()
{
	for(const auto [bubble, renderable] : s_closestBubbles)
	{
		for(size_t i = 0; i < 6; ++i)
		{
			for(const auto & tileID : BubbleFaceRender::getTileIDs())
			{
				BubbleTile::requestBubbleTile(bubble->getQuadKey(), i, tileID);
			}
		}
	}
}