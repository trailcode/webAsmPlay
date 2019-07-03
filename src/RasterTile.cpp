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
#include <webAsmPlay/Util.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/RasterTile.h>

using namespace std;
using namespace glm;
using namespace bingTileSystem;

namespace
{
	unordered_map<string, RasterTile*> a_currTileSet;
}

RasterTile::RasterTile(const dvec2& center, const size_t level) : m_center(center), m_level(level)
{
	
}

RasterTile::~RasterTile()
{
	const uint ID = m_textureID;

	glDeleteTextures(1, &ID); // TODO create and array of them to delete!, You can push here, and delete later

	const string quadKey = tileToQuadKey(latLongToTile(m_center, m_level), m_level);

	a_currTileSet.erase(quadKey);
}

RasterTile* RasterTile::getTile(const dvec2& center, const size_t level)
{
	const string quadKey = tileToQuadKey(latLongToTile(center, level), level);

	unordered_map<string, RasterTile*>::const_iterator i = a_currTileSet.find(quadKey);

	if (i != a_currTileSet.end()) { return i->second; }

	return a_currTileSet[quadKey] = new RasterTile(center, level);
}

RasterTile* RasterTile::getParentTile() const
{
	return getTile(m_center, m_level - 1);
}

#include <algorithm>
#include <sstream>

size_t RasterTile::pruneTiles()
{
	//auto tiles = toVec(a_currTileSet);

	vector<RasterTile*> tiles;

	for (const auto i : a_currTileSet) { tiles.push_back(i.second); }

	sort(tiles.begin(), tiles.end(), [](const RasterTile* A, RasterTile* B)
	{
		return A->m_lastRenderFrame < B->m_lastRenderFrame;
	});

	stringstream ss;

	dmess("tiles " << tiles.size());

	if (tiles.size() < 5500) { return 0; }

	//for (size_t i = 0; i < std::min((int)10, (int)tiles.size()); ++i)
	for (size_t i = 0; i < tiles.size(); ++i)
	{
		//dmess(tiles[i]->m_lastRenderFrame << " ");

		if (tiles[i]->m_loading)
		{
			//dmess("Loading " << i << " " << tiles[i]->m_lastRenderFrame);

			continue;
		}

		//dmess("delete " << i << " " << tiles[i]->m_lastRenderFrame);

		delete tiles[i];
	}

	return 0;
}