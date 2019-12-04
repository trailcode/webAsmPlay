﻿/**
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

#include <limits>
#include <algorithm>
#include <unordered_map>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/bing/BingTileSystem.h>
#include <webAsmPlay/bing/RasterTile.h>

using namespace std;
using namespace glm;
using namespace bingTileSystem;

namespace
{
	unordered_map<string, RasterTile*> a_currTileSet;

	//vector<uint> a_texturesToFree;
}

RasterTile::RasterTile(const dvec2& center, const dvec2& widthHeight, const size_t level) : m_center		(center),
																							m_widthHeight	(widthHeight),
																							m_level			(level)
{
}

RasterTile::~RasterTile()
{
	//if(textureReady()) { a_texturesToFree.push_back(m_textureID) ;}
	
	const string quadKey = tileToQuadKey(latLongToTile(m_center, m_level), m_level);

	a_currTileSet.erase(quadKey);
}

RasterTile* RasterTile::getTile(const dvec2& center, const size_t level, const size_t accessTime)
{
	const auto tileIndex = latLongToTile(center, level);

	const string quadKey = tileToQuadKey(tileIndex, level);

	auto i = a_currTileSet.find(quadKey);

	RasterTile* tile;

	if (i != a_currTileSet.end())	{ tile = i->second ;}
	else
	{
		const dvec2 tMin = tileToLatLong(ivec2(tileIndex.x + 0, tileIndex.y + 1), level);
		const dvec2 tMax = tileToLatLong(ivec2(tileIndex.x + 1, tileIndex.y + 0), level);

		tile = a_currTileSet[quadKey] = new RasterTile((tMin + tMax) * 0.5, tMax - tMin, level);
	}

	tile->m_lastAccessTime = accessTime;

	return tile;
}

RasterTile* RasterTile::getParentTile(const size_t accessTime) const
{
	return getTile(m_center, m_level - 1, accessTime);
}

size_t RasterTile::getNumTiles() { return a_currTileSet.size() ;}

