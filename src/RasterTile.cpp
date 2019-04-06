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
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/RasterTile.h>

using namespace std;
using namespace glm;
using namespace bingTileSystem;

namespace
{
	unordered_map<string, RasterTile*> currTileSet;
}

RasterTile::RasterTile(const dvec2& center, const size_t level) : center(center), level(level)
{
	
}

RasterTile* RasterTile::getTile(const dvec2& center, const size_t level)
{
	const string quadKey = tileToQuadKey(latLongToTile(center, level), level);

	unordered_map<string, RasterTile*>::const_iterator i = currTileSet.find(quadKey);

	if (i != currTileSet.end()) { return i->second; }

	return currTileSet[quadKey] = new RasterTile(center, level);
}

RasterTile* RasterTile::getParentTile() const
{
	return getTile(center, level - 1);
}