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
#include <webAsmPlay/bing/Bubble.h>

using namespace std;
using namespace glm;
using namespace nlohmann;

const array<string, 6> Bubble::s_faceKeys  = { "01",      "02",    "03",   "10",   "11",  "12"		};
const array<string, 6> Bubble::s_faceNames = { "forward", "right", "back", "left", "top", "bottom"	};

Bubble * Bubble::create(const json & bubble)
{
	try
	{
		// https://github.com/openstreetmap/iD/blob/76ffb5ef972b27b2a5658f0465282999da836b0f/modules/services/streetside.js#L372
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

