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

#include <algorithm>
#include <unordered_map>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/renderables/RasterTile.h>

using namespace std;
using namespace glm;
using namespace bingTileSystem;

namespace
{
	unordered_map<string, RasterTile*> a_currTileSet;

	vector<uint> a_texturesToFree;
}

//atomic_size_t RasterTile::s_desiredMaxNumTiles = { 1500 };
atomic_size_t RasterTile::s_desiredMaxNumTiles = { 4000 };
//atomic_size_t RasterTile::s_desiredMaxNumTiles = { 10500 };

RasterTile::RasterTile(const dvec2& center, const size_t level) : m_center(center), m_level(level)
{
	
}

RasterTile::~RasterTile()
{
	if(m_textureID) { a_texturesToFree.push_back(m_textureID) ;}
	
	const string quadKey = tileToQuadKey(latLongToTile(m_center, m_level), m_level);

	a_currTileSet.erase(quadKey);
}

RasterTile* RasterTile::getTile(const dvec2& center, const size_t level, const size_t accessTime)
{
	const string quadKey = tileToQuadKey(latLongToTile(center, level), level);

	unordered_map<string, RasterTile*>::const_iterator i = a_currTileSet.find(quadKey);

	RasterTile* tile;

	if (i != a_currTileSet.end())	{ tile = i->second ;}
	else							{ tile = a_currTileSet[quadKey] = new RasterTile(center, level) ;}

	tile->m_lastAccessTime = accessTime;

	return tile;
}

RasterTile* RasterTile::getParentTile(const size_t accessTime) const
{
	return getTile(m_center, m_level - 1, accessTime);
}

size_t RasterTile::pruneTiles()
{
	vector<RasterTile*> tiles;

	for (const auto tile : a_currTileSet)
	{
		if(!tile.second->m_loading) { tiles.push_back(tile.second) ;}
	}

	sort(tiles.begin(), tiles.end(), [](const RasterTile* A, RasterTile* B)
	{
		return A->m_lastAccessTime < B->m_lastAccessTime;
	});

	const size_t cacheSize = s_desiredMaxNumTiles;

	if (tiles.size() < cacheSize) { return 0; }

	size_t numFreed = 0;

	
	for (size_t i = 0; i < tiles.size() - cacheSize; ++i)
	{
		delete tiles[i];

		++numFreed;
	}

	if(a_texturesToFree.size())
	{
		/*
		unordered_set<uint> texs(a_texturesToFree.begin(), a_texturesToFree.end());

		if(texs.size() != a_texturesToFree.size())
		{
			dmess("Duplicates!");
		}

		vector<uint> texsa;

		for(const auto i : texs)
		{
			texsa.push_back(i);
		}

		glDeleteTextures(texsa.size(), &texsa[0]);
		*/

		glDeleteTextures(a_texturesToFree.size(), &a_texturesToFree[0]);
	}

	a_texturesToFree.clear();

	//dmess("numFreed " << numFreed << " " << tiles.size() - cacheSize << " a_currTileSet " << a_currTileSet.size() << " cacheSize " << cacheSize);

	return numFreed;
}

size_t RasterTile::getNumTiles() { return a_currTileSet.size() ;}

