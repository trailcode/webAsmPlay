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
#include <webAsmPlay/bing/Bubble.h>

using namespace std;
using namespace glm;
using namespace nlohmann;

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

ostream & operator<< (ostream &out, Bubble const & b)
{
	out << "id " << b.m_ID << " lon " << b.m_pos.x << " lat " << b.m_pos.y << " roll " << b.m_rollPitch.x << " pitch " << b.m_rollPitch.y << " altitude " << b.m_altitude;

	/*
	auto base4 = convertFrom10(b.m_ID, 4);

	const size_t paddingNeeded = 16 - base4.length();

	string quadKey;

	for(size_t i = 0; i < paddingNeeded; ++i) { quadKey += "0" ;}

	quadKey += base4;

	const string streetsideImagesApi = "https://t.ssl.ak.tiles.virtualearth.net/tiles/hs";

	const string imgUrlSuffix = ".jpg?g=6338&n=z";

	// Cubemap face code order matters here: front=01, right=02, back=03, left=10, up=11, down=12
    const auto faceKeys = vector<string>{"01","02","03","10","11","12"};

	out << "base4: " << streetsideImagesApi + quadKey + faceKeys[0] + "0" + imgUrlSuffix;
	*/
	
	return out;
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