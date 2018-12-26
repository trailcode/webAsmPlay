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
  \copyright 2018
*/

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>

#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace glm;
using namespace geosUtil;
using namespace BingTileSystem;

namespace
{
    const size_t levelOfDetail = 15;

    vector<Renderable *> tiles;
}

RenderableBingMap::RenderableBingMap(const AABB2D & bounds, const dmat4 & trans) : bounds(bounds)
{
    minTile = latLongToTile(dvec2(get<1>(bounds), get<0>(bounds)), levelOfDetail);

    maxTile = latLongToTile(dvec2(get<3>(bounds), get<2>(bounds)), levelOfDetail);

    int tmp = minTile.y;
    minTile.y = maxTile.y;
    maxTile.y = tmp;

    dmess("minTile " << minTile.x << " " << minTile.y);

    for(int x = minTile.x; x <= maxTile.x; ++x)
    for(int y = minTile.y; y <= maxTile.y; ++y)
    {
        dvec2 tMin = tileToLatLong(ivec2(x + 0, y + 0), levelOfDetail);
        dvec2 tMax = tileToLatLong(ivec2(x + 1, y + 1), levelOfDetail);

        double tmp = tMin.x; tMin.x = tMin.y; tMin.y = tmp;

        tmp = tMax.x; tMax.x = tMax.y; tMax.y = tmp;

        tiles.push_back(Renderable::create(makeBox(tMin, tMax), trans));
    }
}

RenderableBingMap::~RenderableBingMap()
{

}

Renderable * RenderableBingMap::create(const AABB2D & bounds, const dmat4 & trans)
{
    return new RenderableBingMap(bounds, trans);
}

void RenderableBingMap::render(Canvas * canvas, const size_t renderStage) const
{
    //dmess("minTile " << maxTile.x - minTile.x << " " << maxTile.y - minTile.y);

    for(const auto r : tiles)
    {
        r->render(canvas, 0);
    }
}
