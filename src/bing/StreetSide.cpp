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
#include <webAsmPlay/Types.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BingTileSystem.h>
#include <webAsmPlay/bing/StreetSide.h>

using namespace std;
using namespace glm;
using namespace nlohmann;
using namespace curlUtil;
using namespace boostGeom;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

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

vector<Bubble *> StreetSide::query(const double boundsMinX, const double boundsMaxX, const double boundsMinY, const double boundsMaxY)
{
	// curl "https://dev.virtualearth.net/mapcontrol/HumanScaleServices/GetBubbles.ashx?appkey=AuftgJsO0Xs8Ts4M1xZUQJQXJNsvmh3IV8DkNieCiy3tCwCUMq76-WpkrBtNAuEm^&c=2000^&e=2.3268620483818374^&jsCallback=jsonpCache.kzikAeXGbFvsYGP^&n=48.8596821424352^&s=48.85712660600309^&w=2.322977794823074" -H "Sec-Fetch-Mode: no-cors" -H "Referer: https://www.openstreetmap.org/id" -H "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.100 Safari/537.36" --compressed

	CURL * myHandle = nullptr;

	myHandle = curl_easy_init();

	CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.

	BufferStruct output; // Create an instance of out BufferStruct to accept LCs output

	//if(!myHandle) { myHandle = curl_easy_init() ;}

	/* Notice the lack of major error checking, for brevity */

	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback); // Passing the function pointer to LC
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &output); // Passing our BufferStruct to LC

	string key = ""; // Seems to not need the key here. Why is that?

	char url[2048];

	// Seems to return a max of 1000 bubbles, boo.

	sprintf(url, "http://dev.virtualearth.net/mapcontrol/HumanScaleServices/GetBubbles.ashx?c=2000&e=%f&n=%f&s=%f&w=%f", boundsMaxX, boundsMaxY, boundsMinY, boundsMinX);
	//sprintf(url, "http://dev.virtualearth.net/mapcontrol/HumanScaleServices/GetBubbles.ashx?appkey=%s&c=2000&e=%f&n=%f&s=%f&w=%f", key.c_str(), boundsMaxX, boundsMaxY, boundsMinY, boundsMinX);
	
	//curl_easy_setopt(myHandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(myHandle, CURLOPT_URL, url);

	if (result = curl_easy_perform(myHandle)) { dmessError("result " << result << " myHandle " << myHandle) ;}

	dmess("url " << url);

	dmess("Size: " << output.m_size);

	//dmess("output: " << output.buffer);

	struct Membuf : streambuf
	{
		Membuf(char* begin, char* end) { setg(begin, begin, end) ;}
	};

	Membuf sbuf(output.m_buffer, output.m_buffer + output.m_size);

	istream in(&sbuf);

	json j;

	j << in;

	//dmess("j " << j.dump(4));

	vector<Bubble *> ret;

	for(const auto & bubble : j)
	{
		//dmess(bubble.dump(4));

		//continue;

		auto _bubble = Bubble::create(bubble);

		if(!_bubble) { continue ;}

		ret.push_back(_bubble);

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

	dmess("Num bubbles: " << ret.size());

	return ret;
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

pair<Bubble *, Renderable *> StreetSide::query(const dvec2 & pos)
{
	Point p(pos.x, pos.y);

	vector<Value> result;

    a_rtree.query(bgi::nearest(p, 1), std::back_inserter(result));

	dmess("result " << result.size());

	if(!result.size()) { return make_pair(nullptr, nullptr) ;}



	return make_pair(nullptr, nullptr);
}