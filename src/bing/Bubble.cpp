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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/bing/Bubble.h>

using namespace std;
using namespace glm;
using namespace nlohmann;
using namespace curlUtil;

namespace
{
	const auto a_faceKeys = vector<string>{"01","02","03","10","11","12"};
}

Bubble * Bubble::create(const json & bubble)
{
	try
	{
		// https://github.com/microsoft/MicrosoftStreetsidePlugin/blob/master/src/org/openstreetmap/josm/plugins/streetside/StreetsideImage.java

		if(bubble.find("id") == bubble.end()) { return nullptr ;}

		//*
		const auto id	= bubble["id"]; 
		const auto lat	= bubble["la"]; // Latitude of the Streetside image
		const auto lon	= bubble["lo"]; // Longitude of the Streetside image
		const auto roll = bubble["ro"]; // Roll
		const auto pitch = bubble["pi"]; // Pitch
		const auto altitude = bubble["al"]; // The bubble altitude, in meters above the WGS84 ellipsoid
		//*/

		dmess("id " << id << " lat " << lat << " lon " << lon << " roll " << roll << " pitch " << pitch << " altitude " << altitude);
		/*
		https://t.ssl.ak.tiles.virtualearth.net/tiles/hs0011023230012333103.jpg?g=6338&n=z
		https://t.ssl.ak.tiles.virtualearth.net/tiles/hs001102323001233202.jpg?g=6338&n=z
														0201110111322330
		https://t.ssl.ak.tiles.virtualearth.net/tiles/hs020323210121210001.jpg?g=6338&n=z
		https://t.ssl.ak.tiles.virtualearth.net/tiles/hs020111030231301201.jpg?g=6338&n=z
		https://t.ssl.ak.tiles.virtualearth.net/tiles/hs02011103023130120203.jpg?g=6338&n=z
		*/

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

FILE * Bubble::save(FILE * fp, const vector<Bubble *> & bubbles)
{
	const size_t numBubbles = bubbles.size();

	fwrite(&numBubbles, sizeof(size_t), 1, fp);

	for(const auto bubble : bubbles) { fwrite(bubble, sizeof(Bubble), 1, fp) ;}

	return fp;
}

vector<Bubble> Bubble::load(FILE * fp)
{
	vector<Bubble> ret;

	size_t numBubbles;

	fread(&numBubbles, sizeof(size_t), 1, fp);

	ret.resize(numBubbles);

	fread(&ret[0], sizeof(Bubble), numBubbles, fp);

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

GLuint Bubble::getCubeFaceTexture(const size_t face) const
{
	//const string streetsideImagesApi = "https://t.ssl.ak.tiles.virtualearth.net/tiles/hs";
	// TODO With https curl gives 60 error code. Try to fix.
	const string streetsideImagesApi = "http://t.ssl.ak.tiles.virtualearth.net/tiles/hs";

	const string imgUrlSuffix = ".jpg?g=6338&n=z";

	const auto faceQuadKey = getQuadKey() + a_faceKeys[face];

	const auto url = streetsideImagesApi + faceQuadKey + imgUrlSuffix;

	dmess("url " << url);

	// TODO code dup here.
	auto tileBuffer = shared_ptr<BufferStruct>(download(url));

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

	const auto ret = Textures::load(img);

	SDL_FreeSurface(img);

	return ret;
}