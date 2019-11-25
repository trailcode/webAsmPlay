#pragma once

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

#include <unordered_map>
#include <tbb/concurrent_unordered_map.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/bing/Bubble.h>

using namespace std;
using namespace tbb;
using namespace glm;
using namespace nlohmann;
using namespace curlUtil;

namespace
{
	const auto a_faceKeys = vector<string>{"01","02","03","10","11","12"};

	concurrent_unordered_map<string, size_t> a_bubbleTiles;
}

Bubble * Bubble::create(const json & bubble)
{
	try
	{
		// https://github.com/microsoft/MicrosoftStreetsidePlugin/blob/master/src/org/openstreetmap/josm/plugins/streetside/StreetsideImage.java

		if(bubble.find("id") == bubble.end()) { return nullptr ;}

		/*
		const auto id	= bubble["id"]; 
		const auto lat	= bubble["la"]; // Latitude of the Streetside image
		const auto lon	= bubble["lo"]; // Longitude of the Streetside image
		const auto roll = bubble["ro"]; // Roll
		const auto pitch = bubble["pi"]; // Pitch
		const auto altitude = bubble["al"]; // The bubble altitude, in meters above the WGS84 ellipsoid
		dmess("id " << id << " lat " << lat << " lon " << lon << " roll " << roll << " pitch " << pitch << " altitude " << altitude);
		//*/

		return new Bubble(			bubble["id"],
							dvec2(	bubble["lo"],
									bubble["la"]),
							dvec2(	bubble["ro"],
									bubble["pi"]),
									bubble["al"]);
	}
	catch (const std::exception&)
	{
		dmess("Warn could not create bubble: " << bubble.dump(4));

		return nullptr;
	}
}

Bubble::Bubble() :	m_ID		(0),
					m_pos		(),
					m_rollPitch	(),
					m_altitude	(0) {}

Bubble::Bubble(	const size_t	  ID,
				const dvec2		& pos,
				const dvec2		& rollPitch,
				const double	  altitude) :	m_ID		(ID),
												m_pos		(pos),
												m_rollPitch	(rollPitch),
												m_altitude	(altitude)
{
}

void Bubble::save(const string & fileName, const vector<Bubble *> & bubbles)
{
	FILE * fp = fopen(fileName.c_str(), "wb");

	if(!fp)
	{
		dmess("Warn could not open: " << fileName);

		return;
	}

	const size_t numBubbles = bubbles.size();

	fwrite(&numBubbles, sizeof(size_t), 1, fp);

	for(const auto bubble : bubbles) { fwrite(bubble, sizeof(Bubble), 1, fp) ;}

	fclose(fp);
}

vector<Bubble *> Bubble::load(const string & fileName)
{
	vector<Bubble *> ret;

	FILE * fp = fopen(fileName.c_str(), "rb");

	if(!fp)
	{
		dmess("Warn could not open: " << fileName);

		return ret;
	}

	size_t numBubbles;

	fread(&numBubbles, sizeof(size_t), 1, fp);

	ret.resize(numBubbles);

	for(size_t i = 0; i < numBubbles; ++i)
	{
		// TODO Try and not new a lot of little objects
		ret[i] = new Bubble();
		
		fread(ret[i], sizeof(Bubble), 1, fp);
	}

	return ret;
}

string Bubble::getQuadKey() const
{
	auto base4 = convertFrom10(m_ID, 4);

	const size_t paddingNeeded = 16 - base4.length();

	//dmess("paddingNeeded " << paddingNeeded);

	string quadKey;

	for(size_t i = 0; i < paddingNeeded; ++i) { quadKey += "0" ;}

	quadKey += base4;

	return quadKey;
}

#include <SDL_image.h>
#include <webAsmPlay/Textures.h>
#include <filesystem>

using namespace std::filesystem;

GLuint Bubble::requestCubeFaceTexture(const size_t face) const
{
	OpenGL::ensureSharedContext();

	//const string streetsideImagesApi = "https://t.ssl.ak.tiles.virtualearth.net/tiles/hs";
	// TODO With https curl gives 60 error code. Try to fix.
	const string streetsideImagesApi = "http://t.ssl.ak.tiles.virtualearth.net/tiles/hs";

	const string imgUrlSuffix = ".jpg?g=6338&n=z";

	const auto faceQuadKey = getQuadKey() + a_faceKeys[face];

	static unordered_set<string> checkedBubbleFaces;

	if(checkedBubbleFaces.find(faceQuadKey) != checkedBubbleFaces.end()) { return 0;}

	checkedBubbleFaces.insert(faceQuadKey);

	const auto i = a_bubbleTiles.find(faceQuadKey);

	if(i != a_bubbleTiles.end())
	{
		dmess("Cache hit!");

		return i->second;
	}

	const string tileCachePath = "./bubbles/face_" + faceQuadKey;

	if(fileExists(tileCachePath))
	{
		if(!file_size(tileCachePath.c_str()))
		{
			dmess("Here!");

			return 0;
		}

		auto img = IMG_Load(tileCachePath.c_str());

		if (!img) { goto download ;}

		auto ret = Textures::load(img);

		SDL_FreeSurface(img);

		a_bubbleTiles[faceQuadKey] = ret;

		return ret;
	}

	download:

	const auto url = streetsideImagesApi + faceQuadKey + imgUrlSuffix;

	//dmess("url " << url);

	download(url, [faceQuadKey, tileCachePath](BufferStruct * buf)
	{
		// TODO code dup here.
		//auto tileBuffer = shared_ptr<BufferStruct>(download(url));
		auto tileBuffer = shared_ptr<BufferStruct>(buf);

		if (!tileBuffer->m_buffer || tileBuffer->m_size == 11)
		{
			dmess("No data!");

			return 0;
		}

		auto img = IMG_LoadJPG_RW(SDL_RWFromConstMem(tileBuffer->m_buffer, tileBuffer->m_size));

		if(!img)
		{
			dmess("Bad data!");

			return 0;
		}

		const auto bytesPerPixel = img->format->BytesPerPixel;

		if(bytesPerPixel < 3)
		{
			SDL_FreeSurface(img);

			// Must be the no data png image, mark as no data.
			//return markTileNoData(tile);

			dmess("No data!");

			return 0;
		}

		////////////////////////////
		FILE * fp = fopen(tileCachePath.c_str(), "wb");

		if(fp)
		{
			fwrite(tileBuffer->m_buffer, sizeof(char), tileBuffer->m_size, fp);

			fclose(fp);
		}
		else { dmess("Warn could not write file: " << tileCachePath) ;}
		///////////////////////////

		Textures::s_queue.push([img, faceQuadKey](int ID)
		{
			const auto ret = Textures::load(img);

			a_bubbleTiles[faceQuadKey] = ret;

			SDL_FreeSurface(img);
		});
	});

	

	return 0;
}

GLuint Bubble::getCachedCubeFaceTexture(const size_t face) const
{
	const auto faceQuadKey = getQuadKey() + a_faceKeys[face];

	const auto i = a_bubbleTiles.find(faceQuadKey);

	if(i == a_bubbleTiles.end()) { return 0 ;}
	
	return i->second;
}
